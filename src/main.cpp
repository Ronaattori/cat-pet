#include <Arduino.h>
#include <XPT2046_Setup.h>
#include <TFT_Setup.h>
#include "SPIFFS.h"
#include <SPI.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
using namespace std;


// Touchscreen coordinates: (x, y) and pressure (z)
int x, y, z;

TFT_eSPI tft = TFT_eSPI();

SPIClass touchscreenSPI = SPIClass(VSPI);
XPT2046_Touchscreen touchscreen(XPT2046_CS, XPT2046_IRQ);

TFT_eSprite background = TFT_eSprite(&tft);
TFT_eSprite cat = TFT_eSprite(&tft);

String read_file_string(const String path)
{
  if (!SPIFFS.begin(true))
  {
    throw std::invalid_argument("An Error has occurred while mounting SPIFFS");
  }

  File file = SPIFFS.open(path);
  if (!file)
  {
    throw std::invalid_argument("Failed to open file for reading");
  }

  String file_content = "";
  while (file.available())
  {
    file_content += (char)file.read();
  }
  file.close();
  return file_content;
}

// Print Touchscreen info about X, Y and Pressure (Z) on the Serial Monitor
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

uint16_t *string_to_uint16(const String source, size_t num_elements)
{
  num_elements = source.length() / 2;
  uint16_t *data = new uint16_t[num_elements];

  for (size_t i = 0; i < num_elements; ++i)
  {
    data[i] = (static_cast<uint8_t>(source[2 * i]) << 8) | static_cast<uint8_t>(source[2 * i + 1]);
  }

  return data;
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

  String face_raw = read_file_string("/face.raw");
  size_t face_uint16_length;
  uint16_t *face_uint16 = string_to_uint16(face_raw, face_uint16_length);
  Serial.write(face_uint16_length);

  // cat.setColorDepth(8);
  cat.createSprite(32, 32);
  cat.pushImage(0, 0, 32, 32, face_uint16);
}

void loop()
{
  handle_touch();
  cat.pushSprite(0, 0);

  delay(100);
}