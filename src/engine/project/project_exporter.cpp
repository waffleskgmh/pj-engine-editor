#include "engine/project/project_exporter.h"
#include "engine/core/logger.h"

#include <fstream>
#include <sstream>
#include <filesystem>

namespace fs = std::filesystem;

namespace PJEngine {

bool ProjectExporter::copyDirectoryRecursive(const std::string& src, const std::string& dst) {
    if (!fs::exists(src)) return false;

    fs::create_directories(dst);
    for (const auto& entry : fs::recursive_directory_iterator(src)) {
        const auto& path = entry.path();
        auto relative = fs::relative(path, src);
        auto target = fs::path(dst) / relative;

        if (fs::is_directory(path)) {
            fs::create_directories(target);
        } else if (fs::is_regular_file(path)) {
            fs::copy_file(path, target, fs::copy_options::overwrite_existing);
        }
    }
    return true;
}

bool ProjectExporter::createExecutableScript(const std::string& outputFolder, const std::string& gameName, ExportTargetPlatform target) {
    if (target == ExportTargetPlatform::MACOS_DESKTOP || target == ExportTargetPlatform::RASPBERRY_PI_ARM || target == ExportTargetPlatform::LINUX_DESKTOP) {
        std::string runnerPath = outputFolder + "/run_game.sh";
        std::ofstream sh(runnerPath);
        if (!sh.is_open()) return false;

        sh << "#!/bin/bash\n";
        sh << "# Launcher pre hru: " << gameName << "\n";
        sh << "cd \"$(dirname \"$0\")\"\n";
        sh << "./pj-game\n";
        sh.close();

        fs::permissions(runnerPath, fs::perms::owner_all | fs::perms::group_read | fs::perms::others_read, fs::perm_options::add);
    } else {
        std::string batchPath = outputFolder + "/run_game.bat";
        std::ofstream bat(batchPath);
        if (!bat.is_open()) return false;

        bat << "@echo off\n";
        bat << "cd /d \"%~dp0\"\n";
        bat << "start pj-game.exe\n";
        bat.close();
    }
    return true;
}

bool ProjectExporter::exportStandaloneGame(const ProjectConfig& config, ExportTargetPlatform target, const std::string& outputFolder) {
    Logger::info("Zacinam export projektu: " + config.projectName);

    fs::create_directories(outputFolder);

    std::string targetAssetDir = outputFolder + "/assets";
    if (!copyDirectoryRecursive(config.assetDirectory, targetAssetDir)) {
        Logger::error("Zlyhalo kopirovanie assetov do exportneho balika!");
        return false;
    }

    std::string configPath = outputFolder + "/game_config.dat";
    std::ofstream cfg(configPath);
    if (cfg.is_open()) {
        cfg << config.serialize();
        cfg.close();
    }

    std::string srcBin = "./pj-game";
    std::string dstBin = outputFolder + "/pj-game";
    if (fs::exists(srcBin)) {
        fs::copy_file(srcBin, dstBin, fs::copy_options::overwrite_existing);
        fs::permissions(dstBin, fs::perms::owner_all | fs::perms::group_read | fs::perms::others_read, fs::perm_options::add);
        Logger::info("Binarka hry uspesne zbalena.");
    }

    createExecutableScript(outputFolder, config.projectName, target);

    Logger::info("[EXPORT DOKONCENY]: Hra exportovana do: " + outputFolder);
    return true;
}

} // namespace PJEngine