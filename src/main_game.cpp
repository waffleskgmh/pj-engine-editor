#include "engine/platform/platform_window.h"
#include "engine/core/time_step.h"
#include "engine/ecs/ecs_world.h"
#include "engine/physics/physics_engine2d.h"
#include "engine/graphics/camera2d.h"
#include "engine/graphics/particles2d.h"
#include "engine/optimization/quality_manager.h"
#include "engine/ui/hud_system.h"
#include "gameplay/npc_controller.h"
#include "bridge/pj_engine_binding.h"
#include <iostream>

using namespace PJEngine;

int main() {
    std::cout << "========================================================" << std::endl;
    std::cout << "   SPUSTANIE HRY (PJ-ENGINE PRE RASPBERRY PI 3 / PC)    " << std::endl;
    std::cout << "========================================================" << std::endl;

    // 1. Aktivácia profilu pre Raspberry Pi (šetrí batériu a výkon)
    QualityManager::setProfile(PerformanceProfile::BALANCED);

    PlatformWindow window("PJ Engine 2D - Raspberry Pi Edition", 800, 480);
    if (!window.init()) return 1;

    ECSWorld world;
    PhysicsEngine2D physics;
    Camera2D camera(800.0f, 480.0f);
    ParticleSystem2D particles(QualityManager::getSettings().maxParticles);
    HUDSystem hud;
    TimeStep timer;

    // Skriptovací stroj PJ + FFI kontext
    PJVM vm;
    EngineContext engineCtx;
    engineCtx.world = &world;
    engineCtx.physics = &physics;
    PJEngineBinding::bind(&vm, &engineCtx);

    // 2. Vytvorenie herného sveta
    // Hráč (Zelený kruh)
    Entity player = world.createEntity();
    world.addComponent<Transform2D>(player, Transform2D{Vec2(100.0f, 150.0f), 0.0f, Vec2::One(), Vec2::Zero()});
    world.addComponent<RigidBody2D>(player, RigidBody2D(BodyType::DYNAMIC, 1.0f, 0.1f, 0.4f));
    world.addComponent<Collider2D>(player, Collider2D::MakeCircle(16.0f));

    // Nepriateľ (Červený kruh s AI)
    Entity enemy = world.createEntity();
    world.addComponent<Transform2D>(enemy, Transform2D{Vec2(600.0f, 150.0f), 0.0f, Vec2::One(), Vec2::Zero()});
    world.addComponent<RigidBody2D>(enemy, RigidBody2D(BodyType::DYNAMIC, 1.0f, 0.1f, 0.4f));
    world.addComponent<Collider2D>(enemy, Collider2D::MakeCircle(16.0f));
    NPCController enemyAI;
    enemyAI.behavior = AIBehavior::AGGRO_CHASE;

    // Podlaha (Statický kváder)
    Entity ground = world.createEntity();
    world.addComponent<Transform2D>(ground, Transform2D{Vec2(400.0f, 420.0f), 0.0f, Vec2::One(), Vec2::Zero()});
    world.addComponent<RigidBody2D>(ground, RigidBody2D(BodyType::STATIC));
    world.addComponent<Collider2D>(ground, Collider2D::MakeBox(Vec2(800.0f, 40.0f)));

    // Plávajúca plošina
    Entity platform = world.createEntity();
    world.addComponent<Transform2D>(platform, Transform2D{Vec2(400.0f, 280.0f), 0.0f, Vec2::One(), Vec2::Zero()});
    world.addComponent<RigidBody2D>(platform, RigidBody2D(BodyType::STATIC));
    world.addComponent<Collider2D>(platform, Collider2D::MakeBox(Vec2(200.0f, 20.0f)));

    // 3. Herný PJ skript nahraný do virtuálneho stroja
    std::string playerScript = R"(
        let h = Input_getAxis("horizontal");
        let speed = 180.0;
        let v = Entity_getVelocity(0);

        if (h != 0.0) {
            Entity_setVelocity(0, h * speed, v.y);
        }
        if (Input_isKeyDown("space")) {
            // Skok
            Entity_applyImpulse(0, 0.0, -25.0);
        }
    )";
    vm.runSource(playerScript);

    bool running = true;

    // 4. Hlavný herný cyklus (60 FPS)
    while (running) {
        timer.update();
        float dt = timer.getDeltaTime();

        window.pollEvents(engineCtx, running);

        // Beh PJ skriptu
        vm.runSource(playerScript);

        // Krok fyziky
        physics.step(world, dt);

        // AI update
        enemyAI.update(world, enemy, player, 100.0f, dt);

        // Update kamery, častíc a HUDu
        camera.update(dt);
        particles.update(dt);
        hud.update(dt);

        // VYKRESLOVANIE
        window.clear(Color(0.1f, 0.12f, 0.16f, 1.0f)); // Tmavé pozadie

        // Vykreslenie všetkých kolízorov v ECS
        auto entities = world.query<Transform2D, Collider2D>();
        for (Entity e : entities) {
            const auto& t = world.getComponent<Transform2D>(e);
            const auto& c = world.getComponent<Collider2D>(e);

            Color color = Color::White();
            if (e == player) color = Color(0.2f, 0.8f, 0.3f, 1.0f); // Hráč = Zelená
            else if (e == enemy) color = Color(0.9f, 0.2f, 0.2f, 1.0f); // Nepriateľ = Červená
            else color = Color(0.4f, 0.45f, 0.55f, 1.0f); // Prekážky = Sivá

            if (c.type == ColliderType::CIRCLE) {
                window.renderCircle(t.position + c.offset, c.radius, color);
            } else {
                window.renderQuad(t.position + c.offset, c.size, color);
            }
        }

        // Vykreslenie HUDu (Ghost Health Bar hore v rohu)
        window.renderQuad(Vec2(120.0f, 25.0f), Vec2(200.0f, 16.0f), Color(0.2f, 0.2f, 0.2f, 0.8f)); // Pozadie
        window.renderQuad(Vec2(20.0f + hud.playerHealth.ghostHealth, 25.0f), Vec2(hud.playerHealth.ghostHealth * 2.0f, 16.0f), Color::Yellow()); // Ghost Bar
        window.renderQuad(Vec2(20.0f + hud.playerHealth.currentHealth, 25.0f), Vec2(hud.playerHealth.currentHealth * 2.0f, 16.0f), Color::Red()); // Život

        window.present();
    }

    std::cout << "[PJ-Engine]: Hra bola korektne ukoncena." << std::endl;
    return 0;
}