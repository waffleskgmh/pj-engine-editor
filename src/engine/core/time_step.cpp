#include "engine/core/time_step.h"
#include <algorithm>

namespace PJEngine {

TimeStep::TimeStep() {
    lastTime = std::chrono::steady_clock::now();
}

void TimeStep::update() {
    auto currentTime = std::chrono::steady_clock::now();
    std::chrono::duration<float> elapsed = currentTime - lastTime;
    lastTime = currentTime;

    unscaledDeltaTime = elapsed.count();
    // Ochrana pred prepadom pri prepnutí okna alebo zaseknutí (max 0.1s)
    unscaledDeltaTime = std::min(unscaledDeltaTime, 0.1f);
    
    deltaTime = unscaledDeltaTime * timeScale;
    totalTime += deltaTime;
    frameCount++;

    accumulator += deltaTime;

    // Výpočet priemerného FPS za každú polsekundu
    fpsTimer += unscaledDeltaTime;
    fpsCounter++;
    if (fpsTimer >= 0.5f) {
        fps = static_cast<float>(fpsCounter) / fpsTimer;
        fpsCounter = 0;
        fpsTimer = 0.0f;
    }
}

void TimeStep::setTimeScale(float scale) {
    timeScale = std::max(0.0f, scale);
}

float TimeStep::getTimeScale() const { return timeScale; }
float TimeStep::getDeltaTime() const { return deltaTime; }
float TimeStep::getUnscaledDeltaTime() const { return unscaledDeltaTime; }
float TimeStep::getFixedDeltaTime() const { return fixedDeltaTime; }
float TimeStep::getTotalTime() const { return totalTime; }
uint32_t TimeStep::getFrameCount() const { return frameCount; }
float TimeStep::getFPS() const { return fps; }

bool TimeStep::shouldDoFixedStep() {
    return accumulator >= fixedDeltaTime;
}

void TimeStep::consumeFixedStep() {
    accumulator -= fixedDeltaTime;
}

} // namespace PJEngine