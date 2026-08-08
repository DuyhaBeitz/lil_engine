#include "LilEditor.hpp"
#include <utils/MeshHelper.hpp>
#include <utils/FileHelper.hpp>
#include "FileDialogHelper.hpp"

Lil::Editor &Lil::Editor::Get() {
    static Lil::Editor instance;
    return instance;
}

void ResizeTarget(RenderTexture2D& target, int w, int h) {
    if (target.texture.width != w || target.texture.height != h) {
        UnloadRenderTexture(target);
        target = LoadRenderTexture(w, h);
    }
}

void BeginTaretMode(RenderTexture2D& target, int topleftX, int topleftY, int width, int height) {
    SetMouseOffset(-topleftX, -topleftY);
    ResizeTarget(target, width, height);
    SetGizmoRenderSize(width, height);
    BeginTextureMode(target);
}

void EndTargetMode() {
    EndTextureMode();
    SetMouseOffset(0, 0);
}

#define TOGGLE_CURSOR_KEY KEY_P
#define TOGGLE_SIMULATION_KEY KEY_F
#define TOGGLE_DEBUG_KEY KEY_V
#define TOGGLE_FULLSCREEN_KEY KEY_F11

void Lil::Editor::DropSelectedActor() {
    m_selected_actor = nullptr;
    m_selected_component = nullptr;
}

void Lil::Editor::SelectActor(Actor *actor) {
    if (!actor) return;
    if (actor == m_selected_actor) return;
    m_selected_component = nullptr;
    m_selected_actor = actor;
}

void Lil::Editor::LoadScene() {
    const char* source = BrowseSceneDialog();
    if (source) {
        try {
            std::string filename = source;
            Lil::SceneManager().LoadScene(filename);
            DropSelectedActor();
            Notify("Loaded file: " + filename);
        }
        catch (const cereal::Exception& e) {
            Notify(std::string("Failed to load: ") + e.what());
        }
    }
    else {
        Notify("Aborted");
    }
}

void Lil::Editor::SaveScene() {
    const char* source = SaveSceneDialog();
    if (source) {
        try {
            std::string filename = source;
            Lil::SceneManager().SaveScene(filename);
            Notify("Saved file: " + filename);
        }
        catch (const cereal::Exception& e) {
            Notify(std::string("Failed to save: ") + e.what());
        }
    }
    else {
        Notify("Aborted");
    }
}

void Lil::Editor::InitUI() {
    rlImGuiBeginInitImGui();
    ImGui::StyleColorsDark();

    ImGuiIO& io = ImGui::GetIO();

    float s = 3.0f;
    io.DisplayFramebufferScale = ImVec2(s, s);
    io.FontGlobalScale = s; 

#ifdef IMGUI_HAS_DOCK
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
#endif

    float fontSize = 16.0f;
    ImFont* customFont = io.Fonts->AddFontFromFileTTF("assets/font/JetBrainsMono-Medium.ttf", fontSize);
    
    if (customFont == nullptr) {
        LIL_LOG_WARN("ImGui failed to load custom font! Falling back to default.");
    }

    io.FontDefault = customFont;
    
    rlImGuiEndInitImGui();
    Lil::UIStyle::InitGlobalTheme();
}

void Lil::Editor::DrawMenuBar() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Load Scene", "Ctrl+L")) LoadScene();
            if (ImGui::MenuItem("Save Scene", "Ctrl+S")) SaveScene();
            ImGui::EndMenu();
        }

       ImGui::EndMainMenuBar();
    }
}

void Lil::Editor::Init() {
    m_viewport_render_target = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    m_layout_render_target = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    InitUI();
    SetExitKey(KEY_NULL);
}

void Lil::Editor::DrawInspector() {
    ImGui::Begin("Inspector");

    if (m_selected_actor) {
        if (ImGui::SmallButton(ICON_FA_TRASH)) {
            Lil::World().DestroyActor(m_selected_actor);
            DropSelectedActor();
        }
        else {
            ImGui::SameLine();

            m_editor.SetCurrentObjectName(m_selected_actor->GetTypeInfo().Name());
            m_editor.VisitObject(m_selected_actor->GetTypeInfo(), m_selected_actor);
            
            ImGui::Spacing();
            
            float availWidth = ImGui::GetContentRegionAvail().x;
            ImGui::Begin("Components");
            ImGui::SameLine(availWidth - 30.0f);
            
            if (ImGui::Button(ICON_FA_PLUS)) {
                ImGui::OpenPopup("AddComponentPopup");
            }
            ImGui::Separator();
            
            for (auto& component : m_selected_actor->Components()) {            
                ImGui::PushID(component);

                if (component->IsRequired()) {
                    ImGui::BeginDisabled(true);
                    ImGui::SmallButton(ICON_FA_LOCK);
                    ImGui::EndDisabled();
                    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                        ImGui::SetTooltip("This component is required");
                    }
                } else {
                    if (ImGui::SmallButton(ICON_FA_TRASH)) {
                        m_selected_actor->DeattachComponent(component);
                        Lil::World().DestroyComponent(component);
                        ImGui::PopID();
                        continue;
                    }
                }

                ImGui::SameLine();
                m_editor.SetCurrentObjectName(component->GetTypeInfo().Name());

                ImVec2 min = ImGui::GetCursorScreenPos();
                m_editor.VisitObject(component->GetTypeInfo(), component);
                ImVec2 max = ImGui::GetCursorScreenPos();

                float height = max.y - min.y;
                float width = ImGui::GetContentRegionAvail().x;
                ImVec2 rectMax(min.x + width, max.y);
                
                if (ImGui::IsMouseHoveringRect(min, rectMax) && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                    m_selected_component = component;
                }

                ImGui::PopID();
            }
            
            if (ImGui::BeginPopup("AddComponentPopup")) {
                ImGui::Text("Available components:");
                ImGui::Separator();
                
                for (auto& [name, ti] : Lil::Reflection::Get().Types()) {
                    if (ti->IsA<Component>() && *ti != TypeInfo::Get<Component>()) {
                        bool exists = false;
                        for (auto& component : m_selected_actor->Components()) {
                            if (component->GetTypeInfo().Name() == name) {
                                exists = true;
                                break;
                            }
                        }
                        
                        if (!exists && ImGui::MenuItem(name.c_str())) {
                            Component* component = Lil::World().CreateComponent(ti);
                            if (component) {
                                m_selected_actor->AttachComponent(component);
                            }                        
                            ImGui::CloseCurrentPopup();
                        }
                    }
                }
                
                ImGui::EndPopup();
            }
            ImGui::End();
        }
    }

    ImGui::End();
}

void Lil::Editor::DrawLayout() {
    ImGui::Begin("Layout");

    ImVec2 contentSize = ImGui::GetContentRegionAvail();
    ImVec2 viewerTopLeft = ImGui::GetCursorScreenPos();

    int size = fmin(contentSize.x, contentSize.y);
    BeginTaretMode(m_layout_render_target, viewerTopLeft.x, viewerTopLeft.y, size, size);
        ClearBackground(RAYBLACK);   
        BeginMode3D(m_layout_camera);
            if (m_selected_actor) {
                Transform old_actor_transform = m_selected_actor->GetTransform();
                m_selected_actor->SetPosition(Vector3{0.0, 0.0f, 0.0f});
                m_selected_actor->SetRotation(QuaternionIdentity());
                m_selected_actor->LayoutUpdate();

                m_selected_actor->Draw();
                if (Lil::World().m_physics_debug) {
                    m_selected_actor->DebugUpdate();
                    m_selected_actor->DebugDraw();
                }

                if (m_selected_component) {
                    Transform t = m_selected_component->GetTransform();
                    DrawGizmo3D(m_gizmo_mode | m_gizmo_space, &t);
                    m_selected_component->Local() = GetLocalTransform(m_selected_actor->GetTransform(), t);
                }
                m_selected_actor->SetTransform(old_actor_transform);
                m_selected_actor->LayoutUpdate();
            }
        EndMode3D();
    EndTargetMode();
    
    rlImGuiImageRenderTexture(&m_layout_render_target);

    ImGui::End();
}

void Lil::Editor::Update() {
    if (!m_cursor_enabled) {UpdateCamera(&m_viewport_camera, CAMERA_FREE);}
    Lil::Engine::Get().Update();

    UpdateGizmoMode();
    
    if (IsKeyPressed(TOGGLE_SIMULATION_KEY)) {
        Lil::World().ToggleSimulationGoing();
    }
    if (IsKeyPressed(TOGGLE_DEBUG_KEY)) {
        Lil::World().m_physics_debug = !Lil::World().m_physics_debug;
    }

    if (IsKeyPressed(TOGGLE_FULLSCREEN_KEY)) {
        ToggleFullscreen();
    }
    if (IsKeyDown(KEY_LEFT_CONTROL)) {
        if (IsKeyPressed(KEY_S)) SaveScene();
        else if (IsKeyPressed(KEY_L)) LoadScene();
    }
}

void Lil::Editor::UpdateGizmoMode() {
    if (IsKeyPressed(KEY_ONE)) m_gizmo_mode = GIZMO_TRANSLATE;
    if (IsKeyPressed(KEY_TWO)) m_gizmo_mode = GIZMO_ROTATE;
    if (IsKeyPressed(KEY_THREE)) m_gizmo_mode = GIZMO_SCALE;
}

void Lil::Editor::HandleViewportInput() {
    ImGuiIO& io = ImGui::GetIO();

    if (IsKeyPressed(KEY_ESCAPE)) DropSelectedActor();

    if (IsKeyPressed(TOGGLE_CURSOR_KEY)) {
        if (m_cursor_enabled) DisableCursor();
        else EnableCursor();
        m_cursor_enabled = !m_cursor_enabled;
    }

    if (m_cursor_enabled && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        Actor* pick = Lil::World().PickActor(GetMousePosition(), m_viewport_render_target.texture.width, m_viewport_render_target.texture.height, m_viewport_camera);
        if (pick) SelectActor(pick);
    }
}

void Lil::Editor::DrawViewport() {
    ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_MenuBar);

    if (ImGui::BeginMenuBar())  {
        bool isLocalGizmo = (m_gizmo_space == GIZMO_LOCAL); 
        if (ImGui::Button(isLocalGizmo ? "Local" : "World")) {
            isLocalGizmo = !isLocalGizmo;
            if (!isLocalGizmo) m_gizmo_space = GIZMO_DISABLED; 
            else m_gizmo_space = GIZMO_LOCAL; 
        }
        ImGui::SameLine();

        bool isSimulating = Lil::World().IsSumulationGoing();
        ImGui::PushStyleColor(ImGuiCol_Button, isSimulating ? IM_COL32(255, 80, 80, 255) : IM_COL32(80, 180, 80, 255));
        if (ImGui::Button(isSimulating ? "Stop" : "Play")) {
            isSimulating = !isSimulating;
            Lil::World().SetSimulationGoing(isSimulating);
        }
        ImGui::PopStyleColor();
        ImGui::SameLine();


        static const std::unordered_map<std::string, RenderMode> renderModeMap = {
            {"Unlit", RenderMode::Unlit},
            {"Wireframe", RenderMode::Wireframe}
        };

        static RenderMode currentMode = RenderMode::Unlit;
        
        std::string currentName = "Unlit";
        for (const auto& [name, mode] : renderModeMap) {
            if (mode == currentMode) {
                currentName = name;
                break;
            }
        }

        ImGui::SetNextItemWidth(300.0f);
        if (ImGui::BeginCombo("##Render Mode", currentName.c_str())) {
            for (const auto& [name, mode] : renderModeMap)
            {
                bool isSelected = (mode == currentMode);
                if (ImGui::Selectable(name.c_str(), isSelected))
                {
                    currentMode = mode;
                    Lil::World().SetRenderMode(mode);
                }
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        ImGui::SameLine(ImGui::GetWindowWidth() - 230.0f);
        ImGui::Text("FPS: %d", GetFPS());

        ImGui::EndMenuBar();
    }


    ImVec2 contentSize = ImGui::GetContentRegionAvail();
    ImVec2 viewerTopLeft = ImGui::GetCursorScreenPos();

    BeginTaretMode(m_viewport_render_target, viewerTopLeft.x, viewerTopLeft.y, contentSize.x, contentSize.y);
        ImGuiIO& io = ImGui::GetIO();
        if (ImGui::IsWindowHovered() || ImGui::IsWindowFocused() || !m_cursor_enabled) HandleViewportInput();
        ClearBackground(RAYBLACK);   
        BeginMode3D(m_viewport_camera);
            Lil::World().Draw();
            if (Lil::World().m_physics_debug) Lil::World().DebugDraw();
            if (m_selected_actor) {
                Transform t = m_selected_actor->GetTransform();
                DrawGizmo3D(m_gizmo_mode | m_gizmo_space, &t);
                m_selected_actor->SetTransform(t);
            }
        EndMode3D();
    EndTargetMode();
    
    rlImGuiImageRenderTexture(&m_viewport_render_target);
    
    ImGui::End();
}

void Lil::Editor::DrawResources() {
    ImGui::Begin("Models");
    if (ImGui::Button(ICON_FA_PLUS)) {
        const char* source = BrowseModelDialog();
        if (source) {
            Lil::Resources().ModelAdd(NameFromPath(source), source);
            CopyAsset(source);
        }
    }

    Lil::Resources().UpdateModelPreviews();
    for (auto& [key, model] : Lil::Resources().Models()) {
        rlImGuiImageRenderTexture(Lil::Resources().GetModelPreview(key));
        ImGui::SameLine();
        ImGui::Text("%s", key.c_str());
    }
    ImGui::End();


    ImGui::Begin("Textures");
    if (ImGui::Button(ICON_FA_PLUS)) {
        const char* source = BrowseTextureDialog();
        if (source) {
            Lil::Resources().TextureAdd(NameFromPath(source), source);
            CopyAsset(source);
        }
    }

    for (auto& [key, texture] : Lil::Resources().Textures()) {
        rlImGuiImage(&texture);
        ImGui::SameLine();
        if (ImGui::CollapsingHeader(key.c_str())) {
            std::string heightmap_name = HeightmapNameFromImageName(key);
            if (!Lil::Resources().ModelExists(heightmap_name)) {
                if (ImGui::Button("Generate heightmap model")) {
                    Image image = LoadImageFromTexture(texture);
                    Lil::Resources().ModelAdd(heightmap_name, HeightmapModel(image, Vector3{1.0f, 1.0f, 1.0f}));
                    UnloadImage(image);
                }
            }
        }
    }
    ImGui::End();

    ImGui::Begin("Sounds");
    if (ImGui::Button(ICON_FA_PLUS)) {
        const char* source = BrowseSoundDialog();
        if (source) {
            Lil::Resources().SoundAdd(NameFromPath(source), source);
            CopyAsset(source);
        }
    }

    for (auto& [name, sound] : Lil::Resources().Sounds()) {
        ImGui::PushID(&sound);
        if (ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
            auto& volume = sound.volume_multiplier;

            ImGui::SetNextItemWidth(120.0f);
            if (ImGui::Button("Play " ICON_FA_PLAY)) {
                sound.Play();
            }

            ImGui::SetNextItemWidth(300.0f);
            ImGui::DragFloat("Volume", &volume, 0.01f, 0.0f, 1.0f, "%.2f");
        }
        ImGui::PopID();
    }
    ImGui::End();
}

void Lil::Editor::Notify(const std::string &message, float duration, const ImVec4 &color) {
    if (m_notifications.size() > 5) m_notifications.erase(m_notifications.begin());
    m_notifications.emplace_back(message, duration, color);
}

void Lil::Editor::DrawNotifications() {
    if (m_notifications.empty()) return;

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImVec2 screenPos = ImVec2(viewport->Size.x - 20.0f, viewport->Size.y - 20.0f); 

    for (int i = (int)m_notifications.size() - 1; i >= 0; --i) {
        Notification& notif = m_notifications[i];

        float alpha = 1.0f;
        if (notif.TimeLeft() < 0.5f) alpha = notif.TimeLeft() / 0.5f;

        ImVec2 windowPos = ImVec2(screenPos.x, screenPos.y - (i * 65.0f)); 
        ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always, ImVec2(1.0f, 1.0f));
        
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar |
                                 ImGuiWindowFlags_NoResize |
                                 ImGuiWindowFlags_NoMove |
                                 ImGuiWindowFlags_NoScrollbar |
                                 ImGuiWindowFlags_NoSavedSettings |
                                 ImGuiWindowFlags_NoFocusOnAppearing |
                                 ImGuiWindowFlags_NoNav |
                                 ImGuiWindowFlags_NoInputs;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.15f, 0.85f * alpha));
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(notif.color.x, notif.color.y, notif.color.z, alpha));

        if (ImGui::Begin(("##notification_" + std::to_string(i)).c_str(), nullptr, flags)) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(notif.color.x, notif.color.y, notif.color.z, alpha));
            ImGui::TextUnformatted(notif.message.c_str());
            ImGui::PopStyleColor();
        }
        ImGui::End();

        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar(2);
    }

    m_notifications.erase(
        std::remove_if(m_notifications.begin(), m_notifications.end(),
            [](const Notification& n) { return n.TimeLeft() <= 0; }),
        m_notifications.end()
    );
}

void Lil::Editor::Draw() {
    ClearBackground(RAYBLACK);
    rlImGuiBegin();

#ifdef IMGUI_HAS_DOCK
    ImGui::DockSpaceOverViewport(0,  NULL, ImGuiDockNodeFlags_PassthruCentralNode); // set ImGuiDockNodeFlags_PassthruCentralNode so that we can see the raylib contents behind the dockspace
#endif
    DrawInspector();
    DrawResources();
    DrawViewport();
    DrawLayout();
    DrawMenuBar();

    ImGui::Begin("Creation");
    for (auto& [name, ti] : Lil::Reflection::Get().Types()) {
        if (ti->IsA<Actor>()) {
            const char* typeName = name.c_str();

            if (ImGui::Selectable(typeName, false, ImGuiSelectableFlags_AllowDoubleClick)) {
                if (ImGui::IsMouseDoubleClicked(0)) {
                    Actor* actor = Lil::World().CreateActor(ti);
                    if (actor) {
                        SelectActor(actor);
                    }
                }
            }
        }        
    }
    ImGui::End();

    ImGui::Begin("Actors");
    for (auto& [id, actor] : Lil::World().Actors()) {
        std::string typeName = actor->GetTypeInfo().Name();

        bool isSelected = (m_selected_actor == actor.get());
        if (ImGui::Selectable((typeName+"__"+actor->GetIDString()).c_str(), isSelected)) {
            SelectActor(actor.get());
        }
    }
    ImGui::End();
    
    DrawNotifications();

    rlImGuiEnd();
}
