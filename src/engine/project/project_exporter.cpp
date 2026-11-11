#include "engine/project/project_exporter.h"
#include "engine/core/logger.h"

#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <dirent.h>

namespace PJEngine {

bool ProjectExporter::copyDirectoryRecursive(const std::string& src, const std::string& dst) {
    mkdir(dst.c_str(), 0755);

    DIR* dir = opendir(src.c_str());
    if (!dir) return false;

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string name = entry->d_name;
        if (name == "." || name == "..") continue;

        std::string srcPath = src + "/" + name;
        std::string dstPath = dst + "/" + name;
        struct stat st;
        if (stat(srcPath.c_str(), &st) == -1) continue;

        if (S_ISDIR(st.st_mode)) {
            copyDirectoryRecursive(srcPath, dstPath);
        } else if (S_ISREG(st.st_mode)) {
            std::ifstream in(srcPath, std::ios::binary);
            std::ofstream out(dstPath, std::ios::binary);
            if (in.is_open() && out.is_open()) {
                out << in.rdbuf();
            }
        }
    }
    closedir(dir);
    return true;
}

bool ProjectExporter::createExecutableScript(const std::string& outputFolder, const std::string& gameName, ExportTargetPlatform target) {
    std::string runnerPath = outputFolder + "/run_game.sh";
    std::ofstream sh(runnerPath);
    if (!sh.is_open()) return false;

    sh << "#!/bin/bash\n";
    sh << "# Automaticky vygenerovany spusstac hry: " << gameName << "\n";
    sh << "cd \"$(dirname \"$0\")\"\n";

    if (target == ExportTargetPlatform::RASPBERRY_PI_ARM) {
        sh << "echo \"Spustam hru na Raspberry Pi...\"\n";
    } else {
        sh << "echo \"Spustam hru pre desktop...\"\n";
    }

    sh << "./pj-game\n";
    sh.close();

    chmod(runnerPath.c_str(), 0755);
    return true;
}

bool ProjectExporter::exportStandaloneGame(const ProjectConfig& config, ExportTargetPlatform target, const std::string& outputFolder) {
    Logger::info("Zacinam export projektu: " + config.projectName);

    // 1. Vytvorenie cieľového priečinka
    mkdir(outputFolder.c_str(), 0755);

    // 2. Skopírovanie všetkých herných assetov (scény, skripty, textúry, dlaždice)
    std::string targetAssetDir = outputFolder + "/assets";
    if (!copyDirectoryRecursive(config.assetDirectory, targetAssetDir)) {
        Logger::error("Zlyhalo kopirovanie assetov do exportneho balika!");
        return false;
    }

    // 3. Uloženie konfigurácie pre standalone runtime
    std::string configPath = outputFolder + "/game_config.dat";
    std::ofstream cfg(configPath);
    if (cfg.is_open()) {
        cfg << config.serialize();
        cfg.close();
    }

    // 4. Skopírovanie samostatnej hernej binárky (pj-game)
    std::string srcBin = "./pj-game";
    std::string dstBin = outputFolder + "/pj-game";
    std::ifstream binIn(srcBin, std::ios::binary);
    std::ofstream binOut(dstBin, std::ios::binary);
    if (binIn.is_open() && binOut.is_open()) {
        binOut << binIn.rdbuf();
        binIn.close();
        binOut.close();
        chmod(dstBin.c_str(), 0755);
        Logger::info("Binarka hry (pj-game) uspesne zbalena.");
    } else {
        Logger::warn("Binarka ./pj-game nebola najdena v koreni, nezabudnite ju skompilovat.");
    }

    // 5. Vytvorenie spúšťača
    createExecutableScript(outputFolder, config.projectName, target);

    Logger::info("[EXPORT DOKONCENY]: Hra bola uspesne exportovana do: " + outputFolder);
    return true;
}

} // namespace PJEngine