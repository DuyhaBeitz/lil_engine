#pragma once

#include <algorithm>
#include <raylib.h>
#include <raymath.h>
#include <vector>

/*
 * Touch area that follows ONE finger and reports it: down state, position and delta.
 * Meant for things like camera look, where the mouse delta cannot be used on mobile
 * (touch is emulated as the mouse, so the first finger anywhere would drive it).
 *
 *   area : { x, y, width, height }, all 0..1 of the screen (x, y = top-left corner)
 *
 *   ScreenRectangle look({ 0.4f, 0.0f, 0.6f, 1.0f });   // right 60% of the screen
 *
 * A finger belongs to the area only if
 *   - it LANDED inside the area (a finger that slides in from elsewhere is ignored), and
 *   - no other widget (joystick, button) already holds it.
 * Once it belongs to the area it is followed until it is lifted, even outside the area.
 *
 * Values are in player space: right = +X, up = +Y.
 */
class ScreenRectangle {
private:
    static constexpr int MAX_TOUCHES = 10;

    Rectangle m_area;               // 0..1 of the screen
    bool m_visible = false;         // debug drawing

    int m_touch_id = -1;
    bool m_down = false;
    bool m_was_down = false;

    Vector2 m_position_px{ 0.0f, 0.0f };    // finger position, screen pixels
    Vector2 m_delta_px{ 0.0f, 0.0f };       // movement this frame, screen pixels

    int m_prev_ids[MAX_TOUCHES];
    int m_prev_count = 0;

    Rectangle AreaPx() const {
        const float w = (float) GetScreenWidth(), h = (float) GetScreenHeight();
        return { m_area.x * w, m_area.y * h, m_area.width * w, m_area.height * h };
    }
    bool WasDownLastFrame(int id) const;
    void RememberFingers();

public:
    explicit ScreenRectangle(Rectangle area) : m_area(area) {}

    // `claimed`: ids of fingers held by other widgets this frame; they are never taken.
    void Update(const std::vector<int>& claimed = {});
    void Draw();

    // Releases the finger and treats every finger that is down right now as "already down".
    void Reset();

    int GetTouchId() const { return m_touch_id; }   // finger held by this widget, or -1

    bool IsDown() const     { return m_down; }
    bool IsPressed() const  { return m_down && !m_was_down; }
    bool IsReleased() const { return !m_down && m_was_down; }

    // Finger movement this frame in PIXELS (like the mouse delta). Zero when idle and on the frame of landing.
    Vector2 GetDelta() const { return { m_delta_px.x, -m_delta_px.y }; }

    // Finger position inside the area: (0,0) = bottom-left, (1,1) = top-right. Zero when idle.
    Vector2 GetPosition() const;

    void SetVisible(bool visible) { m_visible = visible; }
};