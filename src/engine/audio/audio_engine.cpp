#include "engine/audio/audio_engine.h"

// Automatická detekcia prítomnosti SDL2 hlavičiek
#if defined(__has_include)
  #if __has_include(<SDL2/SDL.h>)
    #include <SDL2/SDL.h>
    #define PJ_HAS_SDL 1
  #elif __has_include(<SDL.h>)
    #include <SDL.h>
    #define PJ_HAS_SDL 1
  #endif
#endif

#include <cmath>
#include <algorithm>
#include <vector>

namespace PJEngine {

AudioEngine::AudioEngine() {
    init();
}

AudioEngine::~AudioEngine() {
    shutdown();
}

bool AudioEngine::init() {
    if (isAudioInitialized) return true;

#ifdef PJ_HAS_SDL
    if (SDL_WasInit(SDL_INIT_AUDIO) == 0) {
        if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
            return false;
        }
    }

    SDL_AudioSpec desired, obtained;
    SDL_zero(desired);
    desired.freq = 44100;
    desired.format = AUDIO_S16SYS;
    desired.channels = 2; // Stereo
    desired.samples = 2048;
    desired.callback = nullptr;

    sdlAudioDeviceId = SDL_OpenAudioDevice(nullptr, 0, &desired, &obtained, 0);
    if (sdlAudioDeviceId != 0) {
        SDL_PauseAudioDevice(sdlAudioDeviceId, 0); // Spustenie audio výstupu
        isAudioInitialized = true;
        return true;
    }
#endif

    // Bezpečný fallback režim (ak SDL audio nie je dostupné)
    isAudioInitialized = true;
    return true;
}

void AudioEngine::shutdown() {
    if (isAudioInitialized) {
#ifdef PJ_HAS_SDL
        if (sdlAudioDeviceId != 0) {
            SDL_CloseAudioDevice(sdlAudioDeviceId);
            sdlAudioDeviceId = 0;
        }
#endif
        isAudioInitialized = false;
    }
}

void AudioEngine::playSynthesizedTone(float frequency, float durationSec, float volume, float pan) {
#ifdef PJ_HAS_SDL
    if (!isAudioInitialized || sdlAudioDeviceId == 0) return;

    int sampleRate = 44100;
    int totalSamples = static_cast<int>(sampleRate * durationSec);
    std::vector<int16_t> buffer(totalSamples * 2);

    float leftVol = std::clamp((1.0f - pan) * 0.5f * volume * masterVolume, 0.0f, 1.0f);
    float rightVol = std::clamp((1.0f + pan) * 0.5f * volume * masterVolume, 0.0f, 1.0f);

    for (int i = 0; i < totalSamples; ++i) {
        float time = static_cast<float>(i) / sampleRate;
        float decay = 1.0f - (time / durationSec);
        float wave = std::sin(2.0f * PI * frequency * time) * decay;

        int16_t sampleLeft = static_cast<int16_t>(wave * leftVol * 30000.0f);
        int16_t sampleRight = static_cast<int16_t>(wave * rightVol * 30000.0f);

        buffer[i * 2] = sampleLeft;
        buffer[i * 2 + 1] = sampleRight;
    }

    SDL_QueueAudio(sdlAudioDeviceId, buffer.data(), buffer.size() * sizeof(int16_t));
#else
    // Ak SDL audio nie je linkované, metóda ticho prebehne
    (void)frequency;
    (void)durationSec;
    (void)volume;
    (void)pan;
#endif
}

uint32_t AudioEngine::playSound(const std::string& name, const std::string& channel, float volume, float pitch, bool loop) {
    SoundInstance sound;
    sound.id = nextId++;
    sound.name = name;
    sound.channel = channel;
    sound.volume = std::max(0.0f, volume);
    sound.pitch = std::max(0.1f, pitch);
    sound.isLooping = loop;
    sound.state = SoundState::PLAYING;
    sound.progress = 0.0f;
    sound.duration = 0.25f;

    float freq = 440.0f * pitch;
    if (name.find("jump") != std::string::npos) freq = 580.0f * pitch;
    else if (name.find("hit") != std::string::npos) freq = 220.0f * pitch;

    playSynthesizedTone(freq, sound.duration, sound.volume, 0.0f);

    activeSounds.push_back(sound);
    return sound.id;
}

uint32_t AudioEngine::playSpatialSound(const std::string& name, const Vec2& emitterPos, float maxDistance, float volume) {
    Vec2 delta = emitterPos - listenerPosition;
    float dist = delta.length();
    if (dist >= maxDistance) return 0;

    float att = 1.0f - std::clamp(dist / maxDistance, 0.0f, 1.0f);
    float pan = std::clamp(delta.x / maxDistance, -1.0f, 1.0f);

    uint32_t id = playSound(name, "sfx", volume * att, 1.0f, false);
    for (auto& s : activeSounds) {
        if (s.id == id) {
            s.pan = pan;
            playSynthesizedTone(440.0f, 0.2f, volume * att, pan);
            break;
        }
    }
    return id;
}

void AudioEngine::update(float dt) {
    for (auto it = activeSounds.begin(); it != activeSounds.end();) {
        if (it->state == SoundState::PLAYING) {
            it->progress += dt * it->pitch;
            if (it->progress >= it->duration) {
                if (it->isLooping) {
                    it->progress = 0.0f;
                    ++it;
                } else {
                    it = activeSounds.erase(it);
                    continue;
                }
            } else {
                ++it;
            }
        } else {
            ++it;
        }
    }
}

void AudioEngine::stop(uint32_t soundId) {
    for (auto it = activeSounds.begin(); it != activeSounds.end(); ++it) {
        if (it->id == soundId) {
            activeSounds.erase(it);
            return;
        }
    }
}

void AudioEngine::stopAll() {
    activeSounds.clear();
#ifdef PJ_HAS_SDL
    if (sdlAudioDeviceId != 0) {
        SDL_ClearQueuedAudio(sdlAudioDeviceId);
    }
#endif
}

size_t AudioEngine::getActiveSoundCount() const {
    return activeSounds.size();
}

const SoundInstance* AudioEngine::getSound(uint32_t soundId) const {
    for (const auto& s : activeSounds) {
        if (s.id == soundId) return &s;
    }
    return nullptr;
}

} // namespace PJEngine