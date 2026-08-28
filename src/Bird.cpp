#include "Bird.hpp"
#include <iostream>

using namespace std;

const IntRect FRAME_UP        ({0,  0},  {16, 16});
const IntRect FRAME_DOWN      ({16, 0},  {16, 16});
const IntRect FRAME_DEAD_UP   ({0,  16}, {16, 16});
const IntRect FRAME_DEAD_DOWN ({16, 16}, {16, 16});

const float GRAVITY    = 1000.0f;
const float FLAP_FORCE = -500.0f;
const float BIRD_SCALE = 10.0f;
const Vector2f START_POS = {400.0f, 600.0f};

Bird::Bird()
    : sprite(texture, FRAME_UP)
{
    if (!texture.loadFromFile("assets/Bird16.png"))
    {
        cout << "Failed to load bird!" << endl;
        return;
    }

    sprite.setScale({BIRD_SCALE, BIRD_SCALE});
    sprite.setPosition(START_POS);
}

void Bird::update(float dt)
{
    if (dead) return;

    velocity += GRAVITY * dt;
    sprite.move({0.0f, velocity * dt});

    if (velocity < 0.0f)
        sprite.setTextureRect(FRAME_UP);
    else
        sprite.setTextureRect(FRAME_DOWN);
}

void Bird::flap()
{
    if (!dead)
        velocity = FLAP_FORCE;
}

void Bird::die()
{
    if (velocity < 0.0f)
        sprite.setTextureRect(FRAME_DEAD_UP);
    else
        sprite.setTextureRect(FRAME_DEAD_DOWN);

    dead = true;
    velocity = 0.0f;
}

void Bird::reset()
{
    dead = false;
    velocity = 0.0f;
    sprite.setPosition(START_POS);
    sprite.setTextureRect(FRAME_UP);
}

Sprite& Bird::getSprite()
{
    return sprite;
}

FloatRect Bird::getBounds()
{
    return sprite.getGlobalBounds();
}

bool Bird::isDead() const
{
    return dead;
}

float Bird::getVelocity() const { return velocity; }