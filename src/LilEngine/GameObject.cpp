#include "GameObject.hpp"
#include "LilEngine.hpp"

GameObject::GameObject(Transform transform) 
: m_id(Lil::GetWorld().GetNewId()), Transformable(transform)
{
}
