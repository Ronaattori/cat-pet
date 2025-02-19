#include <TFT_eSPI.h>

namespace Game
{
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
            const uint16_t* getFrame(int frame);
            int getNextFrame();
            void pushFrame(int frame);
            void pushNextFrame();
            static void pushNextFrameCallback(TimerHandle_t timer);
            void pushSprite(uint32_t x, uint32_t y);
            void startCycleFrames(int frameDurationMs);
            void stopCycleFrames();

        private:
            int _currentFrame = -1;        
            TimerHandle_t _timer;
    };
};
