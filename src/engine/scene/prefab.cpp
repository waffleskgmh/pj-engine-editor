#include "engine/scene/prefab.h"
#include "engine/ecs/components2d.h"
#include "engine/physics/rigidbody2d.h"
#include "engine/physics/collider2d.h"
#include "engine/core/logger.h"

#include <fstream>
#include <sstream>

namespace PJEngine {

bool Prefab::saveEntityAsPrefab(ECSWorld& world, Entity e, const std::string& path) {
    if (!world.isAlive(e)) return false;

    std::ofstream out(path);
    if (!out.is_open()) return false;

    out << "PJ_PREFAB_V1\n";

    if (world.hasComponent<TagComponent>(e)) {
        const auto& c = world.getComponent<TagComponent>(e);
        out << "TAG: " << c.name << "|" << c.tag << "|" << c.layer << "\n";
    }
    if (world.hasComponent<RigidBody2D>(e)) {
        const auto& rb = world.getComponent<RigidBody2D>(e);
        out << "RIGIDBODY: " << static_cast<int>(rb.bodyType) << " " << rb.mass << " "
            << rb.restitution << " " << rb.friction << " " << rb.gravityScale << "\n";
    }
    if (world.hasComponent<Collider2D>(e)) {
        const auto& col = world.getComponent<Collider2D>(e);
        out << "COLLIDER: " << static_cast<int>(col.type) << " " << col.size.x << " "
            << col.size.y << " " << col.radius << " " << (col.isTrigger ? 1 : 0) << "\n";
    }
    if (world.hasComponent<SpriteRendererComponent>(e)) {
        const auto& s = world.getComponent<SpriteRendererComponent>(e);
        out << "SPRITE: " << s.textureGuid << " " << s.size.x << " " << s.size.y << "\n";
    }

    Logger::info("Prefab ulozeny: " + path);
    return true;
}

Entity Prefab::instantiate(ECSWorld& world, const std::string& path, const Vec2& spawnPos) {
    std::ifstream in(path);
    if (!in.is_open()) {
        Logger::error("Prefab sa nepodarilo otvorit: " + path);
        return NULL_ENTITY;
    }

    Entity e = world.createEntity();
    world.addComponent<Transform2D>(e, Transform2D{spawnPos, 0.0f, Vec2::One(), Vec2::Zero()});

    std::string line;
    while (std::getline(in, line)) {
        std::stringstream ss(line);
        std::string header;
        ss >> header;

        if (header == "TAG:") {
            std::string raw;
            ss >> raw;
            std::stringstream st(raw);
            std::string n, t, l;
            std::getline(st, n, '|');
            std::getline(st, t, '|');
            std::getline(st, l, '|');
            world.addComponent<TagComponent>(e, TagComponent(n + "_Instance", t, l.empty() ? 0 : std::stoi(l)));
        } else if (header == "RIGIDBODY:") {
            int bt; float m, r, f, g;
            ss >> bt >> m >> r >> f >> g;
            RigidBody2D rb(static_cast<BodyType>(bt), m, r, f);
            rb.gravityScale = g;
            world.addComponent<RigidBody2D>(e, rb);
        } else if (header == "COLLIDER:") {
            int ct, trig; float sx, sy, rad;
            ss >> ct >> sx >> sy >> rad >> trig;
            Collider2D c;
            c.type = static_cast<ColliderType>(ct);
            c.size = Vec2(sx, sy);
            c.radius = rad;
            c.isTrigger = (trig != 0);
            world.addComponent<Collider2D>(e, c);
        }
    }

    Logger::info("Instanciovany prefab na pozicii (" + std::to_string(spawnPos.x) + ", " + std::to_string(spawnPos.y) + ")");
    return e;
}

} // namespace PJEngine