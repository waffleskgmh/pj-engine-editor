#pragma once

#include "engine/project/project.h"
#include <string>

namespace PJEngine {

enum class ExportTargetPlatform {
    MACOS_DESKTOP,
    RASPBERRY_PI_ARM,
    LINUX_DESKTOP
};

class ProjectExporter {
public:
    static bool exportStandaloneGame(const ProjectConfig& config, ExportTargetPlatform target, const std::string& outputFolder);

private:
    static bool copyDirectoryRecursive(const std::string& src, const std::string& dst);
    static bool createExecutableScript(const std::string& outputFolder, const std::string& gameName, ExportTargetPlatform target);
};

} // namespace PJEngine