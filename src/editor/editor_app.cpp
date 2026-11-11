#include "editor/editor_app.h"
#include "pj/pj_lexer.h"
#include "pj/pj_compiler.h"
#include <iostream>

namespace PJEngine {

EditorApp::EditorApp()
    : sceneCamera(800.0f, 600.0f), gameCamera(800.0f, 600.0f),
      inventory(16, 100.0f) {
    
    engineCtx.world = &world;
    engineCtx.physics = &physics;
    engineCtx.audio = &audio;
    engineCtx.inventory = &inventory;
    engineCtx.quests = &quests;

    PJEngineBinding::bind(&vm, &engineCtx);
    initSampleScene();
}

void EditorApp::initSampleScene() {
    world.clear();
    quests.quests.clear();

    // 1. Hráč
    Entity player = world.createEntity();
    world.addComponent<Transform2D>(player, Transform2D{Vec2(100.0f, 100.0f), 0.0f, Vec2::One(), Vec2::Zero()});
    world.addComponent<RigidBody2D>(player, RigidBody2D(BodyType::DYNAMIC, 1.0f, 0.2f, 0.3f));
    world.addComponent<Collider2D>(player, Collider2D::MakeCircle(16.0f));

    // 2. Podlaha
    Entity ground = world.createEntity();
    world.addComponent<Transform2D>(ground, Transform2D{Vec2(200.0f, 250.0f), 0.0f, Vec2::One(), Vec2::Zero()});
    world.addComponent<RigidBody2D>(ground, RigidBody2D(BodyType::STATIC, 0.0f));
    world.addComponent<Collider2D>(ground, Collider2D::MakeBox(Vec2(400.0f, 20.0f)));

    selectedEntity = player;

    // Východiskový herný skript v jazyku PJ
    activePJScript = R"(
        let hAxis = Input_getAxis("horizontal");
        let speed = 250.0;

        if (hAxis != 0.0) {
            Entity_setVelocity(0, hAxis * speed, 0.0);
        }
    )";

    hotReloadPJScript(activePJScript);
}

void EditorApp::setPlayState(PlayState state) {
    playState = state;
}

void EditorApp::stepFrame(float dt) {
    if (playState == PlayState::EDITING) return;

    // 1. Beh herného skriptu PJ
    if (!activePJScript.empty()) {
        vm.runSource(activePJScript);
    }

    // 2. Krok 2D fyziky
    physics.step(world, dt);

    // 3. Audio update
    audio.update(dt);
}

void EditorApp::setWorkspace(EditorWorkspace ws) {
    activeWorkspace = ws;
}

void EditorApp::selectEntity(Entity entity) {
    selectedEntity = entity;
}

bool EditorApp::hotReloadPJScript(const std::string& newSource) {
    PJLexer lexer(newSource);
    auto tokens = lexer.tokenize();
    PJCompiler compiler;
    auto fn = compiler.compile(tokens);

    if (fn) {
        activePJScript = newSource;
        vm.interpret(fn);
        return true;
    }
    return false;
}

} // namespace PJEngine