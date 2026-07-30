#pragma once

#include <vector>
#include <string>
#include <chrono>

#include "LilEngine.hpp"

struct Notification {
    std::string message;
    float lifetime;
    float maxLifetime;
    ImVec4 color;

    Notification(const std::string& msg, float duration, const ImVec4& col)
        : message(msg), lifetime(duration), maxLifetime(duration), color(col) {}
};


inline const ImVec4 NOTIFY_INFO    = ImVec4(0.8f, 0.8f, 1.0f, 1.0f);
inline const ImVec4 NOTIFY_SUCCESS = ImVec4(0.3f, 0.9f, 0.3f, 1.0f);
inline const ImVec4 NOTIFY_WARNING = ImVec4(0.9f, 0.8f, 0.2f, 1.0f);
inline const ImVec4 NOTIFY_ERROR   = ImVec4(0.9f, 0.3f, 0.3f, 1.0f);