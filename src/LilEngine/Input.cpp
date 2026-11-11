#include "Input.hpp"
#include <cmath>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace Lil::Input {
namespace {

// ------------------------------------------------------------------ Bool

struct AxisThreshold {
    GamepadAxis axis;
    float threshold;
};

struct BoolInfo {
    std::unordered_set<KeyboardKey>    keys;
    std::unordered_set<MouseButton>    mouse_buttons;
    std::unordered_set<GamepadButton>  gamepad_buttons;
    std::vector<AxisThreshold>         gamepad_axes;
    bool was_down = false;
    bool is_down  = false;

    bool IsDown() const     { return is_down; }
    bool IsPressed() const  { return is_down && !was_down; }
    bool IsReleased() const { return !is_down && was_down; }

    bool Evaluate(int gamepad) const {
        for (auto k : keys)          if (IsKeyDown(k)) return true;
        for (auto b : mouse_buttons) if (IsMouseButtonDown(b)) return true;

        if (IsGamepadAvailable(gamepad)) {
            for (auto b : gamepad_buttons)
                if (IsGamepadButtonDown(gamepad, b)) return true;
            for (auto [axis, threshold] : gamepad_axes) {
                float v = GetGamepadAxisMovement(gamepad, axis);
                if (threshold >= 0.f ? v > threshold : v < threshold) return true;
            }
        }
        return false;
    }

    void Update(int gamepad) {
        was_down = is_down;
        is_down  = Evaluate(gamepad);
    }
};

// ----------------------------------------------------------------- Float

using FloatSource = std::function<float(int /*gamepad*/)>;

struct FloatInfo {
    std::vector<FloatSource> sources;
    float value = 0.f;

    void Update(int gamepad) {
        float best = 0.f;
        for (auto& s : sources) {
            float v = s(gamepad);
            if (std::fabs(v) > std::fabs(best)) best = v;
        }
        value = best;
    }
};

// --------------------------------------------------------------- Vector2

using Vec2Source = std::function<Vector2(int /*gamepad*/)>;

struct Vector2Info {
    std::vector<Vec2Source> sources;
    Vector2 value{0.f, 0.f};

    void Update(int gamepad) {
        Vector2 best{0.f, 0.f};
        float best_len2 = 0.f;
        for (auto& s : sources) {
            Vector2 v = s(gamepad);
            float len2 = v.x * v.x + v.y * v.y;
            if (len2 > best_len2) { best = v; best_len2 = len2; }
        }
        value = best;
    }
};

// --------------------------------------------------------------- Helpers

float ApplyDeadzone(float v, float deadzone) {
    float a = std::fabs(v);
    if (a <= deadzone) return 0.f;
    float scaled = (a - deadzone) / (1.f - deadzone);
    return v < 0.f ? -scaled : scaled;
}

Vector2 ApplyRadialDeadzone(Vector2 v, float deadzone) {
    float len = std::sqrt(v.x * v.x + v.y * v.y);
    if (len <= deadzone) return {0.f, 0.f};
    float scaled = (len - deadzone) / (1.f - deadzone);
    if (scaled > 1.f) scaled = 1.f;
    float k = scaled / len;
    return {v.x * k, v.y * k};
}

std::unordered_map<std::string, BoolInfo> bool_actions;
std::unordered_map<std::string, FloatInfo>          float_actions;
std::unordered_map<std::string, Vector2Info>        vector2_actions;
int current_gamepad = 0;

template <class Info>
Info& Lookup(std::unordered_map<std::string, Info>& map, const char* kind, const Action& a) {
    auto it = map.find(a.Key());
    if (it == map.end()) {
        std::cerr << kind << " '" << a.Key() << "' is not registered\n";
    }
    return it->second;
}

BoolInfo& BoolInfo(const Action& a)  { return Lookup(bool_actions,    "BoolAction",    a); }
FloatInfo&          FloatOf(const Action& a)   { return Lookup(float_actions,   "FloatAction",   a); }
Vector2Info&        Vec2Of(const Action& a)    { return Lookup(vector2_actions, "Vector2Action", a); }

template <class Info>
void Register(std::unordered_map<std::string, Info>& map, const char* kind, const Action& a) {
    auto [it, inserted] = map.try_emplace(a.Key());
    if (!inserted)
        std::cerr << kind << " " << a.Key() << " already exists\n";
}

} // anonymous namespace

// -------------------------------------------------------------- Register

void RegisterBoolAction(BoolAction action)       { Register(bool_actions,    "BoolAction",    action); }
void RegisterFloatAction(FloatAction action)     { Register(float_actions,   "FloatAction",   action); }
void RegisterVector2Action(Vector2Action action) { Register(vector2_actions, "Vector2Action", action); }

void Update() {
    for (auto& [name, info] : bool_actions)    info.Update(current_gamepad);
    for (auto& [name, info] : float_actions)   info.Update(current_gamepad);
    for (auto& [name, info] : vector2_actions) info.Update(current_gamepad);
}

// ------------------------------------------------------------ BoolAction

BoolAction& BoolAction::BindKey(KeyboardKey key)               { BoolInfo(*this).keys.insert(key);                 return *this; }
BoolAction& BoolAction::BindMouseButton(MouseButton button)    { BoolInfo(*this).mouse_buttons.insert(button);     return *this; }
BoolAction& BoolAction::BindGamepadButton(GamepadButton b)     { BoolInfo(*this).gamepad_buttons.insert(b);        return *this; }
BoolAction& BoolAction::BindGamepadAxis(GamepadAxis a, float t){ BoolInfo(*this).gamepad_axes.push_back({a, t});   return *this; }

bool BoolAction::IsDown() const     { return BoolInfo(*this).IsDown(); }
bool BoolAction::IsPressed() const  { return BoolInfo(*this).IsPressed(); }
bool BoolAction::IsReleased() const { return BoolInfo(*this).IsReleased(); }

// ----------------------------------------------------------- FloatAction

FloatAction& FloatAction::BindKey(KeyboardKey key, float scale) {
    FloatOf(*this).sources.push_back([=](int) { return IsKeyDown(key) ? scale : 0.f; });
    return *this;
}

FloatAction& FloatAction::BindKeyPair(KeyboardKey negative, KeyboardKey positive) {
    FloatOf(*this).sources.push_back([=](int) {
        return (IsKeyDown(positive) ? 1.f : 0.f) - (IsKeyDown(negative) ? 1.f : 0.f);
    });
    return *this;
}

FloatAction& FloatAction::BindMouseButton(MouseButton button, float scale) {
    FloatOf(*this).sources.push_back([=](int) { return IsMouseButtonDown(button) ? scale : 0.f; });
    return *this;
}

FloatAction& FloatAction::BindGamepadButton(GamepadButton button, float scale) {
    FloatOf(*this).sources.push_back([=](int pad) {
        return IsGamepadAvailable(pad) && IsGamepadButtonDown(pad, button) ? scale : 0.f;
    });
    return *this;
}

FloatAction& FloatAction::BindGamepadAxis(GamepadAxis axis, float scale, float deadzone) {
    FloatOf(*this).sources.push_back([=](int pad) {
        if (!IsGamepadAvailable(pad)) return 0.f;
        return ApplyDeadzone(GetGamepadAxisMovement(pad, axis), deadzone) * scale;
    });
    return *this;
}

FloatAction& FloatAction::BindMouseDelta(MouseAxis axis, float scale) {
    FloatOf(*this).sources.push_back([=](int) {
        Vector2 d = GetMouseDelta();
        return (axis == MouseAxis::X ? d.x : d.y) * scale;
    });
    return *this;
}

FloatAction& FloatAction::BindMouseWheel(float scale) {
    FloatOf(*this).sources.push_back([=](int) { return GetMouseWheelMove() * scale; });
    return *this;
}

float FloatAction::Value() const { return FloatOf(*this).value; }

// --------------------------------------------------------- Vector2Action

Vector2Action& Vector2Action::BindKeys(KeyboardKey up, KeyboardKey down, KeyboardKey left, KeyboardKey right) {
    Vec2Of(*this).sources.push_back([=](int) {
        Vector2 v{
            (IsKeyDown(right) ? 1.f : 0.f) - (IsKeyDown(left) ? 1.f : 0.f),
            (IsKeyDown(up)    ? 1.f : 0.f) - (IsKeyDown(down) ? 1.f : 0.f)
        };
        float len2 = v.x * v.x + v.y * v.y;
        if (len2 > 1.f) {
            float inv = 1.f / std::sqrt(len2);
            v.x *= inv;
            v.y *= inv;
        }
        return v;
    });
    return *this;
}

Vector2Action& Vector2Action::BindGamepadStick(GamepadStick stick, float deadzone) {
    const GamepadAxis ax = stick == GamepadStick::Left ? GAMEPAD_AXIS_LEFT_X : GAMEPAD_AXIS_RIGHT_X;
    const GamepadAxis ay = stick == GamepadStick::Left ? GAMEPAD_AXIS_LEFT_Y : GAMEPAD_AXIS_RIGHT_Y;
    Vec2Of(*this).sources.push_back([=](int pad) -> Vector2 {
        if (!IsGamepadAvailable(pad)) return {0.f, 0.f};
        // the hardware reports up as negative, so negating it
        return ApplyRadialDeadzone({GetGamepadAxisMovement(pad, ax), -GetGamepadAxisMovement(pad, ay)}, deadzone);
    });
    return *this;
}

Vector2Action& Vector2Action::BindMouseDelta(float sensitivity) {
    Vec2Of(*this).sources.push_back([=](int) {
        Vector2 d = GetMouseDelta();
        return Vector2{d.x * sensitivity, d.y * sensitivity};
    });
    return *this;
}

Vector2Action& Vector2Action::BindMouseWheel(float scale) {
    Vec2Of(*this).sources.push_back([=](int) {
        Vector2 w = GetMouseWheelMoveV();
        return Vector2{w.x * scale, w.y * scale};
    });
    return *this;
}

Vector2 Vector2Action::Value() const { return Vec2Of(*this).value; }
float   Vector2Action::X() const     { return Vec2Of(*this).value.x; }
float   Vector2Action::Y() const     { return Vec2Of(*this).value.y; }

} // namespace Lil::Input