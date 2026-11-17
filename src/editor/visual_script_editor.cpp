#include "editor/visual_script_editor.h"
#include "imgui.h"
#include <sstream>

namespace PJEngine {

void VisualScriptGraph::addNode(const std::string& id, const std::string& title, const std::string& type, const Vec2& pos, const std::string& val) {
    nodes.push_back({id, title, type, pos, val});
}

void VisualScriptGraph::connect(const std::string& fromId, const std::string& toId) {
    connections.push_back({fromId, toId});
}

std::string VisualScriptGraph::compileToPJScript() const {
    std::ostringstream ss;
    ss << "// Automaticky kompilovane z Vizualneho Grafu PJ Studio\n\n";

    for (const auto& node : nodes) {
        if (node.type == "Event_Start") {
            ss << "// Spustenie hry (Awake)\n";
            ss << "Game_log(\"Hra odstartovana cez Visual Script!\");\n";
        } else if (node.type == "Event_Update") {
            ss << "// Herny cyklus (Frame Update)\n";
            ss << "let h = Input_getAxis(\"horizontal\");\n";
            ss << "let speed = 200.0;\n";
            ss << "if (h != 0.0) {\n";
            ss << "    Entity_setVelocity(0, h * speed, 0.0);\n";
            ss << "}\n";
        } else if (node.type == "Input_Key") {
            ss << "if (Input_isKeyDown(\"" << node.customValue << "\")) {\n";
            for (const auto& conn : connections) {
                if (conn.fromNodeId == node.id) {
                    for (const auto& target : nodes) {
                        if (target.id == conn.toNodeId) {
                            if (target.type == "Apply_Impulse") {
                                ss << "    Entity_applyImpulse(0, 0.0, -15.0);\n";
                            } else if (target.type == "Play_Sound") {
                                ss << "    Audio_playSound(\"" << target.customValue << "\", 1.0);\n";
                            } else if (target.type == "Log_Message") {
                                ss << "    Game_log(\"" << target.customValue << "\");\n";
                            }
                        }
                    }
                }
            }
            ss << "}\n";
        } else if (node.type == "Distance_Check") {
            ss << "let pPos = Entity_getPosition(0);\n";
            ss << "let ePos = Entity_getPosition(1);\n";
            ss << "if (pPos != null && ePos != null) {\n";
            ss << "    let dx = pPos.x - ePos.x;\n";
            ss << "    let dy = pPos.y - ePos.y;\n";
            ss << "    if ((dx*dx + dy*dy) < 10000.0) {\n";
            ss << "        Game_log(\"Hrac v blizkosti nepriatela!\");\n";
            ss << "    }\n";
            ss << "}\n";
        }
    }
    return ss.str();
}

void VisualScriptGraph::renderGUI() {
    ImGui::Begin("Visual Script Graph");

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 offset = ImGui::GetCursorScreenPos();

    for (const auto& conn : connections) {
        ImVec2 p1(offset.x + 100, offset.y + 50);
        ImVec2 p2(offset.x + 250, offset.y + 120);
        drawList->AddBezierCubic(p1, ImVec2(p1.x + 50, p1.y), ImVec2(p2.x - 50, p2.y), p2, IM_COL32(255, 200, 50, 255), 3.0f);
    }

    for (const auto& node : nodes) {
        ImGui::SetCursorScreenPos(ImVec2(offset.x + node.position.x, offset.y + node.position.y));
        ImGui::BeginGroup();
        ImGui::Button(node.title.c_str(), ImVec2(130, 26));
        ImGui::TextDisabled("Typ: %s", node.type.c_str());
        if (!node.customValue.empty()) {
            ImGui::Text("Hodnota: %s", node.customValue.c_str());
        }
        ImGui::EndGroup();
    }

    ImGui::End();
}

} // namespace PJEngine