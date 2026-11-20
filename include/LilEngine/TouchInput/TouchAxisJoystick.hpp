#pragma once

#include <algorithm>
#include <raylib.h>
#include <raymath.h>
#include <vector>
#include "TouchWidget.hpp"

class TouchAxisJoystick : public TouchWidget {
public:
    enum class Direction { Horizontal, Vertical };

private:
    static constexpr float TRACK_PAD = 1.25f;   // track half-thickness = knob radius * this

    // stored relative to the screen, converted to pixels on demand
    Vector2 m_base_position;        // track centre, 0..1
    Vector2 m_knob_position;        // 0..1

    float m_length;                 // fraction of the shorter side
    float m_knob_radius;            // fraction of the shorter side
    Direction m_direction;
    bool m_stable;                  // true: the knob stays where it was left when the finger lifts

    Vector2 AxisVector() const {
        return m_direction == Direction::Horizontal ? Vector2{ 1.0f, 0.0f } : Vector2{ 0.0f, 1.0f };
    }
    float LengthPx() const      { return m_length * Unit(); }
    float KnobRadiusPx() const  { return m_knob_radius * Unit(); }
    float TrackRadiusPx() const { return KnobRadiusPx() * TRACK_PAD; }
    // how far the knob centre can move from the track centre
    float HalfTravelPx() const  { return std::max(0.0f, LengthPx() * 0.5f - KnobRadiusPx()); }

    bool HitsTrack(Vector2 p) const;

    int m_priority = touch_priority_joystick;

public:
    TouchAxisJoystick(Vector2 position, float length, float knob_radius, Direction direction, bool stable = false) :
            m_base_position(position), m_knob_position(position),
            m_length(length), m_knob_radius(knob_radius), m_direction(direction), m_stable(stable) {}

    void SetStable(bool stable) { m_stable = stable; }
    bool IsStable() const { return m_stable; }

    void Update() override;
    void Draw() override;
    void Reset() override;
    
    float GetValue();

    Vector2 GetKnobPosition() { return ToPixels(m_knob_position); }
    Vector2 GetKnobPositionRelative() { return m_knob_position; }
};