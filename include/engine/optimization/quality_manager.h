#pragma once

#include <cstdint>
#include <cstddef>

namespace PJEngine {

enum class PerformanceProfile {
    ECO_BATTERY,  // Pre 15-20 ročné zostavy a Raspberry Pi (30 FPS, minimum častíc, bez svetiel)
    BALANCED,     // Pre bežné notebooky (60 FPS, vyvážené nastavenia)
    ULTRA         // Pre výkonné počítače (144+ FPS, maximálne častice a osvetlenie)
};

struct QualitySettings {
    uint32_t targetFPS = 60;
    size_t maxParticles = 1000;
    bool enableLighting = true;
    bool enableSoftShadows = false;
    float renderScale = 1.0f;
    uint32_t physicsSubSteps = 1;
};

class QualityManager {
public:
    static void setProfile(PerformanceProfile profile);
    static PerformanceProfile getProfile();
    static const QualitySettings& getSettings();

private:
    static PerformanceProfile currentProfile;
    static QualitySettings currentSettings;
};

} // namespace PJEngine