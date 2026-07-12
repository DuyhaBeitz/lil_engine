#include "Actor.hpp"

void Actor::AttachComponent(Component *component, Transformable *parent) {
    m_components.push_back(component);
    m_attachments.push_back(Attachment{
        .parent = parent,
        .child = component
    });
}
