#include <images.h>
#include <vector>
#include <XPT2046_Touchscreen.h>

extern uint32_t lastTouched;
void printTouchToSerial(int touchX, int touchY, int touchZ);
void handleTouch(XPT2046_Touchscreen &touchscreen, std::vector<Sprite*> &sceneSprites);
bool spriteTouched(int x, int y, Sprite *sprite);
void onTouch(Sprite *sprite);
void onRelease(Sprite *sprite);
