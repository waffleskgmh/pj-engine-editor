#include "engine/scene/scene_manager.h"

namespace PJEngine {

void SceneManager::registerScene(const std::string& name, std::function<std::shared_ptr<Scene>()> factory) {
    registry[name] = factory;
}

bool SceneManager::loadScene(const std::string& name) {
    auto it = registry.find(name);
    if (it == registry.end()) return false;
    activeScene = it->second();
    return true;
}

} // namespace PJEngine