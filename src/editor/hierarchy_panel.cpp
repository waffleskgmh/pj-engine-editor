#include "editor/hierarchy_panel.h"
#include "engine/core/math2d.h"
#include "engine/physics/rigidbody2d.h"
#include "engine/physics/collider2d.h"

namespace PJEngine {

Entity HierarchyPanel::createNewEntity(EditorApp& app, const std::string& /*tag*/, const Vec2& pos) {
    Entity e = app.world.createEntity();
    app.world.addComponent<Transform2D>(e, Transform2D{pos, 0.0f, Vec2::One(), Vec2::Zero()});
    app.world.addComponent<RigidBody2D>(e, RigidBody2D(BodyType::DYNAMIC, 1.0f));
    app.world.addComponent<Collider2D>(e, Collider2D::MakeCircle(16.0f));

    app.selectEntity(e);
    return e;
}

void HierarchyPanel::deleteSelectedEntity(EditorApp& app) {
    if (app.selectedEntity != NULL_ENTITY && app.world.isAlive(app.selectedEntity)) {
        app.world.destroyEntity(app.selectedEntity);
        app.selectedEntity = NULL_ENTITY;
    }
}

std::vector<std::pair<Entity, std::string>> HierarchyPanel::getEntityList(const EditorApp& app) {
    std::vector<std::pair<Entity, std::string>> list;
    auto entities = const_cast<ECSWorld&>(app.world).query<Transform2D>();

    for (Entity e : entities) {
        std::string name = (e == 0) ? "Hrac (Player)" : ("Entita #" + std::to_string(e));
        list.push_back({e, name});
    }
    return list;
}

} // namespace PJEngine