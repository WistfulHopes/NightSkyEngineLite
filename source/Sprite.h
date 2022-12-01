#include "../raylib/src/raylib.h"

typedef struct Sprite {
     Texture2D texture;
     Vector2 frameSize;
     int maxFrame;
     int framesWide;
     Vector2 origin;
     int frame;
 } Sprite;

