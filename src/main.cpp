#include <Arduino.h>
#include <XPT2046_Setup.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>

#include <images/dude.h>
#include <images/backgroundOutside.h>

// Assuming portrait mode (usb port == down)
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320

// Touchscreen coordinates: (x, y) and pressure (z)
int x, y, z;

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite frameBuffer = TFT_eSprite(&tft);

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
    x = map(p.x, 200, 3700, 1, SCREEN_WIDTH);
    y = map(p.y, 240, 3800, 1, SCREEN_HEIGHT);
    z = p.z;
    printTouchToSerial(x, y, z);
  }
}

void renderImage(int x, int y, const uint16_t* image, int width, int height, int frame, int pixelScale)
{
  int pixelCount = width * height;

  for (int i = 0; i < pixelCount; i++)
  {
    uint16_t color = image[i + (pixelCount * frame)];
    if (color == TFT_BLACK) continue;

    int x2 = x + ((i % width) * pixelScale);
    int y2 = y + ((i / width) * pixelScale);

    frameBuffer.fillRect(x2, y2, pixelScale, pixelScale, color);
  }
}

void renderFrame()
{
  frameBuffer.fillScreen(TFT_BLACK);

  renderImage(0, 0, imageBackgroundOutside, IMAGE_BACKGROUNDOUTSIDE_WIDTH, IMAGE_BACKGROUNDOUTSIDE_HEIGHT, 0, 16);
  renderImage(100, 130, imageDude, 16, 16, imageDudeFramecounter, 8);

  imageDudeFramecounter = ++imageDudeFramecounter % IMAGE_DUDE_FRAMES;

  frameBuffer.pushSprite(0, 0);
}

void setup()
{
  Serial.begin(115200);

  touchscreenSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  touchscreen.begin(touchscreenSPI);
  touchscreen.setRotation(0);

  tft.init();
  tft.setRotation(0);

  frameBuffer.setColorDepth(8);
  frameBuffer.createSprite(SCREEN_WIDTH, SCREEN_HEIGHT);
  
  renderFrame();
}

unsigned long lastRenderTime = 0;
void loop()
{
  handleTouch();
  
  // Render frames at about 1fps
  if (millis() - lastRenderTime >= 1000)
  {
    renderFrame();
    lastRenderTime = millis();
  }
}