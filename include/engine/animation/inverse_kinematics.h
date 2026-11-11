#pragma once

#include "engine/core/math2d.h"
#include "engine/animation/bone2d.h"

namespace PJEngine {

class InverseKinematics2D {
public:
    static bool solveTwoBoneIK(Bone2D& root, Bone2D& mid, const Vec2& target, bool bendPositive = true);
};

} // namespace PJEngine