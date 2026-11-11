#pragma once
#include "Assets/AssetRef.hpp"
#include <raylib.h>
#include <vector>
#include <unordered_set>

namespace Lil::Input {

    enum class MouseAxis   { X, Y };
    enum class GamepadStick { Left, Right };

    class Action : public Lil::Assets::AssetRef {
    public:
        using AssetRef::AssetRef;
    };

    class BoolAction : public Action {
    public:
        using Action::Action;

        BoolAction& BindKey(KeyboardKey key);
        BoolAction& BindMouseButton(MouseButton button);
        BoolAction& BindGamepadButton(GamepadButton button);
        BoolAction& BindGamepadAxis(GamepadAxis axis, float threshold = 0.5f);

        bool IsDown() const;
        bool IsPressed() const;
        bool IsReleased() const;
    };

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

        float Value() const;
    };

    class Vector2Action : public Action {
    public:
        using Action::Action;

        Vector2Action& BindKeys(KeyboardKey up, KeyboardKey down, KeyboardKey left, KeyboardKey right);
        Vector2Action& BindGamepadStick(GamepadStick stick, float deadzone = 0.15f);
        Vector2Action& BindMouseDelta(float sensitivity = 1.f);
        Vector2Action& BindMouseWheel(float scale = 1.f);

        Vector2 Value() const;
        float X() const;
        float Y() const;
    };

    void RegisterBoolAction(BoolAction action);
    void RegisterFloatAction(FloatAction action);
    void RegisterVector2Action(Vector2Action action);

    void Update();

} // namespace Lil::Input