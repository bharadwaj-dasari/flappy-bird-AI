#include "Game.hpp"
#include <iostream>
#include <cstdlib>
#include <string>
#include <chrono>
#include <algorithm>
#include <iomanip>

using namespace std;
using namespace sf;

const float FONT_CHAR_WIDTH  = 16.0f;
const float FONT_CHAR_HEIGHT = 32.0f;

const int DOUBLE_TAP_WINDOW = 400;



Game::Game()
    : window(VideoMode({SCREEN_WIDTH, SCREEN_HEIGHT}), "Flappy Bird"),
      background(backgroundTexture),
      ground(groundTexture)
{
    randomEngine.seed(std::chrono::system_clock::now().time_since_epoch().count());
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

    if (!pipeTexture.loadFromFile("assets/Pipes16.png"))
    {
        cout << "Failed to load pipes!" << endl;
        return;
    }

    if (!fontTexture.loadFromFile("assets/Font.png"))
    {
        cout << "Failed to load font!" << endl;
    }

    pipes.clear();
}



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
                    bird.flap();
                }
                else
                {
                    if (waitingForDoubleSpace &&
                        spaceClock.getElapsedTime().asMilliseconds() < DOUBLE_TAP_WINDOW)
                    {
                        waitingForDoubleSpace = false;
                        reset();
                    }
                    else
                    {
                        waitingForDoubleSpace = true;
                        spaceClock.restart();
                    }
                }
            }

            if (key == Keyboard::Key::A)
            {
                aiMode = !aiMode;
                cout << (aiMode ? "AI ON" : "AI OFF") << endl;
                if (aiMode && dead) reset();
            }

            if (key == Keyboard::Key::N)
            {
                nnMode = !nnMode;
                cout << (nnMode ? "NN ON" : "NN OFF") << endl;
                if (nnMode) {
                    aiMode = false;
                    resetNN();
                } else {
                    reset();
                }
            }
        }
    }
}



float Game::randomGapY()
{

    float gapHeight = 400.0f;   // must match Pipe::gapHeight
    float minGapY = PIPE_MARGIN;
    float maxGapY = groundY - gapHeight - PIPE_MARGIN;
    int range = (int)(maxGapY - minGapY);

    if (range <= 0) range = 1;  // safety clamp

    return minGapY + (rand() % range);
}

void Game::update(float dt)
{
    if (nnMode) {
        updateNN(dt);
        return;
    }

    if (dead && aiMode) { reset(); return; }
    if (dead) return;

    if (aiMode) aiUpdate();

    bird.update(dt);

    currentScrollSpeed = 300.0f + (score / 20) * 100.0f;
    spawnInterval = 2.5f * (300.0f / currentScrollSpeed);

    spawnTimer -= dt;
    if (spawnTimer <= 0.0f) {
        float minGapY = PIPE_MARGIN;
        float maxGapY = groundY - 400.0f - PIPE_MARGIN;
        pipes.emplace_back(pipeTexture, (float)SCREEN_WIDTH, randomGapY(), currentScrollSpeed, minGapY, maxGapY);
        spawnTimer = spawnInterval;
    }

    for (auto it = pipes.begin(); it != pipes.end(); ) {
        if (it->isOffScreen()) it = pipes.erase(it);
        else ++it;
    }

    for (auto& pipe : pipes) {
        pipe.setScrollSpeed(currentScrollSpeed);
        pipe.update(dt);
    }

    for (auto& pipe : pipes)
    {
        if (pipe.collides(bird.getBounds()))
        {
            bird.die();
            dead = true;
            return;
        }
    }

    float birdX = bird.getSprite().getPosition().x;
    for (auto& pipe : pipes)
    {
        if (!pipe.counted && pipe.isPassed(birdX))
        {
            pipe.counted = true;
            score++;
        }
    }

    float birdY = bird.getSprite().getPosition().y;
    float birdH = bird.getBounds().size.y;
    float birdX2 = bird.getSprite().getPosition().x;

    if (birdY < 0.0f)
        bird.getSprite().setPosition({birdX2, 0.0f});

    if (birdY + birdH >= groundY)
    {
        bird.getSprite().setPosition({birdX2, groundY - birdH});
        bird.die();
        dead = true;
    }
}



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

    if (nnMode) {
        for (auto& agent : population) {
            if (!agent.bird.isDead()) {
                window.draw(agent.bird.getSprite());
            }
        }
    } else {
        window.draw(bird.getSprite());
    }
    
    window.draw(ground);

    drawBitmapText(to_string(score), SCREEN_WIDTH / 2.0f, 30.0f, 3.0f);

    if (aiMode)
        drawBitmapText("AI", 60.0f, 30.0f, 2.5f);
    if (nnMode) {
        drawBitmapText("NN", 60.0f, 30.0f, 2.5f);
        drawBitmapText("GEN " + to_string(generation), 150.0f, 80.0f, 2.0f);
        drawBitmapText("REC " + to_string(recordScore), 150.0f, 130.0f, 2.0f);
    }

    if (dead)
        drawBitmapText("Double Space to Restart", SCREEN_WIDTH / 2.0f, 550.0f, 2.0f);

    window.display();
}



void Game::aiUpdate()
{
    float birdX       = bird.getSprite().getPosition().x;
    float birdY       = bird.getSprite().getPosition().y;
    float birdH       = bird.getBounds().size.y;
    float birdCenterY = birdY + birdH / 2.0f;

    Pipe* nextPipe = nullptr;
    float closestX = 1e9f;

    for (auto& pipe : pipes)
    {
        float pipeRight = pipe.getX() + 224.0f;  // 32 * 7 scale
        if (pipeRight > birdX && pipe.getX() < closestX)
        {
            closestX = pipe.getX();
            nextPipe = &pipe;
        }
    }

    // inputs
    GameState state;
    state.birdCenterY  = birdCenterY;
    state.birdVelocity = bird.getVelocity();
    state.birdBottomY  = birdY + birdH;

    if (nextPipe)
    {
        state.gapCenterY = nextPipe->getGapStartY() + nextPipe->getGapHeight() / 2.0f;
        state.gapBottomY = nextPipe->getGapStartY() + nextPipe->getGapHeight();
        state.distanceX  = nextPipe->getX() - birdX;
        state.pipeDirection = (float)nextPipe->getDirection();
    }
    else
    {
        state.gapCenterY = groundY / 2.0f;
        state.gapBottomY = groundY;
        state.distanceX  = (float)SCREEN_WIDTH;
        state.pipeDirection = 0.0f;
    }
    state.scrollSpeed = currentScrollSpeed;

    if (ai.shouldFlap(state))
        bird.flap();
}



void Game::reset()
{
    dead  = false;
    score = 0;
    spawnTimer = 0.0f;
    pipes.clear();
    bird.reset();
    pipes.clear();
    spawnTimer = 0.0f;
    score = 0;
    dead = false;
    currentScrollSpeed = 300.0f;
    spawnInterval = 2.5f;
}

void Game::resetNN()
{
    pipes.clear();
    spawnTimer = 0.0f;
    score = 0;
    generation = 1;
    recordScore = 0;

    population.clear();
    population.resize(8);
    
    for (auto& agent : population) {
        agent.bird.reset();
        agent.brain.generateWeights(randomEngine);
        agent.fitness = 0;
    }
}

void Game::evolvePopulation()
{
    generation++;
    if (score > recordScore) recordScore = score;

    std::sort(population.begin(), population.end(), std::greater<Agent>());

    for (size_t i = 2; i < population.size(); i++) {
        population[i].brain.crossover(randomEngine, population[0].brain, population[1].brain);
    }

    for (auto& agent : population) {
        agent.bird.reset();
        agent.fitness = 0;
    }

    pipes.clear();
    spawnTimer = 0.0f;
    score = 0;

    cout << "\n======================================================\n";
    cout << " GENERATION " << generation - 1 << " REPORT\n";
    cout << "======================================================\n";
    cout << " Best Score:   " << recordScore << "\n";
    cout << " Best Fitness: " << population[0].fitness << "\n\n";
    
    cout << " Top Bird's Neural Network Weights:\n";
    cout << "------------------------------------------------------\n";
    const auto& w = population[0].brain.getWeights();
    
    cout << fixed << setprecision(3);
    for (int h = 0; h < 4; h++) {
        cout << " [Hidden Node " << h << "]\n";
        cout << "   Weight from Velocity   : " << setw(7) << w[0][0][h] << "\n";
        cout << "   Weight from Gap Dist   : " << setw(7) << w[0][1][h] << "\n";
        cout << "   Weight from Pipe Dir   : " << setw(7) << w[0][2][h] << "\n";
    }
    cout << " [Output Node (Flap)]\n";
    for (int h = 0; h < 4; h++) {
        cout << "   Weight from Hidden " << h << "   : " << setw(7) << w[1][h][0] << "\n";
    }
    cout << "======================================================\n\n";
}

void Game::updateNN(float dt)
{
    bool allDead = true;

    Pipe* nextPipe = nullptr;
    float closestX = 1e9f;
    float birdX = population[0].bird.getSprite().getPosition().x;

    for (auto& pipe : pipes) {
        float pipeRight = pipe.getX() + 224.0f;
        if (pipeRight > birdX && pipe.getX() < closestX) {
            closestX = pipe.getX();
            nextPipe = &pipe;
        }
    }

    for (auto& agent : population) {
        if (!agent.bird.isDead()) {
            allDead = false;
            agent.fitness++;

            GameState state;
            float bY = agent.bird.getSprite().getPosition().y;
            float bH = agent.bird.getBounds().size.y;
            state.birdCenterY = bY + bH / 2.0f;
            state.birdVelocity = agent.bird.getVelocity();
            state.birdBottomY = bY + bH;

            if (nextPipe) {
                state.gapCenterY = nextPipe->getGapStartY() + nextPipe->getGapHeight() / 2.0f;
                state.gapBottomY = nextPipe->getGapStartY() + nextPipe->getGapHeight();
                state.distanceX = nextPipe->getX() - birdX;
                state.pipeDirection = (float)nextPipe->getDirection();
            } else {
                state.gapCenterY = groundY / 2.0f;
                state.gapBottomY = groundY;
                state.distanceX = (float)SCREEN_WIDTH;
                state.pipeDirection = 0.0f;
            }
            state.scrollSpeed = currentScrollSpeed;

            if (agent.bird.getVelocity() >= 0.0f && agent.brain.shouldFlap(state)) {
                agent.bird.flap();
            }

            agent.bird.update(dt);

            bY = agent.bird.getSprite().getPosition().y;
            if (bY < 0.0f) {
                agent.bird.getSprite().setPosition({birdX, 0.0f});
            }
            if (bY + bH >= groundY) {
                agent.bird.getSprite().setPosition({birdX, groundY - bH});
                agent.bird.die();
            }
        }
    }

    if (allDead) {
        evolvePopulation();
        return;
    }

    currentScrollSpeed = 300.0f + (score / 20) * 100.0f;
    spawnInterval = 2.5f * (300.0f / currentScrollSpeed);

    spawnTimer -= dt;
    if (spawnTimer <= 0.0f)
    {
        float minGapY = PIPE_MARGIN;
        float maxGapY = groundY - 400.0f - PIPE_MARGIN;
        pipes.emplace_back(pipeTexture, (float)SCREEN_WIDTH, randomGapY(), currentScrollSpeed, minGapY, maxGapY);
        spawnTimer = spawnInterval;
    }

    for (auto it = pipes.begin(); it != pipes.end();)
    {
        if (it->isOffScreen())
            it = pipes.erase(it);
        else
            ++it;
    }

    for (auto& pipe : pipes) {
        pipe.setScrollSpeed(currentScrollSpeed);
        pipe.update(dt);
    }

    for (auto& agent : population) {
        if (agent.bird.isDead()) continue;
        for (auto& pipe : pipes) {
            if (pipe.collides(agent.bird.getBounds())) {
                agent.bird.die();
                break;
            }
        }
    }

    for (auto& pipe : pipes) {
        if (!pipe.counted && pipe.isPassed(birdX)) {
            pipe.counted = true;
            score++;
        }
    }
}
