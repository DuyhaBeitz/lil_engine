#include "TouchInput/TouchManager.hpp"
#include <algorithm>
#include <unordered_set>
#include <unordered_map>

namespace Lil::Touch {
    static std::unordered_set<int> prev_touches;
    static std::unordered_set<int> touches;
    static std::unordered_set<int> claimed;

    struct TouchRequest {
        TouchHitFunc hit_func;
        RequesterID requester_id;
        int priority;
    };

    static std::vector<TouchRequest> requests;
    static std::unordered_map<RequesterID, int> results;

    bool TouchExistedLastFrame(int id) {
        return prev_touches.contains(id);
    }

    bool IsTouchClaimed(int id) {
        return claimed.contains(id);
    }

    bool IsTouchActive(int id) {
        return touches.contains(id);
    }

    void UnclaimTouch(int id) {
        claimed.erase(id);
    }

    void RequestTouch(TouchHitFunc hit_func, int priority, RequesterID requester_id) {
        // Replace an existing request from the same requester.
        for (auto& request : requests) {
            if (request.requester_id == requester_id) {
                request.hit_func = std::move(hit_func);
                request.priority = priority;
                return;
            }
        }

        requests.push_back({
            std::move(hit_func),
            requester_id,
            priority
        });
    }


    int ReceiveTouch(RequesterID requester_id) {
        auto it = results.find(requester_id);

        if (it == results.end())
            return -1;

        return it->second;
    }

    void Update() {
        prev_touches = touches;

        touches.clear();
        const int count = GetTouchPointCount();
        touches.reserve(count);
        for (int i = 0; i < count; i++) {
            touches.insert(GetTouchPointId(i));
        }
        for (int prev_id : prev_touches) {
            // if touch was lost
            if (!touches.contains(prev_id)) UnclaimTouch(prev_id);
        }

        results.clear();
        std::stable_sort(
            requests.begin(),
            requests.end(),
            [](const TouchRequest& a, const TouchRequest& b) {
                return a.priority > b.priority;
            }
        );

        // Sort touch IDs for deterministic assignment.
        std::vector<int> sorted_touches(touches.begin(), touches.end());
        std::sort(sorted_touches.begin(), sorted_touches.end());

        for (const auto& request : requests) {
            int assigned_id = -1;

            for (int id : sorted_touches) {
                if (IsTouchClaimed(id)) continue;
                //if (TouchExistedLastFrame(id)) continue;

                if (request.hit_func(GetTouchPosition(id))) {
                    assigned_id = id;
                    claimed.insert(id);
                    break;
                }
            }

            results[request.requester_id] = assigned_id;
        }

        requests.clear();
    }

} // namespace Lil::Touch
