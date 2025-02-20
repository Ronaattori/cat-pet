#include <Arduino.h>
#include <GameSprite.h>

Game::Sprite::Sprite(TFT_eSprite* sprite, const uint16_t* image, int spriteWidth, int spriteHeight, int frameCount)
{
    this->sprite = sprite;
    this->image = image;
    this->width = spriteWidth;
    this->height = spriteHeight;
    this->frameCount = frameCount;
};

void Game::Sprite::init()
{
    // Might want to experiment enabling these at some point
    // sprite->setColorDepth(8);
    // sprite->setSwapBytes(true);
    // sprite->setAttribute(PSRAM_ENABLE, false);
    sprite->createSprite(width, height);
}

static void Game::pushNextFrameCallback(TimerHandle_t timer)
{
    Game::Sprite* sprite =  static_cast<Game::Sprite*>(pvTimerGetTimerID(timer));
    sprite->pushNextFrame();
}

const uint16_t* Game::Sprite::getFrame(int frame)
{
    if (frame > frameCount - 1)
    {
        throw "Sprite doesn't contain enough frames";
    }
    return image + (width * height * frame);
}

void Game::Sprite::pushFrame(int frame)
{
    const uint16_t* frameData = getFrame(frame);
    _currentFrame = frame;
    sprite->pushImage(0, 0, width, height, frameData);
}

void Game::Sprite::pushNextFrame()
{
    int nextFrame =  _currentFrame + 1 & frameCount - 1;
    pushFrame(nextFrame);
}

void Game::Sprite::pushSprite(uint32_t x, uint32_t y)
{
    sprite->pushSprite(x, y);
}

void Game::Sprite::pushToSprite(Game::Sprite toSprite, uint32_t x, uint32_t y, uint16_t transparent)
{
    sprite->pushToSprite(toSprite.sprite, x, y, transparent);
}

void Game::Sprite::startCycleFrames(int frameDurationMs)
{
    // Set the pointer to _this_ as the ID of the timer
    // We can then fetch it in the callback and get a reference to the class instance
    _timer = xTimerCreate("frameCycleTimer", pdMS_TO_TICKS(frameDurationMs), pdTRUE, static_cast<void*>(this), Game::pushNextFrameCallback);
    xTimerStart(_timer, 10);
}

void Game::Sprite::stopCycleFrames()
{
    xTimerStop(_timer, 10);
}