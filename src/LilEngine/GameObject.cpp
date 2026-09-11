#include "GameObject.hpp"
#include "LilEngine.hpp"

Identifiable::Identifiable() : m_id(GenerateID()) {}

const uuids::uuid& Identifiable::GetID() const {return m_id;}
std::string Identifiable::GetIDString() const {return uuids::to_string(m_id);}
void Identifiable::SetID(uuids::uuid id){m_id = id;}

void GameObject::ToggleVisible() { m_visible = !m_visible; }
void GameObject::EnableVisible() { m_visible = true; }
void GameObject::DisableVisible() { m_visible = false; }
bool GameObject::IsVisible() const { return m_visible; }

void GameObject::ToggleRaycastActive() { m_raycast_active = !m_raycast_active; }
void GameObject::EnableRaycastActive() { m_raycast_active = true; }
void GameObject::DisableRaycastActive() { m_raycast_active = false; }
bool GameObject::IsRaycastActive() const { return m_raycast_active; }

uuids::uuid Identifiable::GenerateID() {
    std::random_device rd;
    std::mt19937 engine(rd());
    
    uuids::uuid_random_generator gen{engine};
    return gen();
}

GameObject::GameObject(Transform transform) 
: Transformable(transform) {}