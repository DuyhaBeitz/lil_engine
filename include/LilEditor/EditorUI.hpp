#pragma once

#include "LilEngine.hpp"
#include "Components/ColliderComponent.hpp"
#include "extras/IconsFontAwesome6.h"
#include <set>

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
        static void DrawTransformBlock(const std::string& name, ::Transform* value);
        static void DrawModelKeyField(const std::string& name, std::string* value);
        static void DrawTextureKeyField(const std::string& name, std::string* value);
        static void DrawCollisionShapeField(const std::string& name, CollisionShape* value);
        static void DrawBodyTypeField(const std::string& name, BodyType* value);

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
        static void DrawConstTransformBlock(const std::string& name, const ::Transform* value);
        static void DrawConstModelKeyField(const std::string& name, const std::string* value);
        static void DrawConstTextureKeyField(const std::string& name, const std::string* value);
    };
};

class EditorUIVisitor : public IVisitor {

    std::string m_object_name = "";

    void HandleField(const FieldInfo& field, void* ptr) {
        if (field.HasAttribute("ModelKeyAttribute")) {
            Lil::UIStyle::DrawModelKeyField(field.name, static_cast<std::string*>(field.GetPtr(ptr)));
        }
        else if (field.HasAttribute("TextureKeyAttribute")) {
            Lil::UIStyle::DrawTextureKeyField(field.name, static_cast<std::string*>(field.GetPtr(ptr)));
        }
        else {
            SetCurrentObjectName(field.name);
            if (field.type.IsConst()) {
                VisitObjectConst(field.type, field.GetPtrConst(ptr));
            }
            else {
                VisitObject(field.type, field.GetPtr(ptr));
            }
        }
    }

    // here THE OBJECT is const, the field only follows
    void HandleFieldConst(const FieldInfo& field, const void* ptr) {
        if (field.HasAttribute("ModelKeyAttribute")) {
            Lil::UIStyle::DrawConstModelKeyField(field.name, static_cast<const std::string*>(field.GetPtrConst(ptr)));
        }
        else if (field.HasAttribute("TextureKeyAttribute")) {
            Lil::UIStyle::DrawConstTextureKeyField(field.name, static_cast<const std::string*>(field.GetPtrConst(ptr)));
        }
        else {
            SetCurrentObjectName(field.name);
            VisitObjectConst(field.type, field.GetPtrConst(ptr));
        }
    }

public:
    
    void SetCurrentObjectName(std::string current_object_name) { m_object_name = current_object_name; }

    void VisitObject(const TypeInfo& ti, void* ptr) override {
        if (ptr) {
            ImGui::PushID(reinterpret_cast<intptr_t>(ptr));
            ImGui::PushID(m_object_name.c_str());
            Lil::UIStyle::PushRowSpacing();
            if (ti == TypeInfo::Get<bool>()) {
                Lil::UIStyle::DrawBoolField(m_object_name, static_cast<bool*>(ptr));
            }
            else if (ti == TypeInfo::Get<int>()) {
                Lil::UIStyle::DrawIntField(m_object_name, static_cast<int*>(ptr));
            }
            else if (ti == TypeInfo::Get<uint32_t>()) {
                Lil::UIStyle::DrawUInt32Field(m_object_name, static_cast<uint32_t*>(ptr));
            }
            else if (ti == TypeInfo::Get<float>()) {
                Lil::UIStyle::DrawFloatField(m_object_name, static_cast<float*>(ptr));
            }
            else if (ti == TypeInfo::Get<std::string>()) {
                Lil::UIStyle::DrawStringField(m_object_name, static_cast<std::string*>(ptr));
            }
            else if (ti == TypeInfo::Get<Vector2>()) {
                Lil::UIStyle::DrawVector2Field(m_object_name, static_cast<Vector2*>(ptr));
            }
            else if (ti == TypeInfo::Get<Vector3>()) {
                Lil::UIStyle::DrawVector3Field(m_object_name, static_cast<Vector3*>(ptr));
            }
            else if (ti == TypeInfo::Get<Vector4>()) {
                Lil::UIStyle::DrawVector4Field(m_object_name, static_cast<Vector4*>(ptr));
            }
            else if (ti == TypeInfo::Get<Transform>()) {
                Lil::UIStyle::DrawTransformBlock(m_object_name, static_cast<Transform*>(ptr));
            }
            else if (ti == TypeInfo::Get<CollisionShape>()) {
                Lil::UIStyle::DrawCollisionShapeField(m_object_name, static_cast<CollisionShape*>(ptr));
            }
            else if (ti == TypeInfo::Get<BodyType>()) {
                Lil::UIStyle::DrawBodyTypeField(m_object_name, static_cast<BodyType*>(ptr));
            }
            else if (ti.IsContainer()) {
                ImGui::PushStyleColor(ImGuiCol_Header, Lil::UIStyle::COLOR_HEADER_BG);
                ImGui::PushStyleColor(ImGuiCol_HeaderHovered, Lil::UIStyle::COLOR_HEADER_BG_HOVER);
                
                auto* c = ti.Container();
                if (ImGui::Button(ICON_FA_PLUS)) {
                    c->InsertDefault(ptr);
                }
                ImGui::SameLine();
                if (ImGui::CollapsingHeader(m_object_name.c_str(), 0)) {
                    ImGui::PopStyleColor(2);
                    ImGui::Indent(Lil::UIStyle::STRUCT_INDENT_PADDING);
                    size_t n = c->Size(ptr);

                    std::set<size_t> idx_to_erase = {};
                    c->ForEach(ptr, [c, &idx_to_erase, this](size_t i, void* element){
                        ImGui::PushID((int)i);
                        if (ImGui::Button(ICON_FA_TRASH)) {
                            idx_to_erase.insert(i);
                            return;
                        }
                        ImGui::SameLine();

                        SetCurrentObjectName(TextFormat("element %d", i));
                        VisitObject(c->ElementType(), element);
                        ImGui::PopID();
                    });
                    for (auto i : idx_to_erase) {
                        c->Erase(ptr, i);
                    }

                    ImGui::Unindent(Lil::UIStyle::STRUCT_INDENT_PADDING);
                } else {
                    ImGui::PopStyleColor(2);
                }
            }
            else {
                ImGui::PushStyleColor(ImGuiCol_Header, Lil::UIStyle::COLOR_HEADER_BG);
                ImGui::PushStyleColor(ImGuiCol_HeaderHovered, Lil::UIStyle::COLOR_HEADER_BG_HOVER);
                
                if (ImGui::CollapsingHeader(m_object_name.c_str(), 0)) {
                    ImGui::PopStyleColor(2);
                    ImGui::Indent(Lil::UIStyle::STRUCT_INDENT_PADDING);
                    for (auto& f : ti.Fields()) {
                        HandleField(f, ptr);
                    }
                    ImGui::Unindent(Lil::UIStyle::STRUCT_INDENT_PADDING);
                } else {
                    ImGui::PopStyleColor(2);
                }
            }

            Lil::UIStyle::PopRowSpacing();
            ImGui::PopID();
            ImGui::PopID();
        }
    }

    void VisitObjectConst(const TypeInfo& ti, const void* ptr) override {
        if (ptr) {
            ImGui::PushID(reinterpret_cast<intptr_t>(ptr));
            ImGui::PushID(m_object_name.c_str());
            Lil::UIStyle::PushRowSpacing();

            if (ti == TypeInfo::Get<bool>()) {
                Lil::UIStyle::DrawConstBoolField(m_object_name, static_cast<const bool*>(ptr));
            }
            else if (ti == TypeInfo::Get<int>()) {
                Lil::UIStyle::DrawConstIntField(m_object_name, static_cast<const int*>(ptr));
            }
            else if (ti == TypeInfo::Get<uint32_t>()) {
                Lil::UIStyle::DrawConstUInt32Field(m_object_name, static_cast<const uint32_t*>(ptr));
            }
            else if (ti == TypeInfo::Get<float>()) {
                Lil::UIStyle::DrawConstFloatField(m_object_name, static_cast<const float*>(ptr));
            }
            else if (ti == TypeInfo::Get<std::string>()) {
                Lil::UIStyle::DrawConstStringField(m_object_name, static_cast<const std::string*>(ptr));
            }
            else if (ti == TypeInfo::Get<Vector2>()) {
                Lil::UIStyle::DrawConstVector2Field(m_object_name, static_cast<const Vector2*>(ptr));
            }
            else if (ti == TypeInfo::Get<Vector3>()) {
                Lil::UIStyle::DrawConstVector3Field(m_object_name, static_cast<const Vector3*>(ptr));
            }
            else if (ti == TypeInfo::Get<Vector4>()) {
                Lil::UIStyle::DrawConstVector4Field(m_object_name, static_cast<const Vector4*>(ptr));
            }
            else if (ti == TypeInfo::Get<Transform>()) {
                Lil::UIStyle::DrawConstTransformBlock(m_object_name, static_cast<const Transform*>(ptr));
            }
            // else if (ti.IsContainer()) {
            //     ImGui::PushStyleColor(ImGuiCol_Header, Lil::UIStyle::COLOR_HEADER_BG);
            //     ImGui::PushStyleColor(ImGuiCol_HeaderHovered, Lil::UIStyle::COLOR_HEADER_BG_HOVER);
                
            //     if (ImGui::CollapsingHeader(m_object_name.c_str(), 0)) {
            //         ImGui::PopStyleColor(2);
            //         ImGui::Indent(Lil::UIStyle::STRUCT_INDENT_PADDING);

            //         auto* c = ti.Container();
            //         size_t n = c->Size(ptr);
            //         for (size_t i = 0; i < n; ++i) {
            //             const void* element = c->GetElement(ptr, i);
            //             ImGui::PushID((int)i);
            //             SetCurrentObjectName(TextFormat("element %d", i));
            //             VisitObjectConst(c->ElementType(), element);
            //             ImGui::PopID();
            //         }

            //         ImGui::Unindent(Lil::UIStyle::STRUCT_INDENT_PADDING);
            //     } else {
            //         ImGui::PopStyleColor(2);
            //     }
            // }
            else {
                ImGui::PushStyleColor(ImGuiCol_Header, Lil::UIStyle::COLOR_HEADER_BG);
                ImGui::PushStyleColor(ImGuiCol_HeaderHovered, Lil::UIStyle::COLOR_HEADER_BG_HOVER);
                
                if (ImGui::CollapsingHeader(m_object_name.c_str(), 0)) {
                    ImGui::PopStyleColor(2);
                    ImGui::Indent(Lil::UIStyle::STRUCT_INDENT_PADDING);
                    for (auto& f : ti.Fields()) {
                        HandleFieldConst(f, ptr);
                    }
                    ImGui::Unindent(Lil::UIStyle::STRUCT_INDENT_PADDING);
                } else {
                    ImGui::PopStyleColor(2);
                }
            }

            Lil::UIStyle::PopRowSpacing();
            ImGui::PopID();
            ImGui::PopID();
        }
    }
};