// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <cstdint>
#include <vector>

#include "FighterLocalRunner.h"
#include "include/ggponet.h"

constexpr int TimesyncMultiplier =4;

class FighterMultiplayerRunner : public FighterLocalRunner
{
public:
	FighterMultiplayerRunner(class FighterGameState* InGameState);
	void Init() override;
protected:
	// Called when the game starts or when spawned

	GGPOSessionCallbacks CreateCallbacks();
	bool __cdecl BeginGameCallback(const char*);
	bool __cdecl SaveGameStateCallback(unsigned char** buffer, int32_t* len, int32_t* checksum, int32_t);
	bool __cdecl LoadGameStateCallback(unsigned char* buffer, int32_t len);
	bool __cdecl LogGameState(const char* filename, unsigned char* buffer, int len);
	void __cdecl FreeBuffer(void* buffer);
	bool __cdecl AdvanceFrameCallback(int32_t);
	bool __cdecl OnEventCallback(GGPOEvent* info);

public:
	GGPOSession* ggpo = nullptr;
	std::vector<GGPOPlayerHandle> PlayerHandles;
	std::vector<GGPOPlayer*> Players;
	int PlayerIndex = 0;
	uint16_t LocalPort = 0;
	char IpAddress[32] = {0};
	uint16_t RemotePort = 0;

protected:
	std::vector<int> PlayerInputIndex;
	void GgpoUpdate();

	int MultipliedFramesAhead=0;
	int MultipliedFramesBehind=0;
	
public:	
	void Update(float DeltaTime) override;
	int	fletcher32_checksum(short* data, size_t len);
};
