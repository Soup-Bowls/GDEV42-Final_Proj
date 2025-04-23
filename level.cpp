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

#define GAME_SCENE_MUSIC "Assets/Audio/Music/symphony.ogg"

const int WINDOW_WIDTH(1280);
const int WINDOW_HEIGHT(720);
const float FPS(60.0f);

Level::Level() : 
    game_ongoing(true),
    cam_drift(2.0f),
    current_wave(1),
    base_wave_points(5),
    wave_timer(0.0f),
    wave_delay(2.0f),
    wave_cleared(false),
    player(nullptr),
    music_loaded(false)
{
    // Initialize camera
    camera_view = {0};
    camera_view.offset = {WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2};
    camera_view.zoom = 2.0f;
}

Level::~Level() {
    End();
}

void Level::Begin() {
    // Initialize map
    map.LoadTilemapData("TileInfo.txt");
    
    // Initialize player
    if (player) delete player;
    player = new Player(map.playerPos, 15.0f, 150.0f, 100);
    player->setTileMap(&map);
    
    // Set camera focus on player
    camera_view.target = player->position;
    camera_window = {player->position.x - 150, player->position.y - 150, 300.0f, 300.0f};
    
    // Initialize enemies
    SpawnWave(current_wave);
    
    // Load music
    game_music = LoadMusicStream(GAME_SCENE_MUSIC);
    if (IsMusicReady(game_music)) {
        music_loaded = true;
        AudioManager::GetInstance()->SetCurrentMusic(game_music);
        PlayMusicStream(game_music);
    } else {
        std::cerr << "Failed to load game music" << std::endl;
    }
    
    // Reset game state
    game_ongoing = true;
}

void Level::End() {
    // Clean up enemies
    for (auto* e : enemies) {
        delete e;
    }
    enemies.clear();
    
    // Clean up player
    if (player) {
        delete player;
        player = nullptr;
    }
    
    // Unload music
    if (IsMusicReady(game_music)) {
        StopMusicStream(game_music);
        UnloadMusicStream(game_music);
        game_music = {0};
        music_loaded = false;
    }
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
        SceneManager* sceneManager = GetSceneManager();
        if (sceneManager) {
            sceneManager->SwitchScene(4); // Death scene
        }
    }
}

void Level::Update() {
    float delta_time = GetFrameTime();
    
    // Update music
    if (music_loaded && IsMusicReady(game_music)) {
        UpdateMusicStream(game_music);
        if (!IsMusicStreamPlaying(game_music)) {
            PlayMusicStream(game_music);
        }
    }

    if (game_ongoing) {
        // Update player
        player->Update(delta_time);
        
        // Update enemies
        for (auto* enemy : enemies) {
            if (enemy->active) {
                enemy->Update(delta_time);
            }
        }
        
        // Handle collisions
        HandleCollisions();
        
        // Check wave status
        CheckWaveStatus();
        
        // Check game status
        CheckGameStatus();
        
        // Update camera
        MoveCamera(delta_time);
    }

    // move to death scene
    if(player->health <= 0) {
        if(GetSceneManager() != nullptr) {
            GetSceneManager()->SwitchScene(4);
        }
    } 
    
    // Check for pause or exit
    if (IsKeyPressed(KEY_ESCAPE)) {
        SceneManager* sceneManager = GetSceneManager();
        if (sceneManager) {
            sceneManager->SwitchScene(1); // Main menu scene
        }
    }
}

void Level::Draw() {
    
    if (game_ongoing) {
        BeginMode2D(camera_view);
        ClearBackground(BLACK);

        // Draw world
        map.DrawTilemap();
        
        // Draw player
        player->Draw();
        
        // Draw enemies
        for (auto* enemy : enemies) {
            if (enemy->active) {
                enemy->Draw();
            }
        }
        
        EndMode2D();
        
        // Draw UI elements
        DrawText(TextFormat("Health: %d", player->health), 10, 10, 30, WHITE);
        DrawText(TextFormat("Wave: %d", current_wave), 10, 50, 30, YELLOW);
        DrawText(TextFormat("Position: %.0f %.0f", player->position.x, player->position.y), 10, 80, 30, YELLOW);
    } else {
        ClearBackground(BLACK);
        DrawText("GAME OVER", WINDOW_WIDTH / 4, WINDOW_HEIGHT / 2 - 25, 100, RED);
    }
    
}