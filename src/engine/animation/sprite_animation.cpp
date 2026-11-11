#include "engine/animation/sprite_animation.h"

namespace PJEngine {

void SpriteAnimation::addFrame(uint32_t frameIndex, float duration) {
    frames.push_back({frameIndex, duration});
}

uint32_t SpriteAnimation::update(float dt) {
    if (frames.empty()) return 0;

    timer += dt;
    if (timer >= frames[currentIdx].duration) {
        timer -= frames[currentIdx].duration;
        currentIdx++;

        if (currentIdx >= frames.size()) {
            if (isLooping) {
                currentIdx = 0;
            } else {
                currentIdx = frames.size() - 1;
                finished = true;
            }
        }
    }

    return frames[currentIdx].frameIndex;
}

void SpriteAnimation::reset() {
    currentIdx = 0;
    timer = 0.0f;
    finished = false;
}

uint32_t SpriteAnimation::getCurrentFrame() const {
    return frames.empty() ? 0 : frames[currentIdx].frameIndex;
}

} // namespace PJEngine