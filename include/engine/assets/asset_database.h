#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <cstdint>

namespace PJEngine {

enum class AssetType {
    UNKNOWN,
    TEXTURE,
    AUDIO,
    SCENE,
    SCRIPT_PJ,
    TILEMAP,
    PREFAB
};

struct AssetMetadata {
    std::string guid;
    std::string filePath;
    std::string virtualPath; // napr. "assets://textures/hero.png"
    AssetType type = AssetType::UNKNOWN;
    uint64_t fileSizeBytes = 0;
    uint64_t lastModifiedTimestamp = 0;

    std::string serialize() const;
    static AssetMetadata deserialize(const std::string& data);
};

class AssetDatabase {
public:
    static AssetDatabase& get();

    void initialize(const std::string& projectRootDirectory);
    void scanProjectAssets();
    void refresh();

    // GUID a cesty
    std::string registerAsset(const std::string& relativePath, AssetType type);
    std::string getGuidByPath(const std::string& path) const;
    std::string getPathByGuid(const std::string& guid) const;
    const AssetMetadata* getMetadata(const std::string& guid) const;

    // Pomocné typové zistenia
    static AssetType detectAssetTypeFromExtension(const std::string& extension);
    static std::string generateRandomGuid();

    const std::unordered_map<std::string, AssetMetadata>& getAllAssets() const { return registry; }
    const std::string& getRootDirectory() const { return rootDirectory; }

private:
    AssetDatabase() = default;
    ~AssetDatabase() = default;

    std::string rootDirectory = "assets";
    std::unordered_map<std::string, AssetMetadata> registry;      // GUID -> Metadata
    std::unordered_map<std::string, std::string> pathToGuidMap;   // Path -> GUID

    void scanDirectoryRecursive(const std::string& currentDir);
    bool saveMetaFile(const AssetMetadata& meta);
    bool loadMetaFile(const std::string& metaFilePath, AssetMetadata& outMeta);
};

} // namespace PJEngine