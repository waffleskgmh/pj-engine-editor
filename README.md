# PJ Engine Native

**PJ Engine** is an open-source, native 2D game development ecosystem written in C++17, featuring its own programming language, game engine and integrated development environment.

The project aims to provide a complete development environment for creating 2D games, from writing game logic to building and managing entire game projects.

## Components

### 1. PJ Programming Language
A custom programming language developed specifically for the PJ ecosystem.

- Lexer and compiler
- Abstract Syntax Tree (AST)
- Bytecode and virtual machine
- Standard library
- Native code generation

### 2. PJ Engine
A native 2D game engine designed for multiple genres and game types.

- 2D rendering
- Physics and collision detection
- Entity Component System (ECS)
- Scene management
- Tilemap system
- Sprite animation and inverse kinematics
- Particle systems and lighting
- Audio system
- Game flow management
- Performance optimization

### 3. PJ Studio
The integrated development environment for creating and editing PJ Engine projects.

- Scene editor
- Hierarchy and inspector
- Game view
- Profiler
- Tilemap editor
- Particle editor
- Animation editor
- Code editor
- Visual scripting

### 4. Gameplay Systems

- Inventory
- Crafting
- Quest system
- Dialogue system
- Character system
- Survival mechanics
- NPC controller
- Interactive objects

## Technology

- C++17
- Dear ImGui
- SDL2
- GLFW
- GLAD
- stb libraries

## Project Structure

```text
include/    Public headers
src/        Engine, compiler and editor implementation
tests/      Automated tests
scripts/    Example PJ scripts
assets/     Project assets
vendor/     Third-party dependencies
```

## Building

The project includes CMake and Makefile build configurations.

Build requirements and platform-specific instructions are being developed.

## License

PJ Engine Native is distributed under the MIT License. See [LICENSE](LICENSE) for details.

## Development Status

PJ Engine Native is an actively developed project. Features, APIs and internal architecture may change as development progresses.

Contributions, suggestions and bug reports are welcome.# pj-engine-editor
