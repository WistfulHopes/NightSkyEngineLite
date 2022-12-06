#pragma once
#include <vector>

#include "raylib.h"
#include "../AtlasSprite.h"
#include "Battle/Actors/BattleActor.h"

#define COORD_SCALE 3000.f

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

	void CreateCommonParticleCallback(char* Name, PosType PosType, Vector Offset = Vector(0, 0), int32_t Angle = 0);
	void CreateCharaParticleCallback(char* Name, PosType PosType, Vector Offset = Vector(0, 0), int32_t Angle = 0);
	void LinkCharaParticleCallback(char* Name);
	void PlayCommonSoundCallback(char* Name);
	void PlayCharaSoundCallback(char* Name);
	void PlayVoiceCallback(char* Name);
	void PlayCommonCameraAnimCallback(char* Name);
	void PlayCharaCameraAnimCallback(char* Name);
	void BattleHudVisibilityCallback(bool Visible);

public:
    RenderActor(BattleActor* InActor);
    BattleActor* Actor;

	void CreateCallbacks();

	//updates current sprite
	void SetSprite();
    //loads sprites
	void LoadSprites(char* CharaName);
    //draws the object. called every frame
	void Draw(); 
};