#include "editor/animation_editor.h"
#include "engine/core/logger.h"
#include "imgui.h"

#include <algorithm>

namespace PJEngine {

AnimationEditor::AnimationEditor() {
    // Vytvorenie základného animátora pre testovanie
    SpriteAnimation idleAnim;
    idleAnim.addFrame(0, 0.2f);
    idleAnim.addFrame(1, 0.2f);
    idleAnim.addFrame(2, 0.2f);
    idleAnim.addFrame(3, 0.2f);
    idleAnim.isLooping = true;

    SpriteAnimation walkAnim;
    walkAnim.addFrame(4, 0.15f);
    walkAnim.addFrame(5, 0.15f);
    walkAnim.addFrame(6, 0.15f);
    walkAnim.addFrame(7, 0.15f);
    walkAnim.isLooping = true;

    activeController.addState("idle", idleAnim);
    activeController.addState("walk", walkAnim);
    activeController.addTransition("idle", "walk", false);
    activeController.addCondition("idle", "walk", "speed", ConditionMode::GREATER, 0.1f);
    activeController.addTransition("walk", "idle", false);
    activeController.addCondition("walk", "idle", "speed", ConditionMode::LESS, 0.1f);
}

void AnimationEditor::addKeyframe(SpriteAnimation& anim, uint32_t frameIndex, float duration) {
    anim.addFrame(frameIndex, duration);
}

void AnimationEditor::setLooping(SpriteAnimation& anim, bool looping) {
    anim.isLooping = looping;
}

void AnimationEditor::scrubTo(SpriteAnimation& anim, float normalizedTime) {
    float total = getTotalDuration(anim);
    float target = total * normalizedTime;
    anim.reset();
    anim.update(target);
}

float AnimationEditor::getTotalDuration(const SpriteAnimation& anim) {
    float total = 0.0f;
    for (const auto& f : anim.frames) {
        total += f.duration;
    }
    return total;
}

void AnimationEditor::renderGUI() {
    ImGui::Begin("Animation Timeline");

    // 1. Výber klipu / stavu animátora
    ImGui::Text("Animacia:");
    ImGui::SameLine();

    std::string preview = selectedState.empty() ? "Ziadna" : selectedState;
    if (ImGui::BeginCombo("##AnimCombo", preview.c_str())) {
        for (const auto& kv : activeController.states) {
            bool isSelected = (selectedState == kv.first);
            if (ImGui::Selectable(kv.first.c_str(), isSelected)) {
                selectedState = kv.first;
                scrubTime = 0.0f;
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    ImGui::SameLine();
    if (ImGui::Button("+ Novy Klip")) {
        SpriteAnimation newClip;
        newClip.addFrame(0, 0.1f);
        std::string newName = "anim_" + std::to_string(activeController.states.size());
        activeController.addState(newName, newClip);
        selectedState = newName;
    }

    ImGui::Separator();

    auto it = activeController.states.find(selectedState);
    if (it == activeController.states.end()) {
        ImGui::TextDisabled("Vyberte alebo vytvorte animacny klip...");
        ImGui::End();
        return;
    }

    auto& anim = it->second.clip;
    float totalDuration = getTotalDuration(anim);

    // 2. Ovládacie tlačidlá prehrávania
    if (ImGui::Button(isPlaying ? "⏸ Pauza" : "▶ Prehrat")) {
        isPlaying = !isPlaying;
    }
    ImGui::SameLine();
    if (ImGui::Button("⏹ Zastavit")) {
        isPlaying = false;
        scrubTime = 0.0f;
        anim.reset();
    }
    ImGui::SameLine();
    ImGui::Checkbox("Looping", &anim.isLooping);

    // 3. Posuvník časovej osi (Scrubbing Timeline)
    ImGui::Text("Cas: %.2f s / %.2f s", scrubTime, totalDuration);
    if (ImGui::SliderFloat("##ScrubSlider", &scrubTime, 0.0f, totalDuration > 0.0f ? totalDuration : 1.0f, "%.2f s")) {
        isPlaying = false;
        if (totalDuration > 0.0f) {
            scrubTo(anim, scrubTime / totalDuration);
        }
    }

    ImGui::Separator();

    // 4. Kľúčové snímky (Keyframe Strip)
    ImGui::Text("Klucove snimky (%zu):", anim.frames.size());
    if (ImGui::Button("+ Pridat snimku")) {
        uint32_t nextIdx = anim.frames.empty() ? 0 : (anim.frames.back().frameIndex + 1);
        anim.addFrame(nextIdx, 0.1f);
    }

    ImGui::BeginChild("KeyframeStrip", ImVec2(0, 90), true, ImGuiWindowFlags_HorizontalScrollbar);
    for (size_t i = 0; i < anim.frames.size(); ++i) {
        ImGui::PushID(static_cast<int>(i));

        ImGui::BeginGroup();
        std::string fLabel = "Snimka #" + std::to_string(anim.frames[i].frameIndex);
        ImGui::Button(fLabel.c_str(), ImVec2(80, 40));

        ImGui::SetNextItemWidth(80);
        ImGui::DragFloat("##Dur", &anim.frames[i].duration, 0.01f, 0.01f, 2.0f, "%.2fs");
        ImGui::EndGroup();

        ImGui::SameLine();
        ImGui::PopID();
    }
    ImGui::EndChild();

    ImGui::End();
}

} // namespace PJEngine