#include "TouchInput/TouchArea.hpp"
#include "TouchInput/TouchManager.hpp"

void TouchArea::Reset() {
    if (m_touch_id != -1) {
        Lil::Touch::UnclaimTouch(m_touch_id);
        m_touch_id = -1;
    };
    m_down = false;
    m_delta_px = { 0.0f, 0.0f };
}

void TouchArea::Update() {
    m_was_down = m_down;
    m_delta_px = { 0.0f, 0.0f };

    // the finger that pressed us was lifted
    if (m_touch_id != -1 && !Lil::Touch::IsTouchActive(m_touch_id)) {
        Reset();
    }

    // look for a new finger landing on the button
    if (m_touch_id == -1){
        Lil::Touch::RequestTouch(
            [this](Vector2 pos){
                return CheckCollisionPointRec(pos, AreaPx());
            },
            m_priority,
            this
        );
        m_touch_id = Lil::Touch::ReceiveTouch(this);
        if (m_touch_id != -1) m_position_px = GetTouchPosition(m_touch_id);
    }

    if (m_touch_id != -1){
        const Vector2 p = GetTouchPosition(m_touch_id);
        m_delta_px = Vector2Subtract(p, m_position_px);
        m_position_px = p;
    }

    m_down = (m_touch_id != -1);
}

Vector2 TouchArea::GetPosition() const {
    if (!m_down) return { 0.0f, 0.0f };
    const Rectangle a = AreaPx();
    if (a.width <= 0.0f || a.height <= 0.0f) return { 0.0f, 0.0f };

    const float x = Clamp((m_position_px.x - a.x) / a.width, 0.0f, 1.0f);
    const float y = Clamp((m_position_px.y - a.y) / a.height, 0.0f, 1.0f);
    return { x, 1.0f - y };     // Screen Y points down: make UP positive
}

void TouchArea::Draw() {
    if (!m_visible) return;
    const Rectangle a = AreaPx();
    DrawRectangleRec(a, ColorAlpha(GRAY, m_down ? 0.15f : 0.05f));
    DrawRectangleLinesEx(a, 2.0f, ColorAlpha(DARKGRAY, 0.4f));
}