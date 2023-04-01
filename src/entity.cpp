#include "Entity.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include <algorithm> // for min and max

#include "Utils.hpp"

using std::min;
using std::max;

using std::vector;

Entity::Entity(float xP, float yP, int destW, int destH, int srcW, int srcH, SDL_Texture* textureP, vector<vector<float>> vertextPointsP) {
    x = xP;
    y = yP;
    w = destW;
    h = destH;
    texture = textureP;

    currentFrame.x = 0;
    currentFrame.y = 0;
    currentFrame.w = srcW;
    currentFrame.h = srcH;
    vertexPoints = vertextPointsP;
}

vector<float> Entity::getCenter() {
    vector<float> center = {w / 2, h / 2};
    return center;
}

float Entity::getX() {
    return x;
}

float Entity::getY() {
    return y;
}

int Entity::getW() {
    return w;
}

int Entity::getH() {
    return h;
}

void Entity::setX(float xP) {
    x = xP;
}

void Entity::setY(float yP) {
    y = yP;
}

bool Entity::isOffScreen(int screenWidth, int screenHeight) {
    if (x <= -w) {
        return true;
    } else if (x >= screenWidth + w) {
        return true;
    }

    if (y <= -h) {
        return true;
    } else if (y >= screenHeight + h) {
        return true;
    }

    return false;
}

int Entity::getAngle() {
    return angle;
}

void Entity::setAngle(int newAngle) {
    angle = newAngle;
}

bool Entity::isCollidingWith(Entity& otherEntity, vector<vector<vector<float>>>& testPoints) {

    // AABB collision detection (basic and inaccurate)
    // only used for entities without defined vertices
    if (otherEntity.getPoints().size() == 0 || vertexPoints.size() == 0) {
        return (
            x < otherEntity.getX() + otherEntity.getW() &&
            x + w > otherEntity.getX() &&
            y < otherEntity.getY() + otherEntity.getH() &&
            y + h > otherEntity.getY()
        );
    
    // more advanced and accurate collision detection using vertices
    } else { 
        vector<vector<float>> otherPointsDefault = otherEntity.getPoints();
        vector<vector<float>> otherPoints = utils::rotatePoints(otherPointsDefault, otherEntity.getCenter(), -otherEntity.getAngle());
        vector<vector<float>> rotatedVertices = utils::rotatePoints(vertexPoints, getCenter(), -angle);

        for (int i = 0; i < rotatedVertices.size(); i++) {
            // get points for current line segment
            vector<float> point1A = {rotatedVertices[i][0] + x, rotatedVertices[i][1] + y};
            vector<float> point2A = {rotatedVertices[(i + 1) % rotatedVertices.size()][0] + x, rotatedVertices[(i+1) % rotatedVertices.size()][1] + y};

            for (int j = 0; j < otherPoints.size(); j++) {
                vector<float> point1B = {otherPoints[j][0] + otherEntity.getX(), otherPoints[j][1] + otherEntity.getY()};
                vector<float> point2B = {otherPoints[(j + 1) % otherPoints.size()][0] + otherEntity.getX(), otherPoints[(j+1) % otherPoints.size()][1] + otherEntity.getY()};

                if (
                    max(point1A[0], point1B[0]) < min(point2A[0], point2B[0]) ||
                    max(point1A[1], point1B[1]) < min(point2A[1], point2B[1])
                ) {
                    continue;
                }
            
                float slope1 = (point1A[1] - point2A[1]) / (point1A[0] - point2A[0]);
                float slope2 = (point1B[1] - point2B[1]) / (point1B[0] - point2B[0]);

                float b1 = point1A[1] - slope1 * point1A[0];
                float b2 = point1B[1] - slope2 * point1B[0];

                float intersectX = (b2 - b1) / (slope1 - slope2);
                float intersectY = slope1 * intersectX + b1;

                if (
                    (
                        intersectX <= max(point1A[0], point2A[0]) &&
                        intersectX <= max(point1B[0], point2B[0]) &&
                        intersectX >= min(point1A[0], point2A[0]) &&
                        intersectX >= min(point1B[0], point2B[0])
                    ) &&
                    (
                        intersectY <= max(point1A[1], point2A[1]) &&
                        intersectY <= max(point1B[1], point2B[1]) &&
                        intersectY >= min(point1A[1], point2A[1]) &&
                        intersectY >= min(point1B[1], point2B[1])
                    )
                ) {
                    return true;
                }
            }
        }
        return false;
    }
}

void Entity::wrap(int screenWidth, int screenHeight) {
    if (x < -w) {
        setX(screenWidth);
    } else if (x > screenWidth) {
        setX(-w);
    }

    if (y < -h) {
        setY(screenHeight);
    } else if (y > screenHeight) {
        setY(-h);
    }
}

SDL_Texture* Entity::getTexture() {
    return texture;
}

SDL_Rect Entity::getCurrentFrame() {
    return currentFrame;
}

void Entity::setTexture(SDL_Texture* newTexture) {
    texture = newTexture;
}

void Entity::setCurrentFrame(int srcX, int srcY, int srcW, int srcH) {
    currentFrame.x = srcX;
    currentFrame.y = srcY;
    currentFrame.w = srcW;
    currentFrame.h = srcH;
}

vector<vector<float>> Entity::getPoints() {
    return vertexPoints;
}