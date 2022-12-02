#pragma once

#include "../raylib/src/raylib.h"

typedef struct Sprite {
    char* name;
    Texture2D texture;
    Vector2 frameSize;
    int maxFrame;
    int framesWide;
    Vector2 origin;
    int frame;
 } Sprite;

