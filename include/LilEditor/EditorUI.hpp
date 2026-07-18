#pragma once

#include "LilEngine.hpp"

// DPI scaling functions
inline float ScaleToDPIF(float value) {
	return GetWindowScaleDPI().x * value;
}

inline int ScaleToDPII(int value) {
    return int(GetWindowScaleDPI().x * value);
}

class EditorUIVisitor : public IFieldVisitor {
public:
    bool Visit(const FieldInfo& field, void* ptr) override {
        ImGui::PushID(reinterpret_cast<intptr_t>(ptr));
        ImGui::PushID(field.name.c_str());

        if (field.HasAttribute("ModelKeyAttribute")) {
            ImGui::Text(field.name.c_str());
            
            auto* p = static_cast<std::string*>(ptr);
            const auto& models = Lil::Resources().Models();
            
            int currentIndex = -1;
            std::vector<std::string> modelKeys;
            for (const auto& [key, model] : *models) {
                modelKeys.push_back(key);
                if (key == *p) {
                    currentIndex = modelKeys.size() - 1;
                }
            }
           
            if (ImGui::BeginCombo(("##" + field.name).c_str(), (*p).c_str())) {
                for (int i = 0; i < modelKeys.size(); i++) {
                    bool isSelected = (i == currentIndex);
                    if (ImGui::Selectable(modelKeys[i].c_str(), isSelected)) {
                        *p = modelKeys[i];
                    }
                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
        }
        else if (field.type == TypeInfo::Get<bool>()) {
            auto* p = static_cast<bool*>(ptr);
            ImGui::Checkbox(field.name.c_str(), p);
        }
        else if (field.type == TypeInfo::Get<int>()) {
            auto* p = static_cast<int*>(ptr);
            ImGui::InputInt(field.name.c_str(), p);
        }
        else if (field.type == TypeInfo::Get<uint32_t>()) {
            auto* p = static_cast<uint32_t*>(ptr);
            ImGui::InputScalar(field.name.c_str(), ImGuiDataType_U32, p);
        }
        else if (field.type == TypeInfo::Get<float>()) {
            auto* p = static_cast<float*>(ptr);
            ImGui::DragFloat(field.name.c_str(), p);
        }
        else if (field.type == TypeInfo::Get<std::string>()) {
            auto* p = static_cast<std::string*>(ptr);
            
            char buffer[256];
            strncpy(buffer, p->c_str(), sizeof(buffer) - 1);
            buffer[sizeof(buffer) - 1] = '\0';
            
            if (ImGui::InputText(field.name.c_str(), buffer, sizeof(buffer))) {
                *p = buffer;
            }
        }
        else {
            if (ImGui::CollapsingHeader(field.name.c_str())) {
                ImGui::Indent();
                field.type.VisitFields(ptr, *this);
                ImGui::Unindent();
            }            
        }

        ImGui::PopID();
        ImGui::PopID();

        return true;
    }

    bool VisitConst(const FieldInfo& field, const void* ptr) override {
        ImGui::PushID(reinterpret_cast<intptr_t>(ptr));
        ImGui::PushID(field.name.c_str());

        ImGui::BeginDisabled();

        if (field.type == TypeInfo::Get<bool>()) {
            auto* p = static_cast<const bool*>(ptr);
            ImGui::Text("const %s = %d", field.name.c_str(), *p);
        }
        else if (field.type == TypeInfo::Get<int>()) {
            auto* p = static_cast<const int*>(ptr);
            ImGui::Text("const %s = %d", field.name.c_str(), *p);
        }
        else if (field.type == TypeInfo::Get<uint32_t>()) {
            auto* p = static_cast<const uint32_t*>(ptr);
            ImGui::Text("const %s = %d", field.name.c_str(), *p);
        }
        else if (field.type == TypeInfo::Get<float>()) {
            auto* p = static_cast<const float*>(ptr);
            ImGui::Text("const %s = %f", field.name.c_str(), *p);
        }
        else if (field.type == TypeInfo::Get<std::string>()) {
            auto* p = static_cast<const std::string*>(ptr);
            
            ImGui::Text("const %s = %s", field.name.c_str(), *p);
        }
        else {
            ImGui::Text("const %s", field.name.c_str());
        }

        ImGui::EndDisabled();

        ImGui::PopID();
        ImGui::PopID();

        return true;
    }
};