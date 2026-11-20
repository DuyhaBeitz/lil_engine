#include "TouchInput/TouchAxisJoystick.hpp"
#include "TouchInput/TouchManager.hpp"
#include <algorithm>
#include <cmath>

void TouchAxisJoystick::Reset() {
    if (m_touch_id != -1) {
        Lil::Touch::UnclaimTouch(m_touch_id);
        m_touch_id = -1;
    };
    if (!m_stable) m_knob_position = m_base_position;
}

// True if the point is inside the rectangular track.
bool TouchAxisJoystick::HitsTrack(Vector2 p) const {
    const Vector2 center = ToPixels(m_base_position);
    const float half_len = LengthPx() * 0.5f;
    const float half_wid = TrackRadiusPx();
    const bool horizontal = (m_direction == Direction::Horizontal);

    const float dx = std::fabs(p.x - center.x);
    const float dy = std::fabs(p.y - center.y);
    return horizontal ? (dx <= half_len && dy <= half_wid)
                      : (dx <= half_wid && dy <= half_len);
}

void TouchAxisJoystick::Update() {
    // the finger that pressed us was lifted
    if (m_touch_id != -1 && !Lil::Touch::IsTouchActive(m_touch_id)) {
        Reset();
    }

    // look for a new finger landing on the button
    if (m_touch_id == -1){
        Lil::Touch::RequestTouch([this](Vector2 pos){
                return (this->HitsTrack(pos));
            },
            m_priority,
            this
        );
        m_touch_id = Lil::Touch::ReceiveTouch(this);
    }
    if (m_touch_id == -1) return;

    const Vector2 touch_pos = GetTouchPosition(m_touch_id);
    const Vector2 axis = AxisVector();
    const float half = HalfTravelPx();

    const Vector2 base = ToPixels(m_base_position);
    // only the axis component counts; the knob can't leave the track
    const float t = Clamp(Vector2DotProduct(Vector2Subtract(touch_pos, base), axis), -half, half);

    m_knob_position = ToRelative(Vector2Add(base, Vector2Scale(axis, t)));
}

void TouchAxisJoystick::Draw() {
    const Vector2 base = ToPixels(m_base_position);
    const Vector2 knob = ToPixels(m_knob_position);
    const float len = LengthPx();
    const float wid = TrackRadiusPx() * 2.0f;
    const float knob_r = KnobRadiusPx();

    // Rectangle: long side along the axis, thickness across it.
    const bool horizontal = (m_direction == Direction::Horizontal);
    const float w = horizontal ? len : wid;
    const float h = horizontal ? wid : len;
    const Rectangle track{ base.x - w * 0.5f, base.y - h * 0.5f, w, h };

    if (m_touch_id != -1){
        DrawRectangleRounded(track, 0.25f, 8, ColorAlpha(GRAY, 0.3f));
        DrawCircleV(knob, knob_r, DARKGRAY);
    }
    else {
        DrawRectangleRounded(track, 0.25f, 8, ColorAlpha(GRAY, 0.1f));
        DrawCircleV(knob, knob_r, ColorAlpha(DARKGRAY, 0.2f));
    }
}

float TouchAxisJoystick::GetValue() {
    const float half = HalfTravelPx();
    if (half <= 0.0f) return 0.0f;

    const Vector2 d = Vector2Subtract(ToPixels(m_knob_position), ToPixels(m_base_position));
    const float v = Clamp(Vector2DotProduct(d, AxisVector())/half, -1.0f, 1.0f);
    return (m_direction == Direction::Vertical) ? -v : v;       // Touch Y points down: make UP positive
}