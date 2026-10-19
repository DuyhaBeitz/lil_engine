#include "ComponentRef.hpp"
#include "LilEngine.hpp"

Component* GetComponentByIdFromWorld(const uuids::uuid& id) {
    return Lil::World().GetComponent(id);
}

uuids::uuid ComponentIdOf(const Component *c) {
    return c->GetID();
}
