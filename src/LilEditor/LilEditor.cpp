#include "LilEditor.hpp"
#include <utils/MeshHelper.hpp>
#include <utils/FileHelper.hpp>
#include "FileDialogHelper.hpp"

Lil::Editor &Lil::Editor::Get() {
    static Lil::Editor instance;
    return instance;
}

#define TOGGLE_CURSOR_KEY KEY_L
#define TOGGLE_SIMULATION_KEY KEY_F
#define TOGGLE_DEBUG_KEY KEY_V
#define TOGGLE_FULLSCREEN_KEY KEY_F11

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
        LOG_WARN("ImGui failed to load custom font! Falling back to default.");
    }

    io.FontDefault = customFont;
    
    rlImGuiEndInitImGui();
    Lil::UIStyle::InitGlobalTheme();
}

void Lil::Editor::LoadTarget(int w, int h) {
    m_render_target = LoadRenderTexture(w, h);
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
            Lil::GetWorld().Draw();
            if (Lil::GetWorld().m_physics_debug) Lil::GetWorld().DebugDraw();
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
            if (ImGui::SmallButton("X")) {
                m_selected->DeattachComponent(component);
                Lil::GetWorld().DestroyComponent(component);
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
                        Component* component = Lil::GetWorld().CreateComponent(ti);
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
    Lil::GetWorld().Update();

    if (IsKeyPressed(TOGGLE_SIMULATION_KEY)) {
        Lil::GetWorld().ToggleSimulationGoing();
    }
    if (IsKeyPressed(TOGGLE_DEBUG_KEY)) {
        Lil::GetWorld().m_physics_debug = !Lil::GetWorld().m_physics_debug;
    }

    if (IsKeyPressed(TOGGLE_FULLSCREEN_KEY)) {
        ToggleFullscreen();
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
        Actor* pick = Lil::GetWorld().PickActor(GetMousePosition(), m_render_target.texture.width, m_render_target.texture.height, m_camera);
        if (pick) m_selected = pick;
    }
}

void Lil::Editor::DrawViewport() {
    ImGui::Begin("Viewport");
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
            Lil::Resources().AddModel(NameFromPath(source), source);
            CopyAsset(source);
        }
    }

    for (auto& [key, model] : Lil::Resources().Models()) {
        ImGui::Text(key.c_str());
    }
    ImGui::End();


    ImGui::Begin("Textures");
    if (ImGui::Button(ICON_FA_PLUS)) {
        const char* source = BrowseTexture();
        if (source) {
            Lil::Resources().AddTexture(NameFromPath(source), source);
            CopyAsset(source);
        }
    }

    for (auto& [key, texture] : Lil::Resources().Textures()) {
        if (ImGui::CollapsingHeader(key.c_str())) {
            std::string heightmap_name = HeightmapNameFromImageName(key);
            if (!Lil::Resources().ModelExists(heightmap_name)) {
                if (ImGui::Button("Generate heightmap model")) {
                    Image image = LoadImageFromTexture(texture);
                    Lil::Resources().AddModel(heightmap_name, HeightmapModel(image, Vector3{1.0f, 1.0f, 1.0f}));
                    UnloadImage(image);
                }
            }
        }
    }
    ImGui::End();
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

    ImGui::Begin("Panel");
    if (m_gizmo_space == GIZMO_LOCAL) {
        if (ImGui::Button("global")) m_gizmo_space = GIZMO_DISABLED; 
    }
    else if (ImGui::Button("local")) m_gizmo_space = GIZMO_LOCAL; 

    ImGui::SameLine();
    if (ImGui::Button("save")) {
        std::ofstream os("out.json");
        ArchiveOut a_out(os);
        a_out(Lil::GetWorld());            
    }
    ImGui::SameLine();
    if (ImGui::Button("load")) {
        std::ifstream is("out.json");
        ArchiveIn a_in(is);
        a_in(Lil::GetWorld());
    }
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

    ImGui::SameLine();
    if (ImGui::BeginCombo("Render Mode", currentName.c_str()))
    {
        for (const auto& [name, mode] : renderModeMap)
        {
            bool isSelected = (mode == currentMode);
            if (ImGui::Selectable(name.c_str(), isSelected))
            {
                currentMode = mode;
                Lil::GetWorld().SetRenderMode(mode);
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    ImGui::End();

    ImGui::Begin("Actors");
    for (auto& [name, ti] : Lil::Reflection::Get().Types()) {
        if (ti->IsA<Actor>()) {
            const char* typeName = name.c_str();

            if (ImGui::Selectable(typeName, false, ImGuiSelectableFlags_AllowDoubleClick)) {
                if (ImGui::IsMouseDoubleClicked(0)) {
                    Actor* actor = Lil::GetWorld().CreateActor(ti);
                    if (actor) {
                        m_selected = actor;
                    }
                }
            }
        }        
    }
    ImGui::End();

    rlImGuiEnd();
}
