#pragma once

#include "engine/animation/sprite_animation.h"
#include "engine/animation/animator_controller.h"
#include <string>

namespace PJEngine {

class AnimationEditor {
public:
    AnimatorController activeController;
    std::string selectedState = "idle";
    bool isPlaying = false;
    float scrubTime = 0.0f;

    AnimationEditor();
    ~AnimationEditor() = default;

    // Vykreslenie UI panelu časovej osi a náhľadu animácií
    void renderGUI();

    // Pomocné editačné metódy
    static void addKeyframe(SpriteAnimation& anim, uint32_t frameIndex, float duration);
    static void setLooping(SpriteAnimation& anim, bool looping);
    static void scrubTo(SpriteAnimation& anim, float normalizedTime);
    static float getTotalDuration(const SpriteAnimation& anim);
};

} // namespace PJEngine