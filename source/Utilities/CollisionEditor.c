/*******************************************************************************************
*
*   Night Sky Engine -Lite- Collision Editor v1.0.0 - Tool Description
*
*   LICENSE: Propietary License
*
*   Copyright (c) 2022 WistfulHopes. All Rights Reserved.
*
*   Unauthorized copying of this file, via any medium is strictly prohibited
*   This project is proprietary and confidential unless the owner allows
*   usage in any other form by expresely written permission.
*
**********************************************************************************************/

#include "../../raylib/src/raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui/src/raygui.h"

//----------------------------------------------------------------------------------
// Controls Functions Declaration
//----------------------------------------------------------------------------------


//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main()
{
    // Initialization
    //---------------------------------------------------------------------------------------
    int screenWidth = 800;
    int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Night Sky Engine -Lite- Collision Editor");

    // Night Sky Engine -Lite- Collision Editor: controls initialization
    //----------------------------------------------------------------------------------
    bool WindowBox000Active = true;
    bool TextBox004EditMode = false;
    char TextBox004Text[128] = "0";
    bool TextBox004EditMode = false;
    char TextBox004Text[128] = "0";
    bool TextBox005EditMode = false;
    char TextBox005Text[128] = "0";
    bool TextBox008EditMode = false;
    char TextBox008Text[128] = "0";
    //----------------------------------------------------------------------------------

    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Implement required update logic
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR))); 

            // raygui: controls drawing
            //----------------------------------------------------------------------------------
            if (WindowBox000Active)
            {
                WindowBox000Active = !GuiWindowBox((Rectangle){ 256, 128, 640, 480 }, "Night Sky Engine -Lite- Collision Editor");
                GuiLabel((Rectangle){ 288, 170, 120, 24 }, "X Position");
                GuiLabel((Rectangle){ 288, 216, 120, 24 }, "Y Position");
                if (GuiTextBox((Rectangle){ 288, 240, 120, 24 }, TextBox004Text, 128, TextBox004EditMode)) TextBox004EditMode = !TextBox004EditMode;
                if (GuiTextBox((Rectangle){ 288, 192, 120, 24 }, TextBox004Text, 128, TextBox004EditMode)) TextBox004EditMode = !TextBox004EditMode;
                if (GuiTextBox((Rectangle){ 288, 289, 120, 24 }, TextBox005Text, 128, TextBox005EditMode)) TextBox005EditMode = !TextBox005EditMode;
                GuiLabel((Rectangle){ 288, 265, 120, 24 }, "Width");
                GuiLabel((Rectangle){ 288, 313, 120, 24 }, "Height");
                if (GuiTextBox((Rectangle){ 288, 336, 120, 24 }, TextBox008Text, 128, TextBox008EditMode)) TextBox008EditMode = !TextBox008EditMode;
                GuiPanel((Rectangle){ 431, 191, 120, 32 }, NULL);
            }
            GuiGroupBox((Rectangle){ 288, 384, 120, 32 }, "SAMPLE TEXT");
            //----------------------------------------------------------------------------------

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

//------------------------------------------------------------------------------------
// Controls Functions Definitions (local)
//------------------------------------------------------------------------------------

