#include "engine/project/project.h"
#include "engine/core/logger.h"

#include <fstream>
#include <sstream>
#include <sys/stat.h>

namespace PJEngine {

std::string ProjectConfig::serialize() const {
    std::stringstream ss;
    ss << "PROJECT_NAME: " << projectName << "\n";
    ss << "VERSION: " << version << "\n";
    ss << "AUTHOR: " << author << "\n";
    ss << "START_SCENE: " << startScenePath << "\n";
    ss << "WINDOW_SIZE: " << windowWidth << " " << windowHeight << "\n";
    ss << "FULLSCREEN: " << (fullscreen ? 1 : 0) << "\n";
    ss << "VSYNC: " << (vSync ? 1 : 0) << "\n";
    ss << "TARGET_FPS: " << targetFPS << "\n";
    ss << "PIXEL_PERFECT: " << (pixelPerfectIntegerScale ? 1 : 0) << "\n";
    ss << "ASSET_DIR: " << assetDirectory << "\n";
    ss << "EXPORT_DIR: " << exportDirectory << "\n";
    return ss.str();
}

ProjectConfig ProjectConfig::deserialize(const std::string& data) {
    ProjectConfig cfg;
    std::stringstream ss(data);
    std::string line;

    while (std::getline(ss, line)) {
        if (line.rfind("PROJECT_NAME: ", 0) == 0) cfg.projectName = line.substr(14);
        else if (line.rfind("VERSION: ", 0) == 0) cfg.version = line.substr(9);
        else if (line.rfind("AUTHOR: ", 0) == 0) cfg.author = line.substr(8);
        else if (line.rfind("START_SCENE: ", 0) == 0) cfg.startScenePath = line.substr(13);
        else if (line.rfind("WINDOW_SIZE: ", 0) == 0) {
            std::stringstream sl(line.substr(13));
            sl >> cfg.windowWidth >> cfg.windowHeight;
        } else if (line.rfind("FULLSCREEN: ", 0) == 0) cfg.fullscreen = (std::stoi(line.substr(12)) != 0);
        else if (line.rfind("VSYNC: ", 0) == 0) cfg.vSync = (std::stoi(line.substr(7)) != 0);
        else if (line.rfind("TARGET_FPS: ", 0) == 0) cfg.targetFPS = std::stoi(line.substr(12));
        else if (line.rfind("PIXEL_PERFECT: ", 0) == 0) cfg.pixelPerfectIntegerScale = (std::stoi(line.substr(15)) != 0);
        else if (line.rfind("ASSET_DIR: ", 0) == 0) cfg.assetDirectory = line.substr(11);
        else if (line.rfind("EXPORT_DIR: ", 0) == 0) cfg.exportDirectory = line.substr(12);
    }
    return cfg;
}

Project& Project::get() {
    static Project instance;
    return instance;
}

bool Project::createNew(const std::string& projectDir, const std::string& name) {
    projectRootDirectory = projectDir;
    config.projectName = name;
    currentProjectFilePath = projectDir + "/project.pjproj";

    mkdir(projectDir.c_str(), 0755);
    std::string assetsPath = projectDir + "/" + config.assetDirectory;
    mkdir(assetsPath.c_str(), 0755);

    projectLoaded = save();
    if (projectLoaded) {
        Logger::info("Vytvoreny novy projekt: " + currentProjectFilePath);
    }
    return projectLoaded;
}

bool Project::load(const std::string& projectFilePath) {
    std::ifstream file(projectFilePath);
    if (!file.is_open()) {
        Logger::error("Nepodarilo sa otvorit projektovy subor: " + projectFilePath);
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    config = ProjectConfig::deserialize(buffer.str());
    currentProjectFilePath = projectFilePath;

    size_t lastSlash = projectFilePath.find_last_of("/\\");
    projectRootDirectory = (lastSlash != std::string::npos) ? projectFilePath.substr(0, lastSlash) : ".";

    projectLoaded = true;
    Logger::info("Projekt uspesne nacitany: " + config.projectName);
    return true;
}

bool Project::save() {
    if (currentProjectFilePath.empty()) {
        currentProjectFilePath = "project.pjproj";
    }

    std::ofstream file(currentProjectFilePath);
    if (!file.is_open()) {
        Logger::error("Chyba pri ukladani projektu: " + currentProjectFilePath);
        return false;
    }

    file << config.serialize();
    Logger::info("Projekt ulozeny: " + currentProjectFilePath);
    return true;
}

} // namespace PJEngine