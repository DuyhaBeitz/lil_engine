#include "LilEngine.hpp"

Lil::Engine &Lil::Engine::Get() {
    static Lil::Engine instance;
    return instance;
}

void Lil::Engine::Init() {
    rc::DebugRenderer& debugRenderer = Physics().GetWorld()->getDebugRenderer();
    debugRenderer.setIsDebugItemDisplayed(rc::DebugRenderer::DebugItem::COLLISION_SHAPE, true);
    debugRenderer.setIsDebugItemDisplayed(rc::DebugRenderer::DebugItem::COLLIDER_AABB, true);
    debugRenderer.setIsDebugItemDisplayed(rc::DebugRenderer::DebugItem::CONTACT_POINT, false);
    debugRenderer.setIsDebugItemDisplayed(rc::DebugRenderer::DebugItem::CONTACT_NORMAL, false); 
}
