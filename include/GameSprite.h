#include <TFT_eSPI.h>

namespace Game
{
    static void pushNextFrameCallback(TimerHandle_t timer);
    class Sprite
    {
        public:
            Sprite(TFT_eSprite* sprite, const uint16_t* image, int spriteWidth, int spriteHeight, int frameCount);

            int frameCount;
            int width;
            int height;
            const uint16_t* image;
            TFT_eSprite* sprite;

            void init();
            void pushFrame(int frame);
            void pushNextFrame();
            void pushSprite(uint32_t x, uint32_t y);
            void pushToSprite(Game::Sprite toSprite, uint32_t x, uint32_t y, uint16_t transparent);
            void startCycleFrames(int frameDurationMs);
            void stopCycleFrames();

        private:
            int _currentFrame = -1;        
            TimerHandle_t _timer;

            const uint16_t* getFrame(int frame);
    };
};
