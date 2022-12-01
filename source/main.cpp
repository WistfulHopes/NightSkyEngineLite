#include "../raylib/src/raylib.h"
#include "raymath.h"
#include "Battle/Actors/FighterGameState.h"

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight  = 480;

    InitWindow(screenWidth, screenHeight, "Night Sky Engine -Lite-");

    SetTargetFPS(60);

    FighterGameState* GameState = new FighterGameState();
    GameState->Init();
    
    while (!WindowShouldClose())
    {
        GameState->TickGameState();

        Camera2D Cam;

        Vector2 Offset;
        Offset.x = 0;
        Offset.y = 0;
        Cam.offset = Offset;

        double TargetX = static_cast<double>(GameState->StoredBattleState.CurrentScreenPos) / COORD_SCALE;
        double TargetY = static_cast<double>(GameState->Players[0]->GetInternalValue(VAL_PosY) + GameState->Players[1]->GetInternalValue(VAL_PosY)) / 2 / COORD_SCALE;
        TargetX = Clamp(TargetX, 0, 720);
        Vector2 Target;
        Target.x = TargetX;
        Target.y = TargetY;
        Cam.target = Target;

        Cam.rotation = 0;
        Cam.zoom = 0;
        
        BeginDrawing();
        
            ClearBackground(RAYWHITE);

            BeginMode2D(Cam);
            GameState->Draw();
            EndMode2D();

        EndDrawing();
    }

    CloseWindow();

    return 0;
}