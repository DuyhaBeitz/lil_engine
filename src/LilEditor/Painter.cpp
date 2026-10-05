#include "Painter.hpp"
#include "Components/InstancedModelComponent.hpp"
#include "LilEngine.hpp"
#include "Heightmap.hpp"

Brush &Painter::GetBrush(InstancedModelComponent *instanced_model) {
    return m_brush_settings[instanced_model->GetID()];
}

uint64_t CellKey(int32_t cx, int32_t cz) {
    return (uint64_t(uint32_t(cz)) << 32) | uint64_t(uint32_t(cx));
}

void WorldToCell(float x, float z, float cell_size, int32_t& cx, int32_t& cz) {
    cx = static_cast<int32_t>(std::floor(x / cell_size));
    cz = static_cast<int32_t>(std::floor(z / cell_size));
}

void Brush::Update(InstancedModelComponent *instanced_model) {
    instanced_model->Resize(instances.size());
    if (instances.empty()) return;
    instanced_model->MapInstances();

    for (int i = 0; i < instances.size(); i++) {
        const auto& instance = instances[i];

        instanced_model->SetInstancePosition(instance.position, i);
        instanced_model->SetInstanceRotation(instance.rotation, i);
        instanced_model->SetInstanceScale(instance.scale, i);
        instanced_model->SetInstanceColor(instance.color, i);
    }

    instanced_model->UnmapInstances();
}

bool Brush::TryAdd(const Vector3 &p, const Quaternion &rot, const Vector3 &scale) {
    if (IsTooClose(p.x, p.z)) return false;

    int32_t cx, cz;
    WorldToCell(p.x, p.z, cell_size, cx, cz);
    uint64_t key = CellKey(cx, cz);

    instances.push_back(InstanceInfo{
        .position = p,
        .rotation = rot,
        .scale    = scale,
        .color    = WHITE,
        .cell_key = key
    });

    grid[key].push_back(instances.size()-1);
    return true;
}

void Brush::Remove(int i) {
    const int last = static_cast<int>(instances.size()) - 1;

    const uint64_t removed_key = instances[i].cell_key;

    auto old_it = grid.find(removed_key);
    if (old_it != grid.end()) {
        auto& cell = old_it->second;

        auto it = std::find(cell.begin(), cell.end(), i);
        if (it != cell.end())
            cell.erase(it);
    }

    if (i != last) {
        const uint64_t affected_cell_key = instances[last].cell_key;
        instances[i] = std::move(instances[last]);

        auto affected_it = grid.find(affected_cell_key);
        if (affected_it != grid.end()) {
            auto& affected_cell = affected_it->second;

            auto it = std::find(affected_cell.begin(), affected_cell.end(), last);
            if (it != affected_cell.end()) *it = i;
        }
    }

    instances.pop_back();

    // Remove empty cell.
    auto it = grid.find(removed_key);
    if (it != grid.end() && it->second.empty()) grid.erase(it);
}

bool Brush::IsTooClose(float x, float z) const {
    int32_t cx, cz;
    WorldToCell(x, z, cell_size, cx, cz);
    for (int dz = -1; dz <= 1; ++dz)
        for (int dx = -1; dx <= 1; ++dx) {
            auto it = grid.find(CellKey(cx + dx, cz + dz));
            if (it == grid.end()) continue;
            for (uint32_t i : it->second) {
                float ddx = instances[i].position.x - x;
                float ddz = instances[i].position.z - z;
                if (ddx*ddx + ddz*ddz < min_distance*min_distance) return true;
            }
        }
    return false;
}

float GetRandomFloat(float min, float max) {
    int precision = 1000000;
    return min + (max-min)* float(GetRandomValue(0, 1000000)) / float(precision);
}

bool Brush::Dab(float x, float z, Heightmap* heightmap) {
    float angle = GetRandomFloat(0.0f, 2*PI);
    float r     = std::sqrt(GetRandomFloat(0.0f, 1.0f)) * radius;
    Quaternion q = QuaternionFromAxisAngle(Vector3{0.0f, 1.0f, 0.0f}, angle);
    float xx = x + r * cos(angle);
    float zz = z + r * sin(angle);
    return TryAdd(Vector3{xx, heightmap->GetHeightAt(xx, zz), zz}, q, Vector3{1.0f, 1.0f, 1.0f});
}

void Painter::DrawBrush(Vector2 screen_pos, int render_w, int render_h, Camera camera, Heightmap* heightmap, InstancedModelComponent *instanced_model) {
    RayCollision res{0};
    Actor* pick = Lil::World().PickActor(screen_pos, render_w, render_h, camera, &res);
    if (pick && res.hit) {
        Brush& brush = GetBrush(instanced_model);
        DrawSphereWires(res.point, brush.radius, 12, 12, Fade(RED, 0.5f));

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            if (brush.Dab(res.point.x, res.point.z, heightmap)) {
                brush.Update(instanced_model);
            }
        }
        else if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
            int32_t cx, cz;
            WorldToCell(res.point.x, res.point.z, brush.cell_size, cx, cz);
            const int cell_radius = static_cast<int>(
                std::ceil(brush.radius / brush.cell_size)
            );
            const float radius_sq = brush.radius * brush.radius;

            std::vector<int> to_remove{};

            for (int dz = -cell_radius; dz <= cell_radius; ++dz) {
                for (int dx = -cell_radius; dx <= cell_radius; ++dx) {
                    const uint64_t key = CellKey(cx + dx, cz + dz);
                    auto it = brush.grid.find(key);
                    if (it == brush.grid.end()) continue;

                    for (auto& i : it->second) {
                        Vector3 p = brush.instances[i].position;
                        const float ddx = p.x - res.point.x;
                        const float ddz = p.z - res.point.z;
                        if (ddx*ddx + ddz*ddz < radius_sq) {
                            to_remove.push_back(i);
                        }
                    }
                }
            }

            // sort descending because erasing moves indexes and dedup
            std::sort(to_remove.rbegin(), to_remove.rend());
            to_remove.erase(std::unique(to_remove.begin(), to_remove.end()), to_remove.end());
            for (auto& i : to_remove) {
                brush.Remove(i);
            }
            if (to_remove.size() > 0) brush.Update(instanced_model);
        }
        brush.Update(instanced_model);
    };
}
