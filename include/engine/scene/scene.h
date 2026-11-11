#pragma once

#include "engine/scene/scene_node.h"
#include "engine/ecs/ecs_world.h"
#include "engine/ecs/components2d.h"
#include "engine/physics/rigidbody2d.h"
#include "engine/physics/collider2d.h"

#include <string>
#include <memory>
#include <unordered_map>

namespace PJEngine {

class Scene {
public:
    std::string name;
    std::string filePath;
    std::shared_ptr<SceneNode> root;
    ECSWorld world;

    explicit Scene(const std::string& name = "NovaScena");
    ~Scene() = default;

    // Vytvorenie novej prázdnej scény so základnými nastaveniami
    void clear();

    // Serializácia celej scény a ECS sveta do textového formátu
    std::string serialize() const;
    static std::shared_ptr<Scene> deserialize(const std::string& data);

    // Práca so súbormi na disku
    bool saveToFile(const std::string& path);
    static std::shared_ptr<Scene> loadFromFile(const std::string& path);

    // Vytvorenie a správa entít v scéne
    Entity createEntity(const std::string& name = "Entita", const Vec2& position = Vec2::Zero());
    void destroyEntity(Entity entity);

private:
    void serializeEntity(std::ostream& ss, Entity e) const;
    void serializeHierarchyNode(std::ostream& ss, const std::shared_ptr<SceneNode>& node, int indent) const;
};

} // namespace PJEngine