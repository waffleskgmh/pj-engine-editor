CXX = clang++

OPT_FLAGS = -O1 -g

MACPORTS_CFLAGS = -I/opt/local/include -I/opt/local/include/SDL2
MACPORTS_LDFLAGS = -L/opt/local/lib -lSDL2

SDL_CFLAGS := $(shell /opt/local/bin/sdl2-config --cflags 2>/dev/null || sdl2-config --cflags 2>/dev/null || echo $(MACPORTS_CFLAGS))
SDL_LDFLAGS := $(shell /opt/local/bin/sdl2-config --libs 2>/dev/null || sdl2-config --libs 2>/dev/null || echo $(MACPORTS_LDFLAGS))

CXXFLAGS = -std=c++17 -Wall -Wextra $(OPT_FLAGS) -Iinclude -Ivendor -Ivendor/imgui -I/opt/local/include $(SDL_CFLAGS)
LDFLAGS = $(SDL_LDFLAGS)

SRC_CORE = src/engine/core/math2d.cpp \
           src/engine/core/time_step.cpp \
           src/engine/core/event_bus.cpp \
           src/engine/core/memory_pool.cpp \
           src/engine/core/logger.cpp

SRC_ECS = src/engine/ecs/ecs_world.cpp

SRC_PHYSICS = src/engine/physics/rigidbody2d.cpp \
              src/engine/physics/collider2d.cpp \
              src/engine/physics/spatial_hash.cpp \
              src/engine/physics/physics_engine2d.cpp

SRC_GRAPHICS = src/engine/graphics/camera2d.cpp \
               src/engine/graphics/texture.cpp \
               src/engine/graphics/renderer2d.cpp \
               src/engine/graphics/particles2d.cpp \
               src/engine/graphics/lighting2d.cpp

SRC_ASSETS = src/engine/assets/asset_database.cpp \
             src/engine/assets/sprite_sheet.cpp

SRC_PLATFORM = src/engine/platform/platform_window.cpp

SRC_TILEMAP = src/engine/tilemap/tile_layer.cpp \
              src/engine/tilemap/autotiler.cpp \
              src/engine/tilemap/tilemap.cpp \
              src/engine/tilemap/tile_palette.cpp

SRC_ANIMATION = src/engine/animation/sprite_animation.cpp \
                src/engine/animation/bone2d.cpp \
                src/engine/animation/inverse_kinematics.cpp \
                src/engine/animation/animator_controller.cpp

SRC_AUDIO = src/engine/audio/audio_engine.cpp

SRC_SCENE = src/engine/scene/scene_node.cpp \
            src/engine/scene/scene.cpp \
            src/engine/scene/scene_manager.cpp \
            src/engine/scene/map_transition.cpp

SRC_OPTIMIZATION = src/engine/optimization/culling2d.cpp \
                   src/engine/optimization/quality_manager.cpp

SRC_FLOW = src/engine/flow/game_flow.cpp
SRC_UI = src/engine/ui/hud_system.cpp

SRC_GAMEPLAY = src/gameplay/inventory.cpp \
               src/gameplay/character.cpp \
               src/gameplay/quest.cpp \
               src/gameplay/dialogue.cpp \
               src/gameplay/crafting.cpp \
               src/gameplay/survival.cpp \
               src/gameplay/npc_controller.cpp \
               src/gameplay/interactive_object.cpp

SRC_PROJECT = src/engine/project/project.cpp \
              src/engine/project/project_exporter.cpp

SRC_PJ = src/pj/pj_lexer.cpp \
         src/pj/pj_compiler.cpp \
         src/pj/pj_vm.cpp \
         src/pj/pj_stdlib.cpp

SRC_BRIDGE = src/bridge/pj_engine_binding.cpp

SRC_EDITOR = src/editor/editor_app.cpp \
             src/editor/editor_command.cpp \
             src/editor/scene_view.cpp \
             src/editor/game_view.cpp \
             src/editor/hierarchy_panel.cpp \
             src/editor/inspector_panel.cpp \
             src/editor/profiler_panel.cpp \
             src/editor/tilemap_editor.cpp \
             src/editor/particle_editor.cpp \
             src/editor/animation_editor.cpp \
             src/editor/code_editor_panel.cpp \
             src/editor/visual_script_editor.cpp \
             src/editor/gameplay_editors.cpp \
             src/editor/asset_browser_panel.cpp \
             src/editor/console_panel.cpp \
             src/editor/map_transition_panel.cpp \
             src/editor/project_settings_panel.cpp

SRC_IMGUI = vendor/imgui/imgui.cpp \
            vendor/imgui/imgui_draw.cpp \
            vendor/imgui/imgui_tables.cpp \
            vendor/imgui/imgui_widgets.cpp \
            vendor/imgui/imgui_impl_sdl2.cpp \
            vendor/imgui/imgui_impl_sdlrenderer2.cpp

OBJ_ALL = $(SRC_CORE:.cpp=.o) $(SRC_ECS:.cpp=.o) $(SRC_PHYSICS:.cpp=.o) \
          $(SRC_GRAPHICS:.cpp=.o) $(SRC_ASSETS:.cpp=.o) $(SRC_TILEMAP:.cpp=.o) \
          $(SRC_ANIMATION:.cpp=.o) $(SRC_AUDIO:.cpp=.o) $(SRC_SCENE:.cpp=.o) \
          $(SRC_OPTIMIZATION:.cpp=.o) $(SRC_FLOW:.cpp=.o) $(SRC_UI:.cpp=.o) \
          $(SRC_GAMEPLAY:.cpp=.o) $(SRC_PROJECT:.cpp=.o) $(SRC_PJ:.cpp=.o) $(SRC_BRIDGE:.cpp=.o)

OBJ_EDITOR = $(SRC_EDITOR:.cpp=.o) $(SRC_IMGUI:.cpp=.o)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

all: pj-studio test_engine pj

pj: $(SRC_PJ:.cpp=.o) src/pj/main_cli.o
	$(CXX) $^ -o $@

test_engine: $(OBJ_ALL) tests/test_engine.o
	$(CXX) $^ -o $@ $(LDFLAGS)

pj-game: $(OBJ_ALL) $(SRC_PLATFORM:.cpp=.o) src/main_game.o
	$(CXX) $^ -o $@ $(LDFLAGS)

pj-studio: $(OBJ_ALL) $(OBJ_EDITOR) src/main_editor.o
	$(CXX) $^ -o $@ $(LDFLAGS)

clean:
	rm -f pj test_engine pj-game pj-studio save_slot_*.dat
	rm -f $(OBJ_ALL) $(OBJ_EDITOR) $(SRC_PLATFORM:.cpp=.o) src/main_editor.o src/main_game.o tests/test_engine.o src/pj/main_cli.o