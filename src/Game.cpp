#include "Game.hpp"
#include <iostream>
#include <cstdlib>
#include <string>

using namespace std;
using namespace sf;

// ── Bitmap font layout ──
// Font.png is 1536x32  →  96 characters, 16px wide each (ASCII 32–127)
const float FONT_CHAR_WIDTH  = 16.0f;
const float FONT_CHAR_HEIGHT = 32.0f;

// How long (ms) between two Space presses counts as a "double tap"
const int DOUBLE_TAP_WINDOW = 400;


// ═══════════════════════════════════════════════════
//  Construction & Setup
// ═══════════════════════════════════════════════════

Game::Game()
    : window(VideoMode({SCREEN_WIDTH, SCREEN_HEIGHT}), "Flappy Bird"),
      background(backgroundTexture),
      ground(groundTexture)
{
    // ── Background ──
    if (!backgroundTexture.loadFromFile("assets/Background.png"))
    {
        cout << "Failed to load background!" << endl;
        return;
    }
    background.setTextureRect(IntRect(
        {0, 0},
        {(int)backgroundTexture.getSize().x, (int)backgroundTexture.getSize().y}
    ));
    background.setScale({
        SCREEN_WIDTH  / 180.0f,
        SCREEN_HEIGHT / 320.0f
    });

    // ── Ground ──
    if (!groundTexture.loadFromFile("assets/Ground.png"))
    {
        cout << "Failed to load ground!" << endl;
        return;
    }
    ground.setTextureRect(IntRect(
        {0, 0},
        {(int)groundTexture.getSize().x, (int)groundTexture.getSize().y}
    ));
    ground.setScale({SCREEN_WIDTH / 180.0f, 3.75f});
    ground.setPosition({0.0f, SCREEN_HEIGHT - ground.getGlobalBounds().size.y});
    groundY = ground.getPosition().y;

    // ── Pipes ──
    if (!pipeTexture.loadFromFile("assets/Pipes16.png"))
    {
        cout << "Failed to load pipes!" << endl;
        return;
    }

    // ── Bitmap font ──
    if (!fontTexture.loadFromFile("assets/Font.png"))
    {
        cout << "Failed to load font!" << endl;
    }

    pipes.clear();
}


// ═══════════════════════════════════════════════════
//  Main Loop
// ═══════════════════════════════════════════════════

void Game::run()
{
    while (window.isOpen())
    {
        handleEvents();
        float dt = gameClock.restart().asSeconds();
        update(dt);
        draw();
    }
}


// ═══════════════════════════════════════════════════
//  Input
// ═══════════════════════════════════════════════════

void Game::handleEvents()
{
    while (const optional event = window.pollEvent())
    {
        if (event->is<Event::Closed>())
            window.close();

        if (event->is<Event::KeyPressed>())
        {
            auto key = event->getIf<Event::KeyPressed>()->code;

            if (key == Keyboard::Key::Space)
            {
                if (!dead)
                {
                    // Normal gameplay — flap
                    bird.flap();
                }
                else
                {
                    // Dead — need double-space to restart
                    if (waitingForDoubleSpace &&
                        spaceClock.getElapsedTime().asMilliseconds() < DOUBLE_TAP_WINDOW)
                    {
                        waitingForDoubleSpace = false;
                        reset();
                    }
                    else
                    {
                        // First tap — start waiting for the second
                        waitingForDoubleSpace = true;
                        spaceClock.restart();
                    }
                }
            }
        }
    }
}


// ═══════════════════════════════════════════════════
//  Game Logic
// ═══════════════════════════════════════════════════

float Game::randomGapY()
{
    // Equal margin from ceiling and ground  →  equal chance of
    // a tall top pipe vs a tall bottom pipe.
    //
    //   min = PIPE_MARGIN                  (gap can't be too close to ceiling)
    //   max = groundY - gapHeight - PIPE_MARGIN  (gap can't be too close to ground)
    //
    // The midpoint of this range is exactly the vertical center of the
    // playable area, so the distribution is perfectly symmetric.

    float gapHeight = 400.0f;   // must match Pipe::gapHeight
    float minGapY = PIPE_MARGIN;
    float maxGapY = groundY - gapHeight - PIPE_MARGIN;
    int range = (int)(maxGapY - minGapY);

    if (range <= 0) range = 1;  // safety clamp

    return minGapY + (rand() % range);
}

void Game::update(float dt)
{
    if (dead) return;

    bird.update(dt);

    // ── Spawn pipes on a timer ──
    spawnTimer -= dt;
    if (spawnTimer <= 0.0f)
    {
        pipes.emplace_back(pipeTexture, (float)SCREEN_WIDTH, randomGapY());
        spawnTimer = spawnInterval;
    }

    // ── Remove pipes that scrolled off the left edge ──
    for (auto it = pipes.begin(); it != pipes.end(); )
    {
        if (it->isOffScreen())
            it = pipes.erase(it);
        else
            ++it;
    }

    // ── Move pipes ──
    for (auto& pipe : pipes)
        pipe.update(dt);

    // ── Check pipe collision ──
    for (auto& pipe : pipes)
    {
        if (pipe.collides(bird.getBounds()))
        {
            bird.die();
            dead = true;
            return;
        }
    }

    // ── Count score (bird passed a pipe pair) ──
    float birdX = bird.getSprite().getPosition().x;
    for (auto& pipe : pipes)
    {
        if (!pipe.counted && pipe.isPassed(birdX))
        {
            pipe.counted = true;
            score++;
        }
    }

    // ── Keep bird inside the screen ──
    float birdY = bird.getSprite().getPosition().y;
    float birdH = bird.getBounds().size.y;
    float birdX2 = bird.getSprite().getPosition().x;

    // Ceiling
    if (birdY < 0.0f)
        bird.getSprite().setPosition({birdX2, 0.0f});

    // Ground
    if (birdY + birdH >= groundY)
    {
        bird.getSprite().setPosition({birdX2, groundY - birdH});
        bird.die();
        dead = true;
    }
}


// ═══════════════════════════════════════════════════
//  Rendering
// ═══════════════════════════════════════════════════

void Game::drawBitmapText(const string& text, float centerX, float y, float scale)
{
    float totalWidth = text.size() * FONT_CHAR_WIDTH * scale;
    float startX = centerX - totalWidth / 2.0f;

    for (size_t i = 0; i < text.size(); i++)
    {
        int charIndex = text[i] - 32;   // ASCII 32 = space = first character
        Sprite glyph(fontTexture, IntRect(
            {(int)(charIndex * FONT_CHAR_WIDTH), 0},
            {(int)FONT_CHAR_WIDTH, (int)FONT_CHAR_HEIGHT}
        ));
        glyph.setScale({scale, scale});
        glyph.setPosition({startX + i * FONT_CHAR_WIDTH * scale, y});
        window.draw(glyph);
    }
}

void Game::draw()
{
    window.clear(Color(135, 206, 235));

    window.draw(background);

    for (auto& pipe : pipes)
        pipe.draw(window);

    window.draw(bird.getSprite());
    window.draw(ground);

    // Score — centered, large
    drawBitmapText(to_string(score), SCREEN_WIDTH / 2.0f, 30.0f, 3.0f);

    // Death prompt
    if (dead)
        drawBitmapText("Double Space to Restart", SCREEN_WIDTH / 2.0f, 550.0f, 2.0f);

    window.display();
}


// ═══════════════════════════════════════════════════
//  Reset
// ═══════════════════════════════════════════════════

void Game::reset()
{
    dead  = false;
    score = 0;
    spawnTimer = 0.0f;
    pipes.clear();
    bird.reset();
}