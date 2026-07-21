#pragma once

#include "LilEngine.hpp"
#include "extras/IconsFontAwesome6.h"

// DPI scaling functions
inline float ScaleToDPIF(float value) {
	return GetWindowScaleDPI().x * value;
}

inline int ScaleToDPII(int value) {
    return int(GetWindowScaleDPI().x * value);
}

namespace Lil {

    struct RowSpacingGuard {
        RowSpacingGuard();
        ~RowSpacingGuard();
    };

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

        // Corner Rounding Constants (Flat Theme)
        static constexpr float ROUNDING_FRAME                  = 2.0f;
        static constexpr float ROUNDING_WINDOW                 = 4.0f;
        static constexpr float ROUNDING_GRAB                   = 2.0f;
        static constexpr float ROUNDING_POPUP                  = 2.0f;
        static constexpr float BORDER_SIZE_FRAME               = 1.0f;

        static inline const ImVec2 ITEM_SPACING = ImVec2(8.0f, 4.0f);

        // Icon Spacing Configuration Tokens
        static constexpr float ICON_LABEL_GAP_PADDING = 6.0f;

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

        // Horizontal Vector Layout Tokens
        static constexpr float VECTOR_ITEM_PADDING   = 4.0f;
        static constexpr float VECTOR_LABEL_PADDING  = 3.0f;
        static inline const ImVec4 COLOR_TEXT_WHITE  = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

        // ==========================================
        // Global Theme Engine Initialization
        // ==========================================
        static void InitGlobalTheme();

        // ==========================================
        // Layout and Global Styling API
        // ==========================================
        static void PushRowSpacing();
        static void PopRowSpacing();

        static void BeginPropertyRow(const std::string& name, const ImVec4& typeColorAccent, const char* fontAwesomeIcon = nullptr);
        static void EndPropertyRow();

        static ImVec4 GetNumericColor(const std::string& name);

        // ==========================================
        // Reusable Editable Draw Elements
        // ==========================================
        static void DrawBoolField(const std::string& name, bool* value);
        static void DrawIntField(const std::string& name, int* value);
        static void DrawUInt32Field(const std::string& name, uint32_t* value);
        static void DrawFloatField(const std::string& name, float* value);
        static void DrawStringField(const std::string& name, std::string* value);
        static void DrawVector2Field(const std::string& name, ::Vector2* values);
        static void DrawVector3Field(const std::string& name, ::Vector3* values);
        static void DrawVector4Field(const std::string& name, ::Vector4* values);
        static void DrawTransformBlock(const std::string& sectionName, ::Transform* transform);
        
        // ==========================================
        // Reusable Read-Only (Const) Draw Elements
        // ==========================================

        static void DrawConstBoolField(const std::string& name, const bool* value);
        static void DrawConstIntField(const std::string& name, const int* value);
        static void DrawConstUInt32Field(const std::string& name, const uint32_t* value);
        static void DrawConstFloatField(const std::string& name, const float* value);
        static void DrawConstStringField(const std::string& name, const std::string* value);
        static void DrawConstVector2Field(const std::string& name, const ::Vector2* values);
        static void DrawConstVector3Field(const std::string& name, const ::Vector3* values);
        static void DrawConstVector4Field(const std::string& name, const ::Vector4* values);
        static void DrawConstTransformBlock(const std::string& sectionName, const ::Transform* transform);
    };
};

class EditorUIVisitor : public IFieldVisitor {
public:
    bool Visit(const FieldInfo& field, void* ptr) override {
        ImGui::PushID(reinterpret_cast<intptr_t>(ptr));
        ImGui::PushID(field.name.c_str());

        Lil::UIStyle::PushRowSpacing();

        if (field.HasAttribute("ModelKeyAttribute")) {
            Lil::UIStyle::BeginPropertyRow(field.name, Lil::UIStyle::COLOR_TYPE_STRING, ICON_FA_CAR);
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
        else if (field.type == TypeInfo::Get<Vector2>()) {
            Lil::UIStyle::DrawVector2Field(field.name, static_cast<Vector2*>(ptr));
        }
        else if (field.type == TypeInfo::Get<Vector3>()) {
            Lil::UIStyle::DrawVector3Field(field.name, static_cast<Vector3*>(ptr));
        }
        else if (field.type == TypeInfo::Get<Vector4>()) {
            Lil::UIStyle::DrawVector4Field(field.name, static_cast<Vector4*>(ptr));
        }
        else if (field.type == TypeInfo::Get<Transform>()) {
            Lil::UIStyle::DrawTransformBlock(field.name, static_cast<Transform*>(ptr));
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
        else if (field.type == TypeInfo::Get<Vector2>()) {
            Lil::UIStyle::DrawConstVector2Field(field.name, static_cast<const Vector2*>(ptr));
        }
        else if (field.type == TypeInfo::Get<Vector3>()) {
            Lil::UIStyle::DrawConstVector3Field(field.name, static_cast<const Vector3*>(ptr));
        }
        else if (field.type == TypeInfo::Get<Vector4>()) {
            Lil::UIStyle::DrawConstVector4Field(field.name, static_cast<const Vector4*>(ptr));
        }
        else if (field.type == TypeInfo::Get<Transform>()) {
            Lil::UIStyle::DrawConstTransformBlock(field.name, static_cast<const Transform*>(ptr));
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