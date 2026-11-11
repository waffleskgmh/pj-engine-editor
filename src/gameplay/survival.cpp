#include "gameplay/survival.h"
#include <cmath>
#include <algorithm>

namespace PJEngine {

void SurvivalEnvironment::update(float dt) {
    timeOfDay = std::fmod(timeOfDay + dt * timeScale, 24.0f);

    // Pomalý úbytok potrieb
    hunger = std::max(0.0f, hunger - dt * 0.05f);
    thirst = std::max(0.0f, thirst - dt * 0.1f);
}

bool SurvivalEnvironment::isNight() const {
    return timeOfDay < 6.0f || timeOfDay > 20.0f;
}

float SurvivalEnvironment::getAmbientLightIntensity() const {
    // Poludnie (12:00) = plné svetlo 1.0f, Polnoc (0:00) = tma 0.15f
    float t = std::sin((timeOfDay / 24.0f) * PI);
    return std::clamp(t, 0.15f, 1.0f);
}

Color SurvivalEnvironment::getSkyColor() const {
    float light = getAmbientLightIntensity();
    return Color(0.2f * light, 0.4f * light, 0.7f * light, 1.0f);
}

} // namespace PJEngine