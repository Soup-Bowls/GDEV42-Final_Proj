#ifndef MAIN_MENU_SCENE_H
#define MAIN_MENU_SCENE_H

#include <raylib.h>
#include <vector>
#include <functional>
#include "scene_manager.hpp"

// Forward declaration
class MenuButton;

// Base UI Component
struct UIComponent {
    Rectangle bounds;
    virtual void Draw() = 0;
    virtual bool HandleClick(Vector2 click_position) = 0;
    virtual ~UIComponent() = default;
};

class MainMenu : public Scene {
public:
    MainMenu();
    ~MainMenu();

    void Begin() override;
    void End() override;
    void Update() override;
    void Draw() override;

private:
    std::vector<UIComponent*> buttons;
    Texture backgroundTexture;
    Music menu_theme = {0};
    bool musicLoaded = false;

    void InitializeButtons();
};

// MenuButton definition moved to cpp or another header
class MenuButton : public UIComponent {
public:
    MenuButton(Rectangle bounds, const char* text, Color normalColor, 
               Color hoverColor, Color textColor, 
               std::function<void()> action);

    void Draw() override;
    bool HandleClick(Vector2 click_position) override;

private:
    const char* text;
    Color normalColor;
    Color hoverColor;
    Color textColor;
    bool isHovered = false;
    std::function<void()> action;
};

#endif