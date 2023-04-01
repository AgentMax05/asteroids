#pragma once

#include <SDL2/SDL.h>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <cmath>
#include <string>

#include <iostream>
using std::cout; // REMOVE

using std::string;
using std::vector;

#ifndef UTILS
#define UTILS

namespace utils {
    inline float hireTimeInSeconds() {
        float t = SDL_GetTicks() * 0.001;
        return t;
    }

    inline int randNum(int min, int max) {
        // return (rand() % max) + min;
        return min + (rand() % (max - min + 1));
    }

    template <typename T>
    T randChoice(std::vector<T> vectorP) {
        return vectorP[utils::randNum(0, vectorP.size() - 1)];
    }

    inline float dist(float x1, float y1, float x2, float y2) {
        return sqrt(pow((x1 - x2), 2) + pow((y1 - y2), 2));
    }

    inline string strToUpper(string input) {
        string output = "";
        for (int i = 0; i < input.size(); i++) {
            char current = input[i];
            if (current >= 97 && current <= 122) {
                output += current - 32;
            } else {
                output += current;
            }
        }
        return output;
    }

    inline bool isLetter(char input) {
        return ((input >= 97 && input <= 122) || (input >= 65 && input <= 90));
    }

    inline float degrees(float radians) {
        return radians * (180 / M_PI);
    }

    inline float radians(float degrees) {
        return degrees * (M_PI / 180);
    }

    inline vector<vector<float>> rotatePoints(vector<vector<float>>& points, vector<float> center, int angle) {
        vector<vector<float>> newPoints = {};
        for (int i = 0; i < points.size(); i++) {
            vector<float> currentPoint = points[i];
            // float newX = (center[0] - currentPoint[0]) * cos(radians(angle)) - (center[1] - currentPoint[1]) * -sin(radians(angle));
            // float newY = (center[1] - currentPoint[1]) * -cos(radians(angle)) + (center[0] - currentPoint[0]) * sin(radians(angle));
            float newX = (currentPoint[0] - center[0]) * cos(radians(angle)) - (currentPoint[1] - center[1]) * sin(radians(angle));
            float newY = (currentPoint[1] - center[1]) * cos(radians(angle)) + (currentPoint[0] - center[0]) * sin(radians(angle));
            newPoints.push_back({newX + center[0], newY + center[1]});
        }
        return newPoints;
    }

    inline vector<vector<float>> multiplyAllNums(float factor, vector<vector<float>> inputVector) {
        vector<vector<float>> result;
        for (int i = 0; i < inputVector.size(); i++) {
            result.push_back({inputVector[i][0] * factor, inputVector[i][1] * factor});
        }
        return result;
    }

    // checks if a shape is convex (every line drawn through shape only collides once)
    // inline bool isConvex(vector<vector<float>> vertices) {
    //     for (int i = 0; i < vertices.size(); i++) {
    //         vector<float> point1 = vertices[i];

    //         for (int j = 0; j < vertices.size(); j++) {
    //             vector<float> point2 vertices[j];
    //             if (i == j) {continue;}
    //             float slope = (point2[1] - point1[1]) / (point2[0] - point1[0]);
    //             float yInt = point1[1] - slope * point1[0];

    //         }
    //     }
    // }
}

#endif