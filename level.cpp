
#include "level-h.hpp"
#include "PlayerStateMachine.cpp"
#include "EnemyStateMachine.cpp"
#include "tile_map.cpp"
#include <iostream>

Level::Level() : 
    cam_drift(0.5f),
    cam_zoom(1.0f),
    zoom_in(false),
    game_ongoing(true),
    enemy_lose(false),
    map(nullptr),
    player(nullptr)
{
    camera_window = {(WINDOW_WIDTH / 2) - 150, (WINDOW_HEIGHT / 2) - 150, 300.0f, 300.0f};
}

Level::~Level() {
    // Resources will be cleaned up in End()
}

//temporary fix to bypass state machine
void SafeUpdateEnemy(Enemy& enemy, float delta_time) {
    float dx = GetRandomValue(-10, 10) / 10.0f;
    float dy = GetRandomValue(-10, 10) / 10.0f;
    
    enemy.position.x += dx * delta_time * 50.0f;
    enemy.position.y += dy * delta_time * 50.0f;
    
    if (enemy.invulnerable_timer > 0.0f) {
        enemy.invulnerable_timer -= delta_time;
    }
}

void Level::Begin() {
    std::cout << "Level::Begin() - START" << std::endl;

    map = new TileMap();
    
    map->LoadTilemapData("TileInfo.txt", ResourceManager::GetInstance());
    
    player = new Player(map->playerPos, 15.0f, 150.0f, 5);
    player->setTileMap(map);
    
    // Initialize the enemies
    Enemy enemy01(map->enemyPos, 100.0f, 15.0f, 100.0f, 250.0f, 50.0f, 2);
    Enemy enemy02(map->enemyPos2, 100.0f, 15.0f, 100.0f, 250.0f, 50.0f, 2);
    Enemy enemy03(map->enemyPos3, 100.0f, 15.0f, 100.0f, 250.0f, 50.0f, 2);
    
    enemy01.setTileMap(map);
    enemy02.setTileMap(map);
    enemy03.setTileMap(map);
    
    enemies.clear();
    enemies.push_back(enemy01);
    enemies.push_back(enemy02);
    enemies.push_back(enemy03);
    
    // Initialize camera
    camera_window = {player->position.x - 150, player->position.y - 150, 300.0f, 300.0f};
    
    camera_view = {0};
    camera_view.target = player->position;
    camera_view.offset = {WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2};
    camera_view.zoom = 1.0f;
    
    // Reset game state
    game_ongoing = true;
    enemy_lose = false;
    
    std::cout << "Level::Begin() - COMPLETED SUCCESSFULLY" << std::endl;
}

void Level::End() {
    std::cout << "Level::End() called" << std::endl;
    
    // clean up
    if (map) {
        delete map;
        map = nullptr;
    }
    
    if (player) {
        delete player;
        player = nullptr;
    }
    
    enemies.clear();
}

void Level::Update() {
    std::cout << "Level::Update() - START" << std::endl;
    
    float delta_time = GetFrameTime();
    std::cout << "Delta time: " << delta_time << std::endl;
    
    std::cout << "About to check game_ongoing and player..." << std::endl;
    if (game_ongoing && player) {
        std::cout << "Game is ongoing, updating player and enemies..." << std::endl;
        
        enemy_lose = true;
        std::cout << "About to update player..." << std::endl;
        player->Update(delta_time);
        std::cout << "Player updated" << std::endl;
        
        std::cout << "About to update enemies, count: " << enemies.size() << std::endl;
        for (size_t i = 0; i < enemies.size(); i++) {
            std::cout << "Checking enemy " << i << ", active: " << (enemies[i].active ? "yes" : "no") << std::endl;
            if (enemies[i].active) {
                enemy_lose = false;
                std::cout << "Enemy " << i << " has tile_map: " << (enemies[i].tile_map != nullptr ? "yes" : "no") << std::endl;
                
                // safe update instead of normal update
                SafeUpdateEnemy(enemies[i], delta_time);
                std::cout << "Enemy " << i << " safely updated" << std::endl;
                
                // collision handling crashes the game too
                // enemies[i].HandleCollision(player);
                // player->HandleCollision(&enemies[i]);
            }
}
        
        if (enemy_lose) {
            game_ongoing = false;
        }

        if (player->health <= 0) {
            game_ongoing = false;
            if (GetSceneManager() != nullptr) {
                std::cout << "Switching to death scene..." << std::endl;
                GetSceneManager()->SwitchScene(4);
            }
        }
    }
    
    MoveCamera(&camera_view, player, delta_time);
    
    std::cout << "Level::Update() - END" << std::endl;
}

void Level::Draw() {
    ClearBackground(BLACK);
    
    BeginMode2D(camera_view);
    
    if (map) {
        map->DrawTilemap();
    }
    
    if (game_ongoing && player) {
        player->Draw();
        for (size_t i = 0; i < enemies.size(); i++) {
            if (enemies[i].active) {
                enemies[i].Draw();
            } 
        }
    }
    
    EndMode2D();
    
    if (game_ongoing && player) {
        DrawText(std::to_string(player->health).c_str(), 10, 10, 50, WHITE);
    } else {
        if (enemy_lose) {
            ClearBackground(BLACK);
            DrawText("YOU WIN", WINDOW_WIDTH / 4, WINDOW_HEIGHT / 2 - 25, 100, WHITE);
            DrawText("Press ENTER to continue", WINDOW_WIDTH / 4, WINDOW_HEIGHT / 2 + 100, 20, WHITE);
            
            if (IsKeyPressed(KEY_ENTER)) {
                if (GetSceneManager() != nullptr) {
                    GetSceneManager()->SwitchScene(5); // Go to leaderboard or another scene
                }
            }
        } else if (!game_ongoing) {
            ClearBackground(BLACK);
            DrawText("YOU LOSE", WINDOW_WIDTH / 4, WINDOW_HEIGHT / 2 - 25, 100, WHITE);
            DrawText("Press ENTER to continue", WINDOW_WIDTH / 4, WINDOW_HEIGHT / 2 + 100, 20, WHITE);
            
            if (IsKeyPressed(KEY_ENTER)) {
                if (GetSceneManager() != nullptr) {
                    GetSceneManager()->SwitchScene(4); // Go to death scene
                }
            }
        }
    }
    
    // Draw escape instruction
    DrawText("Press ESC to return to main menu", 10, WINDOW_HEIGHT - 30, 20, WHITE);
}

void Level::MoveCamera(Camera2D* cam, Player* player, float delta_time) {
    if (!player) return;
    
    float cam_push_x = 0.0f;
    float cam_push_y = 0.0f;
    float drift_x = Clamp(player->position.x - (camera_window.x + camera_window.x + camera_window.width) / 2, -cam_drift, cam_drift);
    float drift_y = Clamp(player->position.y - (camera_window.y + camera_window.y + camera_window.height) / 2, -cam_drift, cam_drift);

    if (player->position.x > camera_window.x + camera_window.width) {
        cam_push_x = player->position.x - (camera_window.x + camera_window.width);
        cam->target.x += cam_push_x;
        camera_window.x += cam_push_x;
    }
    else if (player->position.x < camera_window.x) {
        cam_push_x = player->position.x - camera_window.x;
        cam->target.x += cam_push_x;
        camera_window.x += cam_push_x;
    }
    else {
        cam->target.x += drift_x;
        camera_window.x += drift_x;
    }

    if (player->position.y > camera_window.y + camera_window.height) {
        cam_push_y = player->position.y - (camera_window.y + camera_window.height);
        cam->target.y += cam_push_y;
        camera_window.y += cam_push_y;
    }
    else if (player->position.y < camera_window.y) {
        cam_push_y = player->position.y - camera_window.y;
        cam->target.y += cam_push_y;
        camera_window.y += cam_push_y;
    }
    else {
        cam->target.y += drift_y;
        camera_window.y += drift_y;
    }
}