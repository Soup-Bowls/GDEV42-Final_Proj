#include "leaderboard_scene-h.hpp"
#include "scene_manager.hpp"
#include "highscore.hpp"
#include <iostream>
#include <fstream>

LeaderboardScene::LeaderboardScene() {}

void LeaderboardScene::Begin() {
    TextureData leaderboardBackgroundData = ResourceManager::GetInstance()->GetTexture("leaderboard_background.png");
    leaderboardbg = leaderboardBackgroundData.texture;

    highScoreManager.LoadHighScores();

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
    DrawTexturePro(leaderboardbg, {0, 0, 2000, 2000}, {0,0,800,600}, {0,0}, 0.0f, BLUE);
    
    DrawText("LEADERBOARDS", 260, 80, 50, WHITE);
    
    DrawText("RANK", 100, 150, 25, YELLOW);
    DrawText("NAME", 220, 150, 25, YELLOW);
    DrawText("SCORE", 500, 150, 25, YELLOW);
    
    DrawLine(100, 180, 700, 180, WHITE);
    
    const auto& scores = highScoreManager.GetHighScores();
    
    if (scores.empty()) {
        DrawText("NO HIGH SCORES YET", 250, 250, 30, GRAY);
    } else {
        for (size_t i = 0; i < scores.size(); i++) {
            int y = 200 + i * 35;
            
            std::string rank = std::to_string(i + 1) + ".";
            DrawText(rank.c_str(), 110, y, 25, WHITE);
            
            DrawText(scores[i].playerName.c_str(), 220, y, 25, WHITE);
            
            DrawText(std::to_string(scores[i].score).c_str(), 510, y, 25, WHITE);
        }
    }
    
    DrawText("Press ENTER to return", 500, 550, 20, WHITE);
}
