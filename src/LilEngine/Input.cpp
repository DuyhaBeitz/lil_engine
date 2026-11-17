#include "Input.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <raylib.h>
#include <raymath.h>
#include <type_traits>
#include "ScreenAxisJoystick.hpp"
#include "ScreenButton.hpp"
#include "ScreenJoystick.hpp"
#include "ScreenRectangle.hpp"

namespace Lil::Input {
namespace {

// -------------------------------------------------------------- Contexts

std::unordered_map<std::string, bool> contexts;

bool ContextEnabled(const std::string& name) {
    auto it = contexts.find(name);
    return it == contexts.end() || it->second;
}

// ---------------------------------------------------------- Touch widgets

struct TouchWidget {
    virtual ~TouchWidget() = default;
    // `claimed`: fingers held by foreground widgets this frame (only background widgets look at it)
    virtual void Update(const std::vector<int>& claimed) = 0;
    virtual void Draw()   = 0;
    virtual void Reset()  = 0;
    virtual int  TouchId() const = 0;                       // finger this widget holds, or -1
    // Background widgets (touch areas) only take fingers that no foreground widget holds.
    virtual bool IsBackground() const = 0;
};

template <class T>
struct TouchWidgetOf final : TouchWidget {
    static constexpr bool kBackground = std::is_same_v<T, ScreenRectangle>;

    T widget;
    template <class... Args>
    explicit TouchWidgetOf(Args&&... args) : widget(std::forward<Args>(args)...) {}

    void Update(const std::vector<int>& claimed) override {
        if constexpr (kBackground) widget.Update(claimed);
        else                       widget.Update();
    }
    void Draw()  override { widget.Draw(); }
    void Reset() override { widget.Reset(); }
    int  TouchId() const override { return widget.GetTouchId(); }
    bool IsBackground() const override { return kBackground; }
};

// Widgets live behind unique_ptr, so the returned pointer stays valid for the action's lifetime.
template <class T, class... Args>
T* AddWidget(std::vector<std::unique_ptr<TouchWidget>>& widgets, Args&&... args) {
    auto owned = std::make_unique<TouchWidgetOf<T>>(std::forward<Args>(args)...);
    T* raw = &owned->widget;
    widgets.push_back(std::move(owned));
    return raw;
}

// ------------------------------------------------------------ Action state

enum class Kind { Bool, Float, Vector2 };

struct ActionState {
    explicit ActionState(Kind k) : kind(k) {}
    virtual ~ActionState() = default;

    const Kind kind;
    std::string context = "Default";
    bool enabled = true;     // per-action switch
    bool active  = true;     // effective state as of the last Tick
    std::vector<std::unique_ptr<TouchWidget>> widgets;

    virtual void Evaluate(int gamepad) = 0;   // read sources
    virtual void Clear() = 0;                 // become neutral (disabled)
    virtual void OnActivated() {}             // disabled -> enabled transition

    // Frame step 1: decide whether we are active this frame.
    void Begin() {
        now = enabled && ContextEnabled(context);
        if (!now) {
            // Reset every frame while disabled: releases touches and forgets fingers that are
            // already down, so they cannot trigger anything the moment we are re-enabled.
            for (auto& w : widgets) w->Reset();
            Clear();
        }
    }

    // Frame step 3 (after the widgets were updated): read the sources.
    void Finish(int gamepad) {
        if (now) {
            if (!active) OnActivated();
            Evaluate(gamepad);
        }
        active = now;
    }

    bool now = true;         // effective state this frame (set by Begin)
};

// ------------------------------------------------------------------ Bool

struct AxisThreshold {
    GamepadAxis axis;
    float threshold;
};

struct BoolInfo final : ActionState {
    static constexpr Kind kKind = Kind::Bool;
    static constexpr const char* kName = "BoolAction";
    BoolInfo() : ActionState(kKind) {}

    std::unordered_set<KeyboardKey>    keys;
    std::unordered_set<MouseButton>    mouse_buttons;
    std::unordered_set<GamepadButton>  gamepad_buttons;
    std::vector<AxisThreshold>         gamepad_axes;
    std::vector<ScreenButton*>         screen_buttons;   // owned by `widgets`
    std::vector<ScreenRectangle*>      screen_rects;     // owned by `widgets`

    bool was_down = false;
    bool is_down  = false;
    bool latched  = false;   // after re-enable: ignore input until sources go idle once

    bool Poll(int gamepad) const {
        for (auto k : keys)          if (IsKeyDown(k)) return true;
        for (auto b : mouse_buttons) if (IsMouseButtonDown(b)) return true;
        for (auto* b : screen_buttons) if (b->IsDown()) return true;
        for (auto* r : screen_rects)   if (r->IsDown()) return true;

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

    void Evaluate(int gamepad) override {
        was_down = is_down;
        bool raw = Poll(gamepad);
        if (latched) {
            if (raw) raw = false;
            else     latched = false;
        }
        is_down = raw;
    }

    // Held while being disabled => IsReleased() fires once, then everything reads false.
    void Clear() override { was_down = is_down; is_down = false; }
    void OnActivated() override { latched = true; }
};

// ----------------------------------------------------------------- Float

struct FloatInfo final : ActionState {
    static constexpr Kind kKind = Kind::Float;
    static constexpr const char* kName = "FloatAction";
    FloatInfo() : ActionState(kKind) {}

    std::vector<std::function<float(int /*gamepad*/)>> sources;
    float value = 0.f;

    void Evaluate(int gamepad) override {
        float best = 0.f;
        for (auto& s : sources) {
            float v = s(gamepad);
            if (std::fabs(v) > std::fabs(best)) best = v;
        }
        value = best;
    }
    void Clear() override { value = 0.f; }
};

// --------------------------------------------------------------- Vector2

struct Vector2Info final : ActionState {
    static constexpr Kind kKind = Kind::Vector2;
    static constexpr const char* kName = "Vector2Action";
    Vector2Info() : ActionState(kKind) {}

    std::vector<std::function<Vector2(int /*gamepad*/)>> sources;
    Vector2 value{0.f, 0.f};

    void Evaluate(int gamepad) override {
        Vector2 best{0.f, 0.f};
        float best_len2 = 0.f;
        for (auto& s : sources) {
            Vector2 v = s(gamepad);
            float len2 = v.x * v.x + v.y * v.y;
            if (len2 > best_len2) { best = v; best_len2 = len2; }
        }
        value = best;
    }
    void Clear() override { value = {0.f, 0.f}; }
};

// --------------------------------------------------------------- Helpers

// Below `lower` => 0, `lower`..`upper` is remapped to 0..1, above `upper` => 1.
float ApplyDeadzone(float v, float lower, float upper = 1.f) {
    float a = std::fabs(v);
    if (a <= lower) return 0.f;
    float scaled = (a - lower) / std::max(upper - lower, 1e-6f);
    if (scaled > 1.f) scaled = 1.f;
    return v < 0.f ? -scaled : scaled;
}

Vector2 ApplyRadialDeadzone(Vector2 v, float lower, float upper = 1.f) {
    float len = std::sqrt(v.x * v.x + v.y * v.y);
    if (len <= lower) return {0.f, 0.f};
    float scaled = (len - lower) / std::max(upper - lower, 1e-6f);
    if (scaled > 1.f) scaled = 1.f;
    float k = scaled / len;
    return {v.x * k, v.y * k};
}

// One registry for every action type.
std::unordered_map<std::string, std::unique_ptr<ActionState>> actions;
int current_gamepad = 0;

ActionState& Find(const Action& a) {
    auto it = actions.find(a.Key());
    if (it == actions.end()) {
        std::cerr << "Action '" << a.Key() << "' is not registered\n";
        std::abort();   // or throw, or return a static dummy
    }
    return *it->second;
}

template <class Info>
Info& Lookup(const Action& a) {
    ActionState& s = Find(a);
    if (s.kind != Info::kKind) {
        std::cerr << "Action '" << a.Key() << "' is not a " << Info::kName << "\n";
        std::abort();
    }
    return static_cast<Info&>(s);
}

template <class Info>
void Register(const Action& a, const std::string& context) {
    auto [it, inserted] = actions.try_emplace(a.Key());
    if (!inserted) {
        std::cerr << "Action " << a.Key() << " already exists\n";
        return;
    }
    it->second = std::make_unique<Info>();
    it->second->context = context;
}

// Wraps the most recently added binding so `mod` runs on its output. Modifiers stack in call order.
// `mod` is called through a mutable wrapper, so it may keep state (smoothing, etc.).
template <class Info, class Mod>
void ModifyLast(Info& info, const Action& a, Mod mod) {
    if (info.sources.empty()) {
        std::cerr << "Action '" << a.Key() << "': a modifier needs a binding before it (call a Bind* method first)\n";
        std::abort();
    }
    auto& last = info.sources.back();
    last = [inner = std::move(last), mod = std::move(mod)](int gamepad) mutable { return mod(inner(gamepad)); };
}

} // anonymous namespace

// -------------------------------------------------------------- Registry

void RegisterBoolAction(BoolAction action, const std::string& context)       { Register<BoolInfo>(action, context); }
void RegisterFloatAction(FloatAction action, const std::string& context)     { Register<FloatInfo>(action, context); }
void RegisterVector2Action(Vector2Action action, const std::string& context) { Register<Vector2Info>(action, context); }

void SetContextEnabled(const std::string& context, bool enabled) { contexts[context] = enabled; }
bool IsContextEnabled(const std::string& context)                 { return ContextEnabled(context); }

void Update() {
    for (auto& [key, a] : actions) a->Begin();

    // Foreground widgets (joysticks, buttons) first: they grab the fingers that land on them.
    // Background widgets (touch areas) then only take fingers nobody else holds, so a camera
    // area underneath your joystick and buttons never steals their touches.
    std::vector<int> claimed;
    for (auto& [key, a] : actions) {
        if (!a->now) continue;
        for (auto& w : a->widgets) {
            if (w->IsBackground()) continue;
            w->Update(claimed);
            const int id = w->TouchId();
            if (id != -1) claimed.push_back(id);
        }
    }
    for (auto& [key, a] : actions) {
        if (!a->now) continue;
        for (auto& w : a->widgets)
            if (w->IsBackground()) w->Update(claimed);
    }

    for (auto& [key, a] : actions) a->Finish(current_gamepad);
}

void Draw() {
    for (auto& [key, a] : actions) {
        if (!a->active) continue;
        for (auto& w : a->widgets) w->Draw();
    }
}

// ---------------------------------------------------------------- Action

void Action::SetEnabled(bool enabled) { Find(*this).enabled = enabled; }
bool Action::IsEnabled() const        { return Find(*this).enabled; }
bool Action::IsActive() const         { return Find(*this).active; }

// ------------------------------------------------------------ BoolAction

BoolAction& BoolAction::BindKey(KeyboardKey key)               { Lookup<BoolInfo>(*this).keys.insert(key);                 return *this; }
BoolAction& BoolAction::BindMouseButton(MouseButton button)    { Lookup<BoolInfo>(*this).mouse_buttons.insert(button);     return *this; }
BoolAction& BoolAction::BindGamepadButton(GamepadButton b)     { Lookup<BoolInfo>(*this).gamepad_buttons.insert(b);        return *this; }
BoolAction& BoolAction::BindGamepadAxis(GamepadAxis a, float t){ Lookup<BoolInfo>(*this).gamepad_axes.push_back({a, t});   return *this; }

BoolAction& BoolAction::BindScreenButton(Vector2 position, float radius, std::string icon, Font font) {
    auto& info = Lookup<BoolInfo>(*this);
    info.screen_buttons.push_back(AddWidget<ScreenButton>(info.widgets, position, radius, std::move(icon), font));
    return *this;
}

BoolAction& BoolAction::BindScreenRectangle(Rectangle area) {
    auto& info = Lookup<BoolInfo>(*this);
    info.screen_rects.push_back(AddWidget<ScreenRectangle>(info.widgets, area));
    return *this;
}

bool BoolAction::IsDown() const     { auto& i = Lookup<BoolInfo>(*this); return i.is_down; }
bool BoolAction::IsPressed() const  { auto& i = Lookup<BoolInfo>(*this); return i.is_down && !i.was_down; }
bool BoolAction::IsReleased() const { auto& i = Lookup<BoolInfo>(*this); return !i.is_down && i.was_down; }

// ----------------------------------------------------------- FloatAction

FloatAction& FloatAction::BindKey(KeyboardKey key, float scale) {
    Lookup<FloatInfo>(*this).sources.push_back([=](int) { return IsKeyDown(key) ? scale : 0.f; });
    return *this;
}

FloatAction& FloatAction::BindKeyPair(KeyboardKey negative, KeyboardKey positive) {
    Lookup<FloatInfo>(*this).sources.push_back([=](int) {
        return (IsKeyDown(positive) ? 1.f : 0.f) - (IsKeyDown(negative) ? 1.f : 0.f);
    });
    return *this;
}

FloatAction& FloatAction::BindMouseButton(MouseButton button, float scale) {
    Lookup<FloatInfo>(*this).sources.push_back([=](int) { return IsMouseButtonDown(button) ? scale : 0.f; });
    return *this;
}

FloatAction& FloatAction::BindGamepadButton(GamepadButton button, float scale) {
    Lookup<FloatInfo>(*this).sources.push_back([=](int pad) {
        return IsGamepadAvailable(pad) && IsGamepadButtonDown(pad, button) ? scale : 0.f;
    });
    return *this;
}

FloatAction& FloatAction::BindGamepadAxis(GamepadAxis axis, float scale, float deadzone) {
    Lookup<FloatInfo>(*this).sources.push_back([=](int pad) {
        if (!IsGamepadAvailable(pad)) return 0.f;
        return ApplyDeadzone(GetGamepadAxisMovement(pad, axis), deadzone) * scale;
    });
    return *this;
}

FloatAction& FloatAction::BindMouseDelta(MouseAxis axis, float scale) {
    Lookup<FloatInfo>(*this).sources.push_back([=](int) {
        Vector2 d = GetMouseDelta();
        return (axis == MouseAxis::X ? d.x : d.y) * scale;
    });
    return *this;
}

FloatAction& FloatAction::BindMouseWheel(float scale) {
    Lookup<FloatInfo>(*this).sources.push_back([=](int) { return GetMouseWheelMove() * scale; });
    return *this;
}

FloatAction& FloatAction::BindScreenAxis(Vector2 position, float length, float knob_radius, ScreenAxis direction, bool stable) {
    auto& info = Lookup<FloatInfo>(*this);
    const auto dir = direction == ScreenAxis::Horizontal ? ScreenAxisJoystick::Direction::Horizontal
                                                         : ScreenAxisJoystick::Direction::Vertical;
    ScreenAxisJoystick* w = AddWidget<ScreenAxisJoystick>(info.widgets, position, length, knob_radius, dir, stable);
    info.sources.push_back([w](int) { return w->GetValue(); });
    return *this;

    // auto& info = Lookup<FloatInfo>(*this);
    // ScreenJoystick* w = AddWidget<ScreenJoystick>(info.widgets, position, length, knob_radius);
    // if (direction == ScreenAxis::Horizontal)
    //     info.sources.push_back([w](int) { return w->GetDirection().x; });
    // else 
    //     info.sources.push_back([w](int) { return w->GetDirection().y; });
    // return *this;
}

// ---- modifiers (apply to the binding added by the previous Bind* call)

FloatAction& FloatAction::With(FloatModifier modifier) {
    ModifyLast(Lookup<FloatInfo>(*this), *this, std::move(modifier));
    return *this;
}
FloatAction& FloatAction::Negate()            { return With([](float v) { return -v; }); }
FloatAction& FloatAction::Scale(float factor) { return With([=](float v) { return v * factor; }); }
FloatAction& FloatAction::Deadzone(float lower, float upper) {
    return With([=](float v) { return ApplyDeadzone(v, lower, upper); });
}
FloatAction& FloatAction::Clamp(float min, float max) {
    return With([=](float v) { return std::clamp(v, min, max); });
}
FloatAction& FloatAction::ScaleByDeltaTime()  { return With([](float v) { return v * GetFrameTime(); }); }

float FloatAction::Value() const { return Lookup<FloatInfo>(*this).value; }

// --------------------------------------------------------- Vector2Action

Vector2Action& Vector2Action::BindKeys(KeyboardKey up, KeyboardKey down, KeyboardKey left, KeyboardKey right) {
    Lookup<Vector2Info>(*this).sources.push_back([=](int) {
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
    Lookup<Vector2Info>(*this).sources.push_back([=](int pad) -> Vector2 {
        if (!IsGamepadAvailable(pad)) return {0.f, 0.f};
        // Stick pushed up => Y > 0 (the hardware reports up as negative, so negate it)
        return ApplyRadialDeadzone({GetGamepadAxisMovement(pad, ax), -GetGamepadAxisMovement(pad, ay)}, deadzone);
    });
    return *this;
}

Vector2Action& Vector2Action::BindMouseDelta(float sensitivity) {
    Lookup<Vector2Info>(*this).sources.push_back([=](int) {
        Vector2 d = GetMouseDelta();
        return Vector2{d.x * sensitivity, d.y * sensitivity};
    });
    return *this;
}

Vector2Action& Vector2Action::BindMouseWheel(float scale) {
    Lookup<Vector2Info>(*this).sources.push_back([=](int) {
        Vector2 w = GetMouseWheelMoveV();
        return Vector2{w.x * scale, w.y * scale};
    });
    return *this;
}

Vector2Action& Vector2Action::BindScreenJoystick(Vector2 position, float base_radius, float knob_radius) {
    auto& info = Lookup<Vector2Info>(*this);
    ScreenJoystick* w = AddWidget<ScreenJoystick>(info.widgets, position, base_radius, knob_radius);
    info.sources.push_back([w](int) { return w->GetDirection(); });
    return *this;
}

Vector2Action& Vector2Action::BindScreenRectangleDelta(Rectangle area, float sensitivity) {
    auto& info = Lookup<Vector2Info>(*this);
    ScreenRectangle* w = AddWidget<ScreenRectangle>(info.widgets, area);
    info.sources.push_back([w, sensitivity](int) {
        Vector2 d = w->GetDelta();
        return Vector2{d.x * sensitivity, d.y * sensitivity};
    });
    return *this;
}

Vector2Action& Vector2Action::BindScreenRectanglePosition(Rectangle area) {
    auto& info = Lookup<Vector2Info>(*this);
    ScreenRectangle* w = AddWidget<ScreenRectangle>(info.widgets, area);
    info.sources.push_back([w](int) { return w->GetPosition(); });
    return *this;
}

// ---- modifiers (apply to the binding added by the previous Bind* call)

Vector2Action& Vector2Action::With(Vector2Modifier modifier) {
    ModifyLast(Lookup<Vector2Info>(*this), *this, std::move(modifier));
    return *this;
}

Vector2Action& Vector2Action::Negate(Axes axes) {
    const float sx = (static_cast<unsigned>(axes) & 1u) ? -1.f : 1.f;
    const float sy = (static_cast<unsigned>(axes) & 2u) ? -1.f : 1.f;
    return With([=](Vector2 v) { return Vector2{v.x * sx, v.y * sy}; });
}

Vector2Action& Vector2Action::Scale(float factor) { return Scale(factor, factor); }
Vector2Action& Vector2Action::Scale(float x, float y) {
    return With([=](Vector2 v) { return Vector2{v.x * x, v.y * y}; });
}

Vector2Action& Vector2Action::Swizzle() {
    return With([](Vector2 v) { return Vector2{v.y, v.x}; });
}

Vector2Action& Vector2Action::Deadzone(float lower, float upper) {
    return With([=](Vector2 v) { return ApplyRadialDeadzone(v, lower, upper); });
}

Vector2Action& Vector2Action::ClampLength(float max) {
    return With([=](Vector2 v) {
        float len = std::sqrt(v.x * v.x + v.y * v.y);
        if (len > max && len > 0.f) { float k = max / len; v.x *= k; v.y *= k; }
        return v;
    });
}

Vector2Action& Vector2Action::ScaleByDeltaTime() {
    return With([](Vector2 v) { float dt = GetFrameTime(); return Vector2{v.x * dt, v.y * dt}; });
}

Vector2 Vector2Action::Value() const { return Lookup<Vector2Info>(*this).value; }
float   Vector2Action::X() const     { return Lookup<Vector2Info>(*this).value.x; }
float   Vector2Action::Y() const     { return Lookup<Vector2Info>(*this).value.y; }

} // namespace Lil::Input