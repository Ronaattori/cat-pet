#include <images.h>
#include <XPT2046_Touchscreen.h>
#include <vector>

uint32_t lastTouched = 0;
Sprite *touchingSprite = nullptr;

void onTouch(Sprite *sprite)
{
  if (sprite == &spriteDude) {
    spriteDude.image = imageDudeFreak;
    spriteDude.frames = 1;
  }
}

void onRelease(Sprite *sprite)
{
  if (sprite == &spriteDude) {
    spriteDude.image = imageDudeIdle;
    spriteDude.frames = 2;
  }
}

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

bool spriteTouched(int x, int y, Sprite *sprite)
{
  int startX = sprite->x;
  int endX = startX * sprite->pixelScale;
  int startY = sprite->y;
  int endY = startY * sprite->pixelScale;
  if (x > startX && x < endX) {
    if (y > startY && y < endY) {
      return true;
    }
  }
  return false;
}

void handleTouch(XPT2046_Touchscreen &touchscreen, std::vector<Sprite*> &sceneSprites)
{
  uint32_t ms = millis();
  if (!touchscreen.touched()){
    // A touch will count as released after 200ms
    if (touchingSprite && ms - lastTouched > 200) {
      onRelease(touchingSprite);
      touchingSprite = nullptr;
    }
    return;   
  }

  // Need to wait 300ms until a new touch is processed
  if (ms - lastTouched < 300) {
    lastTouched = ms;
    return;
  }

  TS_Point p = touchscreen.getPoint();
  // Calibrate Touchscreen points with map function to the correct width and height
  int x = map(p.x, 200, 3700, 1, SCREEN_WIDTH);
  int y = map(p.y, 240, 3800, 1, SCREEN_HEIGHT);
  int z = p.z;
  printTouchToSerial(x, y, z);

  for (Sprite *sprite : sceneSprites) {
    if (spriteTouched(x, y, sprite)) {
      touchingSprite = sprite;
    }
  }
  if (touchingSprite == nullptr) return;

  onTouch(touchingSprite);
  lastTouched = millis();
}