#pragma once

#include "engine/scene/map_transition.h"

namespace PJEngine {

class MapTransitionPanel {
public:
    MapTransitionPanel() = default;
    ~MapTransitionPanel() = default;

    void render();

private:
    char newTriggerId[64] = "portal_01";
    char targetMapPath[128] = "assets/maps/level2.pjscene";
    float spawnX = 100.0f;
    float spawnY = 200.0f;
    float boundsX = 300.0f;
    float boundsY = 400.0f;
    float boundsW = 40.0f;
    float boundsH = 60.0f;
};

} // namespace PJEngine