#pragma once

#include "engine/tilemap/tilemap.h"
#include "engine/tilemap/tile_palette.h"
#include "engine/ecs/ecs_world.h"
#include "engine/core/math2d.h"

namespace PJEngine {

enum class TileTool {
    BRUSH,       // Kreslenie vybranej dlaždice
    ERASER,      // Mazanie dlaždice (-1)
    BUCKET_FILL, // Výplň ohraničenej oblasti (Flood Fill)
    RECTANGLE,   // Kreslenie plného obdĺžnika
    PICKER       // Pipeta na nasatie dlaždice z mapy
};

class TilemapEditor {
public:
    TileTool currentTool = TileTool::BRUSH;
    size_t activeLayerIndex = 0;
    TilePalette activePalette;

    // Počiatočný bod pre ťahanie obdĺžnika
    bool isDragging = false;
    int dragStartTileX = 0;
    int dragStartTileY = 0;

    TilemapEditor() = default;

    // Interakcia myšou v scéne
    void onMouseDown(Tilemap& map, const Vec2& worldPos);
    void onMouseDrag(Tilemap& map, const Vec2& worldPos);
    void onMouseUp(Tilemap& map, const Vec2& worldPos);

    // Algoritmy kreslenia
    static void paintTileAt(TileLayer& layer, const Vec2& worldPos, int32_t tileId);
    static void eraseTileAt(TileLayer& layer, const Vec2& worldPos);
    static void floodFill(TileLayer& layer, int startX, int startY, int32_t newTileId);
    static void drawRectangle(TileLayer& layer, int x0, int y0, int x1, int y1, int32_t tileId);

    // Generovanie fyziky do ECS sveta
    void generateCollidersForMap(ECSWorld& world, const Tilemap& map);

    // Vykreslenie UI panelu pre Tilemap Editor (Paleta + Nástroje + Vrstvy + Generovanie)
    void renderGUI(Tilemap& map, ECSWorld& world);
};

} // namespace PJEngine