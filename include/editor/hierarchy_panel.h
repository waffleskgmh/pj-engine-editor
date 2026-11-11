#pragma once

#include "editor/editor_app.h"
#include <vector>
#include <string>

namespace PJEngine {

class HierarchyPanel {
public:
    static Entity createNewEntity(EditorApp& app, const std::string& tag = "Nova Entita", const Vec2& pos = Vec2(100.0f, 100.0f));
    static void deleteSelectedEntity(EditorApp& app);
    static std::vector<std::pair<Entity, std::string>> getEntityList(const EditorApp& app);
};

} // namespace PJEngine