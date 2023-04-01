#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "Entity.hpp"
#include <vector>
using std::vector;

class RenderWindow {
public:
    RenderWindow(const char* winTitle, int winWidth, int winHeight);
    SDL_Texture* loadTexture(const char* filePath);

    int getRefreshRate();

    void cleanUp();
    void clear();
    void render(Entity& entity, int angle = 0);
    void drawPoint(float x, float y, vector<int> color, int alpha, float h = 1, float w = 1);
    void drawRect(vector<float> rectP, vector<float> color, int alpha);
    void drawLine(float x1P, float y1P, float x2P, float y2P, vector<int> color, int alpha);
    void display();
    SDL_Window* getWindow();
    void setDebug(bool status);
    bool getDebug();
private:
    bool DEBUG = false;
    SDL_Window* window;
    SDL_Renderer* renderer;
};