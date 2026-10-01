#pragma once

#include "GameObject.hpp"
#include "Reflection.hpp"
#include <unordered_map>
#include "PoissonSampler2D.hpp"

struct Brush {
    float radius  = 1.0f;
    float density = 1.0f;
    PoissonSampler2D sampler;
    float min_rad = 3.0f;
    float max_rad = 4.0f;

    int grid_size = 128;
    float cell_size = 1.0f;
    std::unordered_map<int, float> density_map;
};

class InstancedModelComponent;
class Heightmap;

class Painter {
private:
    // keys are IDs of InstancedModelComponent
    std::unordered_map<uuids::uuid, Brush> m_brush_settings;

public:
    Painter() = default;

    void DrawBrush(Vector2 screen_pos, int render_w, int render_h, Camera camera, Heightmap* heightmap, InstancedModelComponent *instanced_model);
};
