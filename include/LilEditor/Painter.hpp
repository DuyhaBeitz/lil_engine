#pragma once

#include "GameObject.hpp"
#include "Reflection.hpp"
#include <unordered_map>

class InstancedModelComponent;
class Heightmap;

struct InstanceInfo {
    Vector3    position;
    Quaternion rotation;
    Vector3    scale;
    Color      color;
    uint64_t   cell_key;
};

struct Brush : public Reflectable {
    LIL_REFLECTABLE()
    float radius  = 5.0f;
    float min_distance = 2.0f;
    uint iterations = 1;

    void Update(InstancedModelComponent *instanced_model);
    bool TryAdd(const Vector3& p, const Quaternion& rot, const Vector3& scale);
    void Remove(int i);
    bool Dab(float x, float z, Heightmap* heightmap);
    bool IsTooClose(float x, float z) const;

    std::vector<InstanceInfo> instances{};

    float cell_size = 5.0f;       // >= min_distance
    std::unordered_map<uint64_t, std::vector<int>> grid; // holds vector of indexes pointing to parallel vectors
};
LIL_REFLECT(Brush, bases<>,
    field(radius),
    field(min_distance),
    field(iterations)
)

class Painter {
private:
    // keys are IDs of InstancedModelComponent
    std::unordered_map<uuids::uuid, Brush> m_brush_settings;
    Vector2 m_prev_pos = {0.0f, 0.0f};

public:
    Painter() = default;

    void DrawBrush(Vector2 screen_pos, int render_w, int render_h, Camera camera, Heightmap* heightmap, InstancedModelComponent *instanced_model);
    Brush& GetBrush(InstancedModelComponent *instanced_model);
};
