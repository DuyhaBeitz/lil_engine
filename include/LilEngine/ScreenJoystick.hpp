#pragma once

#include <algorithm>
#include <raylib.h>
#include <raymath.h>

/*
 * Floating touch joystick whose layout is relative to the screen, so it looks
 * and behaves the same on any resolution, aspect ratio or orientation.
 *
 *   position     : 0..1 of screen width / height   (0,0 = top-left, 1,1 = bottom-right)
 *   base_radius  : fraction of the SHORTER screen side (keeps the circles round)
 *   knob_radius  : fraction of the SHORTER screen side
 *
 *   ScreenJoystick stick({ 0.15f, 0.78f }, 0.12f, 0.05f);
 *
 * GetDirection() is in player space: right = +X, up = +Y, length 0..1 (analog).
 */
class ScreenJoystick {
private:
    Vector2 m_original_position;
    Vector2 m_base_position;
    Vector2 m_knob_position;

    float m_base_radius;
    float m_knob_radius;

    int m_touch_id = -1;

    int GetTouchIndex();

    static float Unit() {
        return (float) std::max(1, std::min(GetScreenWidth(), GetScreenHeight()));
    }
    static Vector2 ToPixels(Vector2 rel) {
        return { rel.x * (float) GetScreenWidth(), rel.y * (float) GetScreenHeight() };
    }
    static Vector2 ToRelative(Vector2 px) {
        return { px.x / (float) std::max(1, GetScreenWidth()),
                 px.y / (float) std::max(1, GetScreenHeight()) };
    }
    float BaseRadiusPx() const { return m_base_radius * Unit(); }
    float KnobRadiusPx() const { return m_knob_radius * Unit(); }

public:
    ScreenJoystick(Vector2 position, float base_radius, float knob_radius) :
            m_original_position(position), m_base_position(position), m_knob_position(position),
            m_base_radius(base_radius), m_knob_radius(knob_radius) {}

    void Reset(){
        m_touch_id = -1;
        m_base_position = m_original_position;
        m_knob_position = m_original_position;
    }

    int GetTouchId() const { return m_touch_id; }   // finger held by this widget, or -1

    void Update();
    void Draw();

    // How far the knob is pushed, as the player sees it: right = +X, up = +Y.
    // Length is 0 (idle / centred) .. 1 (fully pushed), so it is analog.
    Vector2 GetDirection();

    Vector2 GetKnobPosition() { return ToPixels(m_knob_position); }
    Vector2 GetKnobPositionRelative() { return m_knob_position; }
};