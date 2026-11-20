#include "TouchInput/TouchButton.hpp"
#include "TouchInput/TouchManager.hpp"

#include <algorithm>

void TouchButton::Reset() {
    if (m_touch_id != -1) {
        Lil::Touch::UnclaimTouch(m_touch_id);
        m_touch_id = -1;
    }    
    m_down = false;
}

void TouchButton::Update() {
    m_was_down = m_down;

    // the finger that pressed us was lifted
    if (m_touch_id != -1 && !Lil::Touch::IsTouchActive(m_touch_id)) {
        Reset();
    }

    // look for a new finger landing on the button
    if (m_touch_id == -1){
        Lil::Touch::RequestTouch([this](Vector2 pos){
                return (Vector2Distance(pos, CenterPx()) <= RadiusPx());
            },
            m_priority,
            this
        );
        m_touch_id = Lil::Touch::ReceiveTouch(this);
    }

    m_down = (m_touch_id != -1);
}

void TouchButton::Draw() {
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