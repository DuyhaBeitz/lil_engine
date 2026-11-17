#include "ScreenButton.hpp"

#include <algorithm>

bool ScreenButton::WasDownLastFrame(int id) const {
    for (int i = 0; i < m_prev_count; i++){
        if (m_prev_ids[i] == id) return true;
    }
    return false;
}

void ScreenButton::RememberFingers() {
    const int count = GetTouchPointCount();
    m_prev_count = std::min(count, MAX_TOUCHES);
    for (int i = 0; i < m_prev_count; i++) m_prev_ids[i] = GetTouchPointId(i);
}

void ScreenButton::Reset() {
    m_touch_id = -1;
    m_down = false;
    m_was_down = false;
    RememberFingers();
}

void ScreenButton::Update() {
    m_was_down = m_down;

    const int count = GetTouchPointCount();
    const Vector2 center = CenterPx();
    const float radius = RadiusPx();

    // the finger that pressed us was lifted
    if (m_touch_id != -1){
        bool found = false;
        for (int i = 0; i < count; i++){
            if (GetTouchPointId(i) == m_touch_id){ found = true; break; }
        }
        if (!found) m_touch_id = -1;
    }

    // look for a NEW finger landing on the button
    if (m_touch_id == -1){
        for (int i = 0; i < count; i++){
            int id = GetTouchPointId(i);
            if (WasDownLastFrame(id)) continue;     // dragged in from elsewhere
            if (Vector2Distance(GetTouchPosition(i), center) <= radius){
                m_touch_id = id;
                break;
            }
        }
    }

    m_down = (m_touch_id != -1);

    // remember the fingers for the next frame
    RememberFingers();
}

void ScreenButton::Draw() {
    const Vector2 center = CenterPx();
    const float radius = RadiusPx();

    DrawCircleV(center, radius, ColorAlpha(GRAY, m_down ? 0.5f : 0.25f));
    DrawCircleLines((int) center.x, (int) center.y, radius, ColorAlpha(DARKGRAY, m_down ? 0.9f : 0.4f));

    // centred icon
    const float font_size = radius*m_icon_scale;
    const Vector2 size = MeasureTextEx(m_font, m_icon.c_str(), font_size, 0.0f);
    const Vector2 pos = { center.x - size.x*0.5f, center.y - size.y*0.5f };
    DrawTextEx(m_font, m_icon.c_str(), pos, font_size, 0.0f, ColorAlpha(WHITE, m_down ? 1.0f : 0.7f));
}