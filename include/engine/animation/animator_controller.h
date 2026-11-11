#pragma once

#include "engine/animation/sprite_animation.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace PJEngine {

enum class ConditionMode {
    GREATER,
    LESS,
    EQUALS,
    NOT_EQUALS,
    TRIGGER
};

struct AnimationCondition {
    std::string parameterName;
    ConditionMode mode = ConditionMode::GREATER;
    float threshold = 0.0f;
};

struct AnimationTransition {
    std::string targetState;
    bool hasExitTime = false; // Ak je true, animácia sa musí dohrať do konca pred prechodom
    std::vector<AnimationCondition> conditions;
};

struct AnimationStateNode {
    std::string name;
    SpriteAnimation clip;
    std::vector<AnimationTransition> transitions;
};

class AnimatorController {
public:
    std::string currentState;
    std::unordered_map<std::string, AnimationStateNode> states;

    AnimatorController() = default;

    // Pridanie stavu a prechodov
    void addState(const std::string& name, const SpriteAnimation& clip);
    void addTransition(const std::string& fromState, const std::string& toState, bool hasExitTime = false);
    void addCondition(const std::string& fromState, const std::string& toState, const std::string& param, ConditionMode mode, float threshold);

    // Parametre stavového automatu
    void setFloat(const std::string& name, float val);
    void setBool(const std::string& name, bool val);
    void setTrigger(const std::string& name);

    float getFloat(const std::string& name) const;
    bool getBool(const std::string& name) const;

    // Krokovanie v hernej slučke
    uint32_t update(float dt);
    uint32_t getCurrentFrame() const;

    std::string serialize() const;
    static AnimatorController deserialize(const std::string& data);

private:
    std::unordered_map<std::string, float> floatParams;
    std::unordered_map<std::string, bool> boolParams;
    std::unordered_map<std::string, bool> triggerParams;

    bool checkConditions(const std::vector<AnimationCondition>& conditions) const;
};

} // namespace PJEngine