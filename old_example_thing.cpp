#include <SPI.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
using namespace std;

TFT_eSPI tft = TFT_eSPI();

// Touchscreen pins
#define XPT2046_IRQ 36   // T_IRQ
#define XPT2046_MOSI 32  // T_DIN
#define XPT2046_MISO 39  // T_OUT
#define XPT2046_CLK 25   // T_CLK
#define XPT2046_CS 33    // T_CS

SPIClass touchscreenSPI = SPIClass(VSPI);
XPT2046_Touchscreen touchscreen(XPT2046_CS, XPT2046_IRQ);

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320
#define FONT_SIZE 2

// Touchscreen coordinates: (x, y) and pressure (z)
int x, y, z;

TFT_eSprite background = TFT_eSprite(&tft);
TFT_eSprite cat = TFT_eSprite(&tft);

// Print Touchscreen info about X, Y and Pressure (Z) on the Serial Monitor
void printTouchToSerial(int touchX, int touchY, int touchZ) {
  Serial.print("X = ");
  Serial.print(touchX);
  Serial.print(" | Y = ");
  Serial.print(touchY);
  Serial.print(" | Pressure = ");
  Serial.print(touchZ);
  Serial.println();
}

int read_file(string path, uint16_t* buffer, streamsize file_size)
{
  fstream file(path, ios_base::in | ios_base::binary | ios_base::ate);

  if (!file) {
    // std::cerr << "Error opening file: " << filename << std::endl;
    Serial.print("File not found");
    return -1; // File open error
}

  file_size = file.tellg();
  file.seekg(0, ios::beg);
  
  size_t num_elements = file_size / 2;

  buffer = new uint16_t[num_elements];
  
  char* tmp_buffer = new char[file_size];
  if (!file) {
    Serial.print("Error reading file");
    // std::cerr << "Error reading file: " << filename << std::endl;
    delete[] buffer; // Clean up if an error occurs
    delete[] tmp_buffer;
    return -2; // Read error
  }

  file.read(tmp_buffer, file_size);

  for (int i = 0; i < num_elements; ++i) {
    buffer[i] = (static_cast<uint8_t>(tmp_buffer[2*i]) << 8) | static_cast<uint8_t>(tmp_buffer[2*i + 1]);
  }

  delete[] tmp_buffer;

  return 0;
}

void setup() {
  Serial.begin(115200);

  touchscreenSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  touchscreen.begin(touchscreenSPI);
  touchscreen.setRotation(0);

  tft.init();
  tft.setRotation(0);
  
  background.setColorDepth(8);
  background.createSprite(SCREEN_WIDTH, SCREEN_HEIGHT);

  // Init the cat sprite
  uint16_t* cat_data = nullptr;
  streamsize cat_size = 0;
  read_file("/face.raw", cat_data, cat_size);
  
  cat.pushImage(0, 0, 32, 32, cat_data);
  Serial.print(cat_size);
  
}


void handle_touch()
{
  if (touchscreen.touched()) {
    TS_Point p = touchscreen.getPoint();
    // Calibrate Touchscreen points with map function to the correct width and height
    x = map(p.x, 200, 3700, 1, SCREEN_WIDTH);
    y = map(p.y, 240, 3800, 1, SCREEN_HEIGHT);
    z = p.z;
    printTouchToSerial(x, y, z);
  }
}

void loop() {
  handle_touch();

  background.fillScreen(TFT_GREEN);
  
  cat.pushToSprite(&background, 20, 20);

  background.pushSprite(0, 0);


  delay(100);
}
