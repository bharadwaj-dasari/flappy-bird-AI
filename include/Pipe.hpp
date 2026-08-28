#pragma once

#include <SFML/Graphics.hpp>

using namespace sf;

class Pipe
{
private:
    Sprite topPipe;
    Sprite bottomPipe;

    float xPos;
    float gapStartY;
    float scrollSpeed;
    float gapHeight = 400.0f;

    float verticalSpeed;
    int direction;
    float minY;
    float maxY;

public:
    bool counted = false;

    Pipe(const Texture& texture, float startX, float gapY, float currentScrollSpeed, float minY, float maxY);

    void update(float dt);
    void draw(RenderWindow& window);

    bool isPassed(float birdX);
    bool collides(FloatRect birdBounds);
    bool isOffScreen();

    float getX() const;
    float getGapStartY() const;
    float getGapHeight() const;
    int getDirection() const;
    void setScrollSpeed(float speed);
};