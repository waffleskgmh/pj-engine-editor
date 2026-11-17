#include "editor/asset_browser_panel.h"
#include "engine/core/logger.h"
#include "imgui.h"

#include <filesystem>
#include <fstream>
#include <algorithm>

namespace fs = std::filesystem;

namespace PJEngine {

AssetBrowserPanel::AssetBrowserPanel() {
    fs::create_directories(rootPath);
    refreshDirectory();
}

void AssetBrowserPanel::setCurrentDirectory(const std::string& path) {
    currentDirectory = path;
    refreshDirectory();
}

void AssetBrowserPanel::refreshDirectory() {
    currentItems.clear();

    if (!fs::exists(currentDirectory) || !fs::is_directory(currentDirectory)) {
        return;
    }

    for (const auto& entry : fs::directory_iterator(currentDirectory)) {
        std::string filename = entry.path().filename().string();
        if (filename.length() >= 5 && filename.substr(filename.length() - 5) == ".meta") {
            continue;
        }

        DirectoryItem item;
        item.name = filename;
        item.fullPath = entry.path().string();
        item.isDirectory = entry.is_directory();

        if (!item.isDirectory) {
            std::string ext = entry.path().extension().string();
            item.type = AssetDatabase::detectAssetTypeFromExtension(ext);
        }

        currentItems.push_back(item);
    }

    std::sort(currentItems.begin(), currentItems.end(), [](const DirectoryItem& a, const DirectoryItem& b) {
        if (a.isDirectory != b.isDirectory) return a.isDirectory > b.isDirectory;
        return a.name < b.name;
    });
}

const char* AssetBrowserPanel::getAssetTypeIcon(AssetType type, bool isDir) const {
    if (isDir) return "[DIR]";
    switch (type) {
        case AssetType::TEXTURE: return "[TEX]";
        case AssetType::AUDIO: return "[SND]";
        case AssetType::SCENE: return "[SCN]";
        case AssetType::SCRIPT_PJ: return "[.PJ]";
        case AssetType::TILEMAP: return "[MAP]";
        case AssetType::PREFAB: return "[PRE]";
        default: return "[DOC]";
    }
}

void AssetBrowserPanel::createNewScript(const std::string& filename) {
    std::string path = currentDirectory + "/" + filename + ".pj";
    std::ofstream f(path);
    if (f.is_open()) {
        f << "// PJ Script: " << filename << "\n\n";
        f << "let speed = 200.0;\n";
        f << "let h = Input_getAxis(\"horizontal\");\n\n";
        f << "if (h != 0.0) {\n";
        f << "    Game_log(\"Hrac sa pohybuje...\");\n";
        f << "}\n";
        f.close();
        Logger::info("Vytvoreny novy skript: " + path);
        refreshDirectory();
    }
}

void AssetBrowserPanel::createNewScene(const std::string& filename) {
    std::string path = currentDirectory + "/" + filename + ".pjscene";
    std::ofstream f(path);
    if (f.is_open()) {
        f << "PJ_SCENE_VERSION: 1\n";
        f << "SCENE_NAME: " << filename << "\n";
        f << "ENTITIES_COUNT: 0\n";
        f << "HIERARCHY_START\nNODE: Root ID: node_Root ENTITY: 4294967295\n  POS: 0 0\n  ROT: 0\nHIERARCHY_END\n";
        f.close();
        Logger::info("Vytvorena nova scena: " + path);
        refreshDirectory();
    }
}

void AssetBrowserPanel::render() {
    ImGui::Begin("Asset Browser");

    if (currentDirectory != rootPath) {
        if (ImGui::Button("<- Spat")) {
            fs::path p(currentDirectory);
            currentDirectory = p.parent_path().string();
            if (currentDirectory.empty() || currentDirectory.length() < rootPath.length()) {
                currentDirectory = rootPath;
            }
            refreshDirectory();
        }
        ImGui::SameLine();
    }

    ImGui::Text("Priecinok: %s", currentDirectory.c_str());
    ImGui::SameLine();

    if (ImGui::Button("Obnovit")) {
        refreshDirectory();
    }

    ImGui::Separator();

    float padding = 8.0f;
    float thumbnailSize = 75.0f;
    float cellSize = thumbnailSize + padding;

    float panelWidth = ImGui::GetContentRegionAvail().x;
    int columnCount = (int)(panelWidth / cellSize);
    if (columnCount < 1) columnCount = 1;

    ImGui::Columns(columnCount, 0, false);

    for (const auto& item : currentItems) {
        ImGui::PushID(item.fullPath.c_str());

        const char* icon = getAssetTypeIcon(item.type, item.isDirectory);
        ImGui::Button(icon, ImVec2(thumbnailSize, thumbnailSize * 0.7f));

        if (!item.isDirectory && ImGui::BeginDragDropSource()) {
            ImGui::SetDragDropPayload("PJ_ASSET_PATH", item.fullPath.c_str(), item.fullPath.length() + 1);
            ImGui::Text("Asset: %s", item.name.c_str());
            ImGui::EndDragDropSource();
        }

        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
            if (item.isDirectory) {
                currentDirectory = item.fullPath;
                refreshDirectory();
                ImGui::PopID();
                break;
            } else {
                selectedPath = item.fullPath;
                Logger::info("Vybrany asset: " + selectedPath);
            }
        }

        ImGui::TextWrapped("%s", item.name.c_str());
        ImGui::NextColumn();

        ImGui::PopID();
    }

    ImGui::Columns(1);

    if (ImGui::BeginPopupContextWindow(0, 1)) {
        if (ImGui::MenuItem("Vytvorit novy PJ skript")) {
            createNewScript("NovySkript");
        }
        if (ImGui::MenuItem("Vytvorit novu Scenu")) {
            createNewScene("NovaScena");
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Obnovit zlozku")) {
            refreshDirectory();
        }
        ImGui::EndPopup();
    }

    ImGui::End();
}

} // namespace PJEngine