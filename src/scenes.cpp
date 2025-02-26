#include <images.h>
#include <vector>

void loadOutside(std::vector<Sprite*> &sceneSprites)
{
    sceneSprites.clear();    
    sceneSprites.reserve(2);
    sceneSprites = { &spriteBackgroundOutside, &spriteDude };

    spriteDude.x = 100;
    spriteDude.y = 130;
}
