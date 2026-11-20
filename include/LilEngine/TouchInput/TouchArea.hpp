#pragma once

#include <algorithm>
#include <raylib.h>
#include <raymath.h>
#include <vector>
#include "TouchWidget.hpp"

class TouchArea : public TouchWidget {
private:
    Rectangle m_area;               // 0..1 of the screen
    bool m_visible = false;         // debug drawing

    bool m_down = false;
    bool m_was_down = false;

    Vector2 m_position_px{ 0.0f, 0.0f };    // finger position, screen pixels
    Vector2 m_delta_px{ 0.0f, 0.0f };       // movement this frame, screen pixels

    Rectangle AreaPx() const {
        const float w = (float) GetScreenWidth(), h = (float) GetScreenHeight();
        return { m_area.x * w, m_area.y * h, m_area.width * w, m_area.height * h };
    }

    int m_priority = touch_priority_area;

public:
    explicit TouchArea(Rectangle area) : m_area(area) {}

    void Update() override;
    void Draw() override;
    void Reset() override;

    bool IsDown() const     { return m_down; }
    bool IsPressed() const  { return m_down && !m_was_down; }
    bool IsReleased() const { return !m_down && m_was_down; }

    Vector2 GetDelta() const { return { m_delta_px.x, -m_delta_px.y }; }
    Vector2 GetPosition() const;

    void SetVisible(bool visible) { m_visible = visible; }
};