#include "engine/flow/game_flow.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace PJEngine {

std::string SaveSlotMetadata::getFormattedPlayTime() const {
    int totalSec = static_cast<int>(playTimeSeconds);
    int hours = totalSec / 3600;
    int minutes = (totalSec % 3600) / 60;
    return std::to_string(hours) + "h " + std::to_string(minutes) + "m";
}

bool SaveProfileManager::saveToSlot(uint32_t slotId, const SaveSlotMetadata& meta) {
    std::string filename = "save_slot_" + std::to_string(slotId) + ".dat";
    std::ofstream f(filename);
    if (!f.is_open()) return false;

    f << "NAME:" << meta.characterName << "\n";
    f << "LEVEL:" << meta.level << "\n";
    f << "GOLD:" << meta.gold << "\n";
    f << "TIME:" << meta.playTimeSeconds << "\n";
    f << "TIMESTAMP:Dnes\n";
    return true;
}

bool SaveProfileManager::loadFromSlot(uint32_t slotId, SaveSlotMetadata& outMeta) {
    std::string filename = "save_slot_" + std::to_string(slotId) + ".dat";
    std::ifstream f(filename);
    if (!f.is_open()) {
        outMeta.isEmpty = true;
        return false;
    }

    std::string line;
    outMeta.slotId = slotId;
    outMeta.isEmpty = false;

    while (std::getline(f, line)) {
        if (line.rfind("NAME:", 0) == 0) outMeta.characterName = line.substr(5);
        if (line.rfind("LEVEL:", 0) == 0) outMeta.level = std::stoi(line.substr(6));
        if (line.rfind("GOLD:", 0) == 0) outMeta.gold = std::stoi(line.substr(5));
        if (line.rfind("TIME:", 0) == 0) outMeta.playTimeSeconds = std::stof(line.substr(5));
    }
    return true;
}

std::vector<SaveSlotMetadata> SaveProfileManager::getAllSlots() {
    std::vector<SaveSlotMetadata> list;
    for (uint32_t i = 1; i <= 3; ++i) {
        SaveSlotMetadata slot;
        slot.slotId = i;
        loadFromSlot(i, slot);
        list.push_back(slot);
    }
    return list;
}

LoadingScreen::LoadingScreen() {
    tips = {
        "Tip #1: Liecive lektvary vracaju zdravie okamzite.",
        "Tip #2: Niektore dvere vyzaduju zelezny kluc.",
        "Tip #3: V noci vidis menej, pouzi faklu.",
        "Tip #4: Goblini utekaju, ak im zoberies velku cast zivota.",
        "Tip #5: Silne udery zbraniou zatrasu obrazovkou."
    };
}

void LoadingScreen::update(float dt) {
    spinnerRotation += dt * 360.0f * DEG2RAD;
    tipTimer += dt;
    if (tipTimer >= 3.5f) {
        tipTimer = 0.0f;
        currentTipIndex = (currentTipIndex + 1) % tips.size();
    }
}

std::string LoadingScreen::getCurrentTip() const {
    return tips[currentTipIndex];
}

void LoadingScreen::reset() {
    progress = 0.0f;
    spinnerRotation = 0.0f;
    tipTimer = 0.0f;
}

void ScreenFader::startFadeIn(float durationSec) {
    mode = FadeMode::FADE_IN;
    duration = std::max(0.01f, durationSec);
    timer = 0.0f;
    alpha = 1.0f;
}

void ScreenFader::startFadeOut(float durationSec, const Color& color) {
    mode = FadeMode::FADE_OUT;
    duration = std::max(0.01f, durationSec);
    timer = 0.0f;
    fadeColor = color;
    alpha = 0.0f;
}

void ScreenFader::update(float dt) {
    if (mode == FadeMode::NONE) return;

    timer += dt;
    float t = std::clamp(timer / duration, 0.0f, 1.0f);

    if (mode == FadeMode::FADE_IN) {
        alpha = 1.0f - t;
        if (t >= 1.0f) mode = FadeMode::NONE;
    } else if (mode == FadeMode::FADE_OUT) {
        alpha = t;
    }
}

bool ScreenFader::isFinished() const {
    return timer >= duration;
}

void GameFlowManager::setState(GameState newState) {
    currentState = newState;
    if (newState == GameState::LOADING) {
        loadingScreen.reset();
    }
}

void GameFlowManager::update(float dt) {
    fader.update(dt);
    if (currentState == GameState::LOADING) {
        loadingScreen.update(dt);
    }
}

} // namespace PJEngine