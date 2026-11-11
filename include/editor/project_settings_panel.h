#pragma once

#include "engine/project/project.h"

namespace PJEngine {

class ProjectSettingsPanel {
public:
    ProjectSettingsPanel() = default;
    ~ProjectSettingsPanel() = default;

    void render();

private:
    char nameBuf[128] = "Moja 2D Hra";
    char authorBuf[128] = "Vyvojar";
    char versionBuf[32] = "1.0.0";
    char startSceneBuf[128] = "assets/scenes/level1.pjscene";
    char exportDirBuf[128] = "build/export";

    int targetPlatformIdx = 0; // 0 = macOS Desktop, 1 = Raspberry Pi ARM, 2 = Linux
    bool isLoaded = false;

    void syncFromConfig();
    void syncToConfig();
};

} // namespace PJEngine