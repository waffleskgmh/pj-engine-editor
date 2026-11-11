#pragma once

#include "engine/assets/asset_database.h"
#include <string>
#include <vector>

namespace PJEngine {

struct DirectoryItem {
    std::string name;
    std::string fullPath;
    bool isDirectory = false;
    AssetType type = AssetType::UNKNOWN;
};

class AssetBrowserPanel {
public:
    AssetBrowserPanel();
    ~AssetBrowserPanel() = default;

    void render();
    void setCurrentDirectory(const std::string& path);
    const std::string& getSelectedAssetPath() const { return selectedPath; }

private:
    std::string rootPath = "assets";
    std::string currentDirectory = "assets";
    std::string selectedPath;
    std::vector<DirectoryItem> currentItems;

    void refreshDirectory();
    void createNewScript(const std::string& filename);
    void createNewScene(const std::string& filename);
    const char* getAssetTypeIcon(AssetType type, bool isDir) const;
};

} // namespace PJEngine