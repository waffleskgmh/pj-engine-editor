#include "editor/console_panel.h"
#include "imgui.h"
#include <sstream>

namespace PJEngine {

void ConsolePanel::render() {
    ImGui::Begin("Console");

    auto messages = Logger::get().getMessages();

    // 1. Ovládacia lišta s kopírovaním
    if (ImGui::Button("Clear")) {
        Logger::get().clear();
    }
    ImGui::SameLine();

    // TLAČIDLO PRE KOPÍROVANIE VŠETKÝCH LOGOV DO SCHRÁNKY
    if (ImGui::Button("📋 Copy All")) {
        std::stringstream ss;
        for (const auto& m : messages) {
            const char* pfx = (m.level == LogLevel::LOG_ERROR) ? "[ERROR]" : (m.level == LogLevel::WARNING ? "[WARN]" : "[INFO]");
            ss << "[" << m.timestamp << "] " << pfx << " " << m.message << "\n";
        }
        ImGui::SetClipboardText(ss.str().c_str());
        Logger::info("Vsetky logy z konzoly boli skopirovane do schranky.");
    }
    ImGui::SameLine();

    ImGui::Checkbox("Auto-scroll", &autoScroll);
    ImGui::SameLine();

    ImGui::SetNextItemWidth(140);
    ImGui::InputTextWithHint("##Search", "Search...", searchFilter, IM_ARRAYSIZE(searchFilter));
    ImGui::SameLine();

    ImGui::Checkbox("Info", &showInfo);
    ImGui::SameLine();
    ImGui::Checkbox("Warnings", &showWarnings);
    ImGui::SameLine();
    ImGui::Checkbox("Errors", &showErrors);

    ImGui::Separator();

    // 2. Posuvná oblasť logov
    ImGui::BeginChild("LogScrollingRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

    for (size_t i = 0; i < messages.size(); ++i) {
        const auto& logMsg = messages[i];
        if (logMsg.level == LogLevel::INFO && !showInfo) continue;
        if (logMsg.level == LogLevel::WARNING && !showWarnings) continue;
        if (logMsg.level == LogLevel::LOG_ERROR && !showErrors) continue;

        if (searchFilter[0] != '\0' && logMsg.message.find(searchFilter) == std::string::npos) {
            continue;
        }

        ImVec4 color;
        const char* prefix = "[INFO]";
        if (logMsg.level == LogLevel::LOG_ERROR) {
            color = ImVec4(1.0f, 0.35f, 0.35f, 1.0f);
            prefix = "[ERROR]";
        } else if (logMsg.level == LogLevel::WARNING) {
            color = ImVec4(1.0f, 0.85f, 0.25f, 1.0f);
            prefix = "[WARN]";
        } else {
            color = ImVec4(0.85f, 0.88f, 0.92f, 1.0f);
        }

        ImGui::PushID(static_cast<int>(i));
        ImGui::PushStyleColor(ImGuiCol_Text, color);

        std::string fullLine = "[" + logMsg.timestamp + "] " + prefix + " " + logMsg.message;
        // Kliknutie na konkrétny riadok ho skopíruje do schránky
        if (ImGui::Selectable(fullLine.c_str(), false)) {
            ImGui::SetClipboardText(fullLine.c_str());
            Logger::info("Riadok skopirovany do schranky: " + logMsg.message);
        }

        ImGui::PopStyleColor();
        ImGui::PopID();
    }

    if (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
        ImGui::SetScrollHereY(1.0f);
    }

    ImGui::EndChild();
    ImGui::End();
}

} // namespace PJEngine