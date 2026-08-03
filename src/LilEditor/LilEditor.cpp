#include "LilEditor.hpp"
#include <utils/MeshHelper.hpp>
#include <utils/FileHelper.hpp>
#include "FileDialogHelper.hpp"

Lil::Editor &Lil::Editor::Get() {
    static Lil::Editor instance;
    return instance;
}

#define TOGGLE_CURSOR_KEY KEY_P
#define TOGGLE_SIMULATION_KEY KEY_F
#define TOGGLE_DEBUG_KEY KEY_V
#define TOGGLE_FULLSCREEN_KEY KEY_F11

void Lil::Editor::LoadScene() {
    std::string filename = "scene.json";
    Lil::SceneManager().LoadScene(filename);
    m_selected = nullptr;
    Notify("Loaded file: " + filename);
}

void Lil::Editor::SaveScene() {
    std::string filename = "scene.json";
    Lil::SceneManager().SaveScene(filename);
    Notify("Saved file: " + filename);
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

void Lil::Editor::LoadTarget(int w, int h) {
    m_render_target = LoadRenderTexture(w, h);
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

void Lil::Editor::TryResizeTarget(int w, int h) {
    if (m_render_target.texture.width != w || m_render_target.texture.height != h) {
        LoadTarget(w, h);
    }
}

void Lil::Editor::Init() {
    LoadTarget(GetScreenWidth(), GetScreenHeight());
    InitUI();
    SetExitKey(KEY_NULL);
}

void Lil::Editor::DrawTarget() {
    BeginTextureMode(m_render_target);
        ClearBackground(RAYBLACK);
        
        BeginMode3D(m_camera);
            Lil::World().Draw();
            if (Lil::World().m_physics_debug) Lil::World().DebugDraw();
            if (m_selected) {
                Transform t = m_selected->GetTransform();
                DrawGizmo3D(m_gizmo_mode | m_gizmo_space, &t);
                m_selected->SetTransform(t);
            }
        EndMode3D();
    EndTextureMode();
}

void Lil::Editor::DrawInspector() {
    ImGui::Begin("Inspector");

    if (m_selected) {
        m_editor.SetCurrentObjectName(m_selected->GetTypeInfo().Name());
        m_editor.VisitObject(m_selected->GetTypeInfo(), m_selected);
        
        ImGui::Spacing();
        
        float availWidth = ImGui::GetContentRegionAvail().x;
        ImGui::Begin("Components");
        ImGui::SameLine(availWidth - 30.0f);
        
        if (ImGui::Button(ICON_FA_PLUS)) {
            ImGui::OpenPopup("AddComponentPopup");
        }
        ImGui::Separator();
        
        for (auto& component : m_selected->Components()) {            
            ImGui::PushID(component);
            if (ImGui::SmallButton(ICON_FA_TRASH)) {
                m_selected->DeattachComponent(component);
                Lil::World().DestroyComponent(component);
                ImGui::PopID();
                continue;
            }
            ImGui::SameLine();
            m_editor.SetCurrentObjectName(component->GetTypeInfo().Name());
            m_editor.VisitObject(component->GetTypeInfo(), component);
            
            ImGui::PopID();
        }
        
        if (ImGui::BeginPopup("AddComponentPopup")) {
            ImGui::Text("Available components:");
            ImGui::Separator();
            
            for (auto& [name, ti] : Lil::Reflection::Get().Types()) {
                if (ti->IsA<Component>() && *ti != TypeInfo::Get<Component>()) {
                    bool exists = false;
                    for (auto& component : m_selected->Components()) {
                        if (component->GetTypeInfo().Name() == name) {
                            exists = true;
                            break;
                        }
                    }
                    
                    if (!exists && ImGui::MenuItem(name.c_str())) {
                        Component* component = Lil::World().CreateComponent(ti);
                        if (component) {
                            m_selected->AttachComponent(component);
                        }                        
                        ImGui::CloseCurrentPopup();
                    }
                }
            }
            
            ImGui::EndPopup();
        }
        ImGui::End();
    }

    ImGui::End();
}

void Lil::Editor::Update() {
    if (!m_cursor_enabled) {UpdateCamera(&m_camera, CAMERA_FREE);}
    Lil::Engine::Get().Update();

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

void Lil::Editor::HandleViewportInput() {
    if (IsKeyPressed(TOGGLE_CURSOR_KEY)) {
        if (m_cursor_enabled) DisableCursor();
        else EnableCursor();
        m_cursor_enabled = !m_cursor_enabled;
    }
    
    if (IsKeyPressed(KEY_ESCAPE)) m_selected = nullptr;
    if (IsKeyPressed(KEY_ONE)) m_gizmo_mode = GIZMO_TRANSLATE;
    if (IsKeyPressed(KEY_TWO)) m_gizmo_mode = GIZMO_ROTATE;
    if (IsKeyPressed(KEY_THREE)) m_gizmo_mode = GIZMO_SCALE;

    if (m_cursor_enabled && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        Actor* pick = Lil::World().PickActor(GetMousePosition(), m_render_target.texture.width, m_render_target.texture.height, m_camera);
        if (pick) m_selected = pick;
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
    
    SetMouseOffset(-viewerTopLeft.x, -viewerTopLeft.y);

    ImGuiIO& io = ImGui::GetIO();
    if (ImGui::IsWindowHovered() || ImGui::IsWindowFocused() || !m_cursor_enabled) HandleViewportInput();

    TryResizeTarget(contentSize.x, contentSize.y);
    SetGizmoRenderSize(contentSize.x, contentSize.y);

    DrawTarget();
    rlImGuiImageRenderTexture(&m_render_target);
    
    SetMouseOffset(0, 0);
    
    ImGui::End();
}

void Lil::Editor::DrawResources() {
    ImGui::Begin("Models");
    if (ImGui::Button(ICON_FA_PLUS)) {
        const char* source = BrowseModel();
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
        const char* source = BrowseTexture();
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
        if (notif.lifetime < 0.5f) alpha = notif.lifetime / 0.5f;

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

        notif.lifetime -= GetFrameTime();
    }

    m_notifications.erase(
        std::remove_if(m_notifications.begin(), m_notifications.end(),
            [](const Notification& n) { return n.lifetime <= 0.0f; }),
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
    DrawMenuBar();

    ImGui::Begin("Creation");
    for (auto& [name, ti] : Lil::Reflection::Get().Types()) {
        if (ti->IsA<Actor>()) {
            const char* typeName = name.c_str();

            if (ImGui::Selectable(typeName, false, ImGuiSelectableFlags_AllowDoubleClick)) {
                if (ImGui::IsMouseDoubleClicked(0)) {
                    Actor* actor = Lil::World().CreateActor(ti);
                    if (actor) {
                        m_selected = actor;
                    }
                }
            }
        }        
    }
    ImGui::End();

    ImGui::Begin("Actors");
    for (auto& [id, actor] : Lil::World().Actors()) {
        std::string typeName = actor->GetTypeInfo().Name();

        bool isSelected = (m_selected == actor.get());
        if (ImGui::Selectable((typeName+"__"+actor->GetIDString()).c_str(), isSelected)) {
            m_selected = actor.get();
        }
    }
    ImGui::End();
    
    DrawNotifications();

    rlImGuiEnd();
}
