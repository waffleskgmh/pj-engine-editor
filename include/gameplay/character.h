#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

namespace PJEngine {

struct StatusEffect {
    std::string id;
    std::string name;
    float duration = 5.0f;
    float tickInterval = 1.0f;
    float timer = 0.0f;
    float elapsed = 0.0f;
    bool isBuff = true;
    std::function<void(class CharacterStats&)> onTick;
};

struct SkillNode {
    std::string id;
    std::string name;
    std::string description;
    bool isUnlocked = false;
    uint32_t levelRequired = 1;
    std::string parentSkillId;
};

class CharacterStats {
public:
    uint32_t level = 1;
    float currentXP = 0.0f;
    float maxXP = 100.0f;

    float health = 100.0f;
    float maxHealth = 100.0f;
    float mana = 50.0f;
    float maxMana = 50.0f;
    float stamina = 100.0f;
    float maxStamina = 100.0f;

    float attack = 10.0f;
    float defense = 5.0f;
    float speed = 150.0f;

    std::vector<StatusEffect> activeEffects;
    std::unordered_map<std::string, SkillNode> skillTree;

    CharacterStats() = default;

    bool addXP(float amount);
    void takeDamage(float amount);
    void heal(float amount);
    void applyEffect(const StatusEffect& effect);
    void updateEffects(float dt);

    void addSkill(const SkillNode& skill);
    bool unlockSkill(const std::string& skillId);
};

} // namespace PJEngine