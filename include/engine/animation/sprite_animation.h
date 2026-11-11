#pragma once

#include <vector>
#include <cstdint>

namespace PJEngine {

struct SpriteKeyframe {
    uint32_t frameIndex = 0;
    float duration = 0.1f; // v sekundách
};

class SpriteAnimation {
public:
    std::vector<SpriteKeyframe> frames;
    bool isLooping = true;

    SpriteAnimation() = default;

    void addFrame(uint32_t frameIndex, float duration);
    uint32_t update(float dt);
    void reset();
    
    uint32_t getCurrentFrame() const;
    bool isFinished() const { return finished; }

private:
    size_t currentIdx = 0;
    float timer = 0.0f;
    bool finished = false;
};

} // namespace PJEngine