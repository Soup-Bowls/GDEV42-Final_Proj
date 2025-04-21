#ifndef TITLE_SCENE
#define TITLE_SCENE
#include <raylib.h>
#include <iostream>

#include "scene_manager.hpp"

class TitleScene : public Scene {

public:
    void Begin() override;
    void End() override;
    void Update() override;
    void Draw() override;

private:
    Texture eyeball;
    Texture cavebg;
    Music title_theme = {0};
    bool musicLoaded = false;
};

#endif