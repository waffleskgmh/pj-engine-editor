#pragma once

#include "gameplay/quest.h"
#include "gameplay/dialogue.h"
#include "gameplay/inventory.h"

namespace PJEngine {

class GameplayEditors {
public:
    static void createQuickQuest(QuestManager& qm, const std::string& id, const std::string& title, const std::string& targetItem, uint32_t count);
    static void createBranchingDialogue(DialogueTree& dTree, const std::string& npcName, const std::string& text, const std::string& opt1, const std::string& opt2);
};

} // namespace PJEngine