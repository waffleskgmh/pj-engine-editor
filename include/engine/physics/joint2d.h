#pragma once

#include "engine/ecs/entity.h"
#include "engine/core/math2d.h"

namespace PJEngine {

struct DistanceJoint2D {
    Entity bodyA = NULL_ENTITY;
    Entity bodyB = NULL_ENTITY;
    float targetDistance = 50.0f;
    float stiffness = 0.8f;
    float damping = 0.1f;
};

} // namespace PJEngine