#pragma once

#include "lil_engine.hpp"

class EditorUIVisitor : public IFieldVisitor {
public:
    bool Visit(const FieldInfo& field, void* ptr) override {
        ImGui::PushID(reinterpret_cast<intptr_t>(ptr));
        ImGui::PushID(field.name.c_str());

        if (field.type == TypeInfo::Get<bool>()) {
            auto* p = static_cast<bool*>(ptr);
            ImGui::Checkbox(field.name.c_str(), p);
        }
        else if (field.type == TypeInfo::Get<int>()) {
            auto* p = static_cast<int*>(ptr);
            ImGui::InputInt(field.name.c_str(), p);
        }
        else if (field.type == TypeInfo::Get<float>()) {
            auto* p = static_cast<float*>(ptr);
            ImGui::DragFloat(field.name.c_str(), p);
        }
        // else if (field.type == TypeInfo::Get<Vector2>()) {
        //     auto* p = static_cast<Vector2*>(ptr);
        //     float pp[2] = {p->x, p->y};
        //     ImGui::InputFloat2(field.name.c_str(), pp);
        // }
        // else if (field.type == TypeInfo::Get<Vector3>()) {
        //     auto* p = static_cast<Vector3*>(ptr);
        //     float pp[3] = {p->x, p->y, p->z};
        //     ImGui::InputFloat3(field.name.c_str(), pp);
        // }
        // else if (field.type == TypeInfo::Get<Vector4>()) {
        //     auto* p = static_cast<Vector4*>(ptr);
        //     float pp[4] = {p->x, p->y, p->z, p->w};
        //     ImGui::InputFloat4(field.name.c_str(), pp);
        // }

        else {
            if (ImGui::CollapsingHeader(field.name.c_str())) {
                field.type.VisitFields(ptr, *this);
            }            
        }

        ImGui::PopID();
        ImGui::PopID();

        return true;
    }
};