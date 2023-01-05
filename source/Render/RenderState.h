#pragma once

#include "RenderActor.h"

const float YOFFSET = 80;

class FighterGameState;

class RenderState
{
    Camera2D Cam = {};
	Camera Cam3D = {};
    RenderActor* RenderActors[406] = {};
public:
    RenderState(FighterGameState* InGameState);

    FighterGameState* GameState;
    void Init();
    void Draw();
	void UpdateCamera();
};