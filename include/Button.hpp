#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include "Text.hpp"

using std::vector;

class Button {
public:
    // pos = {x, y, w, h}
    Button(Entity *buttonEntityP, void (*func)(float, float));
    void click(float x, float y);
    Entity getEntity();
    bool isClicked(float x, float y);
private:
    float x;
    float y;
    float w;
    float h;
    // SDL_Texture* buttonSprite;
    Entity* buttonEntity;
    void (*func)(float, float);
};