#include "engine/scene/map_transition.h"
#include "engine/scene/scene.h"
#include "engine/core/logger.h"
#include <iostream>

namespace PJEngine {

MapTransitionManager& MapTransitionManager::get() {
    static MapTransitionManager instance;
    return instance;
}

void MapTransitionManager::addTrigger(const MapTransitionTrigger& trigger) {
    activeTriggers.push_back(trigger);
}

void MapTransitionManager::removeTrigger(const std::string& id) {
    for (auto it = activeTriggers.begin(); it != activeTriggers.end(); ++it) {
        if (it->id == id) {
            activeTriggers.erase(it);
            return;
        }
    }
}

void MapTransitionManager::clearTriggers() {
    activeTriggers.clear();
}

void MapTransitionManager::capturePlayerData(const CharacterStats* stats, const Inventory* inv, uint32_t gold) {
    playerData.hasData = true;
    if (stats) {
        playerData.health = stats->health;
        playerData.maxHealth = stats->maxHealth;
        playerData.level = stats->level;
        playerData.currentXP = stats->currentXP;
    }
    if (inv) {
        playerData.savedInventorySlots = inv->slots;
    }
    playerData.gold = gold;
}

void MapTransitionManager::restorePlayerData(CharacterStats* stats, Inventory* inv, uint32_t& gold) {
    if (!playerData.hasData) return;

    if (stats) {
        stats->maxHealth = playerData.maxHealth;
        stats->health = playerData.health;
        stats->level = playerData.level;
        stats->currentXP = playerData.currentXP;
    }
    if (inv) {
        inv->slots = playerData.savedInventorySlots;
    }
    gold = playerData.gold;
}

void MapTransitionManager::triggerTransition(const MapTransitionTrigger& trigger) {
    if (state != TransitionState::IDLE) return;

    pendingTrigger = trigger;
    state = TransitionState::FADING_OUT;

    Color fadeCol = (trigger.effect == TransitionEffect::FADE_WHITE) ? Color::White() : Color::Black();
    fader.startFadeOut(trigger.fadeDuration, fadeCol);

    Logger::info("Spusteny prechod na mapu: " + trigger.targetMapPath);
}

void MapTransitionManager::triggerDirectLoad(const std::string& mapPath, const Vec2& spawnPos, float duration) {
    MapTransitionTrigger trig;
    trig.targetMapPath = mapPath;
    trig.targetSpawnPoint = spawnPos;
    trig.fadeDuration = duration;
    trig.effect = TransitionEffect::FADE_BLACK;
    triggerTransition(trig);
}

void MapTransitionManager::update(std::shared_ptr<Scene>& activeScene, Entity playerEntity, float dt) {
    // 1. Ak sme v stave IDLE, kontrolujeme kolíziu hráča s portálmi na mape
    if (state == TransitionState::IDLE) {
        if (activeScene && activeScene->world.isAlive(playerEntity) && activeScene->world.hasComponent<Transform2D>(playerEntity)) {
            const auto& pt = activeScene->world.getComponent<Transform2D>(playerEntity);
            for (const auto& trig : activeTriggers) {
                if (trig.triggerBounds.contains(pt.position)) {
                    triggerTransition(trig);
                    break;
                }
            }
        }
        return;
    }

    // 2. Spracovanie animácie stmievania (Fading)
    fader.update(dt);

    if (state == TransitionState::FADING_OUT) {
        if (fader.isFinished()) {
            state = TransitionState::SWITCHING_SCENE;
        }
    } else if (state == TransitionState::SWITCHING_SCENE) {
        if (!pendingTrigger.targetMapPath.empty()) {
            // Načítanie novej scény z disku
            auto newScene = Scene::loadFromFile(pendingTrigger.targetMapPath);
            if (newScene) {
                activeScene = newScene;

                // Nastavenie pozície hráča na cieľový spawn bod
                if (activeScene->world.isAlive(playerEntity) && activeScene->world.hasComponent<Transform2D>(playerEntity)) {
                    auto& pt = activeScene->world.getComponent<Transform2D>(playerEntity);
                    pt.position = pendingTrigger.targetSpawnPoint;
                }
                Logger::info("Nova mapa uspesne nacitana. Hrac premiestneny na spawn bod.");
            } else {
                Logger::error("Zlyhalo nacitanie cielovej mapy: " + pendingTrigger.targetMapPath);
            }
        }

        // Spustenie rozjasnenia
        state = TransitionState::FADING_IN;
        fader.startFadeIn(pendingTrigger.fadeDuration);
    } else if (state == TransitionState::FADING_IN) {
        if (fader.isFinished()) {
            state = TransitionState::IDLE;
            Logger::info("Prechod medzi mapami uspesne ukonceny.");
        }
    }
}

} // namespace PJEngine