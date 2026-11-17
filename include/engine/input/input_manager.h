#pragma once

#include <string>
#include <unordered_map>

namespace PJEngine {

class InputManager {
public:
    static InputManager& get();

    void bindAction(const std::string& actionName, const std::string& primaryKey, const std::string& secondaryKey = "");
    bool isActionPressed(const std::string& actionName, const std::unordered_map<std::string, bool>& keyStates) const;

private:
    InputManager();
    std::unordered_map<std::string, std::pair<std::string, std::string>> actionBindings;
};

} // namespace PJEngine