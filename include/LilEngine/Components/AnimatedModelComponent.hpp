#pragma once

#include "Component.hpp"
#include "ResourceManager.hpp"

#include "ReflAttributes.hpp"

class AnimatedModelComponent : public Component {
public:
    std::string m_model_key = "None";
    int m_anim_idx = 0;
    bool m_playing = false;
    bool m_looping = true;
    float m_speed = 1.0f;

    std::string m_old_model_key = "None";
    bool m_animation_player_loaded = false;
    R3D_AnimationPlayer m_animation_player;

    virtual void LayoutUpdate() override;
    virtual void SimulationUpdate(Actor& actor, float delta_time) override;

public:
    LIL_REFLECTABLE()
    LIL_SERIALIZABLE()

    AnimatedModelComponent() = default;
    AnimatedModelComponent(std::string model_key);
    virtual ~AnimatedModelComponent();

    virtual void Draw() override;

    void SetModel(std::string model_key);

    R3D_Model* GetModel() const;
    std::string& ModelKey();

    void SetPlaying(bool playing) { m_playing = playing; }
    bool IsPlaying() { return m_playing; }
    void TogglePlaying() {m_playing = !m_playing;}

    void SetLooping(bool looping) { m_looping = looping; }
    bool IsLooping() { return m_looping; }
    void ToggleLooping() {m_looping = !m_looping;}

    void SetSpeed(float speed) { m_speed = speed; }
    float GetSpeed() { return m_speed; }

    int GetAnimIndex() { return m_anim_idx; }
    void SetAnimIndex(int anim_idx) { m_anim_idx = anim_idx; }
};
LIL_REFLECT(AnimatedModelComponent, bases<Component>,
    field(m_model_key, ModelKeyAttribute()),
    field(m_anim_idx),
    field(m_playing),
    field(m_looping),
    field(m_speed)
)
LIL_SER_BEGIN(AnimatedModelComponent)
LIL_SER_BASE(Component)
LIL_SER_FIELD(m_model_key)
LIL_SER_FIELD(m_anim_idx)
LIL_SER_FIELD(m_playing)
LIL_SER_FIELD(m_looping)
LIL_SER_FIELD(m_speed)
LIL_SER_END()
