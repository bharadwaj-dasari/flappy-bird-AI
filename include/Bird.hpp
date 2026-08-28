#pragma once

#include <SFML/Graphics.hpp>

using namespace sf;

class Bird
{
private:
    Texture texture;
    Sprite sprite;

    float velocity = 0.0f;
    bool dead = false;

public:
    Bird();

    void update(float dt);
    void flap();
    void die();
    void reset();

    Sprite& getSprite();
    FloatRect getBounds();
    bool isDead() const;
    float getVelocity() const;
};