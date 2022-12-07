#include "RenderState.h"

#include "InputDevice.h"
#include "InputDeviceRaylib.h"
#include "raymath.h"
#include "Battle/Actors/FighterGameState.h"

RenderState::RenderState(FighterGameState* InGameState)
{
    GameState = InGameState;
}

void RenderState::Init()
{
	GameState->InputDevices[0] = new ControllerInputDevice;
	dynamic_cast<ControllerInputDevice*>(GameState->InputDevices[0])->controller_id = 0;
	GameState->InputDevices[1] = new ControllerInputDevice;
	dynamic_cast<ControllerInputDevice*>(GameState->InputDevices[1])->controller_id = 1;
	GameState->InputDevices[2] = new KeyboardInputDevice;
	
	GameState->CommonScript = (char*)LoadFileData("Scripts/NSS_Common.nss", &GameState->CommonScriptLength);
    
    for (int i = 0; i < 400; i++)
    {
        RenderActors[i] = new RenderActor(GameState->Objects[i]);
    }
    
	for (int i = 400; i < 406; i++)
	{
		int CurIndex = i - 400;
		RenderActors[i] = new RenderActor(GameState->Players[CurIndex]);
		if ((CurIndex) % 3 == 0)
		{
			unsigned int CharaScriptBytes = 0;
			GameState->Players[CurIndex]->CharaScript = (char*)LoadFileData("Scripts/NSS_Esther.nss", &CharaScriptBytes);
			GameState->Players[CurIndex]->CharaScriptLength = CharaScriptBytes;
			unsigned int ObjScriptBytes = 0;
			GameState->Players[CurIndex]->ObjectScript = (char*)LoadFileData("Scripts/NSS_EstherObj.nss", &ObjScriptBytes);
			GameState->Players[CurIndex]->ObjectScriptLength = ObjScriptBytes;
			RenderActors[i]->LoadSprites("Esther");
			GameState->Players[CurIndex]->InitStates();
		}
	}
}

void RenderState::Draw()
{
	UpdateCamera();
	for (int i = 0; i < 406; i++)
	{
		if (!RenderActors[i]->Actor->IsPlayer && !RenderActors[i]->Actor->IsActive || !RenderActors[i]->Actor->Player->IsOnScreen)
			continue;
		if (!GameState->SortedObjects[i]->IsPlayer || GameState->SortedObjects[i]->Player->IsOnScreen)
		{
			RenderActors[i]->SetSprite();
			RenderActors[i]->Draw();
		}
	}
	EndMode2D();
}

void RenderState::UpdateCamera()
{
    Vector2 Offset;
    Offset.x = 160;
    Offset.y = 200;
    Cam.offset = Offset;

    double TargetX = static_cast<double>(GameState->Players[0]->GetInternalValue(VAL_PosX) + GameState->Players[3]->GetInternalValue(VAL_PosX)) / 2 / COORD_SCALE;
    double TargetY = -static_cast<double>(GameState->Players[0]->GetInternalValue(VAL_PosY) + GameState->Players[3]->GetInternalValue(VAL_PosY)) / 2 / COORD_SCALE;
    TargetX = Clamp(TargetX, -480, 480);
    Vector2 Target;
    Target.x = Lerp(Cam.target.x, TargetX, 0.1);
    Target.y = Lerp(Cam.target.y, TargetY, 0.1);
    Cam.target = Target;

    Cam.rotation = 0;

	float Distance;
	if (GameState->Players[0]->GetInternalValue(VAL_PosX) > GameState->Players[3]->GetInternalValue(VAL_PosX))
	{
		Distance = GameState->Players[0]->GetInternalValue(VAL_PosX) - GameState->Players[3]->GetInternalValue(VAL_PosX);
	}
	else
	{
		Distance = GameState->Players[3]->GetInternalValue(VAL_PosX) - GameState->Players[0]->GetInternalValue(VAL_PosX);
	}
	Distance = (2160000 - Distance) / COORD_SCALE + 900;
	Distance = Clamp(Distance, 1080, 1440);
	Distance = Remap(Distance, 1080, 1440, 0.48f, 1);
	if (Cam.zoom == 0)
	    Cam.zoom = 1.5;
    Cam.zoom = Lerp(Cam.zoom, Distance, 0.5f);

	BeginMode2D(Cam);
}