#pragma once

#include "engine/core/math2d.h"
#include "engine/ecs/ecs_world.h"
#include <vector>

namespace PJEngine {

class Culling2D {
public:
    static bool isVisible(const Rect& bounds, const Rect& viewBounds);
    static void filterVisibleEntities(ECSWorld& world, const Rect& viewBounds, std::vector<Entity>& outVisible);
};

} // namespace PJEngine