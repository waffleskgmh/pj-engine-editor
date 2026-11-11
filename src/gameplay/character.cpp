#include "gameplay/character.h"
#include <cmath>
#include <algorithm>

namespace PJEngine {

bool CharacterStats::addXP(float amount) {
    currentXP += amount;
    bool leveledUp = false;

    while (currentXP >= maxXP) {
        currentXP -= maxXP;
        level++;
        maxXP = std::floor(100.0f * std::pow(static_cast<float>(level), 1.4f));

        maxHealth += 20.0f;
        health = maxHealth;
        maxMana += 10.0f;
        mana = maxMana;
        attack += 3.0f;
        defense += 2.0f;

        leveledUp = true;
    }
    return leveledUp;
}

void CharacterStats::takeDamage(float amount) {
    float actualDamage = std::max(1.0f, amount - defense);
    health = std::max(0.0f, health - actualDamage);
}

void CharacterStats::heal(float amount) {
    health = std::min(maxHealth, health + amount);
}

void CharacterStats::applyEffect(const StatusEffect& effect) {
    activeEffects.push_back(effect);
}

void CharacterStats::updateEffects(float dt) {
    for (auto it = activeEffects.begin(); it != activeEffects.end();) {
        it->elapsed += dt;
        it->timer += dt;

        if (it->timer >= it->tickInterval) {
            it->timer -= it->tickInterval;
            if (it->onTick) it->onTick(*this);
        }

        if (it->elapsed >= it->duration) {
            it = activeEffects.erase(it);
        } else {
            ++it;
        }
    }
}

void CharacterStats::addSkill(const SkillNode& skill) {
    skillTree[skill.id] = skill;
}

bool CharacterStats::unlockSkill(const std::string& skillId) {
    auto it = skillTree.find(skillId);
    if (it == skillTree.end() || it->second.isUnlocked) return false;
    if (level < it->second.levelRequired) return false;

    if (!it->second.parentSkillId.empty()) {
        auto parentIt = skillTree.find(it->second.parentSkillId);
        if (parentIt == skillTree.end() || !parentIt->second.isUnlocked) {
            return false;
        }
    }

    it->second.isUnlocked = true;
    return true;
}

} // namespace PJEngine