#include "ScreenRectangle.hpp"

bool ScreenRectangle::WasDownLastFrame(int id) const {
    for (int i = 0; i < m_prev_count; i++){
        if (m_prev_ids[i] == id) return true;
    }
    return false;
}

void ScreenRectangle::RememberFingers() {
    const int count = GetTouchPointCount();
    m_prev_count = std::min(count, MAX_TOUCHES);
    for (int i = 0; i < m_prev_count; i++) m_prev_ids[i] = GetTouchPointId(i);
}

void ScreenRectangle::Reset() {
    m_touch_id = -1;
    m_down = false;
    m_was_down = false;
    m_delta_px = { 0.0f, 0.0f };
    RememberFingers();
}

void ScreenRectangle::Update(const std::vector<int>& claimed) {
    m_was_down = m_down;
    m_delta_px = { 0.0f, 0.0f };

    const int count = GetTouchPointCount();

    // follow our finger
    int index = -1;
    if (m_touch_id != -1){
        for (int i = 0; i < count; i++){
            if (GetTouchPointId(i) == m_touch_id){ index = i; break; }
        }
        if (index == -1) m_touch_id = -1;   // lifted
    }

    if (m_touch_id != -1){
        const Vector2 p = GetTouchPosition(index);
        m_delta_px = Vector2Subtract(p, m_position_px);
        m_position_px = p;
    }
    else {
        // look for a NEW finger that landed inside the area and is not held by anyone else
        const Rectangle area = AreaPx();
        for (int i = 0; i < count; i++){
            const int id = GetTouchPointId(i);
            if (std::find(claimed.begin(), claimed.end(), id) != claimed.end()) continue;
            if (WasDownLastFrame(id)) continue;     // slid in from elsewhere
            const Vector2 p = GetTouchPosition(i);
            if (CheckCollisionPointRec(p, area)){
                m_touch_id = id;
                m_position_px = p;
                break;
            }
        }
    }

    m_down = (m_touch_id != -1);
    RememberFingers();
}

Vector2 ScreenRectangle::GetPosition() const {
    if (!m_down) return { 0.0f, 0.0f };
    const Rectangle a = AreaPx();
    if (a.width <= 0.0f || a.height <= 0.0f) return { 0.0f, 0.0f };

    const float x = Clamp((m_position_px.x - a.x) / a.width, 0.0f, 1.0f);
    const float y = Clamp((m_position_px.y - a.y) / a.height, 0.0f, 1.0f);
    return { x, 1.0f - y };     // screen Y points down: make UP positive
}

void ScreenRectangle::Draw() {
    if (!m_visible) return;
    const Rectangle a = AreaPx();
    DrawRectangleRec(a, ColorAlpha(GRAY, m_down ? 0.15f : 0.05f));
    DrawRectangleLinesEx(a, 2.0f, ColorAlpha(DARKGRAY, 0.4f));
}