#include <Arduino.h>
#include <XPT2046_Setup.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include <images.h>

// Assuming portrait mode (usb port == down)
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320

TFT_eSPI tft = TFT_eSPI();
uint8_t frameBuffer[SCREEN_WIDTH * SCREEN_HEIGHT];

SPIClass touchscreenSPI = SPIClass(VSPI);
XPT2046_Touchscreen touchscreen(XPT2046_CS, XPT2046_IRQ);

void printTouchToSerial(int touchX, int touchY, int touchZ)
{
  Serial.print("X = ");
  Serial.print(touchX);
  Serial.print(" | Y = ");
  Serial.print(touchY);
  Serial.print(" | Pressure = ");
  Serial.print(touchZ);
  Serial.println();
}

void handleTouch()
{
  if (touchscreen.touched())
  {
    TS_Point p = touchscreen.getPoint();
    // Calibrate Touchscreen points with map function to the correct width and height
    int x = map(p.x, 200, 3700, 1, SCREEN_WIDTH);
    int y = map(p.y, 240, 3800, 1, SCREEN_HEIGHT);
    int z = p.z;
    printTouchToSerial(x, y, z);
  }
}

void renderSprite(Sprite sprite)
{
  int pixelCount = sprite.w * sprite.h;
  int frame = 0;
  if (sprite.frames > 1)
  {
    frame = millis() / sprite.frameDelay % sprite.frames;
  }

  for (int i = 0; i < pixelCount; i++)
  {
    uint8_t color = sprite.image[i + (pixelCount * frame)];
    if (color == TFT_BLACK) continue;

    int x2 = sprite.x + ((i % sprite.w) * sprite.pixelScale);
    int y2 = sprite.y + ((i / sprite.w) * sprite.pixelScale);

    // Draw the pixel one line at a time
    uint8_t* yp = frameBuffer + (SCREEN_WIDTH * y2 + x2);
    for (int j = 0; j < sprite.pixelScale; j++)
    {
      memset(yp + (j * SCREEN_WIDTH), color, sprite.pixelScale);
    }
  }
}

void renderFrame()
{
  std::fill_n(frameBuffer, SCREEN_WIDTH * SCREEN_HEIGHT, 0x0000);

  renderSprite(spriteBackgroundOutside);
  renderSprite(spriteDude);

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

  spriteDude.x = 100;
  spriteDude.y = 130;

  renderFrame();
}

uint32_t lastRenderTime = 0;
void loop()
{
  handleTouch();

  // Render frames at about 33fps
  if (millis() - lastRenderTime >= 33)
  {
    renderFrame();
    lastRenderTime = millis();
  }
}
