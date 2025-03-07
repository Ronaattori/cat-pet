#include <Arduino.h>

// Assuming portrait mode (usb port == down)
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320

#define IMAGE_DUDE_HEIGHT 16
#define IMAGE_DUDE_WIDTH 16
#define IMAGE_DUDE_FRAMES 2

#define IMAGE_BACKGROUNDOUTSIDE_HEIGHT 20
#define IMAGE_BACKGROUNDOUTSIDE_WIDTH 15
#define IMAGE_BACKGROUNDOUTSIDE_FRAMES 1

#pragma once
struct Sprite
{
    uint8_t* image;
    uint8_t pixelScale;
    uint8_t frames;
    uint16_t frameDelay;
    uint8_t x, y;
    uint8_t w, h;
};

extern struct Sprite spriteDude;
extern uint8_t imageDudeIdle[];
extern uint8_t imageDudeFreak[];

extern struct Sprite spriteBackgroundOutside;
extern uint8_t imageBackgroundOutside[];
