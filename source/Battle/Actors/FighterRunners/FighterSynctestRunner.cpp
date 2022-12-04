// Fill out your copyright notice in the Description page of Project Settings.


#include "FighterSynctestRunner.h"

#include "../FighterGameState.h"

FighterSynctestRunner::FighterSynctestRunner(FighterGameState* InGameState): FighterMultiplayerRunner(InGameState)
{
}

// Called when the game starts or when spawned
void FighterSynctestRunner::Init()
{
	GGPOSessionCallbacks cb = CreateCallbacks();
	ggpo_start_synctest(&ggpo, &cb, "NightSkyEngineLite", 2, sizeof(int), 6);
	ggpo_set_disconnect_timeout(ggpo, 45000);
	ggpo_set_disconnect_notify_start(ggpo, 15000);
	for (int i = 0; i < 2; i++)
	{
		GGPOPlayerHandle handle;
		GGPOPlayer* player = new GGPOPlayer();
		player->type = GGPO_PLAYERTYPE_LOCAL;
		player->player_num = i + 1;
		ggpo_add_player(ggpo, player, &handle);
		Players.push_back(player);
		PlayerInputIndex.push_back(i);
		PlayerHandles.push_back(handle);
	}
}

void FighterSynctestRunner::Update(float DeltaTime)
{
	ElapsedTime+=DeltaTime;
	while (ElapsedTime >= OneFrame)
	{
		GgpoUpdate();
		ElapsedTime -= OneFrame ;
	}
	ggpo_idle(ggpo,1);
}

