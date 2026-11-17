#include "engine/ecs/component_registry.h"
#include "engine/ecs/components2d.h"
#include "engine/ecs/health_regen_component.h"
#include "engine/physics/rigidbody2d.h"
#include "engine/physics/collider2d.h"
#include "imgui.h"

namespace PJEngine {

ComponentRegistry& ComponentRegistry::get() {
    static ComponentRegistry instance;
    static bool initialized = false;
    if (!initialized) {
        initialized = true;

        instance.registerComponent({
            "Health Regen",
            [](ECSWorld& w, Entity e) { return w.hasComponent<HealthRegenComponent>(e); },
            [](ECSWorld& w, Entity e) { w.addComponent<HealthRegenComponent>(e); },
            [](ECSWorld& w, Entity e) { w.removeComponent<HealthRegenComponent>(e); },
            [](ECSWorld& w, Entity e) {
                auto& c = w.getComponent<HealthRegenComponent>(e);
                ImGui::DragFloat("Regen Rate (HP/s)", &c.regenRate, 0.5f, 0.0f, 100.0f);
                ImGui::DragFloat("Max Cap", &c.maxHealthCap, 1.0f, 1.0f, 1000.0f);
            }
        });
    }
    return instance;
}

void ComponentRegistry::registerComponent(const ComponentTypeInfo& info) {
    components.push_back(info);
}

} // namespace PJEngine