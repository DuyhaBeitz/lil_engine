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

bool Brush::TryAdd(const Vector3 &p, const Quaternion &rot, const Vector3 &scale) {
    if (IsTooClose(p.x, p.z)) return false;

    int32_t cx, cz;
    WorldToCell(p.x, p.z, cell_size, cx, cz);
    uint64_t key = CellKey(cx, cz);

    Instances()->push_back(InstanceInfo(p, rot, scale, WHITE));
    instance_keys.push_back(key);

    grid[key].push_back(Instances()->size()-1);    
    return true;
}

void Brush::Remove(int i) {
    std::cout << "Removing " << i << std::endl;

    auto* instances = Instances();
    const int last = static_cast<int>(instances->size()) - 1;

    const uint64_t removed_key = instance_keys[i];

    // Remove i from its grid cell.
    auto old_it = grid.find(removed_key);
    if (old_it != grid.end()) {
        auto& cell = old_it->second;

        auto it = std::find(cell.begin(), cell.end(), i);
        if (it != cell.end())
            cell.erase(it);
    }

    if (i != last) {
        // Move the last instance into the removed slot.
        (*instances)[i] = std::move((*instances)[last]);

        // Move the last instance's key into the same slot.
        const uint64_t moved_key = instance_keys[last];
        instance_keys[i] = moved_key;

        // The grid still refers to `last`, so change it to `i`.
        auto affected_it = grid.find(moved_key);
        if (affected_it != grid.end()) {
            auto& affected_cell = affected_it->second;

            auto it = std::find(
                affected_cell.begin(),
                affected_cell.end(),
                last
            );

            if (it != affected_cell.end())
                *it = i;
        }
    }

    // Remove the duplicated last elements.
    instances->pop_back();
    instance_keys.pop_back();

    // Remove empty grid cell.
    auto it = grid.find(removed_key);
    if (it != grid.end() && it->second.empty())
        grid.erase(it);

    std::cout << "Removing DONE" << std::endl;
}

bool Brush::IsTooClose(float x, float z) const {
    int32_t cx, cz;
    WorldToCell(x, z, cell_size, cx, cz);
    for (int dz = -1; dz <= 1; ++dz)
        for (int dx = -1; dx <= 1; ++dx) {
            auto it = grid.find(CellKey(cx + dx, cz + dz));
            if (it == grid.end()) continue;
            for (uint32_t i : it->second) {
                float ddx = (*Instances())[i].position.x - x;
                float ddz = (*Instances())[i].position.z - z;
                if (ddx*ddx + ddz*ddz < min_distance*min_distance) return true;
            }
        }
    return false;
}

std::vector<InstanceInfo> *Brush::Instances() {
    return &(instanced_model->m_infos);
}

std::vector<InstanceInfo> *Brush::Instances() const {
    return &(instanced_model->m_infos);
}

float GetRandomFloat(float min, float max) {
    int precision = 1000000;
    return min + (max-min)* float(GetRandomValue(0, 1000000)) / float(precision);
}

bool Brush::Dab(float x, float z, Heightmap* heightmap) {
    float angle = GetRandomFloat(0.0f, 2*PI);
    float r     = std::sqrt(GetRandomFloat(0.0f, 1.0f)) * radius;
    float xx = x + r * cos(angle);
    float zz = z + r * sin(angle);
    HeightmapQueryResult query = heightmap->QueryAt(xx, zz);

    Vector3 axis = Vector3Lerp(Vector3{0.0f, 1.0f, 0.0f}, query.normal, slope_coeff);
    Quaternion q = QuaternionFromAxisAngle(axis, angle);

    float size = GetRandomFloat(min_size, max_size);

    return TryAdd(offset+Vector3{xx, query.height, zz}, q, Vector3{size, size, size});
}

void Painter::DrawBrush(Vector2 screen_pos, int render_w, int render_h, Camera camera, Heightmap* heightmap, InstancedModelComponent *instanced_model) {
    RayCollision res{0};
    Actor* pick = Lil::World().PickActor(screen_pos, render_w, render_h, camera, &res);

    auto it = m_brush_settings.find(instanced_model->GetID());
    if (it == m_brush_settings.end()) {
        Brush& brush = m_brush_settings[instanced_model->GetID()];
        brush.instanced_model = instanced_model;
        brush.instance_keys.reserve(instanced_model->m_infos.size());
        if (instanced_model->m_infos.size() > 0) {
            for (int i = 0; i < instanced_model->m_infos.size(); i++) {
                auto& info = instanced_model->m_infos[i];
                int32_t cx, cz;
                WorldToCell(info.position.x, info.position.z, brush.cell_size, cx, cz);
                uint64_t key = CellKey(cx, cz);
                brush.instance_keys.push_back(key);
                brush.grid[key].push_back(i); 
            }
        }
    }
    Brush& brush = m_brush_settings[instanced_model->GetID()];

    for (float dx = -1.0f; dx <= 1.0f; dx+=0.5f) {
        for (float dz = -1.0f; dz <= 1.0f; dz+=0.5f) {
            Vector3 p1 = res.point + Vector3{dx, 0, dz};
            HeightmapQueryResult query = heightmap->QueryAt(p1.x, p1.z);
            Vector3 p2 = p1 + query.normal * 3.0f;
            DrawLine3D(p1, p2, BLUE);
        }
    }

    float c = (IsKeyDown(KEY_RIGHT_BRACKET) - IsKeyDown(KEY_LEFT_BRACKET));
    brush.radius += c*brush.radius*GetFrameTime();
    if (brush.radius <= brush.min_distance) brush.radius = brush.min_distance;

    if (pick && res.hit) {
        DrawSphere(res.point, brush.radius, Fade(PURPLE, 0.2f));

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            bool placed = false;
            for (int i = 0; i < brush.iterations; i++) {
                placed |= brush.Dab(res.point.x, res.point.z, heightmap);
            }
            if (placed) instanced_model->Update();
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
                        Vector3 p = (*brush.Instances())[i].position;
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
            for (auto& i : to_remove) brush.Remove(i);
            if (to_remove.size() > 0) instanced_model->Update();
        }
    };
}
