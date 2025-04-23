#include <raylib.h>
#include <raymath.h>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>

#include "level-h.hpp"
#include "PlayerStateMachine.cpp"
#include "BeeStateMachine.cpp"
#include "slimeStateMachine.cpp"
#include "GhostStateMachine.cpp"
#include "projectile.cpp"
#include "TileMap.cpp"
#include "SaveSystem.hpp"

#define GAME_SCENE_MUSIC "Assets/Audio/Music/symphony.ogg"

const int WINDOW_WIDTH(1280);
const int WINDOW_HEIGHT(720);
const float FPS(60.0f);

Level::Level(int starting_wave, int starting_health) : 
    game_ongoing(true),
    is_paused(false),
    cam_drift(2.0f),
    current_wave(starting_wave),
    starting_player_health(starting_health),
    base_wave_points(5),
    wave_timer(0.0f),
    wave_delay(2.0f),
    wave_cleared(false),
    player(nullptr),
    music_loaded(false),
    continue_hover(false),
    main_menu_hover(false),
    should_exit_to_menu(false)
{
    camera_view = {0};
    camera_view.offset = {WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2};
    camera_view.zoom = 2.0f;
    
    continue_button = { WINDOW_WIDTH/2 - 100, WINDOW_HEIGHT/2 - 60, 200, 50 };
    main_menu_button = { WINDOW_WIDTH/2 - 100, WINDOW_HEIGHT/2 + 10, 200, 50 };
}

Level::Level(int starting_wave) : Level(starting_wave, 100) {
}

Level::Level() : Level(1, 100) {
}

Level::~Level() {
    End();
}

void Level::Begin() {
    map.LoadTilemapData("TileInfo.txt");
    
    if (player) delete player;
    player = new Player(map.playerPos, 15.0f, 150.0f, starting_player_health);
    player->setTileMap(&map);
    
    camera_view.target = player->position;
    camera_window = {player->position.x - 150, player->position.y - 150, 300.0f, 300.0f};
    
    SpawnWave(current_wave);
    
    std::cout << "Attempting to load game music" << std::endl;
    game_music = LoadMusicStream(GAME_SCENE_MUSIC);
    
    if (game_music.ctxData != nullptr) {
        std::cout << "Music loaded successfully" << std::endl;
        music_loaded = true;
        AudioManager::GetInstance()->SetCurrentMusic(game_music);
        PlayMusicStream(game_music);
    } else {
        std::cerr << "Failed to load game music at path: " << GAME_SCENE_MUSIC << std::endl;
        music_loaded = false;
    }
    
    game_ongoing = true;
    
    SaveSystem::GetInstance()->SaveGame(current_wave, player->health);
}

void Level::End() {
    std::cout << "Level::End() - Starting cleanup" << std::endl;

    for (auto* e : enemies) {
        if (e != nullptr) {
            delete e;
        } else {
        }
    }
    enemies.clear();
    
    if (player) {
        delete player;
        player = nullptr;
    }
       
    if (music_loaded && game_music.ctxData != nullptr) {
        StopMusicStream(game_music);
        UnloadMusicStream(game_music);
        
        game_music.ctxData = nullptr;
        game_music.stream = {0};
        game_music.frameCount = 0;
        
        music_loaded = false;
    }


    std::cout << "Level::End() - Cleanup completed" << std::endl;
}

void Level::MoveCamera(float delta_time) {
    if (!player) return;
    
    float cam_push_x = 0.0f;
    float cam_push_y = 0.0f;
    float drift_x = Clamp(player->position.x - (camera_window.x + camera_window.x + camera_window.width) / 2, -cam_drift, cam_drift);
    float drift_y = Clamp(player->position.y - (camera_window.y + camera_window.y + camera_window.height) / 2, -cam_drift, cam_drift);

    if (player->position.x > camera_window.x + camera_window.width) {
        cam_push_x = player->position.x - (camera_window.x + camera_window.width);
        camera_view.target.x += cam_push_x;
        camera_window.x += cam_push_x;
    } else if (player->position.x < camera_window.x) {
        cam_push_x = player->position.x - camera_window.x;
        camera_view.target.x += cam_push_x;
        camera_window.x += cam_push_x;
    } else {
        camera_view.target.x += drift_x;
        camera_window.x += drift_x;
    }

    if (player->position.y > camera_window.y + camera_window.height) {
        cam_push_y = player->position.y - (camera_window.y + camera_window.height);
        camera_view.target.y += cam_push_y;
        camera_window.y += cam_push_y;
    } else if (player->position.y < camera_window.y) {
        cam_push_y = player->position.y - camera_window.y;
        camera_view.target.y += cam_push_y;
        camera_window.y += cam_push_y;
    } else {
        camera_view.target.y += drift_y;
        camera_window.y += drift_y;
    }
}

void Level::SpawnWave(int wave_num) {
    for (auto* e : enemies) delete e;
    enemies.clear();

    int points = base_wave_points + wave_num * 3;

    while (points > 0) {
        int type = GetRandomValue(0, 2); // 0 = Slime(2pts), 1 = Ghost(1pt), 2 = Bee(3pts)
        int spawnLocation = GetRandomValue(0,2);
        Vector2 spawn;

        switch (spawnLocation) {
            case 0:
                spawn = {1220,180};
                break;
            case 1:
                spawn = {1220,550};
                break;
            case 2:
                spawn = {50,550};
                break;
        }

        if (type == 0 && points >= 2) {
            Slime* slime = new Slime(spawn, 50, 15, 100, 250, 15, 2);
            slime->setTileMap(&map);
            enemies.push_back(slime);
            points -= 2;
        } else if (type == 1 && points >= 1) {
            Ghost* ghost = new Ghost(spawn, 50, 15, 100, 250, 15, 2);
            ghost->setTileMap(&map);
            enemies.push_back(ghost);
            points -= 1;
        } else if (type == 2 && points >= 3) {
            Bee* bee = new Bee(spawn, 100, 15, 100, 250, 50, 2);
            bee->setTileMap(&map);
            enemies.push_back(bee);
            points -= 3;
        }
    }

    std::cout << "Wave " << wave_num << " spawned with " << enemies.size() << " enemies.\n";
}

void Level::CheckWaveStatus() {
    wave_cleared = true;
    
    for (auto* enemy : enemies) {
        if (enemy->active) {
            wave_cleared = false;
            break;
        }
    }

    if (wave_cleared) {
        wave_timer += GetFrameTime();
        if (wave_timer >= wave_delay) {
            current_wave++;
            SpawnWave(current_wave);
            wave_timer = 0.0f;
            
            // Save the current wave and player health when a new wave starts
            SaveSystem::GetInstance()->SaveGame(current_wave, player->health);
            std::cout << "Wave " << current_wave << " started - saved game with health " << player->health << std::endl;
        }
    }
}

void Level::HandleCollisions() {
    for (auto* enemy : enemies) {
        if (enemy->active) {
            enemy->HandleCollision(player);
            player->HandleCollision(enemy);
        }
    }
}

void Level::CheckGameStatus() {
    if (player->health <= 0) {
        game_ongoing = false;
        
        SaveSystem::GetInstance()->SaveGame(1, 100);
        
        SceneManager* sceneManager = GetSceneManager();
        if (sceneManager) {
            sceneManager->SwitchScene(4); // Death scene
        }
    }
}

void Level::HandlePauseMenu() {
    Vector2 mouse_pos = GetMousePosition();
    
    continue_hover = CheckCollisionPointRec(mouse_pos, continue_button);
    main_menu_hover = CheckCollisionPointRec(mouse_pos, main_menu_button);
    
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (continue_hover) {
            is_paused = false;
        } else if (main_menu_hover) {
            is_paused = false;
            should_exit_to_menu = true;
        }
    }
}

void Level::Update() {
    float delta_time = GetFrameTime();
    
    if (IsKeyPressed(KEY_P)) {
        is_paused = !is_paused;
    }
    
    if (music_loaded && IsMusicReady(game_music)) {
        UpdateMusicStream(game_music);
        if (!IsMusicStreamPlaying(game_music)) {
            PlayMusicStream(game_music);
        }
    }

    if (is_paused) {
        HandlePauseMenu();
        return; 
    }

    if (game_ongoing) {
        player->Update(delta_time);
        
        for (auto* enemy : enemies) {
            if (enemy->active) {
                enemy->Update(delta_time);
            }
        }
        
        HandleCollisions();
        
        CheckWaveStatus();
        
        CheckGameStatus();

        MoveCamera(delta_time);
    }
    
    if (should_exit_to_menu) {
        should_exit_to_menu = false; 
        game_ongoing = false;
        
        SaveData currentSave = SaveSystem::GetInstance()->LoadGame();
        SaveSystem::GetInstance()->SaveGame(current_wave, currentSave.playerHealth); //Save wave, not health
        
        SceneManager* sceneManager = GetSceneManager();
        if (sceneManager) {
            sceneManager->SwitchScene(1); // Main menu
        }
    }
}

void Level::DrawPauseMenu() {
    DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, Fade(WHITE, 0.7f));
    
    DrawText("PAUSED", WINDOW_WIDTH/2 - MeasureText("PAUSED", 60)/2, WINDOW_HEIGHT/2 - 150, 60, MAROON);
    
    Color continue_color = continue_hover ? MAROON : RED;
    DrawRectangleRec(continue_button, continue_color);
    DrawRectangleLinesEx(continue_button, 2, MAROON);
    DrawText("CONTINUE", 
             continue_button.x + continue_button.width/2 - MeasureText("CONTINUE", 20)/2, 
             continue_button.y + continue_button.height/2 - 10, 
             20, WHITE);
    
    Color menu_color = main_menu_hover ? MAROON : RED;
    DrawRectangleRec(main_menu_button, menu_color);
    DrawRectangleLinesEx(main_menu_button, 2, MAROON);
    DrawText("MAIN MENU", 
             main_menu_button.x + main_menu_button.width/2 - MeasureText("MAIN MENU", 20)/2, 
             main_menu_button.y + main_menu_button.height/2 - 10, 
             20, WHITE);
}

void Level::Draw() {
    ClearBackground(BLACK);
    
    if (game_ongoing) {
        BeginMode2D(camera_view);

        map.DrawTilemap();
        
        player->Draw();
        
        for (auto* enemy : enemies) {
            if (enemy->active) {
                enemy->Draw();
            }
        }
        
        EndMode2D();
        
        DrawText(TextFormat("Health: %d", player->health), 10, 10, 30, WHITE);
        DrawText(TextFormat("Wave: %d", current_wave), 10, 50, 30, YELLOW);
        DrawText(TextFormat("Position: %.0f %.0f", player->position.x, player->position.y), 10, 80, 30, YELLOW);
        
        DrawText("Press P to pause", WINDOW_WIDTH - 200, 10, 20, WHITE);
        
        if (is_paused) {
            DrawPauseMenu();
        }
    } else {
        DrawText("GAME OVER", WINDOW_WIDTH / 4, WINDOW_HEIGHT / 2 - 25, 100, RED);
    }
}