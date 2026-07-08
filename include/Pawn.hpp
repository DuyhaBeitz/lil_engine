#pragma once

#include "Actor.hpp"

// pawn is an actor with a 3D model
class Pawn : public Actor {
private:
    std::string m_model_key;
    Vector3 m_model_local_offset;

public:
    Pawn(rc::BodyType body_type = rc::BodyType::DYNAMIC)
    : Actor(body_type) {}

    void Draw();

    void SetModel(std::string model_key, Vector3 local_offset = {0, 0, 0});

    Model* GetModel();
    std::string& ModelKey();
};