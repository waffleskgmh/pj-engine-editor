#include "editor/tilemap_editor.h"
#include "engine/physics/rigidbody2d.h"
#include "engine/physics/collider2d.h"
#include "engine/core/logger.h"
#include "imgui.h"

#include <queue>
#include <cmath>
#include <algorithm>

namespace PJEngine {

void TilemapEditor::paintTileAt(TileLayer& layer, const Vec2& worldPos, int32_t tileId) {
    int x = static_cast<int>(std::floor(worldPos.x / layer.tileSize));
    int y = static_cast<int>(std::floor(worldPos.y / layer.tileSize));
    if (layer.isValidCoord(x, y)) {
        layer.setTile(x, y, tileId);
    }
}

void TilemapEditor::eraseTileAt(TileLayer& layer, const Vec2& worldPos) {
    paintTileAt(layer, worldPos, -1);
}

void TilemapEditor::floodFill(TileLayer& layer, int startX, int startY, int32_t newTileId) {
    if (!layer.isValidCoord(startX, startY)) return;

    int32_t targetTile = layer.getTile(startX, startY);
    if (targetTile == newTileId) return;

    std::queue<std::pair<int, int>> q;
    q.push({startX, startY});
    layer.setTile(startX, startY, newTileId);

    while (!q.empty()) {
        auto [cx, cy] = q.front();
        q.pop();

        int dx[4] = {0, 0, -1, 1};
        int dy[4] = {-1, 1, 0, 0};

        for (int i = 0; i < 4; ++i) {
            int nx = cx + dx[i];
            int ny = cy + dy[i];

            if (layer.isValidCoord(nx, ny) && layer.getTile(nx, ny) == targetTile) {
                layer.setTile(nx, ny, newTileId);
                q.push({nx, ny});
            }
        }
    }
}

void TilemapEditor::drawRectangle(TileLayer& layer, int x0, int y0, int x1, int y1, int32_t tileId) {
    int minX = std::min(x0, x1);
    int maxX = std::max(x0, x1);
    int minY = std::min(y0, y1);
    int maxY = std::max(y0, y1);

    for (int y = minY; y <= maxY; ++y) {
        for (int x = minX; x <= maxX; ++x) {
            if (layer.isValidCoord(x, y)) {
                layer.setTile(x, y, tileId);
            }
        }
    }
}

void TilemapEditor::onMouseDown(Tilemap& map, const Vec2& worldPos) {
    if (activeLayerIndex >= map.layers.size()) return;
    auto& layer = map.layers[activeLayerIndex];

    Vec2 tileCoord = map.worldToTile(worldPos, layer.tileSize);
    int tx = static_cast<int>(tileCoord.x);
    int ty = static_cast<int>(tileCoord.y);

    if (currentTool == TileTool::BRUSH) {
        paintTileAt(layer, worldPos, activePalette.selectedTileId);
    } else if (currentTool == TileTool::ERASER) {
        eraseTileAt(layer, worldPos);
    } else if (currentTool == TileTool::BUCKET_FILL) {
        floodFill(layer, tx, ty, activePalette.selectedTileId);
    } else if (currentTool == TileTool::PICKER) {
        if (layer.isValidCoord(tx, ty)) {
            activePalette.selectedTileId = layer.getTile(tx, ty);
            currentTool = TileTool::BRUSH;
            Logger::info("Nasata dlazdica ID: " + std::to_string(activePalette.selectedTileId));
        }
    } else if (currentTool == TileTool::RECTANGLE) {
        isDragging = true;
        dragStartTileX = tx;
        dragStartTileY = ty;
    }
}

void TilemapEditor::onMouseDrag(Tilemap& map, const Vec2& worldPos) {
    if (activeLayerIndex >= map.layers.size()) return;
    auto& layer = map.layers[activeLayerIndex];

    if (currentTool == TileTool::BRUSH) {
        paintTileAt(layer, worldPos, activePalette.selectedTileId);
    } else if (currentTool == TileTool::ERASER) {
        eraseTileAt(layer, worldPos);
    }
}

void TilemapEditor::onMouseUp(Tilemap& map, const Vec2& worldPos) {
    if (currentTool == TileTool::RECTANGLE && isDragging) {
        if (activeLayerIndex < map.layers.size()) {
            auto& layer = map.layers[activeLayerIndex];
            Vec2 tileCoord = map.worldToTile(worldPos, layer.tileSize);
            drawRectangle(layer, dragStartTileX, dragStartTileY, static_cast<int>(tileCoord.x), static_cast<int>(tileCoord.y), activePalette.selectedTileId);
        }
        isDragging = false;
    }
}

void TilemapEditor::generateCollidersForMap(ECSWorld& world, const Tilemap& map) {
    size_t createdCount = 0;

    for (const auto& layer : map.layers) {
        for (uint32_t y = 0; y < layer.height; ++y) {
            for (uint32_t x = 0; x < layer.width; ++x) {
                int32_t tileId = layer.getTile(static_cast<int>(x), static_cast<int>(y));
                if (tileId < 0) continue;

                TileCollisionType colType = activePalette.getTileCollision(tileId);
                if (colType == TileCollisionType::SOLID) {
                    Entity wall = world.createEntity();
                    Vec2 centerPos = map.tileToWorld(x, y, layer.tileSize) + Vec2(layer.tileSize * 0.5f, layer.tileSize * 0.5f);

                    world.addComponent<Transform2D>(wall, Transform2D{centerPos, 0.0f, Vec2::One(), Vec2::Zero()});
                    world.addComponent<RigidBody2D>(wall, RigidBody2D(BodyType::STATIC, 0.0f));
                    world.addComponent<Collider2D>(wall, Collider2D::MakeBox(Vec2(layer.tileSize, layer.tileSize)));
                    createdCount++;
                }
            }
        }
    }

    Logger::info("Vygenerovanych " + std::to_string(createdCount) + " pevnych kolizorov pre mapu.");
}

void TilemapEditor::renderGUI(Tilemap& map, ECSWorld& world) {
    ImGui::Begin("Tilemap Editor");

    // 1. Výber nástroja
    ImGui::Text("Nastroje:");
    if (ImGui::RadioButton("Stetec (B)", currentTool == TileTool::BRUSH)) currentTool = TileTool::BRUSH;
    ImGui::SameLine();
    if (ImGui::RadioButton("Guma (E)", currentTool == TileTool::ERASER)) currentTool = TileTool::ERASER;
    ImGui::SameLine();
    if (ImGui::RadioButton("Vypln (F)", currentTool == TileTool::BUCKET_FILL)) currentTool = TileTool::BUCKET_FILL;
    ImGui::SameLine();
    if (ImGui::RadioButton("Obdlznik (R)", currentTool == TileTool::RECTANGLE)) currentTool = TileTool::RECTANGLE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Pipeta (P)", currentTool == TileTool::PICKER)) currentTool = TileTool::PICKER;

    ImGui::Separator();

    // 2. Správa vrstiev (Layers)
    ImGui::Text("Aktivna vrstva:");
    if (ImGui::Button("+ Pridat vrstvu")) {
        map.addLayer(TileLayer(40, 25, 16.0f));
    }
    ImGui::SameLine();

    if (!map.layers.empty()) {
        std::string previewName = (activeLayerIndex < map.layers.size())
                                ? ("Vrstva #" + std::to_string(activeLayerIndex))
                                : "Ziadna vrstva";

        if (ImGui::BeginCombo("##LayerCombo", previewName.c_str())) {
            for (size_t i = 0; i < map.layers.size(); ++i) {
                std::string layerName = "Vrstva #" + std::to_string(i);
                bool isSelected = (activeLayerIndex == i);
                if (ImGui::Selectable(layerName.c_str(), isSelected)) {
                    activeLayerIndex = i;
                }
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        if (activeLayerIndex < map.layers.size()) {
            auto& l = map.layers[activeLayerIndex];
            ImGui::Checkbox("Viditelna", &l.isVisible);
            ImGui::SameLine();
            ImGui::SliderFloat("Priehladnost", &l.opacity, 0.0f, 1.0f);
        }
    }

    ImGui::Separator();

    // 3. Paleta dlaždíc (Mriežka výberu s názvami a farbami)
    const auto* curTile = activePalette.getTile(activePalette.selectedTileId);
    std::string curName = curTile ? curTile->tag : "Neznama";
    ImGui::Text("Paleta dlazdic: [ID %d: %s]", activePalette.selectedTileId, curName.c_str());

    ImGui::BeginChild("PaletteRegion", ImVec2(0, 160), true);
    for (const auto& kv : activePalette.tiles) {
        int32_t tid = kv.first;
        const auto& tDef = kv.second;

        ImVec4 btnColor = ImVec4(0.3f, 0.35f, 0.45f, 1.0f);
        if (tid == 0) btnColor = ImVec4(0.2f, 0.7f, 0.3f, 1.0f);      // Tráva = Zelená
        else if (tid == 1) btnColor = ImVec4(0.55f, 0.35f, 0.2f, 1.0f); // Hlina = Hnedá
        else if (tid == 2) btnColor = ImVec4(0.5f, 0.55f, 0.6f, 1.0f);  // Kameň = Sivá
        else if (tid == 3) btnColor = ImVec4(0.75f, 0.3f, 0.25f, 1.0f); // Tehla = Terakota
        else if (tid == 4) btnColor = ImVec4(0.65f, 0.45f, 0.25f, 1.0f); // Drevo = Dub
        else if (tid == 5) btnColor = ImVec4(0.2f, 0.5f, 0.85f, 1.0f);  // Voda = Modrá
        else if (tid == 6) btnColor = ImVec4(0.85f, 0.8f, 0.4f, 1.0f);  // Piesok = Žltá
        else if (tid == 7) btnColor = ImVec4(0.8f, 0.6f, 0.3f, 1.0f);  // Rebrík = Žltohnedá
        else if (tid == 8) btnColor = ImVec4(0.9f, 0.2f, 0.2f, 1.0f);  // Láva = Červená
        else if (tid == 9) btnColor = ImVec4(0.4f, 0.85f, 0.5f, 1.0f);  // Kvety = Svetlozelená

        bool isSelected = (activePalette.selectedTileId == tid);
        if (isSelected) {
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
        }

        ImGui::PushStyleColor(ImGuiCol_Button, btnColor);
        std::string label = std::to_string(tid) + ": " + tDef.tag;
        if (ImGui::Button(label.c_str(), ImVec2(130, 28))) {
            activePalette.selectedTileId = tid;
        }
        ImGui::PopStyleColor();

        if (isSelected) {
            ImGui::PopStyleColor();
        }

        if ((tid + 1) % 2 != 0) {
            ImGui::SameLine();
        }
    }
    ImGui::EndChild();

    if (ImGui::Button("⚡ Vygenerovat kolizie pre mapu", ImVec2(-1, 26))) {
        generateCollidersForMap(world, map);
    }

    ImGui::Separator();

    // 4. Nastavenie kolízie pre vybranú dlaždicu
    ImGui::Text("Typ kolizie vybranej dlazdice:");
    const char* colTypes[] = { "NONE (Priechodna)", "SOLID (Stena)", "WATER (Voda)", "LADDER (Rebrik)", "HAZARD (Smrt/Lava)" };
    TileCollisionType currentType = activePalette.getTileCollision(activePalette.selectedTileId);
    int colIdx = static_cast<int>(currentType);
    if (ImGui::Combo("Kolizia##Tile", &colIdx, colTypes, 5)) {
        activePalette.setTileCollision(activePalette.selectedTileId, static_cast<TileCollisionType>(colIdx));
    }

    ImGui::End();
}

} // namespace PJEngine