#ifndef LEADERBOARD_SCENE_H
#define LEADERBOARD_SCENE_H

#include "scene_manager.hpp"
#include "main_menu_scene-h.hpp"
#include "highscore.hpp"
#include <raylib.h>
#include <vector>

class LeaderboardScene : public Scene {
public:
    LeaderboardScene();

    void Begin() override;
    void End() override;
    void Update() override;
    void Draw() override;

private:
    Texture leaderboardbg;
    Music menu_theme = {0};
    bool musicLoaded = false;
    void UpdateVolumes();
    
    HighScoreManager highScoreManager;
};

#endif
