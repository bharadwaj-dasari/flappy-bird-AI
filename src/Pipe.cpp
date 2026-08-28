#include "Pipe.hpp"
#include <cstdlib>

const float PIPE_WIDTH_SCALE  = 7.0f;
const float PIPE_HEIGHT_SCALE = 3.5f;

Pipe::Pipe(const Texture& texture, float startX, float gapY, float scrollSpd, float min_Y, float max_Y)
    : topPipe(texture, IntRect({32, 0}, {32, 320})),
      bottomPipe(texture, IntRect({0, 0}, {32, 320})),
      xPos(startX),
      gapStartY(gapY),
      scrollSpeed(scrollSpd),
      minY(min_Y),
      maxY(max_Y)
{
    direction = (rand() % 2 == 0) ? 1 : -1;
    verticalSpeed = scrollSpeed * 0.4f;

    topPipe.setScale({PIPE_WIDTH_SCALE, PIPE_HEIGHT_SCALE});
    bottomPipe.setScale({PIPE_WIDTH_SCALE, PIPE_HEIGHT_SCALE});

    update(0.0f);
}

void Pipe::update(float dt)
{
    xPos -= scrollSpeed * dt;

    gapStartY += direction * verticalSpeed * dt;
    if (gapStartY < minY) {
        gapStartY = minY;
        direction = 1;
    } else if (gapStartY > maxY) {
        gapStartY = maxY;
        direction = -1;
    }

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

float Pipe::getX() const { return xPos; }
float Pipe::getGapStartY() const { return gapStartY; }
float Pipe::getGapHeight() const { return gapHeight; }
int Pipe::getDirection() const { return direction; }
void Pipe::setScrollSpeed(float speed) { scrollSpeed = speed; verticalSpeed = speed * 0.4f; }