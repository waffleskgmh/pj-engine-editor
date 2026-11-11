#include "editor/particle_editor.h"

namespace PJEngine {

void ParticleEditor::triggerBurst(ParticleSystem2D& system, const Vec2& center, const Color& startCol, const Color& endCol, int count) {
    system.emit(center, count, Vec2(0.0f, -80.0f), 40.0f, startCol, endCol, 6.0f, 0.8f);
}

} // namespace PJEngine