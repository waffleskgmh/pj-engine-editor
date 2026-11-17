#include "engine/input/input_manager.h"

namespace PJEngine {

InputManager& InputManager::get() {
    static InputManager instance;
    return instance;
}

InputManager::InputManager() {
    bindAction("Jump", "space", "w");
    bindAction("MoveLeft", "left", "a");
    bindAction("MoveRight", "right", "d");
    bindAction("Interact", "e", "");
}

void InputManager::bindAction(const std::string& actionName, const std::string& primaryKey, const std::string& secondaryKey) {
    actionBindings[actionName] = {primaryKey, secondaryKey};
}

bool InputManager::isActionPressed(const std::string& actionName, const std::unordered_map<std::string, bool>& keyStates) const {
    auto it = actionBindings.find(actionName);
    if (it == actionBindings.end()) return false;

    auto checkKey = [&](const std::string& k) {
        if (k.empty()) return false;
        auto kit = keyStates.find(k);
        return kit != keyStates.end() ? kit->second : false;
    };

    return checkKey(it->second.first) || checkKey(it->second.second);
}

} // namespace PJEngine