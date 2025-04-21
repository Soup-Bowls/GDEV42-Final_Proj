#ifndef DEATH_SCENE_H
#define DEATH_SCENE_H

#include "scene_manager.hpp"
#include <raylib.h>
#include <vector>

class DeathScene : public Scene {
public:
    DeathScene();

    void Begin() override;
    void End() override;
    void Update() override;
    void Draw() override;

private:
    Texture deathbg;
    Music death_theme = {0};
    bool musicLoaded = false;
    void UpdateVolumes();
};


#endif