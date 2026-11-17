cmake_minimum_required(VERSION 3.16)
project(PJEngineNative VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4 /O2)
else()
    add_compile_options(-Wall -Wextra -O2)
endif()

if(APPLE)
    include_directories(/opt/local/include /opt/local/include/SDL2)
    link_directories(/opt/local/lib)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/vendor)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

set(SRC_PJ
    src/pj/pj_lexer.cpp
    src/pj/pj_compiler.cpp
    src/pj/pj_vm.cpp
    src/pj/pj_stdlib.cpp
)

set(SRC_ENGINE
    src/engine/core/math2d.cpp
    src/engine/core/time_step.cpp
    src/engine/core/event_bus.cpp
    src/engine/core/memory_pool.cpp
    src/engine/core/logger.cpp
    src/engine/ecs/ecs_world.cpp
    src/engine/ecs/component_registry.cpp
    src/engine/physics/rigidbody2d.cpp
    src/engine/physics/collider2d.cpp
    src/engine/physics/spatial_hash.cpp
    src/engine/physics/physics_engine2d.cpp
    src/engine/graphics/camera2d.cpp
    src/engine/graphics/texture.cpp
    src/engine/graphics/renderer2d.cpp
    src/engine/graphics/particles2d.cpp
    src/engine/graphics/lighting2d.cpp
    src/engine/assets/asset_database.cpp
    src/engine/assets/sprite_sheet.cpp
    src/engine/platform/platform_window.cpp
    src/engine/tilemap/tile_layer.cpp
    src/engine/tilemap/autotiler.cpp
    src/engine/tilemap/tilemap.cpp
    src/engine/tilemap/tile_palette.cpp
    src/engine/animation/sprite_animation.cpp
    src/engine/animation/bone2d.cpp
    src/engine/animation/inverse_kinematics.cpp
    src/engine/animation/animator_controller.cpp
    src/engine/audio/audio_engine.cpp
    src/engine/scene/scene_node.cpp
    src/engine/scene/scene.cpp
    src/engine/scene/scene_manager.cpp
    src/engine/scene/map_transition.cpp
    src/engine/scene/prefab.cpp
    src/engine/input/input_manager.cpp
    src/engine/optimization/culling2d.cpp
    src/engine/optimization/quality_manager.cpp
    src/engine/flow/game_flow.cpp
    src/engine/ui/hud_system.cpp
    src/gameplay/inventory.cpp
    src/gameplay/character.cpp
    src/gameplay/quest.cpp
    src/gameplay/dialogue.cpp
    src/gameplay/crafting.cpp
    src/gameplay/survival.cpp
    src/gameplay/npc_controller.cpp
    src/gameplay/interactive_object.cpp
    src/engine/project/project.cpp
    src/engine/project/project_exporter.cpp
    src/bridge/pj_engine_binding.cpp
)

set(SRC_EDITOR
    src/editor/editor_app.cpp
    src/editor/editor_command.cpp
    src/editor/scene_view.cpp
    src/editor/game_view.cpp
    src/editor/hierarchy_panel.cpp
    src/editor/inspector_panel.cpp
    src/editor/profiler_panel.cpp
    src/editor/tilemap_editor.cpp
    src/editor/particle_editor.cpp
    src/editor/animation_editor.cpp
    src/editor/code_editor_panel.cpp
    src/editor/visual_script_editor.cpp
    src/editor/gameplay_editors.cpp
    src/editor/asset_browser_panel.cpp
    src/editor/console_panel.cpp
    src/editor/map_transition_panel.cpp
    src/editor/project_settings_panel.cpp
)

set(SRC_IMGUI
    vendor/imgui/imgui.cpp
    vendor/imgui/imgui_draw.cpp
    vendor/imgui/imgui_tables.cpp
    vendor/imgui/imgui_widgets.cpp
    vendor/imgui/imgui_impl_sdl2.cpp
    vendor/imgui/imgui_impl_sdlrenderer2.cpp
)

add_executable(pj ${SRC_PJ} src/pj/main_cli.cpp)

add_executable(test_engine ${SRC_ENGINE} ${SRC_PJ} tests/test_engine.cpp)
target_link_libraries(test_engine PRIVATE ${SDL2_LIBRARIES})

add_executable(pj-game ${SRC_ENGINE} ${SRC_PJ} src/main_game.cpp)
target_link_libraries(pj-game PRIVATE ${SDL2_LIBRARIES})

add_executable(pj-studio ${SRC_ENGINE} ${SRC_PJ} ${SRC_EDITOR} ${SRC_IMGUI} src/main_editor.cpp)
target_link_libraries(pj-studio PRIVATE ${SDL2_LIBRARIES})