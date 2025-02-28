#include <Arduino.h>
#include <XPT2046_Setup.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include <images.h>
#include <scenes.h>
#include <vector>
#include <touch.h>

TFT_eSPI tft = TFT_eSPI();
uint8_t frameBuffer[SCREEN_WIDTH * SCREEN_HEIGHT];

SPIClass touchscreenSPI = SPIClass(VSPI);
XPT2046_Touchscreen touchscreen(XPT2046_CS, XPT2046_IRQ);

std::vector<Sprite*> sceneSprites;

void renderSprite(Sprite *sprite)
{
  int pixelCount = sprite->w * sprite->h;
  int pixelScale = sprite->pixelScale;
  int frame = 0;
  if (sprite->frames > 1 && sprite->frameDelay) {
    frame = millis() / sprite->frameDelay % sprite->frames;
  }

  for (int i = 0; i < pixelCount; i++) {
    uint8_t color = sprite->image[i + (pixelCount * frame)];
    if (color == TFT_BLACK) continue;

    int x2 = sprite->x + ((i % sprite->w) * pixelScale);
    int y2 = sprite->y + ((i / sprite->w) * pixelScale);

    // Draw the pixel one line at a time
    uint8_t *yp = frameBuffer + (SCREEN_WIDTH * y2 + x2);
    for (int h = 0; h < pixelScale; h++) {
      memset(yp + (h * SCREEN_WIDTH), color, pixelScale);
    }
  }
}

void renderFrame()
{
  std::fill_n(frameBuffer, SCREEN_WIDTH * SCREEN_HEIGHT, 0x0000);

  for (Sprite *sprite : sceneSprites) {
    renderSprite(sprite);
  }

  tft.pushImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, frameBuffer, true);
}

void setup()
{
  Serial.begin(115200);

  touchscreenSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  touchscreen.begin(touchscreenSPI);
  touchscreen.setRotation(0);

  tft.init();
  tft.setRotation(0);

  loadOutside(sceneSprites);

  renderFrame();
}

uint32_t lastRenderTime = 0;
void loop()
{
  handleTouch(touchscreen, sceneSprites);

  // Render frames at about 33fps
  if (millis() - lastRenderTime >= 33) {
    renderFrame();
    lastRenderTime = millis();
  }
}
