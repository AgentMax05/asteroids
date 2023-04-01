#include "Text.hpp"

#include <SDL2/SDL.h>
#include <SDl2/SDL_image.h>
#include <vector>
#include <string>
#include "Entity.hpp"
#include "Utils.hpp"
#include "RenderWindow.hpp"

#include <iostream> // REMOVE

Text::Text(float xP, float yP, int fontSizeP, string textP, SDL_Texture* fontP) {
    fontSize = fontSizeP;
    x = xP;
    y = yP;
    text = utils::strToUpper(textP);
    font = fontP;
    entities = {};

    for (int i = 0; i < text.size(); i++) {
        char current = text[i];
        Entity letter(x + (fontSize * i), y, fontSize, fontSize, 64, 64, font);

        if (utils::isLetter(current)) {
            letter.setCurrentFrame((current - 65) * 64, 0, 64, 64);
            entities.push_back(letter);
        } else if (current >= 48 && current <= 57) { // 0 = 48
            letter.setCurrentFrame((current - 48 + 26) * 64, 0, 64, 64);
            entities.push_back(letter);
        } else if (current == ' ') {
            letter.setCurrentFrame(37 * 64, 0, 64, 64);
            entities.push_back(letter);
        }
    }

    w = entities.size() * fontSize;
    h = fontSize;
}

void Text::render(RenderWindow window) {
    for (int i = 0; i < entities.size(); i++) {
        window.render(entities[i]);
    }
}

float Text::getX() {return x;}
float Text::getY() {return y;}
float Text::getW() {return w;}
float Text::getH() {return h;}

void Text::center(int screenWidth, int screenHeight) {
    x = screenWidth / 2 - w / 2;
    y = screenHeight / 2 - h / 2;
    setX(x);
    setY(y);
}

void Text::setText(string newText) {
    text = utils::strToUpper(newText);
    entities = {};

    for (int i = 0; i < text.size(); i++) {
        char current = text[i];
        Entity letter(x + (fontSize * i), y, fontSize, fontSize, 64, 64, font);

        if (utils::isLetter(current)) {
            letter.setCurrentFrame((current - 65) * 64, 0, 64, 64);
            entities.push_back(letter);
        } else if (current >= 48 && current <= 57) { // 0 = 48
            letter.setCurrentFrame((current - 48 + 26) * 64, 0, 64, 64);
            entities.push_back(letter);
        } else if (current == ' ') {
            letter.setCurrentFrame(37 * 64, 0, 64, 64);
            entities.push_back(letter);
        }
    }
    w = entities.size() * fontSize;
    h = fontSize;
}

string Text::getText() {
    return text;
}

void Text::setX(float newX) {
    x = newX;
    for (int i = 0; i < entities.size(); i++) {
        entities[i].setX(x + (i * fontSize));
    }
}
void Text::setY(float newY) {
    y = newY;
    for (int i = 0; i < entities.size(); i++) {
        entities[i].setY(y);
    }
}

