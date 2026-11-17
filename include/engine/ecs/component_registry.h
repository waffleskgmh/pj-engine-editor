#pragma once

#include "engine/ecs/ecs_world.h"
#include <string>
#include <vector>
#include <functional>

namespace PJEngine {

struct ComponentTypeInfo {
    std::string name;
    std::function<bool(ECSWorld&, Entity)> hasComponent;
    std::function<void(ECSWorld&, Entity)> addComponent;
    std::function<void(ECSWorld&, Entity)> removeComponent;
    std::function<void(ECSWorld&, Entity)> renderInspector;
};

class ComponentRegistry {
public:
    static ComponentRegistry& get();

    void registerComponent(const ComponentTypeInfo& info);
    const std::vector<ComponentTypeInfo>& getAllComponents() const { return components; }

private:
    ComponentRegistry() = default;
    std::vector<ComponentTypeInfo> components;
};

} // namespace PJEngine