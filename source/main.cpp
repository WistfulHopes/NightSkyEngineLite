#include "../raylib/src/raylib.h"
#include "Battle/Actors/FighterGameState.h"

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight  = 480;

    FighterGameState* GameState = new FighterGameState();

    InitWindow(screenWidth, screenHeight, "Night Sky Engine -Lite-");

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();

            ClearBackground(RAYWHITE);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}