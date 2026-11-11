#pragma once

#include "engine/core/math2d.h"
#include "engine/flow/game_flow.h"
#include "engine/ecs/entity.h"
#include "gameplay/character.h"
#include "gameplay/inventory.h"
#include "gameplay/quest.h"

#include <string>
#include <vector>
#include <memory>

namespace PJEngine {

class Scene;

enum class TransitionEffect {
    FADE_BLACK,
    FADE_WHITE,
    INSTANT
};

enum class TransitionState {
    IDLE,
    FADING_OUT,
    SWITCHING_SCENE,
    FADING_IN
};

struct MapTransitionTrigger {
    std::string id = "door_01";
    Rect triggerBounds = Rect(0.0f, 0.0f, 32.0f, 32.0f);
    std::string targetMapPath;        // napr. "assets/maps/dungeon_lvl1.pjscene"
    Vec2 targetSpawnPoint = Vec2::Zero();
    TransitionEffect effect = TransitionEffect::FADE_BLACK;
    float fadeDuration = 0.5f;
    std::string requiredKeyItemId;    // Ak je vyplnené, vyžaduje kľúč v inventári

    MapTransitionTrigger() = default;
};

// Dátový balík, ktorý sa prenáša medzi mapami
struct PlayerPersistentData {
    bool hasData = false;
    float health = 100.0f;
    float maxHealth = 100.0f;
    uint32_t level = 1;
    float currentXP = 0.0f;
    uint32_t gold = 0;
    std::vector<InventorySlot> savedInventorySlots;
};

class MapTransitionManager {
public:
    static MapTransitionManager& get();

    void update(std::shared_ptr<Scene>& activeScene, Entity playerEntity, float dt);

    // Spustenie prechodu na novú mapu
    void triggerTransition(const MapTransitionTrigger& trigger);
    void triggerDirectLoad(const std::string& mapPath, const Vec2& spawnPos, float duration = 0.5f);

    // Správa aktívnych triggerov na aktuálnej mape
    void addTrigger(const MapTransitionTrigger& trigger);
    void removeTrigger(const std::string& id);
    void clearTriggers();
    const std::vector<MapTransitionTrigger>& getTriggers() const { return activeTriggers; }

    // Perzistencia hráča
    void capturePlayerData(const CharacterStats* stats, const Inventory* inv, uint32_t gold);
    void restorePlayerData(CharacterStats* stats, Inventory* inv, uint32_t& gold);

    TransitionState getState() const { return state; }
    bool isTransitioning() const { return state != TransitionState::IDLE; }
    float getScreenFadeAlpha() const { return fader.alpha; }
    Color getScreenFadeColor() const { return fader.fadeColor; }

private:
    MapTransitionManager() = default;
    ~MapTransitionManager() = default;

    TransitionState state = TransitionState::IDLE;
    MapTransitionTrigger pendingTrigger;
    ScreenFader fader;
    PlayerPersistentData playerData;
    std::vector<MapTransitionTrigger> activeTriggers;
};

} // namespace PJEngine