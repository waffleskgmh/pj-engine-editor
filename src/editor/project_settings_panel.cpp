#include "editor/project_settings_panel.h"
#include "engine/project/project_exporter.h"
#include "engine/core/logger.h"
#include "imgui.h"

#include <cstring>

namespace PJEngine {

void ProjectSettingsPanel::syncFromConfig() {
    const auto& cfg = Project::get().getConfig();
    strncpy(nameBuf, cfg.projectName.c_str(), sizeof(nameBuf));
    strncpy(authorBuf, cfg.author.c_str(), sizeof(authorBuf));
    strncpy(versionBuf, cfg.version.c_str(), sizeof(versionBuf));
    strncpy(startSceneBuf, cfg.startScenePath.c_str(), sizeof(startSceneBuf));
    strncpy(exportDirBuf, cfg.exportDirectory.c_str(), sizeof(exportDirBuf));
    isLoaded = true;
}

void ProjectSettingsPanel::syncToConfig() {
    auto& cfg = Project::get().getConfig();
    cfg.projectName = nameBuf;
    cfg.author = authorBuf;
    cfg.version = versionBuf;
    cfg.startScenePath = startSceneBuf;
    cfg.exportDirectory = exportDirBuf;
}

void ProjectSettingsPanel::render() {
    if (!isLoaded) {
        syncFromConfig();
    }

    ImGui::Begin("Project Settings & Export");

    auto& cfg = Project::get().getConfig();

    // 1. Základné informácie o projekte
    ImGui::Text("Konfiguracia projektu:");
    ImGui::Separator();

    if (ImGui::InputText("Nazov Hry", nameBuf, sizeof(nameBuf))) syncToConfig();
    if (ImGui::InputText("Autor / Studio", authorBuf, sizeof(authorBuf))) syncToConfig();
    if (ImGui::InputText("Verzia", versionBuf, sizeof(versionBuf))) syncToConfig();
    if (ImGui::InputText("Pociatocna Scena", startSceneBuf, sizeof(startSceneBuf))) syncToConfig();

    ImGui::Spacing();
    ImGui::Text("Rozlisenie a Grafika:");
    ImGui::DragInt2("Rozlisenie Okna", &cfg.windowWidth, 1.0f, 320, 3840);
    ImGui::Checkbox("Fullscreen (Cele okno)", &cfg.fullscreen);
    ImGui::SameLine();
    ImGui::Checkbox("VSync", &cfg.vSync);
    ImGui::SameLine();
    ImGui::Checkbox("Pixel-Perfect Integer Zoom", &cfg.pixelPerfectIntegerScale);

    int fps = cfg.targetFPS;
    if (ImGui::SliderInt("Cielove FPS", &fps, 30, 144)) {
        cfg.targetFPS = fps;
    }

    ImGui::Spacing();
    if (ImGui::Button("Ulozit Nastavenia Projektu (.pjproj)", ImVec2(-1, 26))) {
        syncToConfig();
        Project::get().save();
    }

    ImGui::Separator();
    ImGui::Spacing();

    // 2. Export Standalone Hry
    ImGui::Text("Export Hotovej Hry (Standalone Build):");

    const char* platforms[] = { "macOS Desktop (Intel / Apple Silicon)", "Raspberry Pi 3 (ARM Linux)", "Linux Desktop" };
    ImGui::Combo("Cielova Platforma", &targetPlatformIdx, platforms, 3);
    ImGui::InputText("Exportny Priecinok", exportDirBuf, sizeof(exportDirBuf));

    if (ImGui::Button("🚀 EXPORTOVAT HRAJUCU HRU", ImVec2(-1, 32))) {
        syncToConfig();
        ExportTargetPlatform target = ExportTargetPlatform::MACOS_DESKTOP;
        if (targetPlatformIdx == 1) target = ExportTargetPlatform::RASPBERRY_PI_ARM;
        else if (targetPlatformIdx == 2) target = ExportTargetPlatform::LINUX_DESKTOP;

        ProjectExporter::exportStandaloneGame(cfg, target, exportDirBuf);
    }

    ImGui::End();
}

} // namespace PJEngine