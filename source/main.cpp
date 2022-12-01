#include "../raylib/src/raylib.h"
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
        BeginDrawing();

            ClearBackground(RAYWHITE);
            GameState->Draw();

        EndDrawing();
    }

    CloseWindow();

    return 0;
}