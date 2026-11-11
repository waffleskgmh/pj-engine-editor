#include "engine/physics/rigidbody2d.h"
#include <algorithm>

namespace PJEngine {

RigidBody2D::RigidBody2D(BodyType type, float m, float rest, float frict)
    : bodyType(type), restitution(rest), friction(frict) {
    setMass(m);
}

void RigidBody2D::setMass(float m) {
    mass = std::max(0.0001f, m);
    if (bodyType == BodyType::DYNAMIC) {
        invMass = 1.0f / mass;
    } else {
        invMass = 0.0f; // Statické a kinematické objekty majú nulovú inverznú hmotnosť
    }
}

void RigidBody2D::applyForce(const Vec2& f) {
    if (bodyType == BodyType::DYNAMIC) {
        force += f;
    }
}

void RigidBody2D::applyImpulse(const Vec2& impulse) {
    if (bodyType == BodyType::DYNAMIC) {
        velocity += impulse * invMass;
    }
}

void RigidBody2D::clearForces() {
    force = Vec2::Zero();
}

} // namespace PJEngine