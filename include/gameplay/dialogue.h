#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

namespace PJEngine {

struct DialogueChoice {
    std::string text;
    std::string nextNodeId;
    std::function<bool()> condition = nullptr;
    std::function<void()> onSelect = nullptr;

    DialogueChoice() = default;
    DialogueChoice(const std::string& text, const std::string& nextNodeId,
                   std::function<bool()> cond = nullptr, std::function<void()> onSel = nullptr)
        : text(text), nextNodeId(nextNodeId), condition(cond), onSelect(onSel) {}
};

struct DialogueNode {
    std::string id;
    std::string speaker;
    std::string text;
    std::vector<DialogueChoice> choices;
};

class DialogueTree {
public:
    std::string currentNodeId;
    std::unordered_map<std::string, DialogueNode> nodes;

    void addNode(const DialogueNode& node);
    void start(const std::string& startNodeId);
    bool selectChoice(size_t index);
    const DialogueNode* getCurrentNode() const;
    bool isFinished() const;
};

} // namespace PJEngine