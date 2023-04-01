#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>

#include "RenderWindow.hpp"
#include "Entity.hpp"
#include "Utils.hpp"

RenderWindow::RenderWindow(const char* winTitle, int winWidth, int winHeight) {
    window = NULL;
    renderer = NULL;

    window = SDL_CreateWindow(winTitle, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, winWidth, winHeight, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        std::cout << "Window failed to init. Error: " << SDL_GetError() << "\n";
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
}

SDL_Texture* RenderWindow::loadTexture(const char* filePath) {
    SDL_Texture* texture = NULL;
    texture = IMG_LoadTexture(renderer, filePath);
    if (texture == NULL) {
        std::cout << "Failed to load texture. Error: " << SDL_GetError() << "\n";
    }
    return texture;
}

int RenderWindow::getRefreshRate() {
    int display_index = SDL_GetWindowDisplayIndex(window);
    SDL_DisplayMode mode;
    SDL_GetDisplayMode(display_index, 0, &mode);
    return mode.refresh_rate;
}

void RenderWindow::clear() {
    SDL_RenderClear(renderer);
}

void RenderWindow::render(Entity& entity, int angle) {
    SDL_Rect src = entity.getCurrentFrame();

    SDL_Rect dest;
    dest.x = entity.getX();
    dest.y = entity.getY();
    dest.w = entity.getW();
    dest.h = entity.getH();

    // SDL_RenderCopy(renderer, entity.getTexture(), &src, &dest);
    SDL_RenderCopyEx(renderer, entity.getTexture(), &src, &dest, angle, NULL, SDL_FLIP_NONE);
    if (DEBUG) {
        vector<vector<float>> points = entity.getPoints();
        vector<vector<float>> newPoints = utils::rotatePoints(points, entity.getCenter(), -entity.getAngle());
        if (points.size() != 0) {
            for (int i = 0; i < points.size(); i++) {
                drawPoint(entity.getX() + newPoints[i][0] - 2, entity.getY() + newPoints[i][1] - 2, {255, 0, 0}, 255, 4, 4);
                drawLine(entity.getX() + newPoints[i][0], entity.getY() + newPoints[i][1], entity.getX() + newPoints[(i+1)%newPoints.size()][0], entity.getY() + newPoints[(i+1)%newPoints.size()][1], {255, 0, 0}, 255);
            }
        }
        drawRect({entity.getX(), entity.getY(), entity.getW(), entity.getH()}, {255, 0, 0}, 255);
    }
}

void RenderWindow::drawPoint(float xP, float yP, vector<int> color, int alpha, float hP, float wP) {
    SDL_SetRenderDrawColor(renderer, color[0], color[1], color[2], alpha);
    if (hP == wP && hP == 1) {
        SDL_RenderDrawPoint(renderer, xP, yP);
    } else {
        SDL_Rect rect = {xP, yP, wP, hP};
        SDL_RenderFillRect(renderer, &rect);
    }
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
}

void RenderWindow::drawLine(float x1P, float y1P, float x2P, float y2P, vector<int> color, int alpha) {
    SDL_SetRenderDrawColor(renderer, color[0], color[1], color[2], alpha);
    SDL_RenderDrawLine(renderer, x1P, y1P, x2P, y2P);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
}

void RenderWindow::display() {
    SDL_RenderPresent(renderer);
}

void RenderWindow::cleanUp() {
    SDL_DestroyWindow(window);
}

SDL_Window* RenderWindow::getWindow() {
    return window;
}

void RenderWindow::drawRect(vector<float> rectP, vector<float> color, int alpha) {
    SDL_Rect rect = {rectP[0], rectP[1], rectP[2], rectP[3]};
    SDL_SetRenderDrawColor(renderer, color[0], color[1], color[2], alpha);
    SDL_RenderDrawRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
}

void RenderWindow::setDebug(bool status) {
    DEBUG = status;
}

bool RenderWindow::getDebug() {
    return DEBUG;
}