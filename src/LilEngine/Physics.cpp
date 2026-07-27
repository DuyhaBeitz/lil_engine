#include "Physics.hpp"

void PhysicsSystem::UpdateDebug() {
    rc::DebugRenderer& debugRenderer = m_world->getDebugRenderer();
    debugRenderer.computeDebugRenderingPrimitives(*m_world);
}