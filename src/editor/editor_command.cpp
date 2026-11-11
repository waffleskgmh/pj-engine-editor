#include "editor/editor_command.h"
#include "engine/core/logger.h"

namespace PJEngine {

TransformCommand::TransformCommand(ECSWorld& world, Entity entity, const Transform2D& oldTransform, const Transform2D& newTransform)
    : world(world), entity(entity), oldTrans(oldTransform), newTrans(newTransform) {}

void TransformCommand::execute() {
    if (world.isAlive(entity) && world.hasComponent<Transform2D>(entity)) {
        world.getComponent<Transform2D>(entity) = newTrans;
    }
}

void TransformCommand::undo() {
    if (world.isAlive(entity) && world.hasComponent<Transform2D>(entity)) {
        world.getComponent<Transform2D>(entity) = oldTrans;
    }
}

CommandHistory& CommandHistory::get() {
    static CommandHistory instance;
    return instance;
}

void CommandHistory::executeCommand(std::unique_ptr<IEditorCommand> cmd) {
    if (!cmd) return;

    cmd->execute();
    Logger::info("[UndoRedo]: Vykonany prikaz: " + cmd->getName());

    undoStack.push_back(std::move(cmd));
    redoStack.clear(); // Nová akcia premazáva zásobník Redo

    if (undoStack.size() > MAX_HISTORY_STEPS) {
        undoStack.erase(undoStack.begin());
    }
}

bool CommandHistory::undo() {
    if (undoStack.empty()) return false;

    auto cmd = std::move(undoStack.back());
    undoStack.pop_back();

    cmd->undo();
    Logger::info("[UndoRedo]: Vratena akcia (Undo): " + cmd->getName());

    redoStack.push_back(std::move(cmd));
    return true;
}

bool CommandHistory::redo() {
    if (redoStack.empty()) return false;

    auto cmd = std::move(redoStack.back());
    redoStack.pop_back();

    cmd->execute();
    Logger::info("[UndoRedo]: Zopakိုvana akcia (Redo): " + cmd->getName());

    undoStack.push_back(std::move(cmd));
    return true;
}

std::string CommandHistory::getUndoName() const {
    return undoStack.empty() ? "" : undoStack.back()->getName();
}

std::string CommandHistory::getRedoName() const {
    return redoStack.empty() ? "" : redoStack.back()->getName();
}

void CommandHistory::clear() {
    undoStack.clear();
    redoStack.clear();
}

} // namespace PJEngine