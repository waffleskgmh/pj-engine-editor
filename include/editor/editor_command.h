#pragma once

#include "engine/ecs/ecs_world.h"
#include "engine/core/math2d.h"
#include <string>
#include <vector>
#include <memory>

namespace PJEngine {

// Základné rozhranie pre akýkoľvek vratný príkaz
class IEditorCommand {
public:
    virtual ~IEditorCommand() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
    virtual std::string getName() const = 0;
};

// 1. Príkaz na zmenu transformácie entity
class TransformCommand : public IEditorCommand {
public:
    TransformCommand(ECSWorld& world, Entity entity, const Transform2D& oldTransform, const Transform2D& newTransform);
    void execute() override;
    void undo() override;
    std::string getName() const override { return "Zmena Transformacie"; }

private:
    ECSWorld& world;
    Entity entity;
    Transform2D oldTrans;
    Transform2D newTrans;
};

// 2. Centrálny správca histórie (Undo / Redo zásobníky)
class CommandHistory {
public:
    static CommandHistory& get();

    void executeCommand(std::unique_ptr<IEditorCommand> cmd);
    bool undo();
    bool redo();

    bool canUndo() const { return !undoStack.empty(); }
    bool canRedo() const { return !redoStack.empty(); }

    std::string getUndoName() const;
    std::string getRedoName() const;

    void clear();

private:
    CommandHistory() = default;
    ~CommandHistory() = default;

    std::vector<std::unique_ptr<IEditorCommand>> undoStack;
    std::vector<std::unique_ptr<IEditorCommand>> redoStack;
    static constexpr size_t MAX_HISTORY_STEPS = 100;
};

} // namespace PJEngine