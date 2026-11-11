#pragma once

#include "engine/core/math2d.h"
#include <string>
#include <vector>

namespace PJEngine {

struct GraphNode {
    std::string id;
    std::string title;
    std::string type; // "event", "action", "math"
    Vec2 position;
    std::string customValue;
};

struct GraphConnection {
    std::string fromNodeId;
    std::string toNodeId;
};

class VisualScriptGraph {
public:
    std::vector<GraphNode> nodes;
    std::vector<GraphConnection> connections;

    void addNode(const std::string& id, const std::string& title, const std::string& type, const Vec2& pos, const std::string& val = "");
    void connect(const std::string& fromId, const std::string& toId);
    
    // Automatický generátor čistého PJ kódu z grafu uzlov
    std::string compileToPJScript() const;
};

} // namespace PJEngine