#pragma once

#include "engine/core/math2d.h"

namespace PJEngine {

class SurvivalEnvironment {
public:
    float timeOfDay = 12.0f; // 0.0f až 24.0f hodín
    float timeScale = 0.1f;   // Rýchlosť plynutia dňa
    
    float hunger = 100.0f;   // 0 až 100
    float thirst = 100.0f;   // 0 až 100
    float temperature = 20.0f; // Stupne Celzia

    void update(float dt);
    bool isNight() const;
    float getAmbientLightIntensity() const;
    Color getSkyColor() const;
};

} // namespace PJEngine