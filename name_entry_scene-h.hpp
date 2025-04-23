#ifndef NAME_ENTRY_SCENE_H
#define NAME_ENTRY_SCENE_H

#include "scene_manager.hpp"
#include "HighScore.hpp"
#include <raylib.h>
#include <string>

class NameEntryScene : public Scene {
public:
    NameEntryScene();

    void Begin() override;
    void End() override;
    void Update() override;
    void Draw() override;
    
    void SetPlayerScore(int score);

private:
    Texture background;
    int playerScore;
    std::string playerName;
    int maxNameLength;
    int selectedChar;
    bool nameConfirmed;
    
    HighScoreManager highScoreManager;
    
    void HandleKeyboardInput();
};

#endif
