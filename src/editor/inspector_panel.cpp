#include "editor/inspector_panel.h"
#include "editor/editor_command.h"
#include "engine/core/logger.h"
#include "imgui.h"

namespace PJEngine {

bool InspectorPanel::setEntityPosition(EditorApp& app, Entity e, const Vec2& newPos) {
    if (!app.world.isAlive(e) || !app.world.hasComponent<Transform2D>(e)) return false;
    app.world.getComponent<Transform2D>(e).position = newPos;
    return true;
}

bool InspectorPanel::setEntityRotation(EditorApp& app, Entity e, float radians) {
    if (!app.world.isAlive(e) || !app.world.hasComponent<Transform2D>(e)) return false;
    app.world.getComponent<Transform2D>(e).rotation = radians;
    return true;
}

bool InspectorPanel::setEntityRigidBodyMass(EditorApp& app, Entity e, float mass) {
    if (!app.world.isAlive(e) || !app.world.hasComponent<RigidBody2D>(e)) return false;
    app.world.getComponent<RigidBody2D>(e).setMass(mass);
    return true;
}

bool InspectorPanel::toggleEntityStatic(EditorApp& app, Entity e) {
    if (!app.world.isAlive(e) || !app.world.hasComponent<RigidBody2D>(e)) return false;
    auto& rb = app.world.getComponent<RigidBody2D>(e);
    if (rb.bodyType == BodyType::STATIC) {
        rb.bodyType = BodyType::DYNAMIC;
        rb.setMass(1.0f);
    } else {
        rb.bodyType = BodyType::STATIC;
        rb.invMass = 0.0f;
    }
    return true;
}

void InspectorPanel::render(EditorApp& app) {
    ImGui::Begin("Inspector");

    Entity e = app.selectedEntity;
    if (e == NULL_ENTITY || !app.world.isAlive(e)) {
        ImGui::TextDisabled("Vyberte entitu v paneli Hierarchy...");
        ImGui::End();
        return;
    }

    ImGui::Text("Vybrana entita ID: %u", e);
    ImGui::Separator();

    renderTagComponent(app, e);
    renderTransform2D(app, e);
    renderSpriteRenderer(app, e);
    renderRigidBody2D(app, e);
    renderCollider2D(app, e);
    renderScriptComponent(app, e);
    renderAudioSourceComponent(app, e);

    ImGui::Spacing();
    ImGui::Separator();
    renderAddComponentMenu(app, e);

    ImGui::Spacing();
    if (ImGui::Button("Zmazat Entitu", ImVec2(-1, 26))) {
        app.world.destroyEntity(e);
        app.selectedEntity = NULL_ENTITY;
    }

    ImGui::End();
}

void InspectorPanel::renderTagComponent(EditorApp& app, Entity e) {
    if (!app.world.hasComponent<TagComponent>(e)) return;
    auto& tc = app.world.getComponent<TagComponent>(e);

    char nameBuf[64];
    strncpy(nameBuf, tc.name.c_str(), sizeof(nameBuf));
    if (ImGui::InputText("Nazov", nameBuf, sizeof(nameBuf))) {
        tc.name = nameBuf;
    }

    char tagBuf[32];
    strncpy(tagBuf, tc.tag.c_str(), sizeof(tagBuf));
    if (ImGui::InputText("Tag", tagBuf, sizeof(tagBuf))) {
        tc.tag = tagBuf;
    }

    ImGui::Checkbox("Aktivna", &tc.isActive);
    ImGui::Separator();
}

void InspectorPanel::renderTransform2D(EditorApp& app, Entity e) {
    if (!app.world.hasComponent<Transform2D>(e)) return;
    auto& t = app.world.getComponent<Transform2D>(e);

    if (ImGui::CollapsingHeader("Transform 2D", ImGuiTreeNodeFlags_DefaultOpen)) {
        Transform2D oldT = t;
        bool changed = false;

        if (ImGui::DragFloat2("Pozicia", &t.position.x, 1.0f)) changed = true;

        float rotDeg = t.rotation * RAD2DEG;
        if (ImGui::DragFloat("Rotacia", &rotDeg, 1.0f)) {
            t.rotation = rotDeg * DEG2RAD;
            changed = true;
        }

        if (ImGui::DragFloat2("Mierka", &t.scale.x, 0.05f)) changed = true;

        // Ak sa hodnota prestala ťahať, uložíme akciu do Undo zásobníka
        if (ImGui::IsItemDeactivatedAfterEdit() && changed) {
            CommandHistory::get().executeCommand(std::make_unique<TransformCommand>(app.world, e, oldT, t));
        }
    }
}

void InspectorPanel::renderSpriteRenderer(EditorApp& app, Entity e) {
    if (!app.world.hasComponent<SpriteRendererComponent>(e)) return;
    auto& s = app.world.getComponent<SpriteRendererComponent>(e);

    bool open = true;
    if (ImGui::CollapsingHeader("Sprite Renderer", &open, ImGuiTreeNodeFlags_DefaultOpen)) {
        char guidBuf[64];
        strncpy(guidBuf, s.textureGuid.c_str(), sizeof(guidBuf));
        if (ImGui::InputText("Textura GUID", guidBuf, sizeof(guidBuf))) {
            s.textureGuid = guidBuf;
        }

        ImGui::DragFloat2("Velkost", &s.size.x, 1.0f, 1.0f, 1000.0f);
        ImGui::DragFloat2("Pivot", &s.pivot.x, 0.05f, 0.0f, 1.0f);
        ImGui::ColorEdit4("Farba (Tint)", &s.tint.r);
        ImGui::DragInt("Sorting Order", &s.sortingOrder, 1);
        ImGui::Checkbox("Flip X", &s.flipX);
        ImGui::SameLine();
        ImGui::Checkbox("Flip Y", &s.flipY);
        ImGui::Checkbox("Viditelny", &s.isVisible);
    }

    if (!open) {
        app.world.removeComponent<SpriteRendererComponent>(e);
    }
}

void InspectorPanel::renderRigidBody2D(EditorApp& app, Entity e) {
    if (!app.world.hasComponent<RigidBody2D>(e)) return;
    auto& rb = app.world.getComponent<RigidBody2D>(e);

    bool open = true;
    if (ImGui::CollapsingHeader("RigidBody 2D", &open, ImGuiTreeNodeFlags_DefaultOpen)) {
        const char* types[] = { "STATIC", "DYNAMIC", "KINEMATIC" };
        int currentType = static_cast<int>(rb.bodyType);
        if (ImGui::Combo("Typ telesa", &currentType, types, 3)) {
            rb.bodyType = static_cast<BodyType>(currentType);
            rb.setMass(rb.mass);
        }

        float mass = rb.mass;
        if (ImGui::DragFloat("Hmotnost", &mass, 0.1f, 0.01f, 500.0f)) {
            rb.setMass(mass);
        }
        ImGui::DragFloat("Pruznost (Bounce)", &rb.restitution, 0.05f, 0.0f, 1.0f);
        ImGui::DragFloat("Trenie", &rb.friction, 0.05f, 0.0f, 1.0f);
        ImGui::DragFloat("Gravitacna mierka", &rb.gravityScale, 0.1f, -10.0f, 10.0f);
        ImGui::Checkbox("Zamknut rotaciu", &rb.fixedRotation);
    }

    if (!open) {
        app.world.removeComponent<RigidBody2D>(e);
    }
}

void InspectorPanel::renderCollider2D(EditorApp& app, Entity e) {
    if (!app.world.hasComponent<Collider2D>(e)) return;
    auto& col = app.world.getComponent<Collider2D>(e);

    bool open = true;
    if (ImGui::CollapsingHeader("Collider 2D", &open, ImGuiTreeNodeFlags_DefaultOpen)) {
        const char* colTypes[] = { "BOX (Obdlznik)", "CIRCLE (Kruh)" };
        int currentType = static_cast<int>(col.type);
        if (ImGui::Combo("Typ tvaru", &currentType, colTypes, 2)) {
            col.type = static_cast<ColliderType>(currentType);
        }

        ImGui::DragFloat2("Offset", &col.offset.x, 1.0f);
        if (col.type == ColliderType::CIRCLE) {
            ImGui::DragFloat("Polomer", &col.radius, 1.0f, 1.0f, 500.0f);
        } else {
            ImGui::DragFloat2("Rozmer (W, H)", &col.size.x, 1.0f, 1.0f, 1000.0f);
        }

        ImGui::Checkbox("Trigger (Bez odrazu)", &col.isTrigger);
    }

    if (!open) {
        app.world.removeComponent<Collider2D>(e);
    }
}

void InspectorPanel::renderScriptComponent(EditorApp& app, Entity e) {
    if (!app.world.hasComponent<ScriptComponent>(e)) return;
    auto& sc = app.world.getComponent<ScriptComponent>(e);

    bool open = true;
    if (ImGui::CollapsingHeader("PJ Script", &open, ImGuiTreeNodeFlags_DefaultOpen)) {
        char pathBuf[128];
        strncpy(pathBuf, sc.scriptPath.c_str(), sizeof(pathBuf));
        if (ImGui::InputText("Cesta k .pj", pathBuf, sizeof(pathBuf))) {
            sc.scriptPath = pathBuf;
        }

        ImGui::Checkbox("Povoleny", &sc.isEnabled);
    }

    if (!open) {
        app.world.removeComponent<ScriptComponent>(e);
    }
}

void InspectorPanel::renderAudioSourceComponent(EditorApp& app, Entity e) {
    if (!app.world.hasComponent<AudioSourceComponent>(e)) return;
    auto& a = app.world.getComponent<AudioSourceComponent>(e);

    bool open = true;
    if (ImGui::CollapsingHeader("Audio Source", &open, ImGuiTreeNodeFlags_DefaultOpen)) {
        char sndBuf[64];
        strncpy(sndBuf, a.soundName.c_str(), sizeof(sndBuf));
        if (ImGui::InputText("Zvukovy subor", sndBuf, sizeof(sndBuf))) {
            a.soundName = sndBuf;
        }

        ImGui::SliderFloat("Hlasitost", &a.volume, 0.0f, 1.0f);
        ImGui::SliderFloat("Pitch", &a.pitch, 0.1f, 3.0f);
        ImGui::Checkbox("Looping", &a.isLooping);
        ImGui::Checkbox("Priestorovy (Spatial)", &a.isSpatial);

        if (ImGui::Button("Prehrat zvuk (Test)")) {
            app.audio.playSound(a.soundName, "sfx", a.volume, a.pitch, false);
        }
    }

    if (!open) {
        app.world.removeComponent<AudioSourceComponent>(e);
    }
}

void InspectorPanel::renderAddComponentMenu(EditorApp& app, Entity e) {
    if (ImGui::Button("+ Pridat Komponent", ImVec2(-1, 26))) {
        ImGui::OpenPopup("AddComponentPopup");
    }

    if (ImGui::BeginPopup("AddComponentPopup")) {
        if (!app.world.hasComponent<SpriteRendererComponent>(e) && ImGui::MenuItem("Sprite Renderer")) {
            app.world.addComponent<SpriteRendererComponent>(e);
        }
        if (!app.world.hasComponent<RigidBody2D>(e) && ImGui::MenuItem("RigidBody 2D")) {
            app.world.addComponent<RigidBody2D>(e, RigidBody2D(BodyType::DYNAMIC, 1.0f));
        }
        if (!app.world.hasComponent<Collider2D>(e) && ImGui::MenuItem("Collider 2D")) {
            app.world.addComponent<Collider2D>(e, Collider2D::MakeBox(Vec2(32.0f, 32.0f)));
        }
        if (!app.world.hasComponent<ScriptComponent>(e) && ImGui::MenuItem("PJ Script")) {
            app.world.addComponent<ScriptComponent>(e, ScriptComponent("assets/scripts/player.pj"));
        }
        if (!app.world.hasComponent<AudioSourceComponent>(e) && ImGui::MenuItem("Audio Source")) {
            app.world.addComponent<AudioSourceComponent>(e);
        }

        ImGui::EndPopup();
    }
}

} // namespace PJEngine