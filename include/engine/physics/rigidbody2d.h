#pragma once

#include "engine/core/math2d.h"

namespace PJEngine {

enum class BodyType {
    STATIC,    // Nepohyblivé teleso s nekonečnou hmotnosťou (podlahy, steny)
    DYNAMIC,   // Plne simulované fyzikálne teleso ovplyvnené gravitáciou a silami
    KINEMATIC  // Pohybované skriptom (pohyblivé plošiny, výťahy)
};

class RigidBody2D {
public:
    BodyType bodyType = BodyType::DYNAMIC;
    
    Vec2 velocity = Vec2::Zero();
    Vec2 force = Vec2::Zero();
    
    float mass = 1.0f;
    float invMass = 1.0f;
    float restitution = 0.2f;  // Pružnosť / Odrazivosť (0 = plastelína, 1 = dokonalý odraz)
    float friction = 0.3f;     // Trenie povrchu
    float gravityScale = 1.0f; // Individuálne násobenie gravitácie
    
    bool isGrounded = false;
    bool fixedRotation = true;

    RigidBody2D() = default;
    explicit RigidBody2D(BodyType type, float mass = 1.0f, float restitution = 0.2f, float friction = 0.3f);

    void setMass(float m);
    void applyForce(const Vec2& f);
    void applyImpulse(const Vec2& impulse);
    void clearForces();
};

} // namespace PJEngine