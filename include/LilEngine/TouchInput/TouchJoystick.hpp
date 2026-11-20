#pragma once

#include <algorithm>
#include <raylib.h>
#include <raymath.h>
#include <vector>
#include "TouchWidget.hpp"

class TouchJoystick : public TouchWidget {
private:
    Vector2 m_original_position;
    Vector2 m_base_position;
    Vector2 m_knob_position;

    float m_base_radius; // fraction of the SHORTER screen side
    float m_knob_radius; // fraction of the SHORTER screen side

    float BaseRadiusPx() const { return m_base_radius * Unit(); }
    float KnobRadiusPx() const { return m_knob_radius * Unit(); }

    int m_touch_id = -1;

    int m_priority = touch_priority_joystick;

public:
    TouchJoystick(Vector2 position, float base_radius, float knob_radius) :
            m_original_position(position), m_base_position(position), m_knob_position(position),
            m_base_radius(base_radius), m_knob_radius(knob_radius) {}

    void Update() override;
    void Draw() override;
    void Reset() override;

    Vector2 GetDirection();

    Vector2 GetKnobPosition() { return ToPixels(m_knob_position); }
    Vector2 GetKnobPositionRelative() { return m_knob_position; }
};