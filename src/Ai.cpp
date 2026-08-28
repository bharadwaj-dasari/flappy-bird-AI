#include "Ai.hpp"
#include <cmath>
#include <algorithm>

bool FlappyAI::shouldFlap(const GameState& state) const
{
    float targetY = state.gapCenterY + 60.0f;
    return (state.birdCenterY > targetY) && (state.birdVelocity >= 0.0f);
}