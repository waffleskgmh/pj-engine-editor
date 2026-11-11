#pragma once

#include "engine/core/math2d.h"
#include "engine/audio/sound_instance.h"
#include <vector>
#include <string>

namespace PJEngine {

class AudioEngine {
public:
    Vec2 listenerPosition = Vec2::Zero();
    float masterVolume = 1.0f;
    float sfxVolume = 1.0f;
    float musicVolume = 0.8f;

    AudioEngine();
    ~AudioEngine();

    bool init();
    void shutdown();

    uint32_t playSound(const std::string& name, const std::string& channel = "sfx", float volume = 1.0f, float pitch = 1.0f, bool loop = false);
    uint32_t playSpatialSound(const std::string& name, const Vec2& emitterPos, float maxDistance = 500.0f, float volume = 1.0f);

    void update(float dt);
    void stop(uint32_t soundId);
    void stopAll();

    size_t getActiveSoundCount() const;
    const SoundInstance* getSound(uint32_t soundId) const;

private:
    uint32_t nextId = 1;
    std::vector<SoundInstance> activeSounds;
    uint32_t sdlAudioDeviceId = 0;
    bool isAudioInitialized = false;

    void playSynthesizedTone(float frequency, float durationSec, float volume, float pan);
};

} // namespace PJEngine