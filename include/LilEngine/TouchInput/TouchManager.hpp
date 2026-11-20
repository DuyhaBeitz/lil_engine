#pragma once

#include <raylib.h>
#include <raymath.h>
#include <functional>

namespace Lil::Touch {
    using TouchHitFunc = std::function<bool(Vector2)>;
    using RequesterID = const void*;

    bool TouchExistedLastFrame(int id);
    bool IsTouchClaimed(int id);
    bool IsTouchActive(int id);

    void UnclaimTouch(int id);
    void RequestTouch(TouchHitFunc hit_func, int priority, RequesterID requester_id);
    // returns -1 if request failed
    int ReceiveTouch(RequesterID requester_id);

    void Update();
}; // namespace Lil::Touch
