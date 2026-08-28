#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include "Bird.hpp"
#include "Pipe.hpp"
#include "Ai.hpp"
#include "NN.hpp"

using namespace sf;
using namespace std;

const unsigned int SCREEN_WIDTH  = 1600;
const unsigned int SCREEN_HEIGHT = 1200;
const float PIPE_MARGIN = 150.0f;

struct Agent {
    Bird bird;
    FlappyNN brain;
    unsigned int fitness = 0;
    bool operator>(const Agent& other) const { return fitness > other.fitness; }
};

class Game
{
private:
    RenderWindow window;
    Clock gameClock;

    Texture backgroundTexture;
    Sprite  background;
    Texture groundTexture;
    Sprite  ground;
    Texture pipeTexture;
    Texture fontTexture;

    Bird bird;
    vector<Pipe> pipes;

    float groundY;
    bool  dead = false;
    int   score = 0;

    float spawnTimer    = 0.0f;
    float spawnInterval = 2.5f;
    float currentScrollSpeed = 300.0f;

    Clock spaceClock;
    bool  waitingForDoubleSpace = false;

    bool     aiMode = false;
    FlappyAI ai;

    bool nnMode = false;
    std::mt19937_64 randomEngine;
    unsigned int generation = 1;
    unsigned int recordScore = 0;
    vector<Agent> population;

public:
    Game();
    void run();

private:
    void handleEvents();
    void update(float dt);
    void draw();
    void drawBitmapText(const string& text, float x, float y, float scale);
    void reset();
    void aiUpdate();

    void updateNN(float dt);
    void resetNN();
    void evolvePopulation();

    float randomGapY();
};