#pragma once

#include <string>

namespace PJEngine {

enum class SoundState {
    PLAYING,
    PAUSED,
    STOPPED
};

struct SoundInstance {
    uint32_t id = 0;
    std::string name;
    std::string channel = "sfx";
    float volume = 1.0f;
    float pitch = 1.0f;
    float pan = 0.0f; // -1.0f (Vľavo) až +1.0f (Vpravo)
    bool isLooping = false;
    SoundState state = SoundState::PLAYING;
    float progress = 0.0f;
    float duration = 1.0f;
};

} // namespace PJEngine