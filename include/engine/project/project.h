#pragma once

#include <string>
#include <cstdint>

namespace PJEngine {

struct ProjectConfig {
    std::string projectName = "Moja Nova 2D Hra";
    std::string version = "1.0.0";
    std::string author = "PJ Studio Vyvojar";
    std::string startScenePath = "assets/scenes/level1.pjscene";

    // Nastavenia obrazu a okna
    int windowWidth = 1280;
    int windowHeight = 720;
    bool fullscreen = false;
    bool vSync = true;
    int targetFPS = 60;
    bool pixelPerfectIntegerScale = true;

    // Cesty projektu
    std::string assetDirectory = "assets";
    std::string exportDirectory = "build/export";

    std::string serialize() const;
    static ProjectConfig deserialize(const std::string& data);
};

class Project {
public:
    static Project& get();

    bool createNew(const std::string& projectDir, const std::string& name);
    bool load(const std::string& projectFilePath);
    bool save();

    ProjectConfig& getConfig() { return config; }
    const ProjectConfig& getConfig() const { return config; }
    const std::string& getProjectFilePath() const { return currentProjectFilePath; }
    const std::string& getProjectDirectory() const { return projectRootDirectory; }
    bool isLoaded() const { return projectLoaded; }

private:
    Project() = default;
    ~Project() = default;

    ProjectConfig config;
    std::string currentProjectFilePath;
    std::string projectRootDirectory;
    bool projectLoaded = false;
};

} // namespace PJEngine