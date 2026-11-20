#pragma once
#include <raylib.h>
#include "Assets/AssetRef.hpp"
#include <functional>
#include <string>
#include <vector>
#include <unordered_set>

namespace Lil::Input {

    enum class MouseAxis    { X, Y };
    enum class GamepadStick { Left, Right };
    enum class ScreenAxis   { Horizontal, Vertical };

    // Which components a Vector2 modifier affects (flags: X = 1, Y = 2).
    enum class Axes : unsigned { X = 1, Y = 2, XY = 3 };
    using FloatModifier   = std::function<float(float)>;
    using Vector2Modifier = std::function<Vector2(Vector2)>;

    class Action : public Lil::Assets::AssetRef {
    public:
        using AssetRef::AssetRef;

        // Per-action switch. A disabled action reads as neutral (false / 0 / zero vector),
        // its touch widgets are released and hidden.
        void SetEnabled(bool enabled);
        bool IsEnabled() const;   // this action's own switch
        bool IsActive() const;    // enabled AND its context enabled, as of the last Update()
    };

    // ---------------------------------------------------------------- Bool
    class BoolAction : public Action {
    public:
        using Action::Action;

        BoolAction& BindKey(KeyboardKey key);
        BoolAction& BindMouseButton(MouseButton button);
        BoolAction& BindGamepadButton(GamepadButton button);
        BoolAction& BindGamepadAxis(GamepadAxis axis, float threshold = 0.5f);

        // Touch button with an icon (font must contain the glyph).
        BoolAction& BindTouchButton(Vector2 position, float radius, std::string icon, Font font);

        // True while a finger that landed in `area` is down. area = {x, y, w, h}, all 0..1 of the screen.
        BoolAction& BindTouchArea(Rectangle area);

        bool IsDown() const;
        bool IsPressed() const;
        bool IsReleased() const;
    };

    // --------------------------------------------------------------- Float
    // Value is the source with the largest magnitude (not a sum).
    class FloatAction : public Action {
    public:
        using Action::Action;

        FloatAction& BindKey(KeyboardKey key, float scale = 1.f);
        FloatAction& BindKeyPair(KeyboardKey negative, KeyboardKey positive);
        FloatAction& BindMouseButton(MouseButton button, float scale = 1.f);
        FloatAction& BindGamepadButton(GamepadButton button, float scale = 1.f);

        FloatAction& BindGamepadAxis(GamepadAxis axis, float scale = 1.f, float deadzone = 0.1f);
        FloatAction& BindMouseDelta(MouseAxis axis, float scale = 1.f);
        FloatAction& BindMouseWheel(float scale = 1.f);

        // Single-axis touch slider. Horizontal: right is +. Vertical: up is +.
        FloatAction& BindTouchAxis(Vector2 position, float length, float knob_radius, ScreenAxis direction, bool stable = false);

        // Modifiers: apply, in call order, to the binding added by the PREVIOUS Bind* call.
        // The parameters of the Bind* methods (scale, deadzone, ...) run first.
        //   walk.BindGamepadAxis(GAMEPAD_AXIS_LEFT_Y).Negate().Deadzone(0.2f)
        FloatAction& Negate();
        FloatAction& Scale(float factor);
        FloatAction& Deadzone(float lower, float upper = 1.f);   // below lower => 0, lower..upper => 0..1
        FloatAction& Clamp(float min, float max);
        FloatAction& ScaleByDeltaTime();                          // value * frame time
        FloatAction& With(FloatModifier modifier);                // custom

        float Value() const;
    };

    // ------------------------------------------------------------- Vector2
    class Vector2Action : public Action {
    public:
        using Action::Action;

        // Composite of four keys, e.g. WASD. Diagonals are normalized.
        Vector2Action& BindKeys(KeyboardKey up, KeyboardKey down, KeyboardKey left, KeyboardKey right);
        // Radial deadzone, length clamped to 1.
        Vector2Action& BindGamepadStick(GamepadStick stick, float deadzone = 0.15f);
        Vector2Action& BindMouseDelta(float sensitivity = 1.f);
        Vector2Action& BindMouseWheel(float scale = 1.f);

        // Floating touch joystick. Analog: length 0..1, pushed up => Y > 0.
        Vector2Action& BindTouchJoystick(Vector2 position, float base_radius, float knob_radius);

        // Touch area, e.g. camera look. area = {x, y, w, h}, all 0..1 of the screen (x, y = top-left).
        // Delta: finger movement this frame in pixels * sensitivity (like BindMouseDelta), dragging up => Y > 0.
        Vector2Action& BindTouchAreaDelta(Rectangle area, float sensitivity = 1.f);
        // Position of the finger inside the area, (0,0) = bottom-left, (1,1) = top-right; zero while not touching.
        Vector2Action& BindTouchAreaPosition(Rectangle area);

        // Modifiers: apply, in call order, to the binding added by the PREVIOUS Bind* call.
        // The parameters of the Bind* methods (sensitivity, deadzone, ...) run first.
        //   look.BindMouseDelta(0.15f).Negate(Axes::Y)
        //       .BindGamepadStick(GamepadStick::Right).Scale(300.f).ScaleByDeltaTime()
        Vector2Action& Negate(Axes axes = Axes::XY);
        Vector2Action& Scale(float factor);
        Vector2Action& Scale(float x, float y);
        Vector2Action& Swizzle();                                  // swap X and Y
        Vector2Action& Deadzone(float lower, float upper = 1.f);   // radial: below lower => 0, lower..upper => 0..1
        Vector2Action& ClampLength(float max = 1.f);
        Vector2Action& ScaleByDeltaTime();                         // value * frame time
        Vector2Action& With(Vector2Modifier modifier);             // custom

        Vector2 Value() const;
        float X() const;
        float Y() const;
    };

    // ------------------------------------------------------------ Registry
    // `context` groups actions so they can be switched together ("Gameplay", "Menu", "Touch", ...).
    // Action keys must be unique across all action types.
    void RegisterBoolAction(BoolAction action, const std::string& context = "Default");
    void RegisterFloatAction(FloatAction action, const std::string& context = "Default");
    void RegisterVector2Action(Vector2Action action, const std::string& context = "Default");

    // Contexts are enabled by default (including ones that were never mentioned).
    void SetContextEnabled(const std::string& context, bool enabled);
    bool IsContextEnabled(const std::string& context);

    // Once per frame, before querying actions. Also updates the touch widgets.
    void Update();
    // Draws the touch widgets of all active actions. Call after drawing your world.
    void Draw();

} // namespace Lil::Input