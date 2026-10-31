#include "Components/AnimatedModelComponent.hpp"
#include "LilEngine.hpp"

AnimatedModelComponent::AnimatedModelComponent(Lil::Assets::Model model_key)
: Component(), m_model_key(model_key) {}

void AnimatedModelComponent::SetModel(std::string model_key) { m_model_key = model_key; }
Lil::Assets::Model& AnimatedModelComponent::ModelKey() { return m_model_key; }

void AnimatedModelComponent::Draw() {
    Component::Draw();
    if (m_player.IsLoaded() && Lil::Assets::ModelExists(m_model_key))
        m_player.Draw(m_model_key, GetPosition(), GetRotation(), GetScale());
    else DrawSphere(GetPosition(), 2.f, RAYRED);
}

void AnimatedModelComponent::LayoutUpdate() {
    Component::LayoutUpdate();

    // the animation lib shares the model's key, as before (no scene format change)
    const Lil::Assets::AnimationLib lib(m_model_key.Key());

    if (!m_player.IsLoaded() || m_player.IsStale() || m_model_key != m_old_model_key) {
        if (m_player.Load(m_model_key, lib)) m_old_model_key = m_model_key;
    }

    if (m_player.IsLoaded()) {
        m_player.SetLoop(m_anim_idx, m_looping);
        if (m_playing) m_player.Play(m_anim_idx);
        else m_player.Stop();
    }
}

void AnimatedModelComponent::SimulationUpdate(Actor& actor, float delta_time) {
    Component::SimulationUpdate(actor, delta_time);
    m_player.Update(delta_time * m_speed);
}


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
