#pragma once

#include "editor/editor_app.h"
#include <string>
#include <vector>

namespace PJEngine {

struct ScriptTab {
    std::string filePath;
    std::string title;
    std::vector<char> textBuffer;
    bool isDirty = false;
    std::string lastCompilationError;
    int errorLine = -1;
};

class CodeEditorPanel {
public:
    std::vector<ScriptTab> openTabs;
    int activeTabIndex = -1;

    CodeEditorPanel();
    ~CodeEditorPanel() = default;

    // Správa súborov a záložiek
    void openScript(const std::string& filePath);
    void closeTab(int index);
    bool saveActiveScript(EditorApp& app);

    // Vykreslenie UI panelu editora v ImGui
    void renderGUI(EditorApp& app);

    // Statický pomocník pre kompiláciu
    static bool compileAndHotReload(EditorApp& app, const std::string& sourceCode, std::string& outError);

private:
    void loadSampleScript();
};

} // namespace PJEngine