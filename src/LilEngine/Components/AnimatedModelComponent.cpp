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

void AnimatedModelComponent::SetPlaying(bool playing) { m_playing = playing; }
bool AnimatedModelComponent::IsPlaying() { return m_playing; }
void AnimatedModelComponent::TogglePlaying() {m_playing = !m_playing;}
void AnimatedModelComponent::SetLooping(bool looping) { m_looping = looping; }
bool AnimatedModelComponent::IsLooping() { return m_looping; }
void AnimatedModelComponent::ToggleLooping() {m_looping = !m_looping;}
void AnimatedModelComponent::SetSpeed(float speed) { m_speed = speed; }
float AnimatedModelComponent::GetSpeed() { return m_speed; }
int AnimatedModelComponent::GetAnimIndex() { return m_anim_idx; }
void AnimatedModelComponent::SetAnimIndex(int anim_idx) { m_anim_idx = anim_idx; }

void AnimatedModelComponent::Draw() {
    Component::Draw();
    
    R3D_Model* m = GetModel();
    if (m && m_animation_player_loaded)
        R3D_DrawAnimatedModelEx(*m, m_animation_player, GetPosition(), GetRotation(), GetScale());
    else DrawSphere(GetPosition(), 2.f, RAYRED);
}

void AnimatedModelComponent::LayoutUpdate() {
    Component::LayoutUpdate();

    R3D_Model* m = GetModel();
    R3D_AnimationLib* l = Lil::Resources().GetAnimationLib(m_model_key);
    // load
    if (m_model_key != m_old_model_key && m && l) {
        if (m_animation_player_loaded) {
            R3D_UnloadAnimationPlayer(m_animation_player);
        }
        m_animation_player = R3D_LoadAnimationPlayer(m->skeleton, *l);
        m_animation_player_loaded = true;
        m_old_model_key = m_model_key;
    }    

    // update
    if (m_animation_player_loaded) {
        R3D_SetAnimationLoop(&m_animation_player, m_anim_idx, m_looping);
        if (m_playing) R3D_PlayAnimation(&m_animation_player, m_anim_idx);
        else R3D_StopAnimation(&m_animation_player);
    }
}
void AnimatedModelComponent::SimulationUpdate(Actor &actor, float delta_time) {
    Component::SimulationUpdate(actor, delta_time);

    if (m_animation_player_loaded) {
        R3D_UpdateAnimationPlayer(&m_animation_player, delta_time * m_speed);
    }
}