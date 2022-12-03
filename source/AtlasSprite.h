#pragma once

// Atlas sprite structure
struct AtlasSprite {
    char nameId[128];           // Sprite original filename (without extension)
    int originX, originY;       // Sprite origin (pivot point), useful in some cases
    int positionX, positionY;   // Sprite position in the atlas
    int sourceWidth;            // Sprite source width (before trim)
    int sourceHeight;           // Sprite source height (before trim)
    int padding;                // Sprite padding, must be added to source size
    int trimmed;                // Sprite is trimmed (removed blank space for better packing)
    int trimX, trimY, trimWidth, trimHeight; // Sprite trim rectangle 
    
    // Glyph info, in case sprite is a font character
    // NOTE: This data could probably be ommited 
    int value;                  // Character value (Unicode)
    int offsetX, offsetY;       // Character offset when drawing
    int advanceX;               // Character advance position X

    
    // Load sprite data from .rtpb file
    static AtlasSprite *LoadAtlasSprite(const char *rtpbFilename, int *spriteCount);
    
    // Load sprite data from .rtpb file
    static AtlasSprite *LoadAtlasSpriteFromData(unsigned char *data, int *spriteCount, int rtpbSize);

    // Get one sprite from the array by nameId
    static AtlasSprite *GetSprite(AtlasSprite *sprites, int spriteCount, const char *nameId);
};

