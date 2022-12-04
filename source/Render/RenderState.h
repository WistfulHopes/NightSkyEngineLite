#pragma once

#include "RenderActor.h"

class FighterGameState;

class RenderState
{
    Camera2D Cam = {};
    RenderActor* RenderActors[406];
public:
    RenderState(FighterGameState* InGameState);

    FighterGameState* GameState;
    void Init();
    void Draw();
	void UpdateCamera();
};