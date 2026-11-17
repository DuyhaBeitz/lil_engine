#pragma once

#include <algorithm>
#include <raylib.h>
#include <raymath.h>

/*
 * One-directional (single axis) floating touch joystick, laid out relative to
 * the screen like Joystick / CircleButton:
 *
 *   position    : centre, 0..1 of screen width / height
 *   length      : full length of the track, fraction of the SHORTER screen side
 *   knob_radius : fraction of the SHORTER screen side
 *   direction   : Horizontal or Vertical
 *
 *   ScreenAxisJoystick throttle({ 0.92f, 0.5f }, 0.45f, 0.05f, ScreenAxisJoystick::Direction::Vertical);
 *   ScreenAxisJoystick steer   ({ 0.2f, 0.85f }, 0.40f, 0.05f, ScreenAxisJoystick::Direction::Horizontal);
 *
 *   throttle.Update();
 *   float gas = throttle.GetValue();     // -1 (down) .. 0 .. +1 (up)
 *   steer.Update();
 *   float s = steer.GetValue();          // -1 (left) .. 0 .. +1 (right)
 *   throttle.Draw();
 *
 * The track is a fixed rectangle and the knob slides inside it along the axis.
 * A touch that lands on the track grabs the knob. Lifting the finger returns
 * the knob to the centre, unless the joystick is "stable" (last constructor
 * argument / SetStable), in which case the knob stays where it was left.
 */
class ScreenAxisJoystick {
public:
    enum class Direction { Horizontal, Vertical };

private:
    static constexpr float TRACK_PAD = 1.25f;   // track half-thickness = knob radius * this

    // stored relative to the screen, converted to pixels on demand
    Vector2 m_original_position;    // track centre, 0..1
    Vector2 m_base_position;        // track centre, 0..1
    Vector2 m_knob_position;        // 0..1

    float m_length;                 // fraction of the shorter side
    float m_knob_radius;            // fraction of the shorter side
    Direction m_direction;
    bool m_stable;                  // true: the knob stays where it was left when the finger lifts

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

    Vector2 AxisVector() const {
        return m_direction == Direction::Horizontal ? Vector2{ 1.0f, 0.0f } : Vector2{ 0.0f, 1.0f };
    }
    float LengthPx() const     { return m_length * Unit(); }
    float KnobRadiusPx() const { return m_knob_radius * Unit(); }
    float TrackRadiusPx() const { return KnobRadiusPx() * TRACK_PAD; }
    // how far the knob centre can move from the track centre
    float HalfTravelPx() const { return std::max(0.0f, LengthPx() * 0.5f - KnobRadiusPx()); }

    bool HitsTrack(Vector2 p) const;

public:
    ScreenAxisJoystick(Vector2 position, float length, float knob_radius, Direction direction, bool stable = false) :
            m_original_position(position), m_base_position(position), m_knob_position(position),
            m_length(length), m_knob_radius(knob_radius), m_direction(direction), m_stable(stable) {}

    void Reset(){
        m_touch_id = -1;
        m_base_position = m_original_position;
        m_knob_position = m_original_position;
    }

    void SetStable(bool stable) { m_stable = stable; }
    bool IsStable() const { return m_stable; }

    void Update();
    void Draw();

    // -1..+1. Horizontal: right is positive. Vertical: UP is positive.
    float GetValue();
    bool IsActive() const { return m_touch_id != -1; }
    int GetTouchId() const { return m_touch_id; }   // finger held by this widget, or -1

    // Knob centre in pixels / relative to the screen (0..1).
    Vector2 GetKnobPosition() { return ToPixels(m_knob_position); }
    Vector2 GetKnobPositionRelative() { return m_knob_position; }
};