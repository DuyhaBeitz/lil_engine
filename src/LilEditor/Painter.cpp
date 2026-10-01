#include "Painter.hpp"
#include "Components/InstancedModelComponent.hpp"
#include "LilEngine.hpp"
#include "Heightmap.hpp"

void Painter::DrawBrush(Vector2 screen_pos, int render_w, int render_h, Camera camera, Heightmap* heightmap, InstancedModelComponent *instanced_model) {
    RayCollision res{0};
    Actor* pick = Lil::World().PickActor(screen_pos, render_w, render_h, camera, &res);
    if (pick && res.hit) {
        DrawSphere(res.point, 3.0f, Fade(RED, 0.5f));

        Brush brush = m_brush_settings[instanced_model->GetID()];
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            int gx = int(res.point.x / brush.cell_size);
            int gy = int(res.point.y / brush.cell_size);

            int r = brush.radius / brush.cell_size;
            for (int x = gx - r; x < gx + r; x++) {
                for (int y = gy - r; y < gy + r; y++) {
                    int dx = (x-gx);
                    int dy = (y-gy);
                    int dd = dx*dx + dy*dy;
                    if (dd > r*r) continue;
                    brush.density_map[x + brush.cell_size * y] = brush.density;
                }
            }

            instanced_model->ClearInstances();

            std::vector<Sample> samples = brush.sampler.PoissonDisk(
                brush.grid_size, brush.grid_size, brush.min_rad, brush.max_rad, [&brush](Vector2 p){
                    int x = p.x - brush.grid_size/2;
                    int y = p.y - brush.grid_size/2;
                    if (x > 0) return brush.min_rad;
                    return brush.max_rad;
                    float res = (x*x+y*y);
                    return Clamp(res, brush.min_rad, brush.max_rad);
                    //return brush.min_rad;
                }
            );

            for (auto& s : samples) {
                float x = (s.position.x / brush.grid_size -0.5f) * heightmap->GetScale().x + heightmap->GetPosition().x;
                float z = (s.position.y / brush.grid_size -0.5f) * heightmap->GetScale().z + heightmap->GetPosition().z;
                instanced_model->AddInstance(Transform{
                    .translation = Vector3{
                        x,
                        heightmap->GetHeightAt(x, z),
                        z
                    },
                    .rotation = QuaternionIdentity(),
                    .scale = {1, 1, 1}
                });
            }            
        }
    }
}
