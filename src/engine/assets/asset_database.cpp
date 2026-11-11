#include "engine/assets/asset_database.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <random>
#include <iomanip>
#include <sys/stat.h>
#include <dirent.h>

namespace PJEngine {

AssetDatabase& AssetDatabase::get() {
    static AssetDatabase instance;
    return instance;
}

std::string AssetMetadata::serialize() const {
    std::stringstream ss;
    ss << "GUID: " << guid << "\n";
    ss << "PATH: " << filePath << "\n";
    ss << "VPATH: " << virtualPath << "\n";
    ss << "TYPE: " << static_cast<int>(type) << "\n";
    ss << "SIZE: " << fileSizeBytes << "\n";
    ss << "MODIFIED: " << lastModifiedTimestamp << "\n";
    return ss.str();
}

AssetMetadata AssetMetadata::deserialize(const std::string& data) {
    AssetMetadata meta;
    std::stringstream ss(data);
    std::string line;

    while (std::getline(ss, line)) {
        if (line.rfind("GUID: ", 0) == 0) meta.guid = line.substr(6);
        else if (line.rfind("PATH: ", 0) == 0) meta.filePath = line.substr(6);
        else if (line.rfind("VPATH: ", 0) == 0) meta.virtualPath = line.substr(7);
        else if (line.rfind("TYPE: ", 0) == 0) meta.type = static_cast<AssetType>(std::stoi(line.substr(6)));
        else if (line.rfind("SIZE: ", 0) == 0) meta.fileSizeBytes = std::stoull(line.substr(6));
        else if (line.rfind("MODIFIED: ", 0) == 0) meta.lastModifiedTimestamp = std::stoull(line.substr(10));
    }
    return meta;
}

std::string AssetDatabase::generateRandomGuid() {
    static std::random_device rd;
    static std::mt19937_64 gen(rd());
    static std::uniform_int_distribution<uint64_t> dis;

    uint64_t part1 = dis(gen);
    uint64_t part2 = dis(gen);

    std::stringstream ss;
    ss << std::hex << std::setfill('0') << std::setw(16) << part1 << std::setw(16) << part2;
    return ss.str();
}

AssetType AssetDatabase::detectAssetTypeFromExtension(const std::string& ext) {
    std::string lower = ext;
    for (char& c : lower) c = static_cast<char>(std::tolower(c));

    if (lower == ".png" || lower == ".bmp" || lower == ".jpg" || lower == ".jpeg") return AssetType::TEXTURE;
    if (lower == ".wav" || lower == ".ogg" || lower == ".mp3") return AssetType::AUDIO;
    if (lower == ".pjscene" || lower == ".scene") return AssetType::SCENE;
    if (lower == ".pj") return AssetType::SCRIPT_PJ;
    if (lower == ".pjmap" || lower == ".tmx") return AssetType::TILEMAP;
    if (lower == ".prefab") return AssetType::PREFAB;

    return AssetType::UNKNOWN;
}

void AssetDatabase::initialize(const std::string& projectRootDirectory) {
    rootDirectory = projectRootDirectory;
    scanProjectAssets();
}

void AssetDatabase::scanProjectAssets() {
    registry.clear();
    pathToGuidMap.clear();

    // Vytvorenie root priečinka, ak neexistuje
    mkdir(rootDirectory.c_str(), 0755);
    scanDirectoryRecursive(rootDirectory);
}

void AssetDatabase::scanDirectoryRecursive(const std::string& currentDir) {
    DIR* dir = opendir(currentDir.c_str());
    if (!dir) return;

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string name = entry->d_name;
        if (name == "." || name == "..") continue;

        std::string fullPath = currentDir + "/" + name;
        struct stat st;
        if (stat(fullPath.c_str(), &st) == -1) continue;

        if (S_ISDIR(st.st_mode)) {
            scanDirectoryRecursive(fullPath);
        } else if (S_ISREG(st.st_mode)) {
            // Ignorujeme samotné .meta súbory
            if (fullPath.length() >= 5 && fullPath.substr(fullPath.length() - 5) == ".meta") {
                continue;
            }

            size_t dotPos = name.find_last_of('.');
            std::string ext = (dotPos != std::string::npos) ? name.substr(dotPos) : "";
            AssetType type = detectAssetTypeFromExtension(ext);

            std::string metaPath = fullPath + ".meta";
            AssetMetadata meta;

            if (!loadMetaFile(metaPath, meta)) {
                // Vygenerujeme nové metadáta a pridelíme GUID
                meta.guid = generateRandomGuid();
                meta.filePath = fullPath;
                meta.virtualPath = "assets://" + fullPath.substr(rootDirectory.length() + 1);
                meta.type = type;
                meta.fileSizeBytes = static_cast<uint64_t>(st.st_size);
                meta.lastModifiedTimestamp = static_cast<uint64_t>(st.st_mtime);
                saveMetaFile(meta);
            }

            registry[meta.guid] = meta;
            pathToGuidMap[meta.filePath] = meta.guid;
        }
    }
    closedir(dir);
}

bool AssetDatabase::saveMetaFile(const AssetMetadata& meta) {
    std::string metaPath = meta.filePath + ".meta";
    std::ofstream f(metaPath);
    if (!f.is_open()) return false;
    f << meta.serialize();
    return true;
}

bool AssetDatabase::loadMetaFile(const std::string& metaFilePath, AssetMetadata& outMeta) {
    std::ifstream f(metaFilePath);
    if (!f.is_open()) return false;
    std::stringstream ss;
    ss << f.rdbuf();
    outMeta = AssetMetadata::deserialize(ss.str());
    return !outMeta.guid.empty();
}

std::string AssetDatabase::registerAsset(const std::string& relativePath, AssetType type) {
    auto it = pathToGuidMap.find(relativePath);
    if (it != pathToGuidMap.end()) return it->second;

    AssetMetadata meta;
    meta.guid = generateRandomGuid();
    meta.filePath = relativePath;
    meta.virtualPath = "assets://" + relativePath;
    meta.type = type;

    struct stat st;
    if (stat(relativePath.c_str(), &st) == 0) {
        meta.fileSizeBytes = static_cast<uint64_t>(st.st_size);
        meta.lastModifiedTimestamp = static_cast<uint64_t>(st.st_mtime);
    }

    saveMetaFile(meta);
    registry[meta.guid] = meta;
    pathToGuidMap[meta.filePath] = meta.guid;

    return meta.guid;
}

std::string AssetDatabase::getGuidByPath(const std::string& path) const {
    auto it = pathToGuidMap.find(path);
    return it != pathToGuidMap.end() ? it->second : "";
}

std::string AssetDatabase::getPathByGuid(const std::string& guid) const {
    auto it = registry.find(guid);
    return it != registry.end() ? it->second.filePath : "";
}

const AssetMetadata* AssetDatabase::getMetadata(const std::string& guid) const {
    auto it = registry.find(guid);
    return it != registry.end() ? &it->second : nullptr;
}

void AssetDatabase::refresh() {
    scanProjectAssets();
}

} // namespace PJEngine