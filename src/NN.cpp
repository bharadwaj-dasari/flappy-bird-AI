#include "NN.hpp"
#include <cmath>
#include <cstdlib>

FlappyNN::FlappyNN()
{
    weights.resize(2);
}

void FlappyNN::generateWeights(std::mt19937_64& randomEngine)
{
    weights[0].resize(3, std::vector<float>(4));
    weights[1].resize(4, std::vector<float>(1));

    std::uniform_real_distribution<float> dist(-1.0f, std::nextafter(1.0f, 2.0f));

    for (auto& layer : weights) {
        for (auto& node : layer) {
            for (float& w : node) {
                w = dist(randomEngine);
            }
        }
    }
}

bool FlappyNN::shouldFlap(const GameState& state) const
{
    if (weights[0].empty()) return false;

    std::vector<std::vector<float>> network(3);
    network[0].resize(3);
    network[1].resize(4, 0.0f);
    network[2].resize(1, 0.0f);

    // inputs
    network[0][0] = state.birdVelocity / 125.0f;
    network[0][1] = (state.gapBottomY - state.birdBottomY) / 3.75f;
    network[0][2] = state.pipeDirection;

    for (size_t a = 0; a < network.size() - 1; a++) {
        for (size_t b = 0; b < network[1 + a].size(); b++) {
            for (size_t c = 0; c < network[a].size(); c++) {
                network[1 + a][b] += network[a][c] * weights[a][c][b];
            }

            if (network[1 + a][b] <= 0.0f) {
                network[1 + a][b] = std::pow(2.0f, network[1 + a][b]) - 1.0f;
            } else {
                network[1 + a][b] = 1.0f - std::pow(2.0f, -network[1 + a][b]);
            }
        }
    }

    return network[2][0] >= 0.0f;
}

void FlappyNN::crossover(std::mt19937_64& randomEngine, const FlappyNN& p1, const FlappyNN& p2)
{
    weights[0].resize(3, std::vector<float>(4));
    weights[1].resize(4, std::vector<float>(1));
    
    std::uniform_int_distribution<int> mutateDist(0, 63);
    std::uniform_real_distribution<float> weightDist(-1.0f, std::nextafter(1.0f, 2.0f));

    for (size_t a = 0; a < weights.size(); a++) {
        for (size_t b = 0; b < weights[a].size(); b++) {
            for (size_t c = 0; c < weights[a][b].size(); c++) {
                if (rand() % 2 == 0) {
                    weights[a][b][c] = p1.weights[a][b][c];
                } else {
                    weights[a][b][c] = p2.weights[a][b][c];
                }

                if (mutateDist(randomEngine) == 0) {
                    weights[a][b][c] = weightDist(randomEngine);
                }
            }
        }
    }
}
