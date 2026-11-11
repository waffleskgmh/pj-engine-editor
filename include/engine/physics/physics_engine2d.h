#pragma once

#include "engine/core/math2d.h"
#include "engine/ecs/ecs_world.h"
#include "engine/physics/rigidbody2d.h"
#include "engine/physics/collider2d.h"
#include "engine/physics/spatial_hash.h"
#include <vector>

namespace PJEngine {

struct CollisionManifold {
    Entity a = NULL_ENTITY;
    Entity b = NULL_ENTITY;
    Vec2 normal = Vec2::Zero();
    float penetration = 0.0f;
    Vec2 contactPoint = Vec2::Zero();
    bool isTrigger = false;
};

struct RaycastHit2D {
    bool hit = false;
    Entity entity = NULL_ENTITY;
    Vec2 point = Vec2::Zero();
    Vec2 normal = Vec2::Zero();
    float distance = 0.0f;
};

class PhysicsEngine2D {
public:
    Vec2 gravity = Vec2(0.0f, 980.0f); // 980 pixelov/s^2 (prirodzená 2D gravitácia)
    SpatialHashGrid broadphaseGrid;

    explicit PhysicsEngine2D(float spatialCellSize = 64.0f);

    void step(ECSWorld& world, float dt);
    
    // Raycasting pre zbrane, lasery a viditeľnosť AI
    RaycastHit2D raycast(ECSWorld& world, const Vec2& origin, const Vec2& direction, float maxDistance);

private:
    void integrateForces(ECSWorld& world, float dt);
    void detectCollisions(ECSWorld& world, std::vector<CollisionManifold>& manifolds);
    void resolveCollisions(ECSWorld& world, std::vector<CollisionManifold>& manifolds);
    void integrateVelocities(ECSWorld& world, float dt);

    bool checkCollision(Entity eA, const Transform2D& tA, const Collider2D& cA,
                        Entity eB, const Transform2D& tB, const Collider2D& cB,
                        CollisionManifold& outManifold);
};

} // namespace PJEngine