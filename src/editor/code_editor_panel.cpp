#include "editor/code_editor_panel.h"
#include "pj/pj_lexer.h"
#include "pj/pj_compiler.h"
#include "engine/core/logger.h"
#include "imgui.h"

#include <fstream>
#include <sstream>
#include <algorithm>

namespace PJEngine {

CodeEditorPanel::CodeEditorPanel() {
    loadSampleScript();
}

void CodeEditorPanel::loadSampleScript() {
    ScriptTab tab;
    tab.filePath = "assets/scripts/player_controller.pj";
    tab.title = "player_controller.pj";

    std::string defaultCode =
        "// PJ Script: Ovladanie hraca (WSAD / Sipky + Medzernik)\n\n"
        "let h = Input_getAxis(\"horizontal\");\n"
        "let speed = 220.0;\n\n"
        "if (h != 0.0) {\n"
        "    let v = Entity_getVelocity(0);\n"
        "    Entity_setVelocity(0, h * speed, v.y);\n"
        "}\n\n"
        "if (Input_isKeyDown(\"space\")) {\n"
        "    // Skok s impulzom\n"
        "    Entity_applyImpulse(0, 0.0, -15.0);\n"
        "    Game_log(\"Hrac vyskocil!\");\n"
        "}\n";

    tab.textBuffer.resize(65536, '\0');
    std::copy(defaultCode.begin(), defaultCode.end(), tab.textBuffer.begin());
    tab.isDirty = false;

    openTabs.push_back(tab);
    activeTabIndex = 0;
}

void CodeEditorPanel::openScript(const std::string& path) {
    // Ak už je súbor otvorený, len sa naň prepneme
    for (size_t i = 0; i < openTabs.size(); ++i) {
        if (openTabs[i].filePath == path) {
            activeTabIndex = static_cast<int>(i);
            return;
        }
    }

    std::ifstream file(path);
    if (!file.is_open()) {
        Logger::error("Nepodarilo sa otvorit skript: " + path);
        return;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    ScriptTab tab;
    tab.filePath = path;

    size_t lastSlash = path.find_last_of("/\\");
    tab.title = (lastSlash != std::string::npos) ? path.substr(lastSlash + 1) : path;

    tab.textBuffer.resize(std::max(static_cast<size_t>(65536), content.length() + 1024), '\0');
    std::copy(content.begin(), content.end(), tab.textBuffer.begin());
    tab.isDirty = false;

    openTabs.push_back(tab);
    activeTabIndex = static_cast<int>(openTabs.size() - 1);
    Logger::info("Otvoreny skript v editore: " + path);
}

void CodeEditorPanel::closeTab(int index) {
    if (index >= 0 && index < static_cast<int>(openTabs.size())) {
        openTabs.erase(openTabs.begin() + index);
        if (activeTabIndex >= static_cast<int>(openTabs.size())) {
            activeTabIndex = static_cast<int>(openTabs.size()) - 1;
        }
    }
}

bool CodeEditorPanel::saveActiveScript(EditorApp& app) {
    if (activeTabIndex < 0 || activeTabIndex >= static_cast<int>(openTabs.size())) return false;

    auto& tab = openTabs[activeTabIndex];
    std::string code(tab.textBuffer.data());

    // 1. Zápis na disk
    std::ofstream file(tab.filePath);
    if (!file.is_open()) {
        tab.lastCompilationError = "Chyba pri zapise do suboru na disk!";
        Logger::error("[CodeEditor]: " + tab.lastCompilationError);
        return false;
    }
    file << code;
    file.close();
    tab.isDirty = false;

    // 2. Kompilácia a okamžitý Hot-Reload
    std::string err;
    bool success = compileAndHotReload(app, code, err);
    tab.lastCompilationError = err;

    if (success) {
        tab.errorLine = -1;
        Logger::info("[Hot-Reload]: Skript " + tab.title + " bol uspesne rekompilovany a nahraty do hry.");
    } else {
        Logger::error("[Hot-Reload Chyba v " + tab.title + "]: " + err);
    }

    return success;
}

bool CodeEditorPanel::compileAndHotReload(EditorApp& app, const std::string& sourceCode, std::string& outError) {
    PJLexer lexer(sourceCode);
    auto tokens = lexer.tokenize();

    PJCompiler compiler;
    auto fn = compiler.compile(tokens);

    if (!fn) {
        outError = "Syntakticka chyba pri kompilacii skriptu PJ!";
        return false;
    }

    bool success = app.hotReloadPJScript(sourceCode);
    if (success) {
        outError = "Kompilacia a Hot-Reload uspesne!";
    } else {
        outError = "Behova chyba vo virtuálnom stroji (VM)!";
    }
    return success;
}

void CodeEditorPanel::renderGUI(EditorApp& app) {
    ImGui::Begin("PJ Script Editor");

    if (openTabs.empty()) {
        ImGui::TextDisabled("Ziadny skript nie je otvoreny. Otvorte skript z Asset Browsera...");
        ImGui::End();
        return;
    }

    // 1. Horná lišta so záložkami (Tab Bar)
    if (ImGui::BeginTabBar("ScriptTabs", ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_AutoSelectNewTabs)) {
        for (int i = 0; i < static_cast<int>(openTabs.size()); ++i) {
            auto& tab = openTabs[i];
            std::string label = (tab.isDirty ? "* " : "") + tab.title + "###tab_" + std::to_string(i);

            bool open = true;
            if (ImGui::BeginTabItem(label.c_str(), &open)) {
                activeTabIndex = i;
                ImGui::EndTabItem();
            }

            if (!open) {
                closeTab(i);
                break;
            }
        }
        ImGui::EndTabBar();
    }

    if (activeTabIndex < 0 || activeTabIndex >= static_cast<int>(openTabs.size())) {
        ImGui::End();
        return;
    }

    auto& activeTab = openTabs[activeTabIndex];

    // 2. Ovládacia lišta (Toolbar)
    if (ImGui::Button("💾 Ulozit a Hot-Reload (Ctrl+S)", ImVec2(220, 26))) {
        saveActiveScript(app);
    }
    ImGui::SameLine();
    ImGui::Text("Subor: %s", activeTab.filePath.c_str());

    ImGui::Separator();

    // 3. Viacriadkové textové editačné pole
    ImVec2 editorSize = ImGui::GetContentRegionAvail();
    editorSize.y -= 30.0f; // Miesto pre spodný stavový riadok

    ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
    if (ImGui::InputTextMultiline("##SourceCodeEditor", activeTab.textBuffer.data(), activeTab.textBuffer.size(), editorSize, flags)) {
        activeTab.isDirty = true;
    }

    // 4. Spodný stavový riadok kompilácie
    if (!activeTab.lastCompilationError.empty()) {
        bool isErr = activeTab.lastCompilationError.find("Chyba") != std::string::npos;
        ImVec4 col = isErr ? ImVec4(1.0f, 0.3f, 0.3f, 1.0f) : ImVec4(0.3f, 1.0f, 0.4f, 1.0f);
        ImGui::TextColored(col, "%s", activeTab.lastCompilationError.c_str());
    } else {
        ImGui::TextDisabled("Pripraveny. Stlacte Ulozit a Hot-Reload pre kompilaciu.");
    }

    ImGui::End();
}

} // namespace PJEngine