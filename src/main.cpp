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

void renderImage(int x, int y, const uint8_t* image, int width, int height, int frame, int pixelScale)
{
  int pixelCount = width * height;
  for (int i = 0; i < pixelCount; i++)
  {
    uint8_t color = image[i + (pixelCount * frame)];
    if (color == TFT_BLACK) continue;

    int x2 = x + ((i % width) * pixelScale);
    int y2 = y + ((i / width) * pixelScale);

    // Draw the pixel one line at a time
    uint8_t* yp = frameBuffer + (SCREEN_WIDTH * y2 + x2);
    for (int j = 0; j < pixelScale; j++)
    {
      memset(yp + (j * SCREEN_WIDTH), color, pixelScale);
    }
  }
}

void renderFrame()
{
  std::fill_n(frameBuffer, SCREEN_WIDTH * SCREEN_HEIGHT, 0x0000);

  renderImage(0, 0, imageBackgroundOutside, IMAGE_BACKGROUNDOUTSIDE_WIDTH, IMAGE_BACKGROUNDOUTSIDE_HEIGHT, 0, 16);
  renderImage(100, 130, imageDude, IMAGE_DUDE_WIDTH, IMAGE_DUDE_HEIGHT, imageDudeFramecounter, 8);

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

  renderFrame();
}

uint32_t lastRenderTime = 0;
void loop()
{
  handleTouch();

  int ms = millis();
  imageDudeFramecounter = ms / 1000 % IMAGE_DUDE_FRAMES;
  
  // Render frames at about 1fps
  if (ms - lastRenderTime >= 33)
  {
    renderFrame();
    lastRenderTime = millis();
  }
}
