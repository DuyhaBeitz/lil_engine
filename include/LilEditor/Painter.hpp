#pragma once

#include "GameObject.hpp"
#include "Reflection.hpp"
#include <unordered_map>
#include "Serialization.hpp"

class InstancedModelComponent;
class Heightmap;
class InstanceInfo;

struct Brush : public Reflectable {
    LIL_REFLECTABLE()
    float radius  = 5.0f;
    float min_distance = 2.0f;
    uint iterations = 1;

    bool TryAdd(const Vector3& p, const Quaternion& rot, const Vector3& scale);
    void Remove(int i);
    bool Dab(float x, float z, Heightmap* heightmap);
    bool IsTooClose(float x, float z) const;

    float min_size = 1.0f;
    float max_size = 1.0f;

    float slope_coeff = 0.0f;
    Vector3 offset = Vector3{0.0f, 0.0f, 0.0f};

    std::vector<uint64_t> instance_keys;

    float cell_size = 5.0f;       // >= min_distance
    std::unordered_map<uint64_t, std::vector<int>> grid; // holds vector of indexes pointing to parallel vectors
    InstancedModelComponent *instanced_model;
    std::vector<InstanceInfo>* Instances();
    std::vector<InstanceInfo>* Instances() const;

    uuids::uuid get_instance_model_id() const;
    void construct_from_instance();
    void set_instanced_model(uuids::uuid id);

    template <class Archive>
    void save( Archive & ar ) const {
        ar(radius, min_distance, iterations, min_size, max_size, slope_coeff, offset, cell_size);
        ar(get_instance_model_id());
    }
       
    template <class Archive>
    void load( Archive & ar ) {
        ar(radius, min_distance, iterations, min_size, max_size, slope_coeff, offset, cell_size);

        uuids::uuid id;
        ar(id);
        set_instanced_model(id);
        construct_from_instance();        
    }
};
LIL_REFLECT(Brush, bases<>,
    field(radius),
    field(min_distance),
    field(iterations),
    field(min_size),
    field(max_size),
    field(slope_coeff),
    field(offset)
)
LIL_DISAMBIGUATE_LOAD_SAVE(Brush)

class Painter {
private:
    // keys are IDs of InstancedModelComponent
    std::unordered_map<uuids::uuid, Brush> m_brush_settings;
    Vector2 m_prev_pos = {0.0f, 0.0f};

public:
    LIL_SERIALIZABLE()

    Painter() = default;

    void DrawBrush(Vector2 screen_pos, int render_w, int render_h, Camera camera, Heightmap* heightmap, InstancedModelComponent *instanced_model);
    Brush& GetBrush(InstancedModelComponent *instanced_model);
};
LIL_SER_BEGIN_NO_POLYMORPH(Painter)
LIL_SER_FIELD(m_brush_settings)
LIL_SER_END()