#pragma once

#include "pj/pj_vm.h"
#include "engine/ecs/ecs_world.h"
#include "engine/physics/physics_engine2d.h"
#include "engine/audio/audio_engine.h"
#include "gameplay/inventory.h"
#include "gameplay/quest.h"
#include <unordered_map>
#include <string>

namespace PJEngine {

struct EngineContext {
    ECSWorld* world = nullptr;
    PhysicsEngine2D* physics = nullptr;
    AudioEngine* audio = nullptr;
    Inventory* inventory = nullptr;
    QuestManager* quests = nullptr;
    std::unordered_map<std::string, bool> inputKeys;

    void setKeyDown(const std::string& key, bool isDown) {
        inputKeys[key] = isDown;
    }

    bool isKeyDown(const std::string& key) const {
        auto it = inputKeys.find(key);
        return it != inputKeys.end() ? it->second : false;
    }
};

class PJEngineBinding {
public:
    static void bind(PJVM* vm, EngineContext* context);

private:
    static EngineContext* ctx;

    // Natívne FFI funkcie volateľné priamo zo skriptu .pj
    static PJValue nativeInputIsKeyDown(PJVM* vm, int argCount, PJValue* args);
    static PJValue nativeInputGetAxis(PJVM* vm, int argCount, PJValue* args);
    static PJValue nativeEntityGetPosition(PJVM* vm, int argCount, PJValue* args);
    static PJValue nativeEntitySetPosition(PJVM* vm, int argCount, PJValue* args);
    static PJValue nativeEntityGetVelocity(PJVM* vm, int argCount, PJValue* args);
    static PJValue nativeEntitySetVelocity(PJVM* vm, int argCount, PJValue* args);
    static PJValue nativeEntityApplyImpulse(PJVM* vm, int argCount, PJValue* args);
    static PJValue nativeAudioPlaySound(PJVM* vm, int argCount, PJValue* args);
    static PJValue nativeInventoryAddItem(PJVM* vm, int argCount, PJValue* args);
    static PJValue nativeInventoryHasItem(PJVM* vm, int argCount, PJValue* args);
    static PJValue nativeQuestAdvance(PJVM* vm, int argCount, PJValue* args);
    static PJValue nativeGameLog(PJVM* vm, int argCount, PJValue* args);
};

} // namespace PJEngine