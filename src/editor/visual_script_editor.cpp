#include "editor/visual_script_editor.h"
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
    ss << "// Automaticky vygenerovane z Vizuálneho Skriptu (Node Graph)\n";

    for (const auto& node : nodes) {
        if (node.type == "event" && node.title.find("Start") != std::string::npos) {
            ss << "fn onGameStart() {\n";
            ss << "    Game_log(\"Hra odstartovana z Vizuálneho Grafu!\");\n";

            for (const auto& conn : connections) {
                if (conn.fromNodeId == node.id) {
                    for (const auto& target : nodes) {
                        if (target.id == conn.toNodeId) {
                            if (target.type == "action") {
                                ss << "    Audio_playSound(\"" << target.customValue << "\", 1.0);\n";
                            }
                        }
                    }
                }
            }
            ss << "}\n";
        }
    }
    return ss.str();
}

} // namespace PJEngine