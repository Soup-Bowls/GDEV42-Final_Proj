#ifndef LEVEL_H
#define LEVEL_H

#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <string>
#include "scene_manager.hpp"
#include "Player.hpp"
#include "Bee.hpp"
#include "Ghost.hpp"
#include "Slime.hpp"
#include "TileMap.hpp"

class Level : public Scene {
public:
    Level();
    Level(int starting_wave);
    Level(int starting_wave, int starting_health);
    ~Level();

    void Begin() override;
    void End() override;
    void Update() override;
    void Draw() override;

private:
    // Game state
    bool game_ongoing;
    bool is_paused;
    bool should_exit_to_menu;
    
    // Camera
    Camera2D camera_view;
    Rectangle camera_window;
    float cam_drift;

    // Game entities
    Player* player;
    std::vector<BaseEnemy*> enemies;
    TileMap map;
    
    // Wave system
    int current_wave;
    int starting_player_health;
    int base_wave_points;
    float wave_timer;
    float wave_delay;
    bool wave_cleared;
    
    // Audio
    Music game_music;
    bool music_loaded;

    // Pause menu
    Rectangle continue_button;
    Rectangle main_menu_button;
    bool continue_hover;
    bool main_menu_hover;

    void MoveCamera(float delta_time);
    void SpawnWave(int wave_num);
    void CheckWaveStatus();
    void HandleCollisions();
    void CheckGameStatus();
    void HandlePauseMenu();
    void DrawPauseMenu();
};

#endif