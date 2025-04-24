#include <raylib.h>
#include "title_scene.cpp"
#include "main_menu.cpp"
#include "game_scene.cpp"
#include "settings_scene.cpp"
#include "death_scene.cpp"
#include "leaderboard_scene.cpp"
#include "level.cpp"

#include "scene_manager.hpp"
#include "title_scene-h.hpp"
#include "main_menu_scene-h.hpp"
#include "game_scene-h.hpp"
#include "settings_scene-h.hpp"
#include "death_scene-h.hpp"
#include "leaderboard_scene-h.hpp"
#include "level-h.hpp"
#include <iostream>

int main() {
    InitAudioDevice();

    InitWindow(1280, 720, "Final Project Mesa Reyes Ruiz");
    SetTargetFPS(60);

    SceneManager scene_manager;

    TitleScene title_scene;
    title_scene.SetSceneManager(&scene_manager);

    MainMenu main_menu_scene;
    main_menu_scene.SetSceneManager(&scene_manager);

    GameScene game_scene;
    game_scene.SetSceneManager(&scene_manager);

    SettingsScene settings_scene;
    settings_scene.SetSceneManager(&scene_manager);

    DeathScene death_scene;
    death_scene.SetSceneManager(&scene_manager);

    LeaderboardScene leaderboard_scene;
    leaderboard_scene.SetSceneManager(&scene_manager);

    Level level_scene;
    level_scene.SetSceneManager(&scene_manager);

    scene_manager.RegisterScene(&title_scene, 0);
    scene_manager.RegisterScene(&main_menu_scene, 1);
    scene_manager.RegisterScene(&game_scene, 2);
    scene_manager.RegisterScene(&settings_scene, 3);
    scene_manager.RegisterScene(&death_scene, 4);
    scene_manager.RegisterScene(&leaderboard_scene, 5);
    scene_manager.RegisterScene(&level_scene, 6);

    scene_manager.SwitchScene(0);

    while(!WindowShouldClose()) {
        Scene* active_scene = scene_manager.GetActiveScene();

        if (active_scene != nullptr) {
            //std::cout << "Updating scene" << std::endl;
            active_scene->Update();
        }

        if (scene_manager.ShouldExit()) {
        std::cout << "CLOSING APPLICATION" << std::endl; 
        break;  // Exit the game loop
        }


        BeginDrawing();
        ClearBackground(WHITE);

        if (active_scene != nullptr) {
            //std::cout << "Drawing scene" << std::endl;
            active_scene->Draw();
        }

        EndDrawing();
    }

    Scene* active_scene = scene_manager.GetActiveScene();
    if (active_scene != nullptr) {
        std::cout << "ENDING SCENE" << std::endl;        
        active_scene->End();
    }

    ResourceManager::GetInstance()->UnloadAllTextures();
    
    CloseAudioDevice();

    CloseWindow();
    return 0;
}
