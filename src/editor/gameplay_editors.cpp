#include "editor/gameplay_editors.h"

namespace PJEngine {

void GameplayEditors::createQuickQuest(QuestManager& qm, const std::string& id, const std::string& title, const std::string& targetItem, uint32_t count) {
    Quest q(id, title, "Automaticky vytvorena uloha");
    q.addObjective(QuestObjective{"Zozbieraj: " + targetItem, ObjectiveType::COLLECT, targetItem, 0, count, false});
    qm.registerQuest(q);
}

void GameplayEditors::createBranchingDialogue(DialogueTree& dTree, const std::string& npcName, const std::string& text, const std::string& opt1, const std::string& opt2) {
    DialogueNode root{"start", npcName, text, {{opt1, "node_a"}, {opt2, "node_b"}}};
    DialogueNode nodeA{"node_a", npcName, "Vyborne, dohodnute!", {}};
    DialogueNode nodeB{"node_b", npcName, "Skoda, vrat sa neskor.", {}};

    dTree.addNode(root);
    dTree.addNode(nodeA);
    dTree.addNode(nodeB);
}

} // namespace PJEngine