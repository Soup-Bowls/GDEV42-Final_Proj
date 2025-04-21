#include "leaderboard_scene-h.hpp"
#include "scene_manager.hpp"
#include <iostream>
#include <fstream>

LeaderboardScene::LeaderboardScene() {}

void LeaderboardScene::Begin() {
    TextureData leaderboardBackgroundData = ResourceManager::GetInstance()->GetTexture("leaderboard_background.png");
    leaderboardbg = leaderboardBackgroundData.texture;

    std::ifstream infile("result.txt");
    infile >> Highscore;

    if (!IsMusicReady(menu_theme)) {
        menu_theme = LoadMusicStream("menu_theme.ogg");
    }
    
    if (IsMusicReady(menu_theme)) {
        musicLoaded = true;
        AudioManager::GetInstance()->SetCurrentMusic(menu_theme);
        PlayMusicStream(menu_theme);
    } else {
        std::cerr << "Failed to load menu theme music" << std::endl;
    }
}

void LeaderboardScene::End() {
    if (IsMusicReady(menu_theme)) {
        StopMusicStream(menu_theme);
        UnloadMusicStream(menu_theme);
        
        menu_theme = {0};
        musicLoaded = false;
    }   
}

void LeaderboardScene::Update() {
    if (IsKeyPressed(KEY_ENTER)) {
        if (GetSceneManager() != nullptr) {
            GetSceneManager()->SwitchScene(1);
        }
    }

    if (musicLoaded && IsMusicReady(menu_theme)) {
        UpdateMusicStream(menu_theme);

        if (!IsMusicStreamPlaying(menu_theme)) {
            PlayMusicStream(menu_theme);
        }
    }
}

void LeaderboardScene::Draw() {
    ClearBackground(BLACK);
    DrawTexturePro(leaderboardbg, {0, 0, 2000, 2000}, {0,0,800,600}, {0,0},  0.0f,  BLUE);
    DrawText("LEADERBOARDS: ", 30, 50, 40, WHITE);
    DrawText(std::to_string(Highscore).c_str(), 30, 100, 40, WHITE);
    DrawText("Press ENTER to return", 500, 550, 20, WHITE);
}