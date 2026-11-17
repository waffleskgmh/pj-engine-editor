#pragma once

#include "engine/ecs/ecs_world.h"
#include "engine/core/math2d.h"
#include <string>

namespace PJEngine {

class Prefab {
public:
    static bool saveEntityAsPrefab(ECSWorld& world, Entity entity, const std::string& prefabFilePath);
    static Entity instantiate(ECSWorld& world, const std::string& prefabFilePath, const Vec2& spawnPosition);
};

} // namespace PJEngine