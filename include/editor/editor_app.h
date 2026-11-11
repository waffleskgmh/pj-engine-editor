#pragma once

#include "engine/ecs/ecs_world.h"
#include "engine/physics/physics_engine2d.h"
#include "engine/graphics/renderer2d.h"
#include "engine/graphics/camera2d.h"
#include "engine/audio/audio_engine.h"
#include "gameplay/inventory.h"
#include "gameplay/quest.h"
#include "bridge/pj_engine_binding.h"
#include "pj/pj_vm.h"
#include <string>
#include <memory>

namespace PJEngine {

enum class EditorToolMode {
    SELECT,
    TRANSLATE,
    ROTATE,
    SCALE
};

enum class PlayState {
    EDITING,
    PLAYING,
    PAUSED
};

enum class EditorWorkspace {
    DEFAULT_2D,
    LEVEL_DESIGN,
    VISUAL_SCRIPTING,
    ANIMATION_VFX,
    GAMEPLAY_STORY
};

class EditorApp {
public:
    ECSWorld world;
    PhysicsEngine2D physics;
    Renderer2D renderer;
    Camera2D sceneCamera;
    Camera2D gameCamera;
    AudioEngine audio;
    Inventory inventory;
    QuestManager quests;
    PJVM vm;
    EngineContext engineCtx;

    Entity selectedEntity = NULL_ENTITY;
    EditorToolMode toolMode = EditorToolMode::TRANSLATE;
    PlayState playState = PlayState::EDITING; // Opravené na C++ štvorbodku ::
    EditorWorkspace activeWorkspace = EditorWorkspace::DEFAULT_2D;

    std::string activePJScript;
    bool isDarkTheme = true;

    EditorApp();

    void initSampleScene();
    void setPlayState(PlayState state);
    void stepFrame(float dt = 1.0f / 60.0f);
    void setWorkspace(EditorWorkspace ws);
    void selectEntity(Entity entity);

    bool hotReloadPJScript(const std::string& newSource);
};

} // namespace PJEngine