#include "engine/graphics/texture.h"

#if defined(__has_include)
  #if __has_include(<SDL2/SDL.h>)
    #include <SDL2/SDL.h>
  #elif __has_include(<SDL.h>)
    #include <SDL.h>
  #endif
#else
  #include <SDL.h>
#endif

#include <iostream>
#include <algorithm>
#include <fstream>

namespace PJEngine {

Texture2D::Texture2D() = default;

Texture2D::~Texture2D() {
    destroy();
}

Texture2D::Texture2D(Texture2D&& other) noexcept {
    width = other.width;
    height = other.height;
    channels = other.channels;
    filter = other.filter;
    sourcePath = std::move(other.sourcePath);
    cpuPixelData = std::move(other.cpuPixelData);
    sdlTexture = other.sdlTexture;
    boundRenderer = other.boundRenderer;

    other.sdlTexture = nullptr;
    other.boundRenderer = nullptr;
    other.width = 0;
    other.height = 0;
}

Texture2D& Texture2D::operator=(Texture2D&& other) noexcept {
    if (this != &other) {
        destroy();

        width = other.width;
        height = other.height;
        channels = other.channels;
        filter = other.filter;
        sourcePath = std::move(other.sourcePath);
        cpuPixelData = std::move(other.cpuPixelData);
        sdlTexture = other.sdlTexture;
        boundRenderer = other.boundRenderer;

        other.sdlTexture = nullptr;
        other.boundRenderer = nullptr;
        other.width = 0;
        other.height = 0;
    }
    return *this;
}

void Texture2D::destroy() {
    if (sdlTexture) {
        SDL_DestroyTexture(sdlTexture);
        sdlTexture = nullptr;
    }
    cpuPixelData.clear();
    width = 0;
    height = 0;
    boundRenderer = nullptr;
}

bool Texture2D::createSolid(SDL_Renderer* renderer, uint32_t w, uint32_t h, const Color& color) {
    if (!renderer || w == 0 || h == 0) return false;
    destroy();

    boundRenderer = renderer;
    width = w;
    height = h;
    channels = 4;
    filter = TextureFilter::NEAREST;

    size_t byteCount = width * height * 4;
    cpuPixelData.resize(byteCount);

    uint8_t r = static_cast<uint8_t>(std::clamp(color.r, 0.0f, 1.0f) * 255.0f);
    uint8_t g = static_cast<uint8_t>(std::clamp(color.g, 0.0f, 1.0f) * 255.0f);
    uint8_t b = static_cast<uint8_t>(std::clamp(color.b, 0.0f, 1.0f) * 255.0f);
    uint8_t a = static_cast<uint8_t>(std::clamp(color.a, 0.0f, 1.0f) * 255.0f);

    for (size_t i = 0; i < byteCount; i += 4) {
        cpuPixelData[i + 0] = r;
        cpuPixelData[i + 1] = g;
        cpuPixelData[i + 2] = b;
        cpuPixelData[i + 3] = a;
    }

    sdlTexture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        width, height
    );

    if (!sdlTexture) {
        std::cerr << "[Texture2D]: Zlyhalo SDL_CreateTexture: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_SetTextureBlendMode(sdlTexture, SDL_BLENDMODE_BLEND);
    updateGPUTexture();
    return true;
}

bool Texture2D::loadFromMemory(SDL_Renderer* renderer, const uint8_t* rgbaData, uint32_t w, uint32_t h, TextureFilter filterMode) {
    if (!renderer || !rgbaData || w == 0 || h == 0) return false;
    destroy();

    boundRenderer = renderer;
    width = w;
    height = h;
    channels = 4;
    filter = filterMode;

    size_t byteCount = width * height * 4;
    cpuPixelData.assign(rgbaData, rgbaData + byteCount);

    sdlTexture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        width, height
    );

    if (!sdlTexture) {
        std::cerr << "[Texture2D]: Zlyhalo SDL_CreateTexture: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_SetTextureBlendMode(sdlTexture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureScaleMode(sdlTexture, (filter == TextureFilter::NEAREST) ? SDL_ScaleModeNearest : SDL_ScaleModeLinear);
    updateGPUTexture();
    return true;
}

bool Texture2D::loadFromFile(SDL_Renderer* renderer, const std::string& filePath, TextureFilter filterMode) {
    if (!renderer) return false;

    // Skontrolujeme existenciu súboru
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "[Texture2D]: Subor neexistuje: " << filePath << std::endl;
        return false;
    }
    file.close();

    // Pokus 1: Načítanie cez natívny SDL2 BMP loader
    SDL_Surface* surface = SDL_LoadBMP(filePath.c_str());

    // Pokus 2: Ak to nie je BMP, skúsime vytvoriť fallback farebnú textúru s označením cesty
    if (!surface) {
        std::cerr << "[Texture2D]: Format nepodporovany priamo cez SDL_LoadBMP, vytvaram zastupnu texturu pre: " << filePath << std::endl;
        createSolid(renderer, 64, 64, Color(0.8f, 0.2f, 0.8f, 1.0f)); // Magenta = chybná textúra
        sourcePath = filePath;
        return true;
    }

    // Konverzia formátu na čisté RGBA32
    SDL_Surface* formattedSurface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(surface);

    if (!formattedSurface) {
        std::cerr << "[Texture2D]: Konverzia pixel formatu zlyhala: " << SDL_GetError() << std::endl;
        return false;
    }

    bool success = loadFromMemory(
        renderer,
        static_cast<const uint8_t*>(formattedSurface->pixels),
        formattedSurface->w,
        formattedSurface->h,
        filterMode
    );

    SDL_FreeSurface(formattedSurface);

    if (success) {
        sourcePath = filePath;
    }
    return success;
}

void Texture2D::updateGPUTexture() {
    if (!sdlTexture || cpuPixelData.empty()) return;

    void* pixels = nullptr;
    int pitch = 0;
    if (SDL_LockTexture(sdlTexture, nullptr, &pixels, &pitch) == 0) {
        uint8_t* dst = static_cast<uint8_t*>(pixels);
        const uint8_t* src = cpuPixelData.data();
        int rowBytes = width * 4;

        for (uint32_t y = 0; y < height; ++y) {
            std::memcpy(dst + (y * pitch), src + (y * rowBytes), rowBytes);
        }
        SDL_UnlockTexture(sdlTexture);
    }
}

void Texture2D::setPixel(uint32_t x, uint32_t y, uint32_t rgbaColor) {
    if (x >= width || y >= height || cpuPixelData.empty()) return;
    size_t idx = (y * width + x) * 4;

    cpuPixelData[idx + 0] = rgbaColor & 0xFF;         // R
    cpuPixelData[idx + 1] = (rgbaColor >> 8) & 0xFF;  // G
    cpuPixelData[idx + 2] = (rgbaColor >> 16) & 0xFF; // B
    cpuPixelData[idx + 3] = (rgbaColor >> 24) & 0xFF; // A
}

uint32_t Texture2D::getPixel(uint32_t x, uint32_t y) const {
    if (x >= width || y >= height || cpuPixelData.empty()) return 0;
    size_t idx = (y * width + x) * 4;

    return (static_cast<uint32_t>(cpuPixelData[idx + 3]) << 24) |
           (static_cast<uint32_t>(cpuPixelData[idx + 2]) << 16) |
           (static_cast<uint32_t>(cpuPixelData[idx + 1]) << 8)  |
           (static_cast<uint32_t>(cpuPixelData[idx + 0]));
}

} // namespace PJEngine