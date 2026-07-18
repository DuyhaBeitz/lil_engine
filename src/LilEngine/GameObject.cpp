#include "GameObject.hpp"
#include "LilEngine.hpp"

GameObject::GameObject(Transform transform) 
: m_id(Lil::Engine().GetWorld().GetNewId()), Transformable(transform)
{
}
