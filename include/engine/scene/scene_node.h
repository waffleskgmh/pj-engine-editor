#pragma once

#include "engine/core/math2d.h"
#include "engine/ecs/entity.h"
#include <string>
#include <vector>
#include <memory>

namespace PJEngine {

class SceneNode : public std::enable_shared_from_this<SceneNode> {
public:
    std::string id;
    std::string name;
    Transform2D localTransform;
    Entity entity = NULL_ENTITY;
    bool isVisible = true;
    bool isLocked = false;

    SceneNode* parent = nullptr;
    std::vector<std::shared_ptr<SceneNode>> children;

    explicit SceneNode(const std::string& name, const std::string& id = "");

    void addChild(std::shared_ptr<SceneNode> child);
    void removeChild(std::shared_ptr<SceneNode> child);

    Vec2 getWorldPosition() const;
    float getWorldRotation() const;
    Mat3x2 getWorldMatrix() const;
};

} // namespace PJEngine