#include <switch.h>
#include "../raylib-nx/src/raylib.h"
#include "Battle/Actors/FighterGameState.h"

int main(void)
{
    romfsInit();
    
    const int screenWidth = 320;
    const int screenHeight  = 240;
	const int windowWidth = 960;
    const int windowHeight  = 720;

    InitWindow(windowWidth, windowHeight, "Night Sky Engine -Lite-");

    SetTargetFPS(60);
	const RenderTexture2D renderTexture = LoadRenderTexture(screenWidth, screenHeight);

    FighterGameState* GameState = new FighterGameState();
    GameState->Init();
    
    while (!WindowShouldClose())
    {
		// TODO: Capsulize all this into a "scene"
        GameState->TickGameState();

        BeginDrawing();
			BeginTextureMode(renderTexture);
            	ClearBackground(RAYWHITE);
            	GameState->Draw();
			EndTextureMode();

			// TODO: Keep aspect ratio and draw borders
			DrawTexturePro(
				renderTexture.texture,
				{ 0.0f, 0.0f, (float)screenWidth, -(float)screenHeight },
				{ 0.0f, 0.0f, (float)windowWidth, (float)windowHeight },
				{ 0.0f, 0.0f },
				0.0f, WHITE
			);

        EndDrawing();
    }

    CloseWindow();
    
    romfsExit();
    
    return 0;
}