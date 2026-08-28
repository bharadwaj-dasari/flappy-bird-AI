#pragma once
#include <vector>
#include <random>
#include "Ai.hpp"

class FlappyNN
{
private:
    std::vector<std::vector<std::vector<float>>> weights;

public:
    FlappyNN();
    
    bool shouldFlap(const GameState& state) const;
    void generateWeights(std::mt19937_64& randomEngine);
    void crossover(std::mt19937_64& randomEngine, const FlappyNN& p1, const FlappyNN& p2);

    const std::vector<std::vector<std::vector<float>>>& getWeights() const { return weights; }
};
