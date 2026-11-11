#include "engine/optimization/quality_manager.h"

namespace PJEngine {

PerformanceProfile QualityManager::currentProfile = PerformanceProfile::BALANCED;
QualitySettings QualityManager::currentSettings = QualitySettings{};

void QualityManager::setProfile(PerformanceProfile profile) {
    currentProfile = profile;
    switch (profile) {
        case PerformanceProfile::ECO_BATTERY:
            currentSettings.targetFPS = 30;
            currentSettings.maxParticles = 200;
            currentSettings.enableLighting = false;
            currentSettings.enableSoftShadows = false;
            currentSettings.renderScale = 0.75f;
            currentSettings.physicsSubSteps = 1;
            break;
        case PerformanceProfile::BALANCED:
            currentSettings.targetFPS = 60;
            currentSettings.maxParticles = 1000;
            currentSettings.enableLighting = true;
            currentSettings.enableSoftShadows = false;
            currentSettings.renderScale = 1.0f;
            currentSettings.physicsSubSteps = 1;
            break;
        case PerformanceProfile::ULTRA:
            currentSettings.targetFPS = 144;
            currentSettings.maxParticles = 5000;
            currentSettings.enableLighting = true;
            currentSettings.enableSoftShadows = true;
            currentSettings.renderScale = 1.0f;
            currentSettings.physicsSubSteps = 2;
            break;
    }
}

PerformanceProfile QualityManager::getProfile() {
    return currentProfile;
}

const QualitySettings& QualityManager::getSettings() {
    return currentSettings;
}

} // namespace PJEngine