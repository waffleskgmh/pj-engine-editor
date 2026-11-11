#pragma once

#include "engine/core/logger.h"
#include <string>

namespace PJEngine {

class ConsolePanel {
public:
    ConsolePanel() = default;
    ~ConsolePanel() = default;

    void render();

private:
    bool showInfo = true;
    bool showWarnings = true;
    bool showErrors = true;
    bool autoScroll = true;
    char searchFilter[128] = "";
};

} // namespace PJEngine