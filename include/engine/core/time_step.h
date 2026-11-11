#pragma once

#include <chrono>

namespace PJEngine {

class TimeStep {
public:
    TimeStep();

    void update();
    void setTimeScale(float scale);
    float getTimeScale() const;

    float getDeltaTime() const;
    float getUnscaledDeltaTime() const;
    float getFixedDeltaTime() const;
    float getTotalTime() const;
    uint32_t getFrameCount() const;
    float getFPS() const;

    // Fixed timestep akumulátor pre 2D fyziku
    bool shouldDoFixedStep();
    void consumeFixedStep();

private:
    std::chrono::steady_clock::time_point lastTime;
    float deltaTime = 0.0f;
    float unscaledDeltaTime = 0.0f;
    float fixedDeltaTime = 1.0f / 60.0f;
    float timeScale = 1.0f;
    float totalTime = 0.0f;
    uint32_t frameCount = 0;
    float fps = 60.0f;

    float accumulator = 0.0f;
    float fpsTimer = 0.0f;
    uint32_t fpsCounter = 0;
};

} // namespace PJEngine