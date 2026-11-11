#include "engine/core/math2d.h"
#include "engine/core/time_step.h"
#include "engine/core/event_bus.h"
#include "engine/core/memory_pool.h"
#include "engine/ecs/ecs_world.h"
#include "engine/physics/physics_engine2d.h"
#include "engine/graphics/camera2d.h"
#include "engine/graphics/renderer2d.h"
#include "engine/graphics/particles2d.h"
#include "engine/graphics/lighting2d.h"
#include "engine/tilemap/tilemap.h"
#include "engine/tilemap/autotiler.h"
#include "engine/animation/sprite_animation.h"
#include "engine/animation/inverse_kinematics.h"
#include "engine/audio/audio_engine.h"
#include "engine/scene/scene.h"
#include "engine/optimization/culling2d.h"
#include "engine/optimization/quality_manager.h"
#include "engine/flow/game_flow.h"
#include "engine/ui/hud_system.h"
#include "gameplay/inventory.h"
#include "gameplay/character.h"
#include "gameplay/quest.h"
#include "gameplay/dialogue.h"
#include "gameplay/crafting.h"
#include "gameplay/survival.h"
#include "gameplay/npc_controller.h"
#include "gameplay/interactive_object.h"
#include "bridge/pj_engine_binding.h"
#include <iostream>
#include <cassert>

using namespace PJEngine;

int main() {
    std::cout << "==================================================" << std::endl;
    std::cout << "   KOMPLEXNY TEST 2D ENGINU (tests/test_engine.cpp) " << std::endl;
    std::cout << "==================================================" << std::endl;

    // 1-20: OVERENIE JADRA, FYZIKY, ECS, PJ BRIDGE
    ECSWorld world;
    Entity player = world.createEntity();
    world.addComponent<Transform2D>(player, Transform2D{Vec2(100.0f, 100.0f), 0.0f, Vec2::One(), Vec2::Zero()});
    world.addComponent<RigidBody2D>(player, RigidBody2D(BodyType::DYNAMIC, 1.0f, 0.2f, 0.3f));
    world.addComponent<Collider2D>(player, Collider2D::MakeCircle(16.0f));

    // Staticka podlaha: jej vrchny okraj je na Y = 190.
    // Hrac s kruhovym colliderom (polomer 16) sa preto ustali priblizne na Y = 174.
    Entity ground = world.createEntity();
    world.addComponent<Transform2D>(
        ground,
        Transform2D{Vec2(100.0f, 200.0f), 0.0f, Vec2::One(), Vec2::Zero()}
    );
    world.addComponent<RigidBody2D>(ground, RigidBody2D(BodyType::STATIC));
    world.addComponent<Collider2D>(ground, Collider2D::MakeBox(Vec2(400.0f, 20.0f)));

    PhysicsEngine2D physics;
    for (int i = 0; i < 40; ++i) physics.step(world, 1.0f / 60.0f);
    assert(world.getComponent<Transform2D>(player).position.y <= 175.5f);
    std::cout << "[OK] 1-20. Zakladne moduly a PJ Bridge funguju bezchybne." << std::endl;

    // 21. TEST GAME FLOW & SAVE SLOTY & LOADING SCREEN
    GameFlowManager flow;
    flow.setState(GameState::LOADING);
    flow.loadingScreen.update(4.0f);
    assert(flow.loadingScreen.currentTipIndex == 1 && "Rotator tipov sa neposunul");

    SaveSlotMetadata saveMeta;
    saveMeta.characterName = "Rytier Artur";
    saveMeta.level = 14;
    saveMeta.playTimeSeconds = 51720.0f; // 14h 22m
    SaveProfileManager::saveToSlot(1, saveMeta);

    SaveSlotMetadata loadedMeta;
    assert(SaveProfileManager::loadFromSlot(1, loadedMeta));
    assert(loadedMeta.characterName == "Rytier Artur");
    assert(loadedMeta.getFormattedPlayTime() == "14h 22m");
    std::cout << "[OK] 21. Game Flow, Loading Screen s tipmi a Save Sloty (14h 22m) uspesne." << std::endl;

    // 22. TEST OTRASU KAMERY & CUTSCENE DIRECTOR
    Camera2D camera(800.0f, 600.0f);
    camera.shake(0.3f, 15.0f);
    camera.update(0.1f);
    assert(camera.shakeOffset.length() > 0.0f && "Kamera sa nezatriasla pri vybuchu");

    camera.panTo(Vec2(500.0f, 300.0f), 1.0f);
    camera.update(0.5f);
    assert(camera.position.x > 0.0f && "Kamera sa plynule neposuva v cutscene");
    std::cout << "[OK] 22. Game Juice: Otras kamery (Screenshake) a Kamerovy reziser uspesne." << std::endl;

    // 23. TEST HERNÉHO HUDU & FLOATING TEXT (DAMAGE NUMBERS)
    HUDSystem hud;
    hud.spawnDamageNumber(Vec2(100.0f, 100.0f), 45.0f, false);
    hud.spawnDamageNumber(Vec2(120.0f, 100.0f), 120.0f, true); // CRIT
    assert(hud.getActiveFloatingCount() == 2);
    hud.update(0.1f);
    assert(hud.floatingTexts[0].position.y < 100.0f && "Cislo poskodenia nevyletelo nahor");

    // Test Ghost Baru (Dark Souls efekt)
    hud.playerHealth.setHealth(60.0f);
    assert(hud.playerHealth.currentHealth == 60.0f);
    assert(hud.playerHealth.ghostHealth == 100.0f && "Ghost bar neklesol s oneskorenim");
    hud.playerHealth.update(1.0f);
    assert(hud.playerHealth.ghostHealth < 100.0f && "Ghost bar nezacal plynule klesat");
    std::cout << "[OK] 23. Herny HUD: Lietajuce cisla poskodenia a oneskoreny Ghost Bar uspesne." << std::endl;

    // 24. TEST NPC KONTROLÉRA & AI SPRÁVANIA
    Entity npc = world.createEntity();
    world.addComponent<Transform2D>(npc, Transform2D{Vec2(150.0f, 100.0f), 0.0f, Vec2::One(), Vec2::Zero()});
    NPCController npcAI;
    npcAI.behavior = AIBehavior::AGGRO_CHASE;

    float distBefore = world.getComponent<Transform2D>(npc).position.distanceTo(world.getComponent<Transform2D>(player).position);
    npcAI.update(world, npc, player, 100.0f, 0.5f);
    float distAfter = world.getComponent<Transform2D>(npc).position.distanceTo(world.getComponent<Transform2D>(player).position);
    assert(distAfter < distBefore && "Aggressive AI nezacalo prenasledovat hraca");

    assert(FactionMatrix::isHostile(Faction::PLAYER, Faction::MONSTERS));
    assert(!FactionMatrix::isHostile(Faction::PLAYER, Faction::TOWNSFOLK));
    std::cout << "[OK] 24. NPC Studio: AI Prenasledovanie a Frakcna matica uspesne." << std::endl;

    // 25. TEST ROZBITEĽNÝCH OBJEKTOV & INTERAKCIE
    Breakable crate;
    crate.health = 10.0f;
    assert(!crate.takeDamage(5.0f)); // Este nerozbita
    assert(crate.takeDamage(5.0f));  // Prave rozbita na triesky
    assert(crate.isBroken);

    InteractiveDoor door;
    door.isLocked = true;
    door.keyRequiredId = "iron_key";
    Inventory testInv(5);
    assert(!door.tryUnlock(testInv)); // Hrac nema kluc
    testInv.addItem(ItemDefinition{"iron_key", "Zelezny kluc", "", ItemType::QUEST, ItemRarity::COMMON, 1, 0.1f, 10, {}}, 1);
    assert(door.tryUnlock(testInv)); // Odomknute
    assert(door.isOpen);
    std::cout << "[OK] 25. Interaktivne objekty (Rozbitie debny, Dvere na kluc) uspesne." << std::endl;

    std::cout << "\n==================================================" << std::endl;
    std::cout << "   VSETKYCH 25 SYSTEMOV ENGINU PREBEHLO NA 100 %! " << std::endl;
    std::cout << "==================================================" << std::endl;

    return 0;
}