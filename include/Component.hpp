#pragma once

#include "Reflection.hpp"

class Object : Reflectable {
protected:
    Transform m_transform;

public:
    void SetTransform(Transform transform) {m_transform = transform;}
    void SetPosition(Vector3 position) {m_transform.translation = position;}
    void SetRotation(Quaternion rotation) {m_transform.rotation = rotation;}
    void SetScale(Vector3 scale) {m_transform.scale = scale;}

    Transform GetTransform() {return m_transform;}
    Vector3 GetPosition() {return m_transform.translation;}
    Quaternion GetRotation() {return m_transform.rotation;}
    Vector3 GetScale() {return m_transform.scale;}
};

class Component : Object {
private:
    Object m_local_space;

public:
    void UpdateTransform(Transform parent_transform) {
        m_transform.translation = m_local_space.GetPosition() + parent_transform.translation;
        m_tra
    }
};