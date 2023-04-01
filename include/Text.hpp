#pragma once 

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <vector>
#include "Entity.hpp"
#include "RenderWindow.hpp"

using std::vector;
using std::string;

class Text {
public:
    Text(float x, float y, int fontSize, string text, SDL_Texture* font);
    void render(RenderWindow window);
    float getX();
    float getY();
    float getW();
    float getH();

    void center(int screenWidth, int screenHeight);
    void setX(float newX);
    void setY(float newY);
    void setText(string newText);
    string getText();
private:
    int fontSize;
    vector<Entity> entities;
    SDL_Texture* font;
    float x;
    float y;
    float w;
    float h;
    string text;
};