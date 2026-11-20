#pragma once

#include <algorithm>
#include <string>
#include <utility>
#include <raylib.h>
#include <raymath.h>
#include "TouchWidget.hpp"

class TouchButton : public TouchWidget {
private:
    Vector2 m_position;             // 0..1
    float m_radius;                 // fraction of the shorter side
    std::string m_icon;
    Font m_font;
    float m_icon_scale = 0.9f;      // icon font size as a fraction of the radius

    bool m_down = false;
    bool m_was_down = false;

    Vector2 CenterPx() const { return ToPixels(m_position); }
    float RadiusPx() const { return m_radius * Unit(); }

    int m_priority = touch_priority_button;

public:
    TouchButton(Vector2 position, float radius, std::string icon, Font font) :
        m_position(position), m_radius(radius), m_icon(std::move(icon)), m_font(font) {}

    void Update() override;
    void Draw() override;
    void Reset() override;


    bool IsDown() const     { return m_down; }
    bool IsPressed() const  { return m_down && !m_was_down; }
    bool IsReleased() const { return !m_down && m_was_down; }

    void SetIcon(const std::string& icon) { m_icon = icon; }
    void SetFont(Font font) { m_font = font; }
    void SetIconScale(float scale) { m_icon_scale = scale; }
};