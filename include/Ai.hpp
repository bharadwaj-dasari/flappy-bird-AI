#pragma once

struct GameState
{
    float birdCenterY;
    float birdVelocity;
    float gapCenterY;
    float distanceX;

    float gapBottomY;
    float birdBottomY;

    float pipeDirection;
    float scrollSpeed;
};

class FlappyAI
{
private:
    static constexpr float G = 1000.0f;
    static constexpr float Flap_FORCE = -500.0f;

public:
    bool shouldFlap(const GameState& state) const;
};