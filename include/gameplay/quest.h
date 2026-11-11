#pragma once

#include "gameplay/inventory.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

namespace PJEngine {

class CharacterStats;

enum class QuestState {
    NOT_STARTED,
    IN_PROGRESS,
    COMPLETED,
    FAILED
};

enum class ObjectiveType {
    KILL,
    COLLECT,
    REACH
};

struct QuestObjective {
    std::string description;
    ObjectiveType type = ObjectiveType::COLLECT;
    std::string targetId;
    uint32_t currentCount = 0;
    uint32_t requiredCount = 1;
    bool isCompleted = false;
};

class Quest {
public:
    std::string id;
    std::string title;
    std::string description;
    QuestState state = QuestState::NOT_STARTED;
    std::vector<QuestObjective> objectives;

    float rewardXP = 50.0f;
    uint32_t rewardGold = 20;

    Quest() = default;
    Quest(const std::string& id, const std::string& title, const std::string& desc = "");

    void addObjective(const QuestObjective& obj);
    void advanceObjective(size_t index, uint32_t amount = 1);
    bool isReadyToComplete() const;
};

class QuestManager {
public:
    std::unordered_map<std::string, Quest> quests;

    void registerQuest(const Quest& quest);
    bool startQuest(const std::string& id);
    bool advanceQuest(const std::string& id, size_t objectiveIndex, uint32_t amount = 1);
    bool completeQuest(const std::string& id, CharacterStats& stats);
    QuestState getQuestState(const std::string& id) const;
};

} // namespace PJEngine