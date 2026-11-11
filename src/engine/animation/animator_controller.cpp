#include "engine/animation/animator_controller.h"
#include <sstream>
#include <iostream>

namespace PJEngine {

void AnimatorController::addState(const std::string& name, const SpriteAnimation& clip) {
    AnimationStateNode node;
    node.name = name;
    node.clip = clip;
    states[name] = node;

    if (currentState.empty()) {
        currentState = name;
    }
}

void AnimatorController::addTransition(const std::string& fromState, const std::string& toState, bool hasExitTime) {
    auto it = states.find(fromState);
    if (it == states.end()) return;

    AnimationTransition trans;
    trans.targetState = toState;
    trans.hasExitTime = hasExitTime;
    it->second.transitions.push_back(trans);
}

void AnimatorController::addCondition(const std::string& fromState, const std::string& toState, const std::string& param, ConditionMode mode, float threshold) {
    auto it = states.find(fromState);
    if (it == states.end()) return;

    for (auto& trans : it->second.transitions) {
        if (trans.targetState == toState) {
            AnimationCondition cond;
            cond.parameterName = param;
            cond.mode = mode;
            cond.threshold = threshold;
            trans.conditions.push_back(cond);
            break;
        }
    }
}

void AnimatorController::setFloat(const std::string& name, float val) {
    floatParams[name] = val;
}

void AnimatorController::setBool(const std::string& name, bool val) {
    boolParams[name] = val;
}

void AnimatorController::setTrigger(const std::string& name) {
    triggerParams[name] = true;
}

float AnimatorController::getFloat(const std::string& name) const {
    auto it = floatParams.find(name);
    return it != floatParams.end() ? it->second : 0.0f;
}

bool AnimatorController::getBool(const std::string& name) const {
    auto it = boolParams.find(name);
    return it != boolParams.end() ? it->second : false;
}

bool AnimatorController::checkConditions(const std::vector<AnimationCondition>& conditions) const {
    for (const auto& cond : conditions) {
        if (cond.mode == ConditionMode::TRIGGER) {
            auto it = triggerParams.find(cond.parameterName);
            if (it == triggerParams.end() || !it->second) return false;
        } else if (cond.mode == ConditionMode::GREATER) {
            if (getFloat(cond.parameterName) <= cond.threshold) return false;
        } else if (cond.mode == ConditionMode::LESS) {
            if (getFloat(cond.parameterName) >= cond.threshold) return false;
        } else if (cond.mode == ConditionMode::EQUALS) {
            if (getBool(cond.parameterName) != (cond.threshold > 0.5f)) return false;
        }
    }
    return true;
}

uint32_t AnimatorController::update(float dt) {
    auto it = states.find(currentState);
    if (it == states.end()) return 0;

    auto& activeNode = it->second;

    // Kontrola prechodov do iných stavov
    for (const auto& trans : activeNode.transitions) {
        bool canExit = !trans.hasExitTime || activeNode.clip.isFinished();
        if (canExit && checkConditions(trans.conditions)) {
            // Reset triggerov, ktoré vyvolali prechod
            for (const auto& c : trans.conditions) {
                if (c.mode == ConditionMode::TRIGGER) {
                    triggerParams[c.parameterName] = false;
                }
            }

            currentState = trans.targetState;
            states[currentState].clip.reset();
            return states[currentState].clip.getCurrentFrame();
        }
    }

    return activeNode.clip.update(dt);
}

uint32_t AnimatorController::getCurrentFrame() const {
    auto it = states.find(currentState);
    return it != states.end() ? it->second.clip.getCurrentFrame() : 0;
}

std::string AnimatorController::serialize() const {
    std::stringstream ss;
    ss << "ANIMATOR_CONTROLLER\n";
    ss << "CURRENT_STATE: " << currentState << "\n";
    ss << "STATES_COUNT: " << states.size() << "\n";

    for (const auto& kv : states) {
        const auto& s = kv.second;
        ss << "STATE: " << s.name << " FRAMES: " << s.clip.frames.size() << " LOOP: " << (s.clip.isLooping ? 1 : 0) << "\n";
        for (const auto& f : s.clip.frames) {
            ss << "  FRAME: " << f.frameIndex << " " << f.duration << "\n";
        }
        ss << "TRANSITIONS_COUNT: " << s.transitions.size() << "\n";
        for (const auto& t : s.transitions) {
            ss << "  TRANSITION: " << t.targetState << " " << (t.hasExitTime ? 1 : 0) << " COND_COUNT: " << t.conditions.size() << "\n";
            for (const auto& c : t.conditions) {
                ss << "    COND: " << c.parameterName << " " << static_cast<int>(c.mode) << " " << c.threshold << "\n";
            }
        }
    }
    return ss.str();
}

AnimatorController AnimatorController::deserialize(const std::string& data) {
    AnimatorController ctrl;
    std::stringstream ss(data);
    std::string line;

    while (std::getline(ss, line)) {
        if (line.rfind("CURRENT_STATE: ", 0) == 0) {
            ctrl.currentState = line.substr(15);
        } else if (line.rfind("STATE: ", 0) == 0) {
            std::stringstream sline(line.substr(7));
            std::string stateName, dummy1, dummy2;
            size_t frameCount;
            int loop;
            sline >> stateName >> dummy1 >> frameCount >> dummy2 >> loop;

            SpriteAnimation anim;
            anim.isLooping = (loop != 0);

            for (size_t i = 0; i < frameCount; ++i) {
                std::string fline;
                std::getline(ss, fline);
                std::stringstream sf(fline);
                std::string fhead;
                uint32_t fidx;
                float dur;
                sf >> fhead >> fidx >> dur;
                anim.addFrame(fidx, dur);
            }

            ctrl.addState(stateName, anim);
        }
    }
    return ctrl;
}

} // namespace PJEngine