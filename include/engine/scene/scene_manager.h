#pragma once

#include "engine/scene/scene.h"
#include <memory>
#include <unordered_map>
#include <functional>

namespace PJEngine {

class SceneManager {
public:
    void registerScene(const std::string& name, std::function<std::shared_ptr<Scene>()> factory);
    bool loadScene(const std::string& name);
    
    std::shared_ptr<Scene> getActiveScene() const { return activeScene; }

private:
    std::shared_ptr<Scene> activeScene;
    std::unordered_map<std::string, std::function<std::shared_ptr<Scene>()>> registry;
};

} // namespace PJEngine