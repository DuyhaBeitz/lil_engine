#pragma once

#include "LilEngine.hpp"

// DPI scaling functions
inline float ScaleToDPIF(float value) {
	return GetWindowScaleDPI().x * value;
}

inline int ScaleToDPII(int value) {
    return int(GetWindowScaleDPI().x * value);
}
#pragma once
#include <imgui.h>
#include <string>
#include <vector>

namespace Lil {
    class UIStyle {
    public:
        // ==========================================
        // Sizing & Layout Constraints
        // ==========================================
        static constexpr float PROPERTY_LABEL_COLUMN_WIDTH_PCT = 0.35f;
        static constexpr float STRUCT_INDENT_PADDING           = 10.0f;
        
        // Tab & Docking Hierarchy Design Tokens
        static inline const ImVec4 COLOR_TAB                 = ImVec4(0.11f, 0.11f, 0.11f, 1.0f);
        static inline const ImVec4 COLOR_TAB_HOVERED         = ImVec4(0.18f, 0.18f, 0.18f, 1.0f);
        static inline const ImVec4 COLOR_TAB_SELECTED        = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
        static inline const ImVec4 COLOR_TAB_DIMMED          = ImVec4(0.11f, 0.11f, 0.11f, 1.0f);
        static inline const ImVec4 COLOR_TAB_DIMMED_SELECTED = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);

        // The ghost bounding preview container displayed while dragging/docking a panel
        static inline const ImVec4 COLOR_DOCKING_PREVIEW     = ImVec4(0.25f, 0.25f, 0.25f, 0.60f);

        // Corner Rounding Constants (UE5 Flat Theme)
        static constexpr float ROUNDING_FRAME                  = 2.0f;
        static constexpr float ROUNDING_WINDOW                 = 4.0f;
        static constexpr float ROUNDING_GRAB                   = 2.0f;
        static constexpr float ROUNDING_POPUP                  = 2.0f;
        static constexpr float BORDER_SIZE_FRAME               = 1.0f;

        static inline const ImVec2 ITEM_SPACING = ImVec2(8.0f, 4.0f);

        // ==========================================
        // Dark Panel Themes
        // ==========================================
        static inline const ImVec4 COLOR_LABEL            = ImVec4(0.70f, 0.70f, 0.70f, 1.0f);
        static inline const ImVec4 COLOR_INPUT_BG         = ImVec4(0.09f, 0.09f, 0.09f, 1.0f);
        static inline const ImVec4 COLOR_INPUT_BG_HOVER   = ImVec4(0.13f, 0.13f, 0.13f, 1.0f);
        static inline const ImVec4 COLOR_INPUT_BG_ACTIVE  = ImVec4(0.16f, 0.16f, 0.16f, 1.0f);
        static inline const ImVec4 COLOR_COMBO_BG         = ImVec4(0.14f, 0.14f, 0.14f, 1.0f);
        static inline const ImVec4 COLOR_HEADER_BG        = ImVec4(0.18f, 0.18f, 0.18f, 1.0f);
        static inline const ImVec4 COLOR_HEADER_BG_HOVER  = ImVec4(0.24f, 0.24f, 0.24f, 1.0f);
        static inline const ImVec4 COLOR_CONST_VAL        = ImVec4(0.50f, 0.50f, 0.50f, 1.0f);
        
        // Window Background & Border Overrides
        static inline const ImVec4 COLOR_WINDOW_BG        = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
        static inline const ImVec4 COLOR_POPUP_BG         = ImVec4(0.12f, 0.12f, 0.12f, 1.0f);
        static inline const ImVec4 COLOR_BORDER           = ImVec4(0.22f, 0.22f, 0.22f, 1.0f);

        // Window Title Bar (Header) Styling Tokens
        static inline const ImVec4 COLOR_TITLE_BG         = ImVec4(0.11f, 0.11f, 0.11f, 1.0f);
        static inline const ImVec4 COLOR_TITLE_BG_ACTIVE  = ImVec4(0.11f, 0.11f, 0.11f, 1.0f);
        static inline const ImVec4 COLOR_TITLE_BG_COLLAPSED = ImVec4(0.11f, 0.11f, 0.11f, 0.50f);

        // General Interactive Button Theme Tokens
        static inline const ImVec4 COLOR_BUTTON           = ImVec4(0.24f, 0.24f, 0.24f, 1.0f);
        static inline const ImVec4 COLOR_BUTTON_HOVER     = ImVec4(0.32f, 0.32f, 0.32f, 1.0f);
        static inline const ImVec4 COLOR_BUTTON_ACTIVE    = ImVec4(0.20f, 0.20f, 0.20f, 1.0f);

        // ==========================================
        // Visual Type Tints
        // ==========================================

        // Visual Indicator for Constant (Read-Only) Fields
        static inline const ImVec4 COLOR_CONST_BADGE      = ImVec4(0.30f, 0.30f, 0.30f, 1.0f);
        static inline const ImVec4 COLOR_CONST_BADGE_TXT  = ImVec4(0.60f, 0.60f, 0.60f, 1.0f);

        static inline const ImVec4 COLOR_TYPE_BOOL        = ImVec4(0.70f, 0.00f, 0.00f, 1.0f); 
        static inline const ImVec4 COLOR_TYPE_NUMBER      = ImVec4(0.35f, 0.65f, 0.95f, 1.0f); 
        static inline const ImVec4 COLOR_TYPE_STRING      = ImVec4(0.85f, 0.45f, 0.10f, 1.0f); 
        
        // Axis Color codes
        static inline const ImVec4 COLOR_AXIS_X           = ImVec4(0.95f, 0.20f, 0.20f, 1.0f); 
        static inline const ImVec4 COLOR_AXIS_Y           = ImVec4(0.50f, 0.80f, 0.10f, 1.0f); 
        static inline const ImVec4 COLOR_AXIS_Z           = ImVec4(0.15f, 0.55f, 0.95f, 1.0f); 
        static inline const ImVec4 COLOR_AXIS_W           = ImVec4(0.90f, 0.90f, 0.90f, 1.0f);

        // Input Modifiers
        static constexpr float DRAG_FLOAT_SPEED           = 0.1f;
        static constexpr float DRAG_FLOAT_MIN             = 0.0f;
        static constexpr float DRAG_FLOAT_MAX             = 0.0f;
        static constexpr size_t STRING_BUFFER_SIZE        = 256;

        // ==========================================
        // Global Theme Engine Initialization
        // ==========================================
        static void InitGlobalTheme() {
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

        // ==========================================
        // Layout and Global Styling API
        // ==========================================
        static void PushRowSpacing() {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ITEM_SPACING);
        }

        static void PopRowSpacing() {
            ImGui::PopStyleVar();
        }

        static void BeginPropertyRow(const std::string& name, const ImVec4& typeColorAccent) {
            ImGui::Columns(2, nullptr, false);
            
            static bool widthSet = false;
            if (!widthSet) {
                ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() * PROPERTY_LABEL_COLUMN_WIDTH_PCT);
                widthSet = true;
            }

            ImGui::PushStyleColor(ImGuiCol_Text, typeColorAccent);
            ImGui::TextUnformatted("|");
            ImGui::PopStyleColor();
            ImGui::SameLine();

            ImGui::PushStyleColor(ImGuiCol_Text, COLOR_LABEL);
            ImGui::TextUnformatted(name.c_str());
            ImGui::PopStyleColor();
            
            ImGui::NextColumn();
            ImGui::PushItemWidth(-1.0f);
        }

        static void EndPropertyRow() {
            ImGui::PopItemWidth();
            ImGui::Columns(1);
        }

        static ImVec4 GetNumericColor(const std::string& name) {
            if (name == "x" || name == "X") return COLOR_AXIS_X;
            if (name == "y" || name == "Y") return COLOR_AXIS_Y;
            if (name == "z" || name == "Z") return COLOR_AXIS_Z;
            if (name == "w" || name == "W") return COLOR_AXIS_W;
            return COLOR_TYPE_NUMBER;
        }

        // ==========================================
        // Reusable Editable Draw Elements
        // ==========================================
        static void DrawBoolField(const std::string& name, bool* value) {
            BeginPropertyRow(name, COLOR_TYPE_BOOL);
            ImGui::PushStyleColor(ImGuiCol_FrameBg, COLOR_INPUT_BG);
            ImGui::PushStyleColor(ImGuiCol_CheckMark, COLOR_TYPE_BOOL);
            
            ImGui::Checkbox(("##" + name).c_str(), value);
            
            ImGui::PopStyleColor(2);
            EndPropertyRow();
        }

        static void DrawIntField(const std::string& name, int* value) {
            ImVec4 fieldColor = GetNumericColor(name);
            BeginPropertyRow(name, fieldColor);
            ImGui::PushStyleColor(ImGuiCol_FrameBg, COLOR_INPUT_BG);
            ImGui::PushStyleColor(ImGuiCol_Text, fieldColor);
            
            ImGui::InputInt(("##" + name).c_str(), value, 0, 0);
            
            ImGui::PopStyleColor(2);
            EndPropertyRow();
        }

        static void DrawUInt32Field(const std::string& name, uint32_t* value) {
            BeginPropertyRow(name, COLOR_TYPE_NUMBER);
            ImGui::PushStyleColor(ImGuiCol_FrameBg, COLOR_INPUT_BG);
            ImGui::PushStyleColor(ImGuiCol_Text, COLOR_TYPE_NUMBER);
            
            ImGui::InputScalar(("##" + name).c_str(), ImGuiDataType_U32, value, nullptr, nullptr, "%u");
            
            ImGui::PopStyleColor(2);
            EndPropertyRow();
        }

        static void DrawFloatField(const std::string& name, float* value) {
            ImVec4 fieldColor = GetNumericColor(name);
            BeginPropertyRow(name, fieldColor);
            ImGui::PushStyleColor(ImGuiCol_FrameBg, COLOR_INPUT_BG);
            ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, COLOR_INPUT_BG_HOVER);
            ImGui::PushStyleColor(ImGuiCol_Text, fieldColor);
            
            ImGui::DragFloat(("##" + name).c_str(), value, DRAG_FLOAT_SPEED, DRAG_FLOAT_MIN, DRAG_FLOAT_MAX, "%.3f");
            
            ImGui::PopStyleColor(3);
            EndPropertyRow();
        }

        static void DrawStringField(const std::string& name, std::string* value) {
            BeginPropertyRow(name, COLOR_TYPE_STRING);
            ImGui::PushStyleColor(ImGuiCol_FrameBg, COLOR_INPUT_BG);
            ImGui::PushStyleColor(ImGuiCol_Text, COLOR_TYPE_STRING);
            
            char buffer[STRING_BUFFER_SIZE];
            strncpy(buffer, value->c_str(), sizeof(buffer) - 1);
            buffer[sizeof(buffer) - 1] = '\0';
            
            if (ImGui::InputText(("##" + name).c_str(), buffer, sizeof(buffer))) {
                *value = buffer;
            }
            
            ImGui::PopStyleColor(2);
            EndPropertyRow();
        }

        // ==========================================
        // Reusable Read-Only (Const) Draw Elements
        // ==========================================
        static void DrawConstIndicator() {
            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Button, COLOR_CONST_BADGE);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, COLOR_CONST_BADGE);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, COLOR_CONST_BADGE);
            ImGui::PushStyleColor(ImGuiCol_Text, COLOR_CONST_BADGE_TXT);
            
            // Tiny, flat, non-interactive visual tag next to read-only parameters
            ImGui::SmallButton("CONST");
            
            ImGui::PopStyleColor(4);
        }

        static void DrawConstBoolField(const std::string& name, const bool* value) {
            BeginPropertyRow(name, COLOR_TYPE_BOOL);
            ImGui::TextColored(COLOR_CONST_VAL, *value ? "True" : "False");
            DrawConstIndicator();
            EndPropertyRow();
        }

        static void DrawConstIntField(const std::string& name, const int* value) {
            BeginPropertyRow(name, GetNumericColor(name));
            ImGui::TextColored(COLOR_CONST_VAL, "%d", *value);
            DrawConstIndicator();
            EndPropertyRow();
        }

        static void DrawConstUInt32Field(const std::string& name, const uint32_t* value) {
            BeginPropertyRow(name, COLOR_TYPE_NUMBER);
            ImGui::TextColored(COLOR_CONST_VAL, "%u", *value);
            DrawConstIndicator();
            EndPropertyRow();
        }

        static void DrawConstFloatField(const std::string& name, const float* value) {
            BeginPropertyRow(name, GetNumericColor(name));
            ImGui::TextColored(COLOR_CONST_VAL, "%.3f", *value);
            DrawConstIndicator();
            EndPropertyRow();
        }

        static void DrawConstStringField(const std::string& name, const std::string* value) {
            BeginPropertyRow(name, COLOR_TYPE_STRING);
            ImGui::TextColored(COLOR_CONST_VAL, "%s", value->c_str());
            DrawConstIndicator();
            EndPropertyRow();
        }
    };
};

class EditorUIVisitor : public IFieldVisitor {
public:
    bool Visit(const FieldInfo& field, void* ptr) override {
        ImGui::PushID(reinterpret_cast<intptr_t>(ptr));
        ImGui::PushID(field.name.c_str());

        Lil::UIStyle::PushRowSpacing();

        if (field.HasAttribute("ModelKeyAttribute")) {
            // Keep specialized resource combo inline or call generic layout row strings
            Lil::UIStyle::BeginPropertyRow(field.name, Lil::UIStyle::COLOR_TYPE_STRING);
            ImGui::PushStyleColor(ImGuiCol_FrameBg, Lil::UIStyle::COLOR_COMBO_BG);
            
            auto* p = static_cast<std::string*>(ptr);
            const auto& models = Lil::Resources().Models();
            int currentIndex = -1;
            std::vector<std::string> modelKeys;
            for (const auto& [key, model] : *models) {
                modelKeys.push_back(key);
                if (key == *p) currentIndex = modelKeys.size() - 1;
            }
           
            if (ImGui::BeginCombo(("##" + field.name).c_str(), (*p).c_str())) {
                for (size_t i = 0; i < modelKeys.size(); i++) {
                    bool isSelected = (static_cast<int>(i) == currentIndex);
                    if (ImGui::Selectable(modelKeys[i].c_str(), isSelected)) *p = modelKeys[i];
                    if (isSelected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            ImGui::PopStyleColor();
            Lil::UIStyle::EndPropertyRow();
        }
        else if (field.type == TypeInfo::Get<bool>()) {
            Lil::UIStyle::DrawBoolField(field.name, static_cast<bool*>(ptr));
        }
        else if (field.type == TypeInfo::Get<int>()) {
            Lil::UIStyle::DrawIntField(field.name, static_cast<int*>(ptr));
        }
        else if (field.type == TypeInfo::Get<uint32_t>()) {
            Lil::UIStyle::DrawUInt32Field(field.name, static_cast<uint32_t*>(ptr));
        }
        else if (field.type == TypeInfo::Get<float>()) {
            Lil::UIStyle::DrawFloatField(field.name, static_cast<float*>(ptr));
        }
        else if (field.type == TypeInfo::Get<std::string>()) {
            Lil::UIStyle::DrawStringField(field.name, static_cast<std::string*>(ptr));
        }
        else {
            ImGui::PushStyleColor(ImGuiCol_Header, Lil::UIStyle::COLOR_HEADER_BG);
            ImGui::PushStyleColor(ImGuiCol_HeaderHovered, Lil::UIStyle::COLOR_HEADER_BG_HOVER);
            
            if (ImGui::CollapsingHeader(field.name.c_str(), 0)) {
                ImGui::PopStyleColor(2);
                ImGui::Indent(Lil::UIStyle::STRUCT_INDENT_PADDING);
                field.type.VisitFields(ptr, *this);
                ImGui::Unindent(Lil::UIStyle::STRUCT_INDENT_PADDING);
            } else {
                ImGui::PopStyleColor(2);
            }
        }

        Lil::UIStyle::PopRowSpacing();
        ImGui::PopID();
        ImGui::PopID();
        return true;
    }

    bool VisitConst(const FieldInfo& field, const void* ptr) override {
        ImGui::PushID(reinterpret_cast<intptr_t>(ptr));
        ImGui::PushID(field.name.c_str());
        
        ImGui::PushStyleColor(ImGuiCol_FrameBg, Lil::UIStyle::COLOR_COMBO_BG);
        Lil::UIStyle::PushRowSpacing();
        ImGui::BeginDisabled();

        if (field.type == TypeInfo::Get<bool>()) {
            Lil::UIStyle::DrawConstBoolField(field.name, static_cast<const bool*>(ptr));
        }
        else if (field.type == TypeInfo::Get<int>()) {
            Lil::UIStyle::DrawConstIntField(field.name, static_cast<const int*>(ptr));
        }
        else if (field.type == TypeInfo::Get<uint32_t>()) {
            Lil::UIStyle::DrawConstUInt32Field(field.name, static_cast<const uint32_t*>(ptr));
        }
        else if (field.type == TypeInfo::Get<float>()) {
            Lil::UIStyle::DrawConstFloatField(field.name, static_cast<const float*>(ptr));
        }
        else if (field.type == TypeInfo::Get<std::string>()) {
            Lil::UIStyle::DrawConstStringField(field.name, static_cast<const std::string*>(ptr));
        }
        else {
            ImGui::PushStyleColor(ImGuiCol_Header, Lil::UIStyle::COLOR_COMBO_BG);
            ImGui::CollapsingHeader(field.name.c_str(), ImGuiTreeNodeFlags_Leaf);
            ImGui::PopStyleColor();
        }

        ImGui::EndDisabled();
        Lil::UIStyle::PopRowSpacing();
        ImGui::PopStyleColor();
        ImGui::PopID();
        ImGui::PopID();
        return true;
    }
};