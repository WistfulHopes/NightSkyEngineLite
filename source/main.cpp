#include "../raylib/src/raylib.h"
#include "../raylib/src/raymath.h"
#include "Battle/Actors/FighterGameState.h"

int main(void)
{
    const int screenWidth = 320;
    const int screenHeight  = 240;
	const int windowWidth = screenWidth * 2;
    const int windowHeight  = screenHeight * 2;

    InitWindow(windowWidth, windowHeight, "Night Sky Engine -Lite-");

    SetTargetFPS(60);
	const RenderTexture2D renderTexture = LoadRenderTexture(screenWidth, screenHeight);

    FighterGameState* GameState = new FighterGameState();
    GameState->Init();
    
    while (!WindowShouldClose())
    {
		// TODO: Capsulize all this into a "scene"
        GameState->TickGameState();

        Camera2D Cam;

        Vector2 Offset;
        Offset.x = 150;
        Offset.y = 100;
        Cam.offset = Offset;

        double TargetX = static_cast<double>(GameState->StoredBattleState.CurrentScreenPos) / COORD_SCALE;
        double TargetY = -static_cast<double>(GameState->Players[0]->GetInternalValue(VAL_PosY) + GameState->Players[1]->GetInternalValue(VAL_PosY)) / 2 / COORD_SCALE;
        TargetX = Clamp(TargetX, -1440, 1440);
        Vector2 Target;
        Target.x = TargetX;
        Target.y = TargetY;
        Cam.target = Target;

        Cam.rotation = 0;

		float Distance = (2160000 - abs(abs(GameState->Players[0]->GetInternalValue(VAL_PosX)) - abs(GameState->Players[3]->GetInternalValue(VAL_PosX)))) / COORD_SCALE + 720;
		Distance = Clamp(Distance, 1080, 1440);
    	Cam.zoom = Remap(Distance, 1080, 1440, 0.525, 1);
        
        BeginDrawing();
			BeginTextureMode(renderTexture);
            	ClearBackground(RAYWHITE);

            	BeginMode2D(Cam);
            		GameState->Draw();
            	EndMode2D();
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

    return 0;
}