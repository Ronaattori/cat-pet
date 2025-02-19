#include <Arduino.h>
#include <XPT2046_Setup.h>
#include <TFT_Setup.h>
#include "SPIFFS.h"
#include <SPI.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include <GameSprite.h>

#include <images/dude.h>

// Touchscreen coordinates: (x, y) and pressure (z)
int x, y, z;

TFT_eSPI tft = TFT_eSPI();

SPIClass touchscreenSPI = SPIClass(VSPI);
XPT2046_Touchscreen touchscreen(XPT2046_CS, XPT2046_IRQ);

TFT_eSprite tft_sprite = TFT_eSprite(&tft);
Game::Sprite sprite = Game::Sprite(&tft_sprite, image_dude, 128, 128, 2);

void print_touch_to_serial(int touchX, int touchY, int touchZ)
{
  Serial.print("X = ");
  Serial.print(touchX);
  Serial.print(" | Y = ");
  Serial.print(touchY);
  Serial.print(" | Pressure = ");
  Serial.print(touchZ);
  Serial.println();
}

void handle_touch()
{
  if (touchscreen.touched())
  {
    TS_Point p = touchscreen.getPoint();
    // Calibrate Touchscreen points with map function to the correct width and height
    x = map(p.x, 200, 3700, 1, SCREEN_WIDTH);
    y = map(p.y, 240, 3800, 1, SCREEN_HEIGHT);
    z = p.z;
    print_touch_to_serial(x, y, z);
  }
}

void setup()
{
  Serial.begin(115200);

  touchscreenSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  touchscreen.begin(touchscreenSPI);
  touchscreen.setRotation(0);

  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  
  sprite.init();
  sprite.startCycleFrames(1000);
}

void loop()
{
  handle_touch();

  sprite.pushSprite(SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4);
}