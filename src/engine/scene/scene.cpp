#include "engine/scene/scene.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace PJEngine {

Scene::Scene(const std::string& name) : name(name) {
    clear();
}

void Scene::clear() {
    world.clear();
    root = std::make_shared<SceneNode>("Root");
}

Entity Scene::createEntity(const std::string& entityName, const Vec2& position) {
    Entity e = world.createEntity();
    world.addComponent<TagComponent>(e, TagComponent(entityName));
    world.addComponent<Transform2D>(e, Transform2D{position, 0.0f, Vec2::One(), Vec2::Zero()});

    // Pridanie uzla do hierarchie scény
    auto node = std::make_shared<SceneNode>(entityName);
    node->entity = e;
    node->localTransform.position = position;
    root->addChild(node);

    return e;
}

void Scene::destroyEntity(Entity entity) {
    if (!world.isAlive(entity)) return;

    // Odstránenie uzla z hierarchie
    for (auto it = root->children.begin(); it != root->children.end(); ++it) {
        if ((*it)->entity == entity) {
            root->children.erase(it);
            break;
        }
    }

    world.destroyEntity(entity);
}

void Scene::serializeEntity(std::ostream& ss, Entity e) const {
    ss << "ENTITY_START: " << e << "\n";

    // 1. TagComponent
    if (world.hasComponent<TagComponent>(e)) {
        const auto& c = world.getComponent<TagComponent>(e);
        ss << "  TAG: " << c.name << "|" << c.tag << "|" << c.layer << "|" << (c.isActive ? 1 : 0) << "\n";
    }

    // 2. Transform2D
    if (world.hasComponent<Transform2D>(e)) {
        const auto& t = world.getComponent<Transform2D>(e);
        ss << "  TRANSFORM: " << t.position.x << " " << t.position.y << " "
           << t.rotation << " " << t.scale.x << " " << t.scale.y << " "
           << t.origin.x << " " << t.origin.y << "\n";
    }

    // 3. RigidBody2D
    if (world.hasComponent<RigidBody2D>(e)) {
        const auto& rb = world.getComponent<RigidBody2D>(e);
        ss << "  RIGIDBODY: " << static_cast<int>(rb.bodyType) << " "
           << rb.mass << " " << rb.restitution << " " << rb.friction << " "
           << rb.gravityScale << " " << (rb.fixedRotation ? 1 : 0) << "\n";
    }

    // 4. Collider2D
    if (world.hasComponent<Collider2D>(e)) {
        const auto& col = world.getComponent<Collider2D>(e);
        ss << "  COLLIDER: " << static_cast<int>(col.type) << " "
           << col.offset.x << " " << col.offset.y << " "
           << col.size.x << " " << col.size.y << " "
           << col.radius << " " << (col.isTrigger ? 1 : 0) << " "
           << col.layer << " " << col.mask << "\n";
    }

    // 5. SpriteRendererComponent
    if (world.hasComponent<SpriteRendererComponent>(e)) {
        const auto& s = world.getComponent<SpriteRendererComponent>(e);
        ss << "  SPRITE: " << (s.textureGuid.empty() ? "null" : s.textureGuid) << " "
           << (s.regionId.empty() ? "null" : s.regionId) << " "
           << s.uvRect.x << " " << s.uvRect.y << " " << s.uvRect.width << " " << s.uvRect.height << " "
           << s.size.x << " " << s.size.y << " " << s.pivot.x << " " << s.pivot.y << " "
           << s.tint.r << " " << s.tint.g << " " << s.tint.b << " " << s.tint.a << " "
           << s.sortingOrder << " " << (s.flipX ? 1 : 0) << " " << (s.flipY ? 1 : 0) << " "
           << (s.isVisible ? 1 : 0) << "\n";
    }

    // 6. ScriptComponent
    if (world.hasComponent<ScriptComponent>(e)) {
        const auto& sc = world.getComponent<ScriptComponent>(e);
        ss << "  SCRIPT: " << sc.scriptPath << " " << (sc.isEnabled ? 1 : 0) << "\n";
    }

    // 7. AudioSourceComponent
    if (world.hasComponent<AudioSourceComponent>(e)) {
        const auto& a = world.getComponent<AudioSourceComponent>(e);
        ss << "  AUDIO: " << a.soundName << " " << a.volume << " " << a.pitch << " "
           << (a.isLooping ? 1 : 0) << " " << (a.playOnAwake ? 1 : 0) << " "
           << (a.isSpatial ? 1 : 0) << " " << a.maxDistance << "\n";
    }

    ss << "ENTITY_END\n";
}

void Scene::serializeHierarchyNode(std::ostream& ss, const std::shared_ptr<SceneNode>& node, int indent) const {
    std::string ind(indent * 2, ' ');
    ss << ind << "NODE: " << node->name << " ID: " << node->id << " ENTITY: " << node->entity << "\n";
    ss << ind << "  POS: " << node->localTransform.position.x << " " << node->localTransform.position.y << "\n";
    ss << ind << "  ROT: " << node->localTransform.rotation << "\n";

    for (const auto& child : node->children) {
        serializeHierarchyNode(ss, child, indent + 1);
    }
}

std::string Scene::serialize() const {
    std::stringstream ss;
    ss << "PJ_SCENE_VERSION: 1\n";
    ss << "SCENE_NAME: " << name << "\n";

    // Serializácia všetkých entít v ECS svete
    auto allEntities = const_cast<ECSWorld&>(world).query<Transform2D>();
    ss << "ENTITIES_COUNT: " << allEntities.size() << "\n";
    for (Entity e : allEntities) {
        serializeEntity(ss, e);
    }

    // Serializácia stromu hierarchie
    ss << "HIERARCHY_START\n";
    serializeHierarchyNode(ss, root, 0);
    ss << "HIERARCHY_END\n";

    return ss.str();
}

std::shared_ptr<Scene> Scene::deserialize(const std::string& data) {
    auto scene = std::make_shared<Scene>("NacitanaScena");
    scene->clear();

    std::stringstream ss(data);
    std::string line;

    std::unordered_map<uint32_t, Entity> oldToNewEntityMap;
    Entity currentEntity = NULL_ENTITY;

    while (std::getline(ss, line)) {
        if (line.rfind("SCENE_NAME: ", 0) == 0) {
            scene->name = line.substr(12);
        } else if (line.rfind("ENTITY_START: ", 0) == 0) {
            uint32_t oldId = std::stoul(line.substr(14));
            currentEntity = scene->world.createEntity();
            oldToNewEntityMap[oldId] = currentEntity;
        } else if (line == "ENTITY_END") {
            currentEntity = NULL_ENTITY;
        } else if (currentEntity != NULL_ENTITY) {
            std::stringstream sline(line);
            std::string header;
            sline >> header;

            if (header == "TAG:") {
                std::string tagData;
                sline >> tagData;
                std::stringstream st(tagData);
                std::string name, tag, layerStr, activeStr;
                std::getline(st, name, '|');
                std::getline(st, tag, '|');
                std::getline(st, layerStr, '|');
                std::getline(st, activeStr, '|');

                TagComponent tc;
                tc.name = name.empty() ? "Entita" : name;
                tc.tag = tag.empty() ? "Untagged" : tag;
                tc.layer = layerStr.empty() ? 0 : std::stoi(layerStr);
                tc.isActive = activeStr.empty() ? true : (std::stoi(activeStr) != 0);
                scene->world.addComponent<TagComponent>(currentEntity, tc);

            } else if (header == "TRANSFORM:") {
                Transform2D t;
                sline >> t.position.x >> t.position.y >> t.rotation
                      >> t.scale.x >> t.scale.y >> t.origin.x >> t.origin.y;
                scene->world.addComponent<Transform2D>(currentEntity, t);

            } else if (header == "RIGIDBODY:") {
                int bodyType;
                float mass, rest, frict, grav;
                int fixedRot;
                sline >> bodyType >> mass >> rest >> frict >> grav >> fixedRot;

                RigidBody2D rb(static_cast<BodyType>(bodyType), mass, rest, frict);
                rb.gravityScale = grav;
                rb.fixedRotation = (fixedRot != 0);
                scene->world.addComponent<RigidBody2D>(currentEntity, rb);

            } else if (header == "COLLIDER:") {
                int type, isTrig;
                Vec2 offset, size;
                float radius;
                uint32_t layer, mask;
                sline >> type >> offset.x >> offset.y >> size.x >> size.y >> radius >> isTrig >> layer >> mask;

                Collider2D col;
                col.type = static_cast<ColliderType>(type);
                col.offset = offset;
                col.size = size;
                col.radius = radius;
                col.isTrigger = (isTrig != 0);
                col.layer = layer;
                col.mask = mask;
                scene->world.addComponent<Collider2D>(currentEntity, col);

            } else if (header == "SPRITE:") {
                SpriteRendererComponent s;
                std::string texGuid, regId;
                int order, flipX, flipY, isVis;
                sline >> texGuid >> regId
                      >> s.uvRect.x >> s.uvRect.y >> s.uvRect.width >> s.uvRect.height
                      >> s.size.x >> s.size.y >> s.pivot.x >> s.pivot.y
                      >> s.tint.r >> s.tint.g >> s.tint.b >> s.tint.a
                      >> order >> flipX >> flipY >> isVis;

                s.textureGuid = (texGuid == "null") ? "" : texGuid;
                s.regionId = (regId == "null") ? "" : regId;
                s.sortingOrder = order;
                s.flipX = (flipX != 0);
                s.flipY = (flipY != 0);
                s.isVisible = (isVis != 0);
                scene->world.addComponent<SpriteRendererComponent>(currentEntity, s);

            } else if (header == "SCRIPT:") {
                ScriptComponent sc;
                int isEn;
                sline >> sc.scriptPath >> isEn;
                sc.isEnabled = (isEn != 0);
                scene->world.addComponent<ScriptComponent>(currentEntity, sc);

            } else if (header == "AUDIO:") {
                AudioSourceComponent a;
                int loop, awake, spat;
                sline >> a.soundName >> a.volume >> a.pitch >> loop >> awake >> spat >> a.maxDistance;
                a.isLooping = (loop != 0);
                a.playOnAwake = (awake != 0);
                a.isSpatial = (spat != 0);
                scene->world.addComponent<AudioSourceComponent>(currentEntity, a);
            }
        }
    }

    // Znovuvytvorenie základného koreňového uzla hierarchie pre načítané entity
    for (const auto& pair : oldToNewEntityMap) {
        Entity newE = pair.second;
        std::string name = scene->world.hasComponent<TagComponent>(newE)
                         ? scene->world.getComponent<TagComponent>(newE).name
                         : ("Entita #" + std::to_string(newE));

        auto node = std::make_shared<SceneNode>(name);
        node->entity = newE;
        if (scene->world.hasComponent<Transform2D>(newE)) {
            node->localTransform = scene->world.getComponent<Transform2D>(newE);
        }
        scene->root->addChild(node);
    }

    return scene;
}

bool Scene::saveToFile(const std::string& path) {
    std::ofstream file(path);
    if (!file.is_open()) {
        std::cerr << "[Scene]: Nepodarilo sa otvorit subor pre zapis: " << path << std::endl;
        return false;
    }
    file << serialize();
    filePath = path;
    return true;
}

std::shared_ptr<Scene> Scene::loadFromFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "[Scene]: Nepodarilo sa otvorit subor pre citanie: " << path << std::endl;
        return nullptr;
    }
    std::stringstream ss;
    ss << file.rdbuf();

    auto scene = deserialize(ss.str());
    if (scene) {
        scene->filePath = path;
    }
    return scene;
}

} // namespace PJEngine