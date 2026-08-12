#include "EditorUI.hpp"

Lil::RowSpacingGuard::RowSpacingGuard() { UIStyle::PushRowSpacing(); }
Lil::RowSpacingGuard::~RowSpacingGuard() { UIStyle::PopRowSpacing(); }

void Lil::UIStyle::InitGlobalTheme() {
    ImGuiStyle& style = ImGui::GetStyle();
    
    style.FrameRounding    = ROUNDING_FRAME;
    style.WindowRounding   = ROUNDING_WINDOW;
    style.GrabRounding     = ROUNDING_GRAB;
    style.PopupRounding    = ROUNDING_POPUP;
    style.FrameBorderSize  = BORDER_SIZE_FRAME;

    style.Colors[ImGuiCol_TitleBg]           = COLOR_TITLE_BG;
    style.Colors[ImGuiCol_TitleBgActive]     = COLOR_TITLE_BG_ACTIVE;
    style.Colors[ImGuiCol_TitleBgCollapsed]  = COLOR_TITLE_BG_COLLAPSED;

    style.Colors[ImGuiCol_DockingPreview]        = COLOR_DOCKING_PREVIEW;
    style.Colors[ImGuiCol_Tab]                   = COLOR_TAB;
    style.Colors[ImGuiCol_TabHovered]            = COLOR_TAB_HOVERED;
    style.Colors[ImGuiCol_TabActive]             = COLOR_TAB_SELECTED;
    style.Colors[ImGuiCol_TabUnfocused]          = COLOR_TAB_DIMMED;
    style.Colors[ImGuiCol_TabUnfocusedActive]   = COLOR_TAB_DIMMED_SELECTED;
    style.Colors[ImGuiCol_TabSelectedOverline]   = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    style.Colors[ImGuiCol_TabDimmedSelectedOverline] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    
    style.Colors[ImGuiCol_WindowBg]          = COLOR_WINDOW_BG;
    style.Colors[ImGuiCol_PopupBg]           = COLOR_POPUP_BG;
    style.Colors[ImGuiCol_Border]            = COLOR_BORDER;

    style.Colors[ImGuiCol_TitleBg]          = COLOR_TITLE_BG;
    style.Colors[ImGuiCol_TitleBgActive]    = COLOR_TITLE_BG_ACTIVE;
    style.Colors[ImGuiCol_TitleBgCollapsed] = COLOR_TITLE_BG_COLLAPSED;

    style.Colors[ImGuiCol_FrameBg]           = COLOR_INPUT_BG;
    style.Colors[ImGuiCol_FrameBgHovered]    = COLOR_INPUT_BG_HOVER;
    style.Colors[ImGuiCol_FrameBgActive]     = COLOR_INPUT_BG_ACTIVE;

    style.Colors[ImGuiCol_Header]            = COLOR_HEADER_BG;
    style.Colors[ImGuiCol_HeaderHovered]     = COLOR_HEADER_BG_HOVER;
    style.Colors[ImGuiCol_HeaderActive]      = COLOR_HEADER_BG;

    style.Colors[ImGuiCol_Button]            = COLOR_BUTTON;
    style.Colors[ImGuiCol_ButtonHovered]     = COLOR_BUTTON_HOVER;
    style.Colors[ImGuiCol_ButtonActive]      = COLOR_BUTTON_ACTIVE;
}

void Lil::UIStyle::PushRowSpacing() {ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ITEM_SPACING);}
void Lil::UIStyle::PopRowSpacing(){ImGui::PopStyleVar();}

void Lil::UIStyle::BeginPropertyRow(const std::string &name, const ImVec4 &typeColorAccent, const char *fontAwesomeIcon) {
    ImGui::Columns(2, nullptr, false);
    
    static bool widthSet = false;
    if (!widthSet) {
        ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() * PROPERTY_LABEL_COLUMN_WIDTH_PCT);
        widthSet = true;
    }

    // Leftmost Type Strip Accent
    ImGui::PushStyleColor(ImGuiCol_Text, typeColorAccent);
    ImGui::TextUnformatted("|");
    ImGui::PopStyleColor();
    ImGui::SameLine();

    // Inline Icon Injection (if provided)
    if (fontAwesomeIcon != nullptr) {
        ImGui::PushStyleColor(ImGuiCol_Text, typeColorAccent);
        ImGui::TextUnformatted(fontAwesomeIcon);
        ImGui::PopStyleColor();
        ImGui::SameLine(0.0f, ICON_LABEL_GAP_PADDING);
    }

    // Property Label
    ImGui::PushStyleColor(ImGuiCol_Text, COLOR_LABEL);
    ImGui::TextUnformatted(name.c_str());
    ImGui::PopStyleColor();
    
    ImGui::NextColumn();
    ImGui::PushItemWidth(-1.0f);
}

void Lil::UIStyle::EndPropertyRow() {
    ImGui::PopItemWidth();
    ImGui::Columns(1);
}

ImVec4 Lil::UIStyle::GetNumericColor(const std::string &name) {
    if (name == "x" || name == "X") return COLOR_AXIS_X;
    if (name == "y" || name == "Y") return COLOR_AXIS_Y;
    if (name == "z" || name == "Z") return COLOR_AXIS_Z;
    if (name == "w" || name == "W") return COLOR_AXIS_W;
    return COLOR_TYPE_NUMBER;
}

bool Lil::UIStyle::DrawAttributeEnumCombo(const std::string &name, int *value, const EnumAttribute &attr) {
    std::vector<int> values;
    values.reserve(attr.count);
    for (int v : attr.get_values()) {
        values.push_back(v);
    }

    std::vector<const char*> labels;
    labels.reserve(attr.count);
    for (std::string_view sv : attr.get_labels()) {
        labels.push_back(sv.data());
    }

    return DrawEnumComboField<int>(name, value, values, labels);
}

bool Lil::UIStyle::DrawBoolField(const std::string &name, bool *value)
{
    BeginPropertyRow(name, COLOR_TYPE_BOOL);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, COLOR_INPUT_BG);
    ImGui::PushStyleColor(ImGuiCol_CheckMark, COLOR_TYPE_BOOL);
    
    bool res = ImGui::Checkbox(("##" + name).c_str(), value);
    
    ImGui::PopStyleColor(2);
    EndPropertyRow();

    return res;
}

bool Lil::UIStyle::DrawIntField(const std::string &name, int *value) {
    ImVec4 fieldColor = GetNumericColor(name);
    BeginPropertyRow(name, fieldColor);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, COLOR_INPUT_BG);
    ImGui::PushStyleColor(ImGuiCol_Text, fieldColor);
    
    bool res = ImGui::InputInt(("##" + name).c_str(), value, 0, 0);
    
    ImGui::PopStyleColor(2);
    EndPropertyRow();

    return res;
}

bool Lil::UIStyle::DrawUInt32Field(const std::string &name, uint32_t *value) {
    BeginPropertyRow(name, COLOR_TYPE_NUMBER);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, COLOR_INPUT_BG);
    ImGui::PushStyleColor(ImGuiCol_Text, COLOR_TYPE_NUMBER);
    
    bool res = ImGui::InputScalar(("##" + name).c_str(), ImGuiDataType_U32, value, nullptr, nullptr, "%u");
    
    ImGui::PopStyleColor(2);
    EndPropertyRow();
    return res;
}

bool Lil::UIStyle::DrawFloatField(const std::string &name, float *value) {
    ImVec4 fieldColor = GetNumericColor(name);
    BeginPropertyRow(name, fieldColor);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, COLOR_INPUT_BG);
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, COLOR_INPUT_BG_HOVER);
    ImGui::PushStyleColor(ImGuiCol_Text, fieldColor);
    
    bool res = ImGui::DragFloat(("##" + name).c_str(), value, DRAG_FLOAT_SPEED, DRAG_FLOAT_MIN, DRAG_FLOAT_MAX, "%.3f");
    
    ImGui::PopStyleColor(3);
    EndPropertyRow();
    return res;
}

bool Lil::UIStyle::DrawStringField(const std::string &name, std::string *value) {
    BeginPropertyRow(name, COLOR_TYPE_STRING);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, COLOR_INPUT_BG);
    ImGui::PushStyleColor(ImGuiCol_Text, COLOR_TYPE_STRING);
    
    char buffer[STRING_BUFFER_SIZE];
    strncpy(buffer, value->c_str(), sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';
    
    bool res = ImGui::InputText(("##" + name).c_str(), buffer, sizeof(buffer));
    if (res) *value = buffer;
    
    ImGui::PopStyleColor(2);
    EndPropertyRow();

    return res;
}

bool Lil::UIStyle::DrawVector2Field(const std::string &name, ::Vector2 *values) {
    BeginPropertyRow(name, COLOR_TYPE_NUMBER);
    ImGui::PopItemWidth(); 
    ImGui::PushID(values);

    float totalWidthAvailable = ImGui::GetContentRegionAvail().x;
    float singleItemWidth = (totalWidthAvailable - VECTOR_ITEM_PADDING) / 2.0f; 

    const char* labels[] = { "X", "Y" };
    ImVec4 colors[]      = { COLOR_AXIS_X, COLOR_AXIS_Y };
    float* dataRefs[]    = { &values->x, &values->y };

    ImGui::PushStyleColor(ImGuiCol_FrameBg, COLOR_INPUT_BG);
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, COLOR_INPUT_BG_HOVER);
    ImGui::PushStyleColor(ImGuiCol_Text, COLOR_TEXT_WHITE);

    bool res = false;
    for (int i = 0; i < 2; ++i) {
        ImGui::SetNextItemWidth(singleItemWidth);
        
        // Render a clean, blank label so the field spans the full calculated space
        res |= ImGui::DragFloat((std::string("##Field") + labels[i]).c_str(), dataRefs[i], DRAG_FLOAT_SPEED, DRAG_FLOAT_MIN, DRAG_FLOAT_MAX, "%.2f");
        
        // Calculate the bounding box of the box we just rendered to draw our accent line
        ImVec2 minBound = ImGui::GetItemRectMin();
        ImVec2 maxBound = ImGui::GetItemRectMax();
        
        // Draw an absolute 3-pixel-wide colored line flush on the left border inside the field box
        maxBound.x = minBound.x + 3.0f; 
        ImGui::GetWindowDrawList()->AddRectFilled(minBound, maxBound, ImGui::ColorConvertFloat4ToU32(colors[i]), ROUNDING_FRAME);

        if (i < 1) ImGui::SameLine(0.0f, VECTOR_ITEM_PADDING);
    }

    ImGui::PopStyleColor(3);
    ImGui::PopID();
    ImGui::PushItemWidth(-1.0f);
    EndPropertyRow();

    return res;
}

bool Lil::UIStyle::DrawVector3Field(const std::string &name, ::Vector3 *values) {
    BeginPropertyRow(name, COLOR_TYPE_NUMBER);
    ImGui::PopItemWidth(); 
    ImGui::PushID(values);

    float totalWidthAvailable = ImGui::GetContentRegionAvail().x;
    float singleItemWidth = (totalWidthAvailable - (VECTOR_ITEM_PADDING * 2.0f)) / 3.0f; 

    const char* labels[] = { "X", "Y", "Z" };
    ImVec4 colors[]      = { COLOR_AXIS_X, COLOR_AXIS_Y, COLOR_AXIS_Z };
    float* dataRefs[]    = { &values->x, &values->y, &values->z };

    ImGui::PushStyleColor(ImGuiCol_FrameBg, COLOR_INPUT_BG);
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, COLOR_INPUT_BG_HOVER);
    ImGui::PushStyleColor(ImGuiCol_Text, COLOR_TEXT_WHITE);

    bool res = false;
    for (int i = 0; i < 3; ++i) {
        ImGui::SetNextItemWidth(singleItemWidth);
        res |= ImGui::DragFloat((std::string("##Field") + labels[i]).c_str(), dataRefs[i], DRAG_FLOAT_SPEED, DRAG_FLOAT_MIN, DRAG_FLOAT_MAX, "%.2f");
        
        ImVec2 minBound = ImGui::GetItemRectMin();
        ImVec2 maxBound = ImGui::GetItemRectMax();
        
        maxBound.x = minBound.x + 3.0f; 
        ImGui::GetWindowDrawList()->AddRectFilled(minBound, maxBound, ImGui::ColorConvertFloat4ToU32(colors[i]), ROUNDING_FRAME);

        if (i < 2) ImGui::SameLine(0.0f, VECTOR_ITEM_PADDING);
    }

    ImGui::PopStyleColor(3);
    ImGui::PopID();
    ImGui::PushItemWidth(-1.0f);
    EndPropertyRow();

    return res;
}

bool Lil::UIStyle::DrawVector4Field(const std::string &name, ::Vector4 *values) {
    BeginPropertyRow(name, COLOR_TYPE_NUMBER);
    ImGui::PopItemWidth(); 
    ImGui::PushID(values);

    float totalWidthAvailable = ImGui::GetContentRegionAvail().x;
    float singleItemWidth = (totalWidthAvailable - (VECTOR_ITEM_PADDING * 3.0f)) / 4.0f; 

    const char* labels[] = { "X", "Y", "Z", "W" };
    ImVec4 colors[]      = { COLOR_AXIS_X, COLOR_AXIS_Y, COLOR_AXIS_Z, COLOR_AXIS_W };
    float* dataRefs[]    = { &values->x, &values->y, &values->z, &values->w };

    ImGui::PushStyleColor(ImGuiCol_FrameBg, COLOR_INPUT_BG);
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, COLOR_INPUT_BG_HOVER);
    ImGui::PushStyleColor(ImGuiCol_Text, COLOR_TEXT_WHITE);

    bool res = false;
    for (int i = 0; i < 4; ++i) {
        ImGui::SetNextItemWidth(singleItemWidth);
        res |= ImGui::DragFloat((std::string("##Field") + labels[i]).c_str(), dataRefs[i], DRAG_FLOAT_SPEED, DRAG_FLOAT_MIN, DRAG_FLOAT_MAX, "%.2f");
        
        ImVec2 minBound = ImGui::GetItemRectMin();
        ImVec2 maxBound = ImGui::GetItemRectMax();
        
        maxBound.x = minBound.x + 3.0f; 
        ImGui::GetWindowDrawList()->AddRectFilled(minBound, maxBound, ImGui::ColorConvertFloat4ToU32(colors[i]), ROUNDING_FRAME);

        if (i < 3) ImGui::SameLine(0.0f, VECTOR_ITEM_PADDING);
    }

    ImGui::PopStyleColor(3);
    ImGui::PopID();
    ImGui::PushItemWidth(-1.0f);
    EndPropertyRow();

    return res;
}

bool Lil::UIStyle::DrawColorField(const std::string &name, ::Color *value) {
    BeginPropertyRow(name, COLOR_TYPE_STRING);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, COLOR_INPUT_BG);
    ImGui::PushStyleColor(ImGuiCol_CheckMark, COLOR_TYPE_BOOL);
    
    ImVec4 col (value->r/255.f, value->g/255.f, value->b/255.f, value->a/255.f);
    bool res = ImGui::ColorButton(("##" + name).c_str(), col);

    if (res) {
        ImGui::OpenPopup("ColorPickerPopup");
    }

    if (ImGui::BeginPopup("ColorPickerPopup")) {
        if (ImGui::ColorPicker4("##picker", &col.x)) {
            value->r = static_cast<unsigned char>(col.x * 255.0f);
            value->g = static_cast<unsigned char>(col.y * 255.0f);
            value->b = static_cast<unsigned char>(col.z * 255.0f);
            value->a = static_cast<unsigned char>(col.w * 255.0f);
        }
        ImGui::EndPopup();
    }
    
    ImGui::PopStyleColor(2);
    EndPropertyRow();

    return res;
}

bool Lil::UIStyle::DrawTransformBlock(const std::string &name, ::Transform *value) {
    RowSpacingGuard spacing;
    ImGui::PushID(value);

    ImGui::PushStyleColor(ImGuiCol_Header, COLOR_HEADER_BG);
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, COLOR_HEADER_BG_HOVER);
    
    // Injects a solid geometric node icon before the Transform section header text
    std::string compositeHeaderName = std::string(ICON_FA_ARROWS_UP_DOWN_LEFT_RIGHT) + "  " + name;
    bool isOpen = ImGui::CollapsingHeader(compositeHeaderName.c_str());
    ImGui::PopStyleColor(2);

    bool res = false;
    if (isOpen) {
        ImGui::Indent(STRUCT_INDENT_PADDING);
        
        res |= DrawVector3Field("Position", &value->translation);
        res |= DrawVector4Field("Rotation", &value->rotation);
        res |= DrawVector3Field("Scale",    &value->scale);
        
        ImGui::Unindent(STRUCT_INDENT_PADDING);
        ImGui::Spacing();
    }

    ImGui::PopID();
    return res;
}

bool Lil::UIStyle::DrawModelKeyField(const std::string &name, std::string *value) {
    Lil::UIStyle::BeginPropertyRow(name, Lil::UIStyle::COLOR_TYPE_STRING, ICON_FA_CAR);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, Lil::UIStyle::COLOR_COMBO_BG);
    
    int currentIndex = -1;
    std::vector<std::string> modelKeys;
    modelKeys.push_back("None");
    for (const auto& [key, model] : Lil::Resources().Models()) {
        modelKeys.push_back(key);
        if (key == *value) currentIndex = modelKeys.size() - 1;
    }

    bool res = false;
    if (ImGui::BeginCombo(("##" + name).c_str(), (*value).c_str())) {
        for (size_t i = 0; i < modelKeys.size(); i++) {
            bool isSelected = (static_cast<int>(i) == currentIndex);
            if (ImGui::Selectable(modelKeys[i].c_str(), isSelected)) {
                if (*value != modelKeys[i]) res = true;
                *value = modelKeys[i];
            }
            if (isSelected) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    ImGui::PopStyleColor();
    Lil::UIStyle::EndPropertyRow();

    return res;
}

bool Lil::UIStyle::DrawTextureKeyField(const std::string &name, std::string *value) {
    Lil::UIStyle::BeginPropertyRow(name, Lil::UIStyle::COLOR_TYPE_STRING, ICON_FA_MAP);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, Lil::UIStyle::COLOR_COMBO_BG);
    ImGui::PushID(value);
    
    int currentIndex = -1;
    std::vector<std::string> textureKeys;
    textureKeys.push_back("None");
    for (const auto& [key, texture] : Lil::Resources().Textures()) {
        textureKeys.push_back(key);
        if (key == *value) currentIndex = textureKeys.size() - 1;
    }

    bool res = false;
    if (ImGui::BeginCombo(("##" + name).c_str(), (*value).c_str())) {
        for (size_t i = 0; i < textureKeys.size(); i++) {
            bool isSelected = (static_cast<int>(i) == currentIndex);
            if (ImGui::Selectable(textureKeys[i].c_str(), isSelected)) {
                if (*value != textureKeys[i]) res = true;
                *value = textureKeys[i];
            }
            if (isSelected) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    ImGui::PopStyleColor();
    Lil::UIStyle::EndPropertyRow();
    ImGui::PopID();

    return res;
}

bool Lil::UIStyle::DrawCollisionShapeField(const std::string &name, CollisionShape *value) {
    if (value) {
        RowSpacingGuard spacing;
        ImGui::PushID(value);

        ImGui::PushStyleColor(ImGuiCol_Header, COLOR_HEADER_BG);
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, COLOR_HEADER_BG_HOVER);
        
        std::string compositeHeaderName = name;
        bool isOpen = ImGui::CollapsingHeader(compositeHeaderName.c_str());
        ImGui::PopStyleColor(2);

        bool res = false;
        if (isOpen) {
            ImGui::Indent(STRUCT_INDENT_PADDING);
            
            res |= DrawVector3Field("local position", &(value->m_local_position));
            res |= DrawVector4Field("local rotation", &(value->m_local_rotation));

            std::string labels[] = {"Sphere", "Box", "Heightmap"};
            ::CollisionShapeType types[] = {::CollisionShapeType::SPHERE, ::CollisionShapeType::BOX, ::CollisionShapeType::HEIGHTMAP};
            int currentIndex = -1;
            for (size_t i = 0; i < (int)::CollisionShapeType::COUNT; i++) {
                if (value->m_type == types[i]) {
                    currentIndex = i;
                    break;
                }
            }

            if (ImGui::BeginCombo(("##" + name).c_str(), labels[currentIndex].c_str())) {
                for (size_t i = 0; i < (int)::CollisionShapeType::COUNT; i++) {
                    bool isSelected = (static_cast<int>(i) == currentIndex);
                    if (ImGui::Selectable(labels[i].c_str(), isSelected)) {
                        if (value->m_type != types[i]) res = true;
                        value->m_type = types[i];
                    }
                    if (isSelected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }


            switch (value->m_type) {
            case ::CollisionShapeType::SPHERE:
                res |= DrawFloatField("radius", &(value->m_radius));
                break;
            
            case ::CollisionShapeType::BOX:
                res |= DrawVector3Field("half extends", &(value->m_half_extends));
                break;

            case ::CollisionShapeType::HEIGHTMAP:
                res |= DrawTextureKeyField("heightmap texture key", &(value->m_heightmap_texture_key));
                res |= DrawVector3Field("map size", &(value->m_map_size));
                break;

            default:
                break;
            }
            
            ImGui::Unindent(STRUCT_INDENT_PADDING);
            ImGui::Spacing();
        }

        ImGui::PopID();
        if (res) {
            value->m_needs_rebuild = true;
        }
        return res;
    }
    return false;
}

bool Lil::UIStyle::DrawBodyTypeField(const std::string &name, BodyType *value) {
    Lil::UIStyle::BeginPropertyRow(name, Lil::UIStyle::COLOR_TYPE_STRING, ICON_FA_MAP);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, Lil::UIStyle::COLOR_COMBO_BG);
    
    std::string labels[] = {"Static", "Dynamic", "Kinematic"};
    ::BodyType types[] = {::BodyType::STATIC, ::BodyType::DYNAMIC, ::BodyType::KINEMATIC};
    int currentIndex = -1;
    for (size_t i = 0; i < (int)::BodyType::COUNT; i++) {
        if (*value == types[i]) {
            currentIndex = i;
            break;
        }
    }

    bool res = false;
    if (ImGui::BeginCombo(("##" + name).c_str(), labels[currentIndex].c_str())) {
        for (size_t i = 0; i < (int)::CollisionShapeType::COUNT; i++) {
            bool isSelected = (static_cast<int>(i) == currentIndex);
            if (ImGui::Selectable(labels[i].c_str(), isSelected)) {
                if (*value != types[i]) res = true;
                *value = types[i];
            }
            if (isSelected) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    ImGui::PopStyleColor();
    Lil::UIStyle::EndPropertyRow();

    return res;
}

bool DrawTextureFieldToKey(std::string name, Texture2D* texture) {
    std::string texture_key = "None";
    for (auto& [key, t] : Lil::Resources().Textures()) {
        if (t.id != 0 && texture->id == t.id) {
            texture_key = key;
            break;
        }
    }
    bool res = Lil::UIStyle::DrawTextureKeyField(name, &texture_key);
    Texture2D* tex = Lil::Resources().GetTexture(texture_key);
    if (tex) *texture = *tex;
    return res;
}

bool Lil::UIStyle::DrawMaterialField(const std::string &name, R3D_Material *value) {
    ImGui::PushID(value);
    bool res = false;   

    res |= DrawVector2Field("UV scale", &(value->uvScale));
    res |= DrawVector2Field("UV offset", &(value->uvOffset));

    if (ImGui::CollapsingHeader(name.c_str())) {
        res |= DrawTextureFieldToKey("albedo", &(value->albedo.texture));
        res |= DrawColorField("albedo color", &(value->albedo.color));

        res |= DrawTextureFieldToKey("normal", &(value->normal.texture));
        res |= DrawFloatField("normal scale", &(value->normal.scale));

        res |= DrawTextureFieldToKey("emission", &(value->emission.texture));
        res |= DrawFloatField("emission energy", &(value->emission.energy));

        res |= DrawTextureFieldToKey("orm", &(value->orm.texture));
        res |= DrawFloatField("orm occlusion", &(value->orm.occlusion));
        res |= DrawFloatField("orm roughness", &(value->orm.roughness));
        res |= DrawFloatField("orm metalness", &(value->orm.metalness));
        res |= DrawFloatField("orm specular", &(value->orm.specular));
    }

    ImGui::PopID();
    return res;
}

bool Lil::UIStyle::DrawModelField(const std::string &name, R3D_Model *value) {
    ImGui::PushID(value);
    bool res = false;
    
    if (ImGui::CollapsingHeader(name.c_str())) {
        for (int i = 0; i < value->materialCount; i++) {
            res |= DrawMaterialField(TextFormat("material_%d", i), &(value->materials[i]));
        }
    }
    ImGui::PopID();

    return res;
}

void Lil::UIStyle::DrawConstBoolField(const std::string &name, const bool *value) {
    BeginPropertyRow(name + " " + ICON_FA_LOCK, COLOR_TYPE_BOOL);
    ImGui::TextColored(COLOR_CONST_VAL, *value ? "True" : "False");
    
    EndPropertyRow();
}

void Lil::UIStyle::DrawConstIntField(const std::string &name, const int *value) {
    BeginPropertyRow(name + " " + ICON_FA_LOCK, GetNumericColor(name));
    ImGui::TextColored(COLOR_CONST_VAL, "%d", *value);
    
    EndPropertyRow();
}

void Lil::UIStyle::DrawConstUInt32Field(const std::string &name, const uint32_t *value) {
    BeginPropertyRow(name + " " + ICON_FA_LOCK, COLOR_TYPE_NUMBER);
    ImGui::TextColored(COLOR_CONST_VAL, "%u", *value);
    
    EndPropertyRow();
}

void Lil::UIStyle::DrawConstFloatField(const std::string &name, const float *value) {
    BeginPropertyRow(name + " " + ICON_FA_LOCK, GetNumericColor(name));
    ImGui::TextColored(COLOR_CONST_VAL, "%.3f", *value);
    
    EndPropertyRow();
}

void Lil::UIStyle::DrawConstStringField(const std::string &name, const std::string *value) {
    BeginPropertyRow(name + " " + ICON_FA_LOCK, COLOR_TYPE_STRING);
    ImGui::TextColored(COLOR_CONST_VAL, "%s", value->c_str());
    
    EndPropertyRow();
}

void Lil::UIStyle::DrawConstVector2Field(const std::string &name, const ::Vector2 *values) {
    BeginPropertyRow(name + " " + ICON_FA_LOCK, COLOR_TYPE_NUMBER);
    ImGui::PopItemWidth(); 
    ImGui::PushID(values);

    float totalWidthAvailable = ImGui::GetContentRegionAvail().x;
    float singleItemWidth = (totalWidthAvailable - VECTOR_ITEM_PADDING) / 2.0f; 

    const char* labels[] = { "X", "Y" };
    ImVec4 colors[]      = { COLOR_AXIS_X, COLOR_AXIS_Y };
    float data[]         = { values->x, values->y };

    for (int i = 0; i < 2; ++i) {
        ImGui::BeginGroup();
        
        // Render a flat background container frame mimicking the input boxes
        ImVec2 startPos = ImGui::GetCursorScreenPos();
        ImVec2 endPos = ImVec2(startPos.x + singleItemWidth, startPos.y + ImGui::GetFrameHeight());
        ImGui::GetWindowDrawList()->AddRectFilled(startPos, endPos, ImGui::ColorConvertFloat4ToU32(COLOR_INPUT_BG), ROUNDING_FRAME);
        
        // Draw the colored left edge line inside the simulated box
        ImVec2 lineEnd = ImVec2(startPos.x + 3.0f, endPos.y);
        ImGui::GetWindowDrawList()->AddRectFilled(startPos, lineEnd, ImGui::ColorConvertFloat4ToU32(colors[i]), ROUNDING_FRAME);

        // Place the read-only text cleanly inside the container area
        ImGui::SetCursorScreenPos(ImVec2(startPos.x + 8.0f, startPos.y + ImGui::GetStyle().FramePadding.y));
        ImGui::TextColored(COLOR_CONST_VAL, "%.2f", data[i]);
        
        // Advance cursor position safely to handle group spacing mechanics
        ImGui::SetCursorScreenPos(startPos);
        ImGui::Dummy(ImVec2(singleItemWidth, ImGui::GetFrameHeight()));
        ImGui::EndGroup();

        if (i < 1) ImGui::SameLine(0.0f, VECTOR_ITEM_PADDING);
    }

    
    ImGui::PopID();
    ImGui::PushItemWidth(-1.0f);
    EndPropertyRow();
}

void Lil::UIStyle::DrawConstVector3Field(const std::string &name, const ::Vector3 *values) {
    BeginPropertyRow(name + " " + ICON_FA_LOCK, COLOR_TYPE_NUMBER);
    ImGui::PopItemWidth(); 
    ImGui::PushID(values);

    float totalWidthAvailable = ImGui::GetContentRegionAvail().x;
    float singleItemWidth = (totalWidthAvailable - (VECTOR_ITEM_PADDING * 2.0f)) / 3.0f; 

    const char* labels[] = { "X", "Y", "Z" };
    ImVec4 colors[]      = { COLOR_AXIS_X, COLOR_AXIS_Y, COLOR_AXIS_Z };
    float data[]         = { values->x, values->y, values->z };

    for (int i = 0; i < 3; ++i) {
        ImGui::BeginGroup();
        
        ImVec2 startPos = ImGui::GetCursorScreenPos();
        ImVec2 endPos = ImVec2(startPos.x + singleItemWidth, startPos.y + ImGui::GetFrameHeight());
        ImGui::GetWindowDrawList()->AddRectFilled(startPos, endPos, ImGui::ColorConvertFloat4ToU32(COLOR_INPUT_BG), ROUNDING_FRAME);
        
        ImVec2 lineEnd = ImVec2(startPos.x + 3.0f, endPos.y);
        ImGui::GetWindowDrawList()->AddRectFilled(startPos, lineEnd, ImGui::ColorConvertFloat4ToU32(colors[i]), ROUNDING_FRAME);

        ImGui::SetCursorScreenPos(ImVec2(startPos.x + 8.0f, startPos.y + ImGui::GetStyle().FramePadding.y));
        ImGui::TextColored(COLOR_CONST_VAL, "%.2f", data[i]);
        
        ImGui::SetCursorScreenPos(startPos);
        ImGui::Dummy(ImVec2(singleItemWidth, ImGui::GetFrameHeight()));
        ImGui::EndGroup();

        if (i < 2) ImGui::SameLine(0.0f, VECTOR_ITEM_PADDING);
    }

    
    ImGui::PopID();
    ImGui::PushItemWidth(-1.0f);
    EndPropertyRow();
}

void Lil::UIStyle::DrawConstVector4Field(const std::string &name, const ::Vector4 *values) {
    BeginPropertyRow(name + " " + ICON_FA_LOCK, COLOR_TYPE_NUMBER);
    ImGui::PopItemWidth(); 
    ImGui::PushID(values);

    float totalWidthAvailable = ImGui::GetContentRegionAvail().x;
    float singleItemWidth = (totalWidthAvailable - (VECTOR_ITEM_PADDING * 3.0f)) / 4.0f; 

    const char* labels[] = { "X", "Y", "Z", "W" };
    ImVec4 colors[]      = { COLOR_AXIS_X, COLOR_AXIS_Y, COLOR_AXIS_Z, COLOR_AXIS_W };
    float data[]         = { values->x, values->y, values->z, values->w };

    for (int i = 0; i < 4; ++i) {
        ImGui::BeginGroup();
        
        ImVec2 startPos = ImGui::GetCursorScreenPos();
        ImVec2 endPos = ImVec2(startPos.x + singleItemWidth, startPos.y + ImGui::GetFrameHeight());
        ImGui::GetWindowDrawList()->AddRectFilled(startPos, endPos, ImGui::ColorConvertFloat4ToU32(COLOR_INPUT_BG), ROUNDING_FRAME);
        
        ImVec2 lineEnd = ImVec2(startPos.x + 3.0f, endPos.y);
        ImGui::GetWindowDrawList()->AddRectFilled(startPos, lineEnd, ImGui::ColorConvertFloat4ToU32(colors[i]), ROUNDING_FRAME);

        ImGui::SetCursorScreenPos(ImVec2(startPos.x + 8.0f, startPos.y + ImGui::GetStyle().FramePadding.y));
        ImGui::TextColored(COLOR_CONST_VAL, "%.2f", data[i]);
        
        ImGui::SetCursorScreenPos(startPos);
        ImGui::Dummy(ImVec2(singleItemWidth, ImGui::GetFrameHeight()));
        ImGui::EndGroup();

        if (i < 3) ImGui::SameLine(0.0f, VECTOR_ITEM_PADDING);
    }

    
    ImGui::PopID();
    ImGui::PushItemWidth(-1.0f);
    EndPropertyRow();
}

void Lil::UIStyle::DrawConstColorField(const std::string &name, const ::Color *value) {
    BeginPropertyRow(name + " " + ICON_FA_LOCK, COLOR_TYPE_STRING);
    
    ImVec4 col (value->r/255.f, value->g/255.f, value->b/255.f, value->a/255.f);
    ImGui::ColorButton(("##" + name).c_str(), col);
    
    EndPropertyRow();
}

void Lil::UIStyle::DrawConstTransformBlock(const std::string &name, const ::Transform *value) {
    RowSpacingGuard spacing;
    ImGui::PushID(value);

    ImGui::PushStyleColor(ImGuiCol_Header, COLOR_HEADER_BG);
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, COLOR_HEADER_BG_HOVER);
    
    std::string compositeHeaderName = std::string(ICON_FA_ARROWS_UP_DOWN_LEFT_RIGHT) + "  " + name  + " " + ICON_FA_LOCK;
    bool isOpen = ImGui::CollapsingHeader(compositeHeaderName.c_str());
    ImGui::PopStyleColor(2);

    if (isOpen) {
        ImGui::Indent(STRUCT_INDENT_PADDING);
        
        DrawConstVector3Field("Position", &value->translation);
        DrawConstVector4Field("Rotation", &value->rotation);
        DrawConstVector3Field("Scale",    &value->scale);
        
        ImGui::Unindent(STRUCT_INDENT_PADDING);
        ImGui::Spacing();
    }

    ImGui::PopID();
}

void Lil::UIStyle::DrawConstModelKeyField(const std::string &name, const std::string *value) {
    BeginPropertyRow(name + " " + ICON_FA_LOCK, COLOR_TYPE_STRING, ICON_FA_CAR);
    ImGui::TextColored(COLOR_CONST_VAL, "%s", value->c_str());
    
    EndPropertyRow();
}

void Lil::UIStyle::DrawConstTextureKeyField(const std::string &name, const std::string *value) {
    BeginPropertyRow(name + " " + ICON_FA_LOCK, COLOR_TYPE_STRING, ICON_FA_MAP);
    ImGui::TextColored(COLOR_CONST_VAL, "%s", value->c_str());
    EndPropertyRow();
}
