#include <algorithm>
#include "TouchInput/TouchJoystick.hpp"
#include "TouchInput/TouchManager.hpp"

void TouchJoystick::Reset() {
    if (m_touch_id != -1) {
        Lil::Touch::UnclaimTouch(m_touch_id);
        m_touch_id = -1;
    }
    m_base_position = m_original_position;
    m_knob_position = m_original_position;
}

void TouchJoystick::Update() {

    // the finger that pressed us was lifted
    if (m_touch_id != -1 && !Lil::Touch::IsTouchActive(m_touch_id)) {
        Reset();
    }

    // look for a new finger landing on the button
    if (m_touch_id == -1){
        Lil::Touch::RequestTouch([this](Vector2 pos){
                return (Vector2Distance(pos, ToPixels(m_base_position)) <= BaseRadiusPx());
            },
            m_priority,
            this
        );
        m_touch_id = Lil::Touch::ReceiveTouch(this);
    }

    if (m_touch_id == -1) return;

    // work in pixels, store relative
    const Vector2 touch_pos = GetTouchPosition(m_touch_id);
    const float base_r = BaseRadiusPx();
    const float knob_r = KnobRadiusPx();

    Vector2 base = ToPixels(m_base_position);
    Vector2 dist = Vector2Subtract(touch_pos, base);
    float dist_l = Vector2Length(dist);

    // drag the base along so the knob always stays inside it
    if (dist_l + knob_r > base_r) {
        float delta_l = dist_l - base_r + knob_r;
        base = Vector2Add(base, Vector2Scale(Vector2Normalize(dist), delta_l));
    }

    m_base_position = ToRelative(base);
    m_knob_position = ToRelative(touch_pos);
}

void TouchJoystick::Draw() {
    const Vector2 base = ToPixels(m_base_position);
    const Vector2 knob = ToPixels(m_knob_position);
    const float base_r = BaseRadiusPx();
    const float knob_r = KnobRadiusPx();

    if (m_touch_id != -1){
        DrawCircleV(base, base_r, ColorAlpha(GRAY, 0.3f));
        DrawCircleV(knob, knob_r, DARKGRAY);
    }
    else {
        DrawCircleV(base, base_r, ColorAlpha(GRAY, 0.1f));
        DrawCircleV(knob, knob_r, ColorAlpha(DARKGRAY, 0.2f));
    }
}

Vector2 TouchJoystick::GetDirection() {
    // The knob can move this far from the base centre (the base follows the finger beyond it).
    const float travel = BaseRadiusPx() - KnobRadiusPx();
    if (travel <= 0.0f) return { 0.0f, 0.0f };

    Vector2 d = Vector2Scale(Vector2Subtract(ToPixels(m_knob_position), ToPixels(m_base_position)), 1.0f / travel);
    if (Vector2Length(d) > 1.0f) d = Vector2Normalize(d);

    return { d.x, -d.y };   // Touch Y points down: make UP positive
}