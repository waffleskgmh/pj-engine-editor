#pragma once

#include "engine/core/math2d.h"
#include <string>
#include <vector>
#include <cstdint>

namespace PJEngine {

enum class GameState {
    MAIN_MENU,
    LOADING,
    PLAYING,
    PAUSED,
    GAME_OVER
};

struct SaveSlotMetadata {
    uint32_t slotId = 1;
    std::string characterName = "Hrdina";
    uint32_t level = 1;
    uint32_t gold = 0;
    float playTimeSeconds = 0.0f;
    std::string timestamp;
    bool isEmpty = true;

    std::string getFormattedPlayTime() const;
};

class SaveProfileManager {
public:
    static bool saveToSlot(uint32_t slotId, const SaveSlotMetadata& meta);
    static bool loadFromSlot(uint32_t slotId, SaveSlotMetadata& outMeta);
    static std::vector<SaveSlotMetadata> getAllSlots();
};

class LoadingScreen {
public:
    float progress = 0.0f; // 0.0f až 1.0f
    float spinnerRotation = 0.0f;
    std::vector<std::string> tips;
    size_t currentTipIndex = 0;
    float tipTimer = 0.0f;

    LoadingScreen();
    void update(float dt);
    std::string getCurrentTip() const;
    void reset();
};

class ScreenFader {
public:
    enum class FadeMode { NONE, FADE_IN, FADE_OUT };

    FadeMode mode = FadeMode::NONE;
    Color fadeColor = Color::Black();
    float alpha = 0.0f;
    float duration = 1.0f;
    float timer = 0.0f;

    void startFadeIn(float durationSec);
    void startFadeOut(float durationSec, const Color& color = Color::Black());
    void update(float dt);
    bool isFinished() const;
};

class GameFlowManager {
public:
    GameState currentState = GameState::MAIN_MENU;
    LoadingScreen loadingScreen;
    ScreenFader fader;
    uint32_t activeSaveSlot = 1;

    void setState(GameState newState);
    void update(float dt);
    bool isGamePaused() const { return currentState == GameState::PAUSED; }
};

} // namespace PJEngine