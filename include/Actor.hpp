#pragma once

#include "CommonIncludes.hpp"

class Actor {
public:
    Actor(rc::RigidBody* body) 
    : m_body(body) {
    }

    void SetModel(std::string model_key, Vector3 local_offset = {0, 0, 0});
    
    void Draw();

    Vector3 GetAxisAngle(float& angle);
    Vector3 GetPosition();
    void SetPosition(Vector3 position);

    rc::RigidBody* GetBody();
    Model* GetModel();

    std::string& ModelKey();

private:
    rc::RigidBody* m_body;
    std::string m_model_key;
    Vector3 m_model_local_offset;
};