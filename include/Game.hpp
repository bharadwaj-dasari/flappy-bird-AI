#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "Bird.hpp"
#include "Pipe.hpp"

using namespace sf;
using namespace std;

// ── Window & World Constants ──
const unsigned int SCREEN_WIDTH  = 1600;
const unsigned int SCREEN_HEIGHT = 1200;

// Minimum distance the gap must keep from ceiling and ground (equal = fair)
const float PIPE_MARGIN = 150.0f;

class Game
{
private:
    // ── Core ──
    RenderWindow window;
    Clock gameClock;

    // ── Visuals ──
    Texture backgroundTexture;
    Sprite  background;

    Texture groundTexture;
    Sprite  ground;

    Texture pipeTexture;
    Texture fontTexture;

    // ── Gameplay ──
    Bird bird;
    vector<Pipe> pipes;

    float groundY;
    bool  dead = false;
    int   score = 0;

    // ── Pipe Spawning ──
    float spawnTimer    = 0.0f;
    float spawnInterval = 2.5f;   // seconds between each pipe pair

    // ── Restart (double-space) ──
    Clock spaceClock;
    bool  waitingForDoubleSpace = false;

public:
    Game();
    void run();

private:
    void handleEvents();
    void update(float dt);
    void draw();
    void drawBitmapText(const string& text, float x, float y, float scale);
    void reset();

    float randomGapY();           // returns a fair random gap position
};