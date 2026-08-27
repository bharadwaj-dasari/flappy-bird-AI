#pragma once

#include <SFML/Graphics.hpp>

using namespace sf;

class Pipe
{
private:
    Sprite topPipe;
    Sprite bottomPipe;

    float xPos;
    float gapStartY;                  // where the gap begins (top edge of gap)
    float scrollSpeed = 300.0f;
    float gapHeight = 400.0f;         // vertical space between top and bottom pipe

public:
    bool counted = false;

    Pipe(const Texture& texture, float startX, float gapY);

    void update(float dt);
    void draw(RenderWindow& window);

    bool isPassed(float birdX);
    bool collides(FloatRect birdBounds);
    bool isOffScreen();
};