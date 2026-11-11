#include "gameplay/dialogue.h"

namespace PJEngine {

void DialogueTree::addNode(const DialogueNode& node) {
    nodes[node.id] = node;
}

void DialogueTree::start(const std::string& startNodeId) {
    currentNodeId = startNodeId;
}

bool DialogueTree::selectChoice(size_t index) {
    const auto* cur = getCurrentNode();
    if (!cur || index >= cur->choices.size()) return false;

    const auto& choice = cur->choices[index];
    if (choice.condition && !choice.condition()) return false;

    if (choice.onSelect) choice.onSelect();
    currentNodeId = choice.nextNodeId;
    return true;
}

const DialogueNode* DialogueTree::getCurrentNode() const {
    auto it = nodes.find(currentNodeId);
    return it != nodes.end() ? &it->second : nullptr;
}

bool DialogueTree::isFinished() const {
    return currentNodeId.empty() || nodes.find(currentNodeId) == nodes.end();
}

} // namespace PJEngine