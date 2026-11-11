#include "engine/scene/scene_node.h"

namespace PJEngine {

SceneNode::SceneNode(const std::string& name, const std::string& id)
    : name(name) {
    this->id = id.empty() ? ("node_" + name) : id;
}

void SceneNode::addChild(std::shared_ptr<SceneNode> child) {
    if (child->parent) {
        child->parent->removeChild(child);
    }
    child->parent = this;
    children.push_back(child);
}

void SceneNode::removeChild(std::shared_ptr<SceneNode> child) {
    for (auto it = children.begin(); it != children.end(); ++it) {
        if (*it == child) {
            child->parent = nullptr;
            children.erase(it);
            return;
        }
    }
}

Vec2 SceneNode::getWorldPosition() const {
    if (!parent) return localTransform.position;
    Vec2 pPos = parent->getWorldPosition();
    float pRot = parent->getWorldRotation();
    return pPos + localTransform.position.rotated(pRot);
}

float SceneNode::getWorldRotation() const {
    if (!parent) return localTransform.rotation;
    return parent->getWorldRotation() + localTransform.rotation;
}

Mat3x2 SceneNode::getWorldMatrix() const {
    if (!parent) return localTransform.getMatrix();
    return localTransform.getMatrix().multiply(parent->getWorldMatrix());
}

} // namespace PJEngine