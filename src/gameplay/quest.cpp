#include "gameplay/quest.h"
#include "gameplay/character.h"

namespace PJEngine {

Quest::Quest(const std::string& id, const std::string& title, const std::string& desc)
    : id(id), title(title), description(desc) {}

void Quest::addObjective(const QuestObjective& obj) {
    objectives.push_back(obj);
}

void Quest::advanceObjective(size_t index, uint32_t amount) {
    if (index >= objectives.size() || state != QuestState::IN_PROGRESS) return;

    auto& obj = objectives[index];
    obj.currentCount = std::min(obj.requiredCount, obj.currentCount + amount);
    if (obj.currentCount >= obj.requiredCount) {
        obj.isCompleted = true;
    }

    if (isReadyToComplete()) {
        state = QuestState::COMPLETED;
    }
}

bool Quest::isReadyToComplete() const {
    if (objectives.empty()) return false;
    for (const auto& obj : objectives) {
        if (!obj.isCompleted) return false;
    }
    return true;
}

void QuestManager::registerQuest(const Quest& quest) {
    quests[quest.id] = quest;
}

bool QuestManager::startQuest(const std::string& id) {
    auto it = quests.find(id);
    if (it == quests.end() || it->second.state != QuestState::NOT_STARTED) return false;
    it->second.state = QuestState::IN_PROGRESS;
    return true;
}

bool QuestManager::advanceQuest(const std::string& id, size_t objectiveIndex, uint32_t amount) {
    auto it = quests.find(id);
    if (it == quests.end()) return false;
    it->second.advanceObjective(objectiveIndex, amount);
    return true;
}

bool QuestManager::completeQuest(const std::string& id, CharacterStats& stats) {
    auto it = quests.find(id);
    if (it == quests.end() || it->second.state != QuestState::COMPLETED) return false;

    stats.addXP(it->second.rewardXP);
    return true;
}

QuestState QuestManager::getQuestState(const std::string& id) const {
    auto it = quests.find(id);
    return it != quests.end() ? it->second.state : QuestState::NOT_STARTED;
}

} // namespace PJEngine
