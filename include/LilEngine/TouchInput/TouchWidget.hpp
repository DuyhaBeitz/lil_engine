#pragma once

#include <algorithm>
#include <vector>
#include <raylib.h>
#include <raymath.h>

constexpr int touch_priority_area = 0;
constexpr int touch_priority_joystick = 5;
constexpr int touch_priority_button = 10;

class TouchWidget {
protected:
    int m_touch_id = -1;

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

public:
    virtual ~TouchWidget() = default;

    virtual void Update() = 0;
    virtual void Draw() = 0;
    // Releases the finger and treats every finger that is down right now as "already down".
    virtual void Reset() = 0;

    int GetTouchId() const { return m_touch_id; }
    bool IsActive() const  { return m_touch_id != -1; }
};