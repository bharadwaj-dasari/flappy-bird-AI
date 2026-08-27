#include "Pipe.hpp"

const float PIPE_WIDTH_SCALE  = 7.0f;
const float PIPE_HEIGHT_SCALE = 3.5f;

Pipe::Pipe(const Texture& texture, float startX, float gapY)
    : topPipe(texture, IntRect({32, 0}, {32, 320})),
      bottomPipe(texture, IntRect({0, 0}, {32, 320})),
      xPos(startX),
      gapStartY(gapY)
{
    topPipe.setScale({PIPE_WIDTH_SCALE, PIPE_HEIGHT_SCALE});
    bottomPipe.setScale({PIPE_WIDTH_SCALE, PIPE_HEIGHT_SCALE});

    // Top pipe hangs down from above, its bottom edge touches gapStartY
    topPipe.setPosition({
        xPos,
        gapStartY - topPipe.getGlobalBounds().size.y
    });

    // Bottom pipe starts right below the gap
    bottomPipe.setPosition({
        xPos,
        gapStartY + gapHeight
    });
}

void Pipe::update(float dt)
{
    xPos -= scrollSpeed * dt;

    topPipe.setPosition({
        xPos,
        gapStartY - topPipe.getGlobalBounds().size.y
    });

    bottomPipe.setPosition({
        xPos,
        gapStartY + gapHeight
    });
}

void Pipe::draw(RenderWindow& window)
{
    window.draw(topPipe);
    window.draw(bottomPipe);
}

bool Pipe::isPassed(float birdX)
{
    return xPos + topPipe.getGlobalBounds().size.x < birdX;
}

bool Pipe::collides(FloatRect birdBounds)
{
    return birdBounds.findIntersection(topPipe.getGlobalBounds()) ||
           birdBounds.findIntersection(bottomPipe.getGlobalBounds());
}

bool Pipe::isOffScreen()
{
    return xPos + topPipe.getGlobalBounds().size.x < 0;
}