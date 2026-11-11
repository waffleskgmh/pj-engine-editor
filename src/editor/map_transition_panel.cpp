#include "editor/map_transition_panel.h"
#include "engine/core/logger.h"
#include "imgui.h"

namespace PJEngine {

void MapTransitionPanel::render() {
    ImGui::Begin("Map Transitions");

    ImGui::Text("Aktivne portaly na mape:");
    ImGui::Separator();

    auto& manager = MapTransitionManager::get();
    const auto& triggers = manager.getTriggers();

    if (triggers.empty()) {
        ImGui::TextDisabled("Na tejto mape zatial nie su ziadne portaly.");
    } else {
        for (size_t i = 0; i < triggers.size(); ++i) {
            const auto& trig = triggers[i];
            ImGui::PushID(trig.id.c_str());

            ImGui::Text("Portal: %s -> %s", trig.id.c_str(), trig.targetMapPath.c_str());
            ImGui::Text("  Spawn: (%.1f, %.1f) | Oblast: (%.0f, %.0f, %.0f, %.0f)",
                        trig.targetSpawnPoint.x, trig.targetSpawnPoint.y,
                        trig.triggerBounds.x, trig.triggerBounds.y, trig.triggerBounds.width, trig.triggerBounds.height);

            ImGui::SameLine();
            if (ImGui::Button("Otestovat prechod")) {
                manager.triggerTransition(trig);
            }
            ImGui::SameLine();
            if (ImGui::Button("Zmazat")) {
                manager.removeTrigger(trig.id);
                ImGui::PopID();
                break;
            }

            ImGui::Separator();
            ImGui::PopID();
        }
    }

    ImGui::Spacing();
    ImGui::Text("Pridat novy prechod:");

    ImGui::InputText("ID Portalu", newTriggerId, IM_ARRAYSIZE(newTriggerId));
    ImGui::InputText("Cielova Mapa (.pjscene)", targetMapPath, IM_ARRAYSIZE(targetMapPath));

    ImGui::DragFloat2("Spawn Bod (X, Y)", &spawnX, 1.0f);
    ImGui::DragFloat2("Pozicia Portalu (X, Y)", &boundsX, 1.0f);
    ImGui::DragFloat2("Velkost Portalu (W, H)", &boundsW, 1.0f);

    if (ImGui::Button("+ Pridat Portal do mapy", ImVec2(-1, 26))) {
        MapTransitionTrigger newTrig;
        newTrig.id = newTriggerId;
        newTrig.targetMapPath = targetMapPath;
        newTrig.targetSpawnPoint = Vec2(spawnX, spawnY);
        newTrig.triggerBounds = Rect(boundsX, boundsY, boundsW, boundsH);
        newTrig.effect = TransitionEffect::FADE_BLACK;
        newTrig.fadeDuration = 0.5f;

        manager.addTrigger(newTrig);
        Logger::info("Pridany novy portal: " + std::string(newTriggerId));
    }

    ImGui::End();
}

} // namespace PJEngine