#pragma once

#include "engine/core/math2d.h"
#include <cstdint>
#include <vector>
#include <string>
#include <memory>

struct SDL_Renderer;
struct SDL_Texture;

namespace PJEngine {

enum class TextureFilter {
    NEAREST, // Pixel-art, ostré hrany (predvolené pre 2D)
    LINEAR   // Vyhladené filtrovanie
};

class Texture2D {
public:
    Texture2D();
    ~Texture2D();

    // Zákaz náhodného plytkého kopírovania GPU handlu
    Texture2D(const Texture2D&) = delete;
    Texture2D& operator=(const Texture2D&) = delete;

    // Presunová sémantika (Move semantics)
    Texture2D(Texture2D&& other) noexcept;
    Texture2D& operator=(Texture2D&& other) noexcept;

    // Tvorba a načítanie
    bool loadFromFile(SDL_Renderer* renderer, const std::string& filePath, TextureFilter filter = TextureFilter::NEAREST);
    bool loadFromMemory(SDL_Renderer* renderer, const uint8_t* rgbaData, uint32_t w, uint32_t h, TextureFilter filter = TextureFilter::NEAREST);
    bool createSolid(SDL_Renderer* renderer, uint32_t w, uint32_t h, const Color& color);
    void destroy();

    // CPU prístup k pixelom (ak sú načítané v pamäti)
    void setPixel(uint32_t x, uint32_t y, uint32_t rgbaColor);
    uint32_t getPixel(uint32_t x, uint32_t y) const;
    void updateGPUTexture();

    // Gettery
    uint32_t getWidth() const { return width; }
    uint32_t getHeight() const { return height; }
    Vec2 getSize() const { return Vec2(static_cast<float>(width), static_cast<float>(height)); }
    bool isValid() const { return sdlTexture != nullptr; }
    SDL_Texture* getNativeHandle() const { return sdlTexture; }
    TextureFilter getFilter() const { return filter; }
    const std::string& getFilePath() const { return sourcePath; }

private:
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t channels = 4;
    TextureFilter filter = TextureFilter::NEAREST;
    std::string sourcePath;
    std::vector<uint8_t> cpuPixelData;
    SDL_Texture* sdlTexture = nullptr;
    SDL_Renderer* boundRenderer = nullptr;
};

} // namespace PJEngine