#include "engine/physics/physics_engine2d.h"
#include <cmath>
#include <algorithm>
#include <unordered_set>

namespace PJEngine {

PhysicsEngine2D::PhysicsEngine2D(float spatialCellSize)
    : broadphaseGrid(spatialCellSize) {}

void PhysicsEngine2D::step(ECSWorld& world, float dt) {
    integrateForces(world, dt);

    std::vector<CollisionManifold> manifolds;
    detectCollisions(world, manifolds);
    resolveCollisions(world, manifolds);

    integrateVelocities(world, dt);
}

void PhysicsEngine2D::integrateForces(ECSWorld& world, float dt) {
    world.forEach<RigidBody2D>([&](Entity /*e*/, RigidBody2D& body) {
        if (body.bodyType != BodyType::DYNAMIC) return;

        // Aplikácia gravitácie
        body.force += gravity * (body.mass * body.gravityScale);

        // Integrácia zrýchlenia do rýchlosti
        Vec2 acceleration = body.force * body.invMass;
        body.velocity += acceleration * dt;

        body.clearForces();
        body.isGrounded = false;
    });
}

void PhysicsEngine2D::detectCollisions(ECSWorld& world, std::vector<CollisionManifold>& manifolds) {
    broadphaseGrid.clear();

    // 1. Zápis do Spatial Hash Grid
    world.forEach<Transform2D, Collider2D>([&](Entity e, const Transform2D& t, const Collider2D& col) {
        broadphaseGrid.insert(e, col.getAABB(t.position));
    });

    // 2. Úzka fáza (Narrowphase) s elimináciou duplicitných párov
    std::unordered_set<uint64_t> checkedPairs;
    auto collidableEntities = world.query<Transform2D, Collider2D>();

    for (Entity eA : collidableEntities) {
        const auto& tA = world.getComponent<Transform2D>(eA);
        const auto& cA = world.getComponent<Collider2D>(eA);

        std::vector<Entity> candidates;
        broadphaseGrid.query(cA.getAABB(tA.position), candidates);

        for (Entity eB : candidates) {
            if (eA >= eB) continue;

            uint64_t pairKey = (static_cast<uint64_t>(eA) << 32) | static_cast<uint64_t>(eB);
            if (checkedPairs.find(pairKey) != checkedPairs.end()) continue;
            checkedPairs.insert(pairKey);

            const auto& tB = world.getComponent<Transform2D>(eB);
            const auto& cB = world.getComponent<Collider2D>(eB);

            if ((cA.mask & cB.layer) == 0 || (cB.mask & cA.layer) == 0) continue;

            CollisionManifold manifold;
            if (checkCollision(eA, tA, cA, eB, tB, cB, manifold)) {
                manifolds.push_back(manifold);
            }
        }
    }
}

bool PhysicsEngine2D::checkCollision(Entity eA, const Transform2D& tA, const Collider2D& cA,
                                     Entity eB, const Transform2D& tB, const Collider2D& cB,
                                     CollisionManifold& m) {
    m.a = eA;
    m.b = eB;
    m.isTrigger = cA.isTrigger || cB.isTrigger;

    Vec2 posA = tA.position + cA.offset;
    Vec2 posB = tB.position + cB.offset;

    // Prípad 1: Kruh vs Kruh
    if (cA.type == ColliderType::CIRCLE && cB.type == ColliderType::CIRCLE) {
        Vec2 diff = posB - posA;
        float distSq = diff.lengthSq();
        float radiusSum = cA.radius + cB.radius;

        if (distSq >= radiusSum * radiusSum) return false;

        float dist = std::sqrt(distSq);
        if (dist > 0.0001f) {
            m.normal = diff / dist;
            m.penetration = radiusSum - dist;
        } else {
            m.normal = Vec2(0.0f, 1.0f);
            m.penetration = cA.radius;
        }
        m.contactPoint = posA + m.normal * cA.radius;
        return true;
    }

    // Prípad 2: Box vs Box (AABB)
    if (cA.type == ColliderType::BOX && cB.type == ColliderType::BOX) {
        Vec2 diff = posB - posA;
        float overlapX = (cA.size.x + cB.size.x) * 0.5f - std::abs(diff.x);
        if (overlapX <= 0.0f) return false;

        float overlapY = (cA.size.y + cB.size.y) * 0.5f - std::abs(diff.y);
        if (overlapY <= 0.0f) return false;

        if (overlapX < overlapY) {
            m.normal = Vec2(diff.x < 0.0f ? -1.0f : 1.0f, 0.0f);
            m.penetration = overlapX;
        } else {
            m.normal = Vec2(0.0f, diff.y < 0.0f ? -1.0f : 1.0f);
            m.penetration = overlapY;
        }
        m.contactPoint = posA + diff * 0.5f;
        return true;
    }

    // Prípad 3: Kruh (A) vs Box (B) - OPRAVENÁ ORIENTÁCIA NORMÁLY (od A k B)
    if (cA.type == ColliderType::CIRCLE && cB.type == ColliderType::BOX) {
        Vec2 halfBox = cB.size * 0.5f;
        Vec2 diff = posA - posB;
        Vec2 clamped(std::clamp(diff.x, -halfBox.x, halfBox.x),
                     std::clamp(diff.y, -halfBox.y, halfBox.y));

        Vec2 closest = posB + clamped;
        Vec2 d = closest - posA; // Smer od Kruhu (A) k Boxu (B)
        float distSq = d.lengthSq();

        if (distSq >= cA.radius * cA.radius) return false;

        float dist = std::sqrt(distSq);
        if (dist > 0.0001f) {
            m.normal = d / dist; // Normála smeruje od A k B (dole)
            m.penetration = cA.radius - dist;
        } else {
            m.normal = Vec2(0.0f, 1.0f);
            m.penetration = cA.radius;
        }
        m.contactPoint = closest;
        return true;
    }

    // Prípad 4: Box (A) vs Kruh (B)
    if (cA.type == ColliderType::BOX && cB.type == ColliderType::CIRCLE) {
        bool hit = checkCollision(eB, tB, cB, eA, tA, cA, m);
        if (hit) {
            m.a = eA;
            m.b = eB;
            m.normal = -m.normal;
        }
        return hit;
    }

    return false;
}

void PhysicsEngine2D::resolveCollisions(ECSWorld& world, std::vector<CollisionManifold>& manifolds) {
    for (const auto& m : manifolds) {
        if (m.isTrigger) continue;

        bool hasRbA = world.hasComponent<RigidBody2D>(m.a);
        bool hasRbB = world.hasComponent<RigidBody2D>(m.b);
        if (!hasRbA && !hasRbB) continue;

        RigidBody2D* rbA = hasRbA ? &world.getComponent<RigidBody2D>(m.a) : nullptr;
        RigidBody2D* rbB = hasRbB ? &world.getComponent<RigidBody2D>(m.b) : nullptr;

        float invMassA = rbA ? rbA->invMass : 0.0f;
        float invMassB = rbB ? rbB->invMass : 0.0f;
        float totalInvMass = invMassA + invMassB;
        if (totalInvMass <= 0.0f) continue;

        // 1. Pozičná korekcia (Baumgarte stabilizácia)
        const float percent = 0.8f;
        const float slop = 0.01f;
        Vec2 correction = m.normal * (std::max(m.penetration - slop, 0.0f) / totalInvMass * percent);

        if (rbA && rbA->bodyType == BodyType::DYNAMIC) {
            world.getComponent<Transform2D>(m.a).position -= correction * invMassA;
        }
        if (rbB && rbB->bodyType == BodyType::DYNAMIC) {
            world.getComponent<Transform2D>(m.b).position += correction * invMassB;
        }

        // 2. Výpočet relatívnej rýchlosti pozdĺž normály
        Vec2 velA = rbA ? rbA->velocity : Vec2::Zero();
        Vec2 velB = rbB ? rbB->velocity : Vec2::Zero();
        Vec2 relVel = velB - velA;
        float velAlongNormal = relVel.dot(m.normal);

        if (velAlongNormal > 0.0f) continue; // Objekty sa už od seba vzďaľujú

        float restA = rbA ? rbA->restitution : 0.2f;
        float restB = rbB ? rbB->restitution : 0.2f;
        float e = std::min(restA, restB);

        float j = -(1.0f + e) * velAlongNormal;
        j /= totalInvMass;

        Vec2 impulse = m.normal * j;
        if (rbA && rbA->bodyType == BodyType::DYNAMIC) rbA->velocity -= impulse * invMassA;
        if (rbB && rbB->bodyType == BodyType::DYNAMIC) rbB->velocity += impulse * invMassB;

        // 3. Trenie pozdĺž dotyčnice
        Vec2 tangent = relVel - m.normal * velAlongNormal;
        if (tangent.lengthSq() > 0.0001f) {
            tangent = tangent.normalized();
            float frictA = rbA ? rbA->friction : 0.3f;
            float frictB = rbB ? rbB->friction : 0.3f;
            float mu = std::sqrt(frictA * frictB);

            float jt = -relVel.dot(tangent);
            jt /= totalInvMass;
            jt = std::clamp(jt, -j * mu, j * mu);

            Vec2 frictionImpulse = tangent * jt;
            if (rbA && rbA->bodyType == BodyType::DYNAMIC) rbA->velocity -= frictionImpulse * invMassA;
            if (rbB && rbB->bodyType == BodyType::DYNAMIC) rbB->velocity += frictionImpulse * invMassB;
        }

        // 4. Detekcia státia na podlahe (isGrounded)
        if (m.normal.y > 0.7f && rbA) rbA->isGrounded = true;
        if (m.normal.y < -0.7f && rbB) rbB->isGrounded = true;
    }
}

void PhysicsEngine2D::integrateVelocities(ECSWorld& world, float dt) {
    world.forEach<Transform2D, RigidBody2D>([&](Entity /*e*/, Transform2D& t, const RigidBody2D& body) {
        if (body.bodyType != BodyType::STATIC) {
            t.position += body.velocity * dt;
        }
    });
}

RaycastHit2D PhysicsEngine2D::raycast(ECSWorld& world, const Vec2& origin, const Vec2& direction, float maxDistance) {
    RaycastHit2D closestHit;
    closestHit.distance = maxDistance;

    Vec2 dir = direction.normalized();
    auto entities = world.query<Transform2D, Collider2D>();

    for (Entity e : entities) {
        const auto& t = world.getComponent<Transform2D>(e);
        const auto& c = world.getComponent<Collider2D>(e);

        if (c.type == ColliderType::CIRCLE) {
            Vec2 center = t.position + c.offset;
            Vec2 m = origin - center;
            float b = m.dot(dir);
            float cVal = m.dot(m) - c.radius * c.radius;

            if (cVal > 0.0f && b > 0.0f) continue;
            float discr = b * b - cVal;
            if (discr < 0.0f) continue;

            float tHit = -b - std::sqrt(discr);
            if (tHit < 0.0f) tHit = 0.0f;

            if (tHit < closestHit.distance) {
                closestHit.hit = true;
                closestHit.entity = e;
                closestHit.distance = tHit;
                closestHit.point = origin + dir * tHit;
                closestHit.normal = (closestHit.point - center).normalized();
            }
        } else if (c.type == ColliderType::BOX) {
            Rect aabb = c.getAABB(t.position);
            float tmin = 0.0f;
            float tmax = closestHit.distance;

            if (std::abs(dir.x) > 0.0001f) {
                float t1 = (aabb.x - origin.x) / dir.x;
                float t2 = (aabb.x + aabb.width - origin.x) / dir.x;
                if (t1 > t2) std::swap(t1, t2);
                tmin = std::max(tmin, t1);
                tmax = std::min(tmax, t2);
                if (tmin > tmax) continue;
            } else if (origin.x < aabb.x || origin.x > aabb.x + aabb.width) {
                continue;
            }

            if (std::abs(dir.y) > 0.0001f) {
                float t1 = (aabb.y - origin.y) / dir.y;
                float t2 = (aabb.y + aabb.height - origin.y) / dir.y;
                if (t1 > t2) std::swap(t1, t2);
                tmin = std::max(tmin, t1);
                tmax = std::min(tmax, t2);
                if (tmin > tmax) continue;
            } else if (origin.y < aabb.y || origin.y > aabb.y + aabb.height) {
                continue;
            }

            if (tmin < closestHit.distance) {
                closestHit.hit = true;
                closestHit.entity = e;
                closestHit.distance = tmin;
                closestHit.point = origin + dir * tmin;
                Vec2 center = aabb.center();
                Vec2 p = closestHit.point - center;
                Vec2 halfSize = aabb.size() * 0.5f;
                float dx = std::abs(halfSize.x - std::abs(p.x));
                float dy = std::abs(halfSize.y - std::abs(p.y));
                if (dx < dy) {
                    closestHit.normal = Vec2(p.x > 0 ? 1.0f : -1.0f, 0.0f);
                } else {
                    closestHit.normal = Vec2(0.0f, p.y > 0 ? 1.0f : -1.0f);
                }
            }
        }
    }

    return closestHit;
}

} // namespace PJEngine