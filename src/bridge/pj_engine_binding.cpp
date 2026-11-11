#include "bridge/pj_engine_binding.h"
#include "engine/core/math2d.h"
#include "engine/core/logger.h"
#include <iostream>

namespace PJEngine {

EngineContext* PJEngineBinding::ctx = nullptr;

void PJEngineBinding::bind(PJVM* vm, EngineContext* context) {
    ctx = context;

    vm->registerNative("Input_isKeyDown", nativeInputIsKeyDown);
    vm->registerNative("Input_getAxis", nativeInputGetAxis);
    vm->registerNative("Entity_getPosition", nativeEntityGetPosition);
    vm->registerNative("Entity_setPosition", nativeEntitySetPosition);
    vm->registerNative("Entity_getVelocity", nativeEntityGetVelocity);
    vm->registerNative("Entity_setVelocity", nativeEntitySetVelocity);
    vm->registerNative("Entity_applyImpulse", nativeEntityApplyImpulse);
    vm->registerNative("Audio_playSound", nativeAudioPlaySound);
    vm->registerNative("Inventory_addItem", nativeInventoryAddItem);
    vm->registerNative("Inventory_hasItem", nativeInventoryHasItem);
    vm->registerNative("Quest_advance", nativeQuestAdvance);
    vm->registerNative("Game_log", nativeGameLog);
}

PJValue PJEngineBinding::nativeInputIsKeyDown(PJVM* /*vm*/, int argCount, PJValue* args) {
    if (!ctx || argCount < 1 || args[0].type != PJValueType::STRING) return PJValue::Bool(false);
    return PJValue::Bool(ctx->isKeyDown(args[0].str));
}

PJValue PJEngineBinding::nativeInputGetAxis(PJVM* /*vm*/, int argCount, PJValue* args) {
    if (!ctx || argCount < 1 || args[0].type != PJValueType::STRING) return PJValue::Number(0.0);
    const std::string& axis = args[0].str;

    if (axis == "horizontal") {
        float val = 0.0f;
        if (ctx->isKeyDown("left") || ctx->isKeyDown("a")) val -= 1.0f;
        if (ctx->isKeyDown("right") || ctx->isKeyDown("d")) val += 1.0f;
        return PJValue::Number(val);
    }
    if (axis == "vertical") {
        float val = 0.0f;
        if (ctx->isKeyDown("up") || ctx->isKeyDown("w")) val -= 1.0f;
        if (ctx->isKeyDown("down") || ctx->isKeyDown("s")) val += 1.0f;
        return PJValue::Number(val);
    }
    return PJValue::Number(0.0);
}

PJValue PJEngineBinding::nativeEntityGetPosition(PJVM* /*vm*/, int argCount, PJValue* args) {
    if (!ctx || !ctx->world || argCount < 1 || args[0].type != PJValueType::NUMBER) return PJValue::Nil();
    Entity e = static_cast<Entity>(args[0].as.number);

    if (ctx->world->isAlive(e) && ctx->world->hasComponent<Transform2D>(e)) {
        const auto& t = ctx->world->getComponent<Transform2D>(e);
        PJValue obj = PJValue::Object();
        (*obj.object)["x"] = PJValue::Number(t.position.x);
        (*obj.object)["y"] = PJValue::Number(t.position.y);
        return obj;
    }
    return PJValue::Nil();
}

PJValue PJEngineBinding::nativeEntitySetPosition(PJVM* /*vm*/, int argCount, PJValue* args) {
    if (!ctx || !ctx->world || argCount < 3 || args[0].type != PJValueType::NUMBER ||
        args[1].type != PJValueType::NUMBER || args[2].type != PJValueType::NUMBER) {
        return PJValue::Nil();
    }
    Entity e = static_cast<Entity>(args[0].as.number);

    if (ctx->world->isAlive(e) && ctx->world->hasComponent<Transform2D>(e)) {
        auto& t = ctx->world->getComponent<Transform2D>(e);
        t.position.x = static_cast<float>(args[1].as.number);
        t.position.y = static_cast<float>(args[2].as.number);
    }
    return PJValue::Nil();
}

PJValue PJEngineBinding::nativeEntityGetVelocity(PJVM* /*vm*/, int argCount, PJValue* args) {
    if (!ctx || !ctx->world || argCount < 1 || args[0].type != PJValueType::NUMBER) return PJValue::Nil();
    Entity e = static_cast<Entity>(args[0].as.number);

    if (ctx->world->isAlive(e) && ctx->world->hasComponent<RigidBody2D>(e)) {
        const auto& rb = ctx->world->getComponent<RigidBody2D>(e);
        PJValue obj = PJValue::Object();
        (*obj.object)["x"] = PJValue::Number(rb.velocity.x);
        (*obj.object)["y"] = PJValue::Number(rb.velocity.y);
        return obj;
    }
    return PJValue::Nil();
}

PJValue PJEngineBinding::nativeEntitySetVelocity(PJVM* /*vm*/, int argCount, PJValue* args) {
    if (!ctx || !ctx->world || argCount < 3 || args[0].type != PJValueType::NUMBER ||
        args[1].type != PJValueType::NUMBER || args[2].type != PJValueType::NUMBER) {
        return PJValue::Nil();
    }
    Entity e = static_cast<Entity>(args[0].as.number);

    if (ctx->world->isAlive(e) && ctx->world->hasComponent<RigidBody2D>(e)) {
        auto& rb = ctx->world->getComponent<RigidBody2D>(e);
        rb.velocity.x = static_cast<float>(args[1].as.number);
        rb.velocity.y = static_cast<float>(args[2].as.number);
    }
    return PJValue::Nil();
}

PJValue PJEngineBinding::nativeEntityApplyImpulse(PJVM* /*vm*/, int argCount, PJValue* args) {
    if (!ctx || !ctx->world || argCount < 3 || args[0].type != PJValueType::NUMBER ||
        args[1].type != PJValueType::NUMBER || args[2].type != PJValueType::NUMBER) {
        return PJValue::Nil();
    }
    Entity e = static_cast<Entity>(args[0].as.number);

    if (ctx->world->isAlive(e) && ctx->world->hasComponent<RigidBody2D>(e)) {
        auto& rb = ctx->world->getComponent<RigidBody2D>(e);
        rb.applyImpulse(Vec2(static_cast<float>(args[1].as.number), static_cast<float>(args[2].as.number)));
    }
    return PJValue::Nil();
}

PJValue PJEngineBinding::nativeAudioPlaySound(PJVM* /*vm*/, int argCount, PJValue* args) {
    if (!ctx || !ctx->audio || argCount < 1 || args[0].type != PJValueType::STRING) return PJValue::Nil();
    float vol = (argCount >= 2 && args[1].type == PJValueType::NUMBER) ? static_cast<float>(args[1].as.number) : 1.0f;
    ctx->audio->playSound(args[0].str, "sfx", vol);
    return PJValue::Nil();
}

PJValue PJEngineBinding::nativeInventoryAddItem(PJVM* /*vm*/, int argCount, PJValue* args) {
    if (!ctx || !ctx->inventory || argCount < 2 || args[0].type != PJValueType::STRING || args[1].type != PJValueType::NUMBER) {
        return PJValue::Bool(false);
    }
    std::string itemId = args[0].str;
    uint32_t count = static_cast<uint32_t>(args[1].as.number);

    ItemDefinition def;
    def.id = itemId;
    def.name = itemId;
    def.maxStack = 64;
    def.weight = 0.5f;

    bool added = ctx->inventory->addItem(def, count);
    return PJValue::Bool(added);
}

PJValue PJEngineBinding::nativeInventoryHasItem(PJVM* /*vm*/, int argCount, PJValue* args) {
    if (!ctx || !ctx->inventory || argCount < 1 || args[0].type != PJValueType::STRING) {
        return PJValue::Bool(false);
    }
    uint32_t count = (argCount >= 2 && args[1].type == PJValueType::NUMBER) ? static_cast<uint32_t>(args[1].as.number) : 1;
    return PJValue::Bool(ctx->inventory->hasItem(args[0].str, count));
}

PJValue PJEngineBinding::nativeQuestAdvance(PJVM* /*vm*/, int argCount, PJValue* args) {
    if (!ctx || !ctx->quests || argCount < 1 || args[0].type != PJValueType::STRING) {
        return PJValue::Bool(false);
    }
    uint32_t amount = (argCount >= 2 && args[1].type == PJValueType::NUMBER) ? static_cast<uint32_t>(args[1].as.number) : 1;
    return PJValue::Bool(ctx->quests->advanceQuest(args[0].str, 0, amount));
}

PJValue PJEngineBinding::nativeGameLog(PJVM* /*vm*/, int argCount, PJValue* args) {
    if (argCount >= 1) {
        std::string logMsg = "[PJ Script]: " + args[0].toString();
        Logger::info(logMsg);
    }
    return PJValue::Nil();
}

} // namespace PJEngine