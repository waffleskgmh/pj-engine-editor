#pragma once

#include "engine/core/math2d.h"
#include <string>
#include <vector>
#include <memory>

namespace PJEngine {

class Bone2D {
public:
    std::string name;
    Vec2 localPosition = Vec2::Zero();
    float localRotation = 0.0f; // v radiánoch
    float length = 40.0f;

    Bone2D* parent = nullptr;
    std::vector<std::shared_ptr<Bone2D>> children;

    explicit Bone2D(const std::string& name, float length = 40.0f);

    void addChild(std::shared_ptr<Bone2D> child);
    
    Vec2 getWorldPosition() const;
    float getWorldRotation() const;
    Vec2 getEndEffectorPosition() const;
};

} // namespace PJEngine