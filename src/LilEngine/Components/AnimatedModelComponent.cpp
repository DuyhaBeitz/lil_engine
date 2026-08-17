#include "Components/AnimatedModelComponent.hpp"

#include "LilEngine.hpp"
#include "utils/MeshHelper.hpp"

AnimatedModelComponent::AnimatedModelComponent(std::string model_key)
: m_model_key(model_key), Component()
{
}

AnimatedModelComponent::~AnimatedModelComponent() {
    if (m_animation_player_loaded) R3D_UnloadAnimationPlayer(m_animation_player);
}

void AnimatedModelComponent::SetModel(std::string model_key) {m_model_key = model_key;}
R3D_Model *AnimatedModelComponent::GetModel() const { return Lil::Resources().GetModel(m_model_key); }
std::string &AnimatedModelComponent::ModelKey() { return m_model_key; }

void AnimatedModelComponent::Draw() {
    R3D_Model* m = GetModel();
    if (m && m_animation_player_loaded)
        R3D_DrawAnimatedModelEx(*m, m_animation_player, GetPosition(), GetRotation(), GetScale());
    else DrawSphere(GetPosition(), 2.f, RAYRED);
}

void AnimatedModelComponent::OnLayoutUpdate() {
    R3D_Model* m = GetModel();
    R3D_AnimationLib* l = Lil::Resources().GetAnimationLib(m_model_key);
    // load
    if (m_model_key != m_old_model_key && m && l) {
        if (m_animation_player_loaded) {
            R3D_UnloadAnimationPlayer(m_animation_player);
        }
        m_animation_player = R3D_LoadAnimationPlayer(m->skeleton, *l);
        m_animation_player_loaded = true;
    }
    m_old_model_key = m_model_key;

    // update
    if (m_animation_player_loaded) {
        R3D_SetAnimationLoop(&m_animation_player, m_anim_idx, m_looping);
    }
}
void AnimatedModelComponent::SimulationUpdate(Actor &actor, float delta_time) {
    if (m_animation_player_loaded) {
        R3D_UpdateAnimationPlayer(&m_animation_player, delta_time * m_speed);
    }
}