#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>
using std::vector;

class Entity {
public:
    Entity(float xP, float yP, int destW, int destH, int srcW, int srcH, SDL_Texture* textureP, vector<vector<float>> vertexPoints = {});
    float getX();
    float getY();
    int getW();
    int getH();

    bool isOffScreen(int screenWidth, int screenHeight);

    void setX(float xP);
    void setY(float yP);

    int getAngle();
    void setAngle(int newAngle);

    bool isCollidingWith(Entity& otherEntity, vector<vector<vector<float>>>& testPoints);

    void wrap(int screenWidth, int screenHeight);

    void setTexture(SDL_Texture* newTexture);
    SDL_Texture* getTexture();
    SDL_Rect getCurrentFrame();
    void setCurrentFrame(int srcX, int srcY, int srcW, int srcH);

    vector<float> getCenter();

    vector<vector<float>> getPoints();
private:
    int angle;
    float x, y;
    int w, h;
    SDL_Rect currentFrame;
    SDL_Texture* texture;
    vector<vector<float>> vertexPoints;
};