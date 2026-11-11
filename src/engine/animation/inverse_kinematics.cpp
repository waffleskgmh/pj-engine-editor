#include "engine/animation/inverse_kinematics.h"
#include <cmath>
#include <algorithm>

namespace PJEngine {

bool InverseKinematics2D::solveTwoBoneIK(Bone2D& root, Bone2D& mid, const Vec2& target, bool bendPositive) {
    Vec2 rootPos = root.getWorldPosition();
    Vec2 diff = target - rootPos;
    float dist = diff.length();

    float l1 = root.length;
    float l2 = mid.length;

    // Cieľ je mimo dosahu kostí - natiahneme ruku/nohu priamo k cieľu
    if (dist >= l1 + l2) {
        float angle = std::atan2(diff.y, diff.x);
        root.localRotation = angle;
        mid.localRotation = 0.0f;
        return false;
    }

    // Analytické riešenie pomocou kosínusovej vety
    float cosAngle2 = (dist * dist - l1 * l1 - l2 * l2) / (2.0f * l1 * l2);
    cosAngle2 = std::clamp(cosAngle2, -1.0f, 1.0f);
    float angle2 = std::acos(cosAngle2) * (bendPositive ? 1.0f : -1.0f);

    float cosAngle1 = (dist * dist + l1 * l1 - l2 * l2) / (2.0f * dist * l1);
    cosAngle1 = std::clamp(cosAngle1, -1.0f, 1.0f);
    float baseAngle = std::atan2(diff.y, diff.x);
    float angle1 = baseAngle - std::acos(cosAngle1) * (bendPositive ? 1.0f : -1.0f);

    root.localRotation = angle1;
    mid.localRotation = angle2;
    return true;
}

} // namespace PJEngine