#include "engine/animation/bone2d.h"
#include <cmath>

namespace PJEngine {

Bone2D::Bone2D(const std::string& name, float length)
    : name(name), length(length) {}

void Bone2D::addChild(std::shared_ptr<Bone2D> child) {
    child->parent = this;
    children.push_back(child);
}

Vec2 Bone2D::getWorldPosition() const {
    if (!parent) return localPosition;

    Vec2 pPos = parent->getWorldPosition();
    float pRot = parent->getWorldRotation();
    Vec2 rotatedLocal = localPosition.rotated(pRot);

    return pPos + rotatedLocal;
}

float Bone2D::getWorldRotation() const {
    if (!parent) return localRotation;
    return parent->getWorldRotation() + localRotation;
}

Vec2 Bone2D::getEndEffectorPosition() const {
    Vec2 wPos = getWorldPosition();
    float wRot = getWorldRotation();
    return wPos + Vec2(std::cos(wRot), std::sin(wRot)) * length;
}

} // namespace PJEngine