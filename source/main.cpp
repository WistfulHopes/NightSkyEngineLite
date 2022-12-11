#include <iostream>

#include "raylib.h"
#include "Battle/Actors/FighterGameState.h"
#include "Render/RenderState.h"
#include "FighterRunners/FighterSynctestRunner.h"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#define _WINDOWS_GGPO
#include "WindowsIncludeFix.h"
#endif

int main(int argc, char* argv[])
{
	if (argc != 5 && argc != 1 && argc != 2)
	{
		std::cout << "Incorrect arguments! Either launch the game with no parameters, or with the following syntax:" << std::endl;
		std::cout << "NightSkyEngineLite.exe <local_port>  ('local' | <remote ip> <remote port>)" << std::endl;
		return 1;
	}
    const int screenWidth = 320;
    const int screenHeight  = 240;
	const int windowWidth = screenWidth * 6;
    const int windowHeight  = screenHeight * 6;

    InitWindow(windowWidth, windowHeight, "Night Sky Engine -Lite-");

    SetTargetFPS(60);
	const RenderTexture2D renderTexture = LoadRenderTexture(screenWidth, screenHeight);

    FighterGameState* GameState = new FighterGameState();
    GameState->Init();
    
    RenderState* CurRenderState = new RenderState(GameState);
    CurRenderState->Init();
	
	FighterMultiplayerRunner* MultiplayerRunner = new FighterMultiplayerRunner(GameState);

	if (argc == 5)
	{
#ifdef _WINDOWS_GGPO
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2,2), &wsaData);
#endif
		MultiplayerRunner->LocalPort = (unsigned short)atoi(argv[1]);
		if (!strcmp(argv[2], "local"))
		{
			strcpy(MultiplayerRunner->IpAddress, argv[3]);
			MultiplayerRunner->RemotePort = (unsigned short)atoi(argv[4]);
			MultiplayerRunner->PlayerIndex = 0;
		}
		else
		{
			strcpy(MultiplayerRunner->IpAddress, argv[2]);
			MultiplayerRunner->RemotePort = (unsigned short)atoi(argv[3]);
			MultiplayerRunner->PlayerIndex = 1;
		}
		MultiplayerRunner->Init();
	}
	else
	{
		delete MultiplayerRunner;
	}
	
	FighterMultiplayerRunner* SynctestRunner = new FighterSynctestRunner(GameState);
	if (argc == 2)
	{
		if (!strcmp(argv[1], "synctest"))
		{
			SynctestRunner->Init();
		}
		else
		{
			std::cout << "Invalid argument! Argument should be synctest. No arguments to play without synctest." << std::endl;
			return 1;
		}
	}
	else
	{
		delete SynctestRunner;
	}
	
	double NewTime = 0;
    while (!WindowShouldClose())
    {
    	if (argc == 5)
    	{
    		double OldTime = NewTime;
    		NewTime = GetTime();
    		double DeltaTime = NewTime - OldTime;
    		MultiplayerRunner->Update(DeltaTime);
    	}
    	else if (argc == 2)
    	{
    		double OldTime = NewTime;
    		NewTime = GetTime();
    		double DeltaTime = NewTime - OldTime;
    		SynctestRunner->Update(DeltaTime);
    	}
		else
			// TODO: Capsulize all this into a "scene"
			GameState->TickGameState();
    	
        BeginDrawing();
			BeginTextureMode(renderTexture);
            	ClearBackground(RAYWHITE);
            	CurRenderState->Draw();
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

#ifdef _WINDOWS_GGPO
	if (argc == 5)
		WSACleanup();
#endif
        
    return 0;
}