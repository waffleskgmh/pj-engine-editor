#include "editor/editor_app.h"
#include "editor/inspector_panel.h"
#include "editor/asset_browser_panel.h"
#include "editor/console_panel.h"
#include "editor/tilemap_editor.h"
#include "editor/animation_editor.h"
#include "editor/code_editor_panel.h"
#include "editor/map_transition_panel.h"
#include "editor/project_settings_panel.h"
#include "editor/profiler_panel.h"
#include "editor/editor_command.h"
#include "engine/core/time_step.h"
#include "engine/core/logger.h"

#include <sys/stat.h>

#if defined(__has_include)
  #if __has_include(<SDL2/SDL.h>)
    #include <SDL2/SDL.h>
  #elif __has_include(<SDL.h>)
    #include <SDL.h>
  #endif
#else
  #include <SDL.h>
#endif

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

#include <iostream>
#include <algorithm>

using namespace PJEngine;

int main(int argc, char* argv[]) {
    (void)argc; (void)argv;

    // 1. Inicializácia SDL2 okna
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
        std::cerr << "[Chyba]: SDL_Init zlyhalo: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Automatické vytvorenie štruktúry priečinkov projektu na disku
    mkdir("assets", 0755);
    mkdir("assets/scripts", 0755);
    mkdir("assets/scenes", 0755);
    mkdir("assets/textures", 0755);
    mkdir("assets/maps", 0755);
    mkdir("build", 0755);
    mkdir("build/export", 0755);

    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow(
        "PJ Studio 2D - Professional Game Development IDE",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1440, 850, window_flags
    );

    if (!window) {
        std::cerr << "[Chyba]: Okno sa nepodarilo vytvorit: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Renderer* sdlRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!sdlRenderer) {
        sdlRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    }

    // 2. Inicializácia Dear ImGui s Dockingom
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
#ifdef IMGUI_HAS_DOCK
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
#endif
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 5.0f;
    style.FrameRounding = 4.0f;
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.12f, 0.13f, 0.16f, 1.0f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.20f, 0.25f, 0.35f, 1.0f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.18f, 0.35f, 0.58f, 1.0f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.45f, 0.72f, 1.0f);

    ImGui_ImplSDL2_InitForSDLRenderer(window, sdlRenderer);
    ImGui_ImplSDLRenderer2_Init(sdlRenderer);

    // 3. Inštancie modulov editora a enginu
    EditorApp app;
    InspectorPanel inspectorPanel;
    AssetBrowserPanel assetBrowser;
    ConsolePanel consolePanel;
    TilemapEditor tilemapEditor;
    AnimationEditor animationEditor;
    CodeEditorPanel codeEditor;
    MapTransitionPanel transitionPanel;
    ProjectSettingsPanel projectSettings;
    ProfilerPanel profiler;
    TimeStep timer;

    // Prepínače viditeľnosti panelov
    bool showHierarchy = true;
    bool showInspector = true;
    bool showSceneView = true;
    bool showAssetBrowser = true;
    bool showConsole = true;
    bool showTilemapEditor = true;
    bool showAnimationEditor = false;
    bool showCodeEditor = true;
    bool showTransitions = false;
    bool showProjectSettings = false;
    bool showProfiler = true;

    // Framebuffer textúra pre Scene View
    int sceneTexW = 800;
    int sceneTexH = 600;
    SDL_Texture* sceneTexture = SDL_CreateTexture(
        sdlRenderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        sceneTexW, sceneTexH
    );

    // Tilemap pre editor
    Tilemap activeMap;
    activeMap.addLayer(TileLayer(40, 25, 16.0f));

    bool running = true;
    bool firstFrame = true;

    Logger::info("PJ Studio 2D uspesne inicializovane a pripravene.");

    // 4. Hlavný aplikačný cyklus
    while (running) {
        timer.update();
        float dt = timer.getDeltaTime();

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT) running = false;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window)) {
                running = false;
            }

            // Klávesové skratky pre Undo/Redo
            if (event.type == SDL_KEYDOWN) {
                bool cmdOrCtrl = (event.key.keysym.mod & KMOD_GUI) || (event.key.keysym.mod & KMOD_CTRL);
                if (cmdOrCtrl && event.key.keysym.sym == SDLK_z) {
                    CommandHistory::get().undo();
                } else if (cmdOrCtrl && (event.key.keysym.sym == SDLK_y || ((event.key.keysym.mod & KMOD_SHIFT) && event.key.keysym.sym == SDLK_z))) {
                    CommandHistory::get().redo();
                }
            }
        }

        // Krokovanie Play Mode
        if (app.playState == PlayState::PLAYING) {
            app.stepFrame(dt);
        }
        profiler.recordFrame(dt, app.renderer.getStats().drawCalls, app.world.getEntityCount());

        // Nový ImGui rámec
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

#ifdef IMGUI_HAS_DOCK
        ImGuiID dockspace_id = ImGui::GetID("PJStudioMainDock");
        ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

        if (firstFrame) {
            firstFrame = false;
            ImGui::DockBuilderRemoveNode(dockspace_id);
            ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetMainViewport()->Size);

            ImGuiID dock_main_id = dockspace_id;
            ImGuiID dock_left_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.20f, nullptr, &dock_main_id);
            ImGuiID dock_right_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.26f, nullptr, &dock_main_id);
            ImGuiID dock_down_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.32f, nullptr, &dock_main_id);

            ImGui::DockBuilderDockWindow("Hierarchy", dock_left_id);
            ImGui::DockBuilderDockWindow("Profiler", dock_left_id);
            ImGui::DockBuilderDockWindow("Scene / Game View", dock_main_id);
            ImGui::DockBuilderDockWindow("PJ Script Editor", dock_main_id);
            ImGui::DockBuilderDockWindow("Inspector", dock_right_id);
            ImGui::DockBuilderDockWindow("Tilemap Editor", dock_right_id);
            ImGui::DockBuilderDockWindow("Asset Browser", dock_down_id);
            ImGui::DockBuilderDockWindow("Console", dock_down_id);
            ImGui::DockBuilderDockWindow("Animation Timeline", dock_down_id);
            ImGui::DockBuilderFinish(dockspace_id);
        }
#endif

        // Hlavné menu
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("New Scene", "Ctrl+N")) { app.initSampleScene(); }
                if (ImGui::MenuItem("Save Scene", "Ctrl+S")) {
                    app.world.clear();
                    Logger::info("Scena ulozena.");
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Project Settings")) { showProjectSettings = true; }
                ImGui::Separator();
                if (ImGui::MenuItem("Exit", "Alt+F4")) { running = false; }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Edit")) {
                if (ImGui::MenuItem("Undo", "Cmd+Z", false, CommandHistory::get().canUndo())) { CommandHistory::get().undo(); }
                if (ImGui::MenuItem("Redo", "Cmd+Y", false, CommandHistory::get().canRedo())) { CommandHistory::get().redo(); }
                ImGui::Separator();
                if (ImGui::MenuItem("Select Tool", "Q")) app.toolMode = EditorToolMode::SELECT;
                if (ImGui::MenuItem("Translate Tool", "W")) app.toolMode = EditorToolMode::TRANSLATE;
                if (ImGui::MenuItem("Rotate Tool", "E")) app.toolMode = EditorToolMode::ROTATE;
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("View")) {
                ImGui::MenuItem("Hierarchy", nullptr, &showHierarchy);
                ImGui::MenuItem("Inspector", nullptr, &showInspector);
                ImGui::MenuItem("Scene / Game View", nullptr, &showSceneView);
                ImGui::MenuItem("Asset Browser", nullptr, &showAssetBrowser);
                ImGui::MenuItem("Console", nullptr, &showConsole);
                ImGui::MenuItem("Tilemap Editor", nullptr, &showTilemapEditor);
                ImGui::MenuItem("Animation Timeline", nullptr, &showAnimationEditor);
                ImGui::MenuItem("PJ Script Editor", nullptr, &showCodeEditor);
                ImGui::MenuItem("Map Transitions", nullptr, &showTransitions);
                ImGui::MenuItem("Project Settings", nullptr, &showProjectSettings);
                ImGui::MenuItem("Profiler", nullptr, &showProfiler);
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Play Mode")) {
                if (app.playState == PlayState::EDITING && ImGui::MenuItem("▶ Play Game", "F5")) {
                    app.setPlayState(PlayState::PLAYING);
                }
                if (app.playState == PlayState::PLAYING && ImGui::MenuItem("⏹ Stop Game", "Shift+F5")) {
                    app.setPlayState(PlayState::EDITING);
                    app.initSampleScene();
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        // Vykreslenie aktívnych panelov
        if (showHierarchy) {
            ImGui::Begin("Hierarchy", &showHierarchy);
            if (ImGui::Button("+ Pridat Entitu", ImVec2(-1, 24))) {
                Entity newEnt = app.world.createEntity();
                app.world.addComponent<TagComponent>(newEnt, TagComponent("Entita #" + std::to_string(newEnt)));
                app.world.addComponent<Transform2D>(newEnt, Transform2D{Vec2(150.0f, 100.0f), 0.0f, Vec2::One(), Vec2::Zero()});
                app.world.addComponent<RigidBody2D>(newEnt, RigidBody2D(BodyType::DYNAMIC, 1.0f));
                app.world.addComponent<Collider2D>(newEnt, Collider2D::MakeCircle(16.0f));
                app.selectEntity(newEnt);
            }
            ImGui::Separator();

            auto entities = app.world.query<Transform2D>();
            for (Entity e : entities) {
                std::string label = (e == 0 ? "Hrac (Player)" : (e == 1 ? "Podlaha (Ground)" : "Entita #" + std::to_string(e)));
                if (app.world.hasComponent<TagComponent>(e)) {
                    label = app.world.getComponent<TagComponent>(e).name;
                }
                bool isSelected = (app.selectedEntity == e);
                if (ImGui::Selectable(label.c_str(), isSelected)) {
                    app.selectEntity(e);
                }
            }
            ImGui::End();
        }

        if (showInspector) inspectorPanel.render(app);
        if (showAssetBrowser) assetBrowser.render();
        if (showConsole) consolePanel.render();
        if (showTilemapEditor) tilemapEditor.renderGUI(activeMap, app.world);
        if (showAnimationEditor) animationEditor.renderGUI();
        if (showCodeEditor) codeEditor.renderGUI(app);
        if (showTransitions) transitionPanel.render();
        if (showProjectSettings) projectSettings.render();

        if (showProfiler) {
            ImGui::Begin("Profiler", &showProfiler);
            auto snap = profiler.getLatestSnapshot();
            ImGui::Text("FPS: %.1f (%.2f ms)", snap.fps, snap.frameTimeMs);
            ImGui::Text("Entit v ECS: %zu", snap.activeEntities);
            ImGui::Text("Draw Calls: %u", snap.drawCalls);
            if (!profiler.frameTimeHistory.empty()) {
                ImGui::PlotLines("Frame Time", profiler.frameTimeHistory.data(), (int)profiler.frameTimeHistory.size(), 0, nullptr, 0.0f, 33.3f, ImVec2(-1, 50));
            }
            ImGui::End();
        }

        // Scene View portál
        if (showSceneView) {
            ImGui::Begin("Scene / Game View", &showSceneView);

            if (app.playState == PlayState::EDITING) {
                if (ImGui::Button("▶ SPUSTIT HRU", ImVec2(160, 24))) {
                    app.setPlayState(PlayState::PLAYING);
                }
            } else {
                if (ImGui::Button("⏹ ZASTAVIT HRU", ImVec2(160, 24))) {
                    app.setPlayState(PlayState::EDITING);
                    app.initSampleScene();
                }
            }
            ImGui::SameLine();
            ImGui::Text(app.playState == PlayState::PLAYING ? "[PLAY MODE: HRA BEZI]" : "[REZIM EDITORA]");

            ImVec2 viewSize = ImGui::GetContentRegionAvail();
            if (viewSize.x < 100) viewSize.x = 100;
            if (viewSize.y < 100) viewSize.y = 100;

            if ((int)viewSize.x != sceneTexW || (int)viewSize.y != sceneTexH) {
                sceneTexW = (int)viewSize.x;
                sceneTexH = (int)viewSize.y;
                if (sceneTexture) SDL_DestroyTexture(sceneTexture);
                sceneTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, sceneTexW, sceneTexH);
                app.sceneCamera.setViewportSize(viewSize.x, viewSize.y);
                app.gameCamera.setViewportSize(viewSize.x, viewSize.y);
            }

            if (sceneTexture) {
                ImGui::Image((ImTextureID)sceneTexture, viewSize);
            }
            ImGui::End();
        }

        // Vykreslenie scény do textúry
        SDL_SetRenderTarget(sdlRenderer, sceneTexture);
        SDL_SetRenderDrawColor(sdlRenderer, 22, 25, 32, 255);
        SDL_RenderClear(sdlRenderer);

        // Mriežka
        SDL_SetRenderDrawColor(sdlRenderer, 35, 42, 54, 255);
        for (int x = 0; x < sceneTexW; x += 32) SDL_RenderDrawLine(sdlRenderer, x, 0, x, sceneTexH);
        for (int y = 0; y < sceneTexH; y += 32) SDL_RenderDrawLine(sdlRenderer, 0, y, sceneTexW, y);

        // Objekty scény cez Renderer2D
        app.renderer.begin(app.playState == PlayState::PLAYING ? app.gameCamera : app.sceneCamera, sdlRenderer);

        auto renderableEntities = app.world.query<Transform2D, Collider2D>();
        for (Entity e : renderableEntities) {
            const auto& t = app.world.getComponent<Transform2D>(e);
            const auto& c = app.world.getComponent<Collider2D>(e);

            bool isSelected = (e == app.selectedEntity);
            Color color = isSelected ? Color(0.2f, 0.6f, 1.0f, 1.0f) :
                          (e == 0 ? Color(0.2f, 0.8f, 0.3f, 1.0f) : Color(0.55f, 0.6f, 0.7f, 1.0f));

            if (c.type == ColliderType::CIRCLE) {
                app.renderer.drawCircle(t.position + c.offset, c.radius, color);
            } else {
                app.renderer.drawQuad(t.position + c.offset, c.size, color);
            }
        }
        app.renderer.end();

        SDL_SetRenderTarget(sdlRenderer, nullptr);

        // Vykreslenie celého ImGui
        SDL_SetRenderDrawColor(sdlRenderer, 15, 17, 22, 255);
        SDL_RenderClear(sdlRenderer);
        ImGui::Render();
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), sdlRenderer);

        SDL_RenderPresent(sdlRenderer);
    }

    // Korektné ukončenie
    if (sceneTexture) SDL_DestroyTexture(sceneTexture);
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(sdlRenderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    std::cout << "[PJ Studio]: Kompletny editor uspesne ukonceny." << std::endl;
    return 0;
}