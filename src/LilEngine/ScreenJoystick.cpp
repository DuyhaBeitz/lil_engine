#include <algorithm>
#include "ScreenJoystick.hpp"

int ScreenJoystick::GetTouchIndex() {
    const int count = GetTouchPointCount();

    if (m_touch_id != -1){
        for (int i = 0; i < count; i++){
            if (m_touch_id == GetTouchPointId(i)) return i;
        }
        // touch is lost
        Reset();
    }

    // if there was no touch or we just lost one, try to find new touch
    const Vector2 base = ToPixels(m_base_position);
    const float base_r = BaseRadiusPx();
    for (int i = 0; i < count; i++){
        if (Vector2Distance(GetTouchPosition(i), base) <= base_r){
            m_touch_id = GetTouchPointId(i);
            return i;
        }
    }
    return -1;
}

void ScreenJoystick::Update() {
    int i = GetTouchIndex();
    if (i == -1) return;

    // work in pixels, store relative
    const Vector2 touch_pos = GetTouchPosition(i);
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

void ScreenJoystick::Draw() {
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

Vector2 ScreenJoystick::GetDirection() {
    // The knob can move this far from the base centre (the base follows the finger beyond it).
    const float travel = BaseRadiusPx() - KnobRadiusPx();
    if (travel <= 0.0f) return { 0.0f, 0.0f };

    Vector2 d = Vector2Scale(Vector2Subtract(ToPixels(m_knob_position), ToPixels(m_base_position)), 1.0f / travel);
    if (Vector2Length(d) > 1.0f) d = Vector2Normalize(d);

    return { d.x, -d.y };   // screen Y points down: make UP positive
}