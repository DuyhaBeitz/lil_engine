#include "GameObject.hpp"
#include "LilEngine.hpp"

Identifiable::Identifiable() : m_id(GenerateID()) {}

const uuids::uuid& Identifiable::GetID() const {return m_id;}
std::string Identifiable::GetIDString() const {return uuids::to_string(m_id);}
void Identifiable::SetID(uuids::uuid id){m_id = id;}

uuids::uuid Identifiable::GenerateID() {
    std::random_device rd;
    std::mt19937 engine(rd());
    
    uuids::uuid_random_generator gen{engine};
    return gen();
}

GameObject::GameObject(Transform transform) 
: Transformable(transform) {}