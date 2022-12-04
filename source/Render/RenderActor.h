#pragma once
#include <vector>

#include "raylib.h"
#include "../AtlasSprite.h"

class BattleActor;

struct Sprite
{
	Texture2D Atlas;
	std::vector<AtlasSprite> Sprites;
	AtlasSprite CurrentSprite;
};

class RenderActor 
{
	std::vector<Sprite> Sprites;
	Sprite CurrentSprite;

public:
    RenderActor(BattleActor* InActor);
    BattleActor* Actor;
    
	//updates current sprite
	void SetSprite();
    //loads sprites
	void LoadSprites(char* CharaName);
    //draws the object. called every frame
	void Draw(); 
};