#include "Button.hpp"
#include <iostream> // REMOVE
Button::Button(Entity *buttonEntityP, void (*funcP)(float, float)) {
    // x = posP[0];
    // y = posP[1];
    // w = posP[2];
    // h = posP[3];
    func = funcP;
    // buttonSprite = buttonSpriteP;
    // Entity newEntity = Entity(x, y, w, h, 64, 64, buttonSprite);
    // Entity* pointer = newEntity; 
    buttonEntity = buttonEntityP;
    x = buttonEntity->getX();
    y = buttonEntity->getY();
    w = buttonEntity->getW();
    h = buttonEntity->getH();
}

void Button::click(float x, float y) {
    func(x, y);
}

Entity Button::getEntity() {
    return *buttonEntity;
}

bool Button::isClicked(float clickX, float clickY) {
    return (
        clickX <= x + w &&
        clickX >= x &&
        clickY <= y + h &&
        clickY >= y
    );
}