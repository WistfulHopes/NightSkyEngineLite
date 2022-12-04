#include "RenderActor.h"

#include "Battle/Actors/BattleActor.h"
#include "Utilities/CreateSpriteList.h"

#define RRES_IMPLEMENTATION
#include "../../rres/src/rres.h"

#define RRES_RAYLIB_IMPLEMENTATION
#define RRES_SUPPORT_COMPRESSION_LZ4
#define RRES_SUPPORT_ENCRYPTION_AES
#define RRES_SUPPORT_ENCRYPTION_XCHACHA20
#include "../../rres/src/rres-raylib.h"

RenderActor::RenderActor(BattleActor* InActor)
{
    Actor = InActor;
}

void RenderActor::SetSprite()
{
	if (strcmp(Actor->CelNameInternal.GetString(), ""))
	{
		for (int i = 0; i < Sprites.size(); i++)
		{
			AtlasSprite* TmpSprite = AtlasSprite::GetSprite(Sprites[i].Sprites.data(), Sprites[i].Sprites.size(), Actor->CelNameInternal.GetString());
			if (TmpSprite)
			{
				Sprites[i].CurrentSprite = *TmpSprite;
				CurrentSprite = Sprites[i];
			}
		}
	}
}

void RenderActor::LoadSprites(char* CharaName)
{
	int SpriteListLength;
	char SpriteListPath[256] = "Sprites/";
	strcat(SpriteListPath, CharaName);
	strcat(SpriteListPath, ".rres");
	rresCentralDir CentralDir = rresLoadCentralDirectory(SpriteListPath);	
	char SpriteListName[32];
	strcpy(SpriteListName, CharaName);
	strcat(SpriteListName, ".spls");
	int SpriteListID = rresGetResourceId(CentralDir, SpriteListName);
	rresResourceChunk SpriteListChunk = rresLoadResourceChunk(SpriteListPath, SpriteListID);
	UnpackResourceChunk(&SpriteListChunk);
	unsigned char* SpriteListData = (unsigned char*)LoadDataFromResource(SpriteListChunk, &SpriteListLength);
	SpriteList List;
	List.SpriteCount = *reinterpret_cast<uint32_t*>(SpriteListData + 4);
	for (uint32_t i = 0; i < List.SpriteCount; i++)
	{
		char* SpriteName = (char*)malloc(64);
		memcpy(SpriteName, SpriteListData + 8 + i * 64, 64);
		CString<64> FinalSpriteName;
		FinalSpriteName.SetString(SpriteName);
		List.NameList.push_back(FinalSpriteName);
	}
	rresUnloadResourceChunk(SpriteListChunk);
	for (uint32_t i = 0; i < List.SpriteCount; i++)
	{
		char PngName[68];
		strcpy(PngName, List.NameList[i].GetString());
		strcat(PngName, ".png");
		int PngID = rresGetResourceId(CentralDir, PngName);
		rresResourceChunk PngChunk = rresLoadResourceChunk(SpriteListPath, PngID);
		UnpackResourceChunk(&PngChunk);
		Image Png = LoadImageFromResource(PngChunk);
		Texture2D Tex = LoadTextureFromImage(Png);
		
		char AtlasName[69];
		strcpy(AtlasName, List.NameList[i].GetString());
		strcat(AtlasName, ".rtpb");
		int AtlasID = rresGetResourceId(CentralDir, AtlasName);
		rresResourceChunk AtlasChunk = rresLoadResourceChunk(SpriteListPath, AtlasID);
		UnpackResourceChunk(&AtlasChunk);
		int AtlasSize = 0;
		unsigned char* Atlas = (unsigned char*)LoadDataFromResource(AtlasChunk, &AtlasSize);
		int SpriteCount = 0;
		AtlasSprite* InSprites = AtlasSprite::LoadAtlasSpriteFromData(Atlas, &SpriteCount, AtlasSize);

		Sprite InSprite;
		InSprite.Atlas = Tex;
		for (int i = 0; i < SpriteCount; i++)
		{
			InSprite.Sprites.push_back(InSprites[i]);
		}
		InSprite.CurrentSprite = InSprite.Sprites[0];
		Sprites.push_back(InSprite);
		if (i == 0)
			CurrentSprite = InSprite;
		rresUnloadResourceChunk(PngChunk);
		rresUnloadResourceChunk(AtlasChunk);
	}
}

void RenderActor::Draw()
{
	Rectangle Source;
	Source.x = CurrentSprite.CurrentSprite.positionX;
	Source.y = CurrentSprite.CurrentSprite.positionY;
	if (Actor->FacingRight)
	{
		Source.width = CurrentSprite.CurrentSprite.trimWidth;
	}
	else
	{
		Source.width = -CurrentSprite.CurrentSprite.trimWidth;
	}
	Source.height = CurrentSprite.CurrentSprite.trimHeight;

	Rectangle Dest;
	Dest.x = Actor->GetInternalValue(VAL_PosX) / COORD_SCALE;
	Dest.y = -Actor->GetInternalValue(VAL_PosY) / COORD_SCALE;
	Dest.width = CurrentSprite.CurrentSprite.trimWidth;
	Dest.height = CurrentSprite.CurrentSprite.trimHeight;

	Vector2 Origin;
	if (Actor->FacingRight)
	{
		Origin.x = CurrentSprite.CurrentSprite.originX;
	}
	else
	{
		Origin.x = CurrentSprite.CurrentSprite.trimWidth - CurrentSprite.CurrentSprite.originX;
	}
	Origin.y = CurrentSprite.CurrentSprite.originY;
	
	DrawTexturePro(CurrentSprite.Atlas, Source, Dest, Origin, 0, RAYWHITE);
}