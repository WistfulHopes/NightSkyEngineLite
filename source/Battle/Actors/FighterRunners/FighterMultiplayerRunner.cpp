// Fill out your copyright notice in the Description page of Project Settings.

#include "FighterMultiplayerRunner.h"

#include <iostream>
#include <string>

#include "../FighterGameState.h"

constexpr int MaxRollbackFrames = 10;

FighterMultiplayerRunner::FighterMultiplayerRunner(FighterGameState* InGameState): FighterLocalRunner(InGameState)
{
	
}

// Called when the game starts or when spawned
void FighterMultiplayerRunner::Init()
{
	GGPOSessionCallbacks cb = CreateCallbacks();
	ggpo_start_session(&ggpo, &cb, "NightSkyEngineLite",2, sizeof(int), LocalPort);
	ggpo_set_disconnect_timeout(ggpo, 45000);
	ggpo_set_disconnect_notify_start(ggpo, 15000);
	for (int i = 0; i < 2; i++)
	{
		GGPOPlayerHandle handle;
		GGPOPlayer* player = new GGPOPlayer();
		player->type = GGPO_PLAYERTYPE_REMOTE;
		if(i == PlayerIndex)
		{
			player->type = GGPO_PLAYERTYPE_LOCAL;
		}
		else
		{
			strcpy(player->u.remote.ip_address, IpAddress);
			player->u.remote.port = RemotePort;
		}
		player->player_num = i + 1;
		ggpo_add_player(ggpo, player, &handle);
		if(player->type == GGPO_PLAYERTYPE_LOCAL)
			ggpo_set_frame_delay(ggpo,handle,2);// TODO: Logic for framedelay (based on ping? or make user choose?)
		Players.push_back(player);
		PlayerInputIndex.push_back(-1);
		PlayerHandles.push_back(handle);
	}
}

GGPOSessionCallbacks FighterMultiplayerRunner::CreateCallbacks()
{
	GGPOSessionCallbacks cb = {0};

	cb.begin_game = std::bind(&FighterMultiplayerRunner::BeginGameCallback, this, std::placeholders::_1);
	cb.save_game_state = std::bind(&FighterMultiplayerRunner::SaveGameStateCallback, this,
								   std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
								   std::placeholders::_4);
	cb.load_game_state = std::bind(&FighterMultiplayerRunner::LoadGameStateCallback, this,
								   std::placeholders::_1, std::placeholders::_2);
	cb.log_game_state = std::bind(&FighterMultiplayerRunner::LogGameState, this,
								  std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	cb.free_buffer = std::bind(&FighterMultiplayerRunner::FreeBuffer, this, std::placeholders::_1);
	cb.advance_frame = std::bind(&FighterMultiplayerRunner::AdvanceFrameCallback, this, std::placeholders::_1);
	cb.on_event = std::bind(&FighterMultiplayerRunner::OnEventCallback, this, std::placeholders::_1);

	return cb;
}

bool FighterMultiplayerRunner::BeginGameCallback(const char*)
{
	return true;
}

bool FighterMultiplayerRunner::SaveGameStateCallback(unsigned char** buffer, int32_t* len, int32_t* checksum, int32_t)
{
	GameState->SaveGameState();
	RollbackData rollbackdata = GameState->StoredRollbackData;
	*len = sizeof(RollbackData);
	*buffer = new unsigned char[*len];
	memcpy(*buffer, &rollbackdata, *len);
	*checksum = fletcher32_checksum((short*)*buffer, *len / 2);
	return true;
}

bool FighterMultiplayerRunner::LoadGameStateCallback(unsigned char* buffer, int32_t len)
{
	RollbackData* rollbackdata = &GameState->StoredRollbackData;
	memcpy(rollbackdata, buffer, len);
	GameState->LoadGameState();
	return true;
}

bool FighterMultiplayerRunner::LogGameState(const char* filename, unsigned char* buffer, int len)
{
	FILE* fp = nullptr;
	fopen_s(&fp, filename, "w");
	if (fp)
	{
		int BackupFrame = GameState->LocalFrame % MaxRollbackFrames;
		RollbackData* rollbackdata = (RollbackData*)malloc(sizeof(RollbackData));
		memcpy(rollbackdata, buffer, sizeof(RollbackData));
		fprintf(fp, "GameState:\n");
		BattleState InBattleState;
		memcpy(&InBattleState, rollbackdata->BattleStateBuffer, SIZEOF_BATTLESTATE);
		fprintf(fp, "\tFrameNumber:%d\n", InBattleState.FrameNumber);
		fprintf(fp, "\tActiveObjectCount:%d\n", rollbackdata->ActiveObjectCount);
		for (int i = 0; i < 400; i++)
		{
			if (rollbackdata->ObjActive[i])
			{
				BattleActor* CurBattleActor = new BattleActor;
				memcpy((char*)CurBattleActor + offsetof(BattleActor, ObjSync), rollbackdata->ObjBuffer[i], SIZEOF_BATTLEACTOR);
				CurBattleActor->LogForSyncTest(fp);
				delete CurBattleActor;
			}
		}
		for (int i = 400; i < 406; i++)
		{
			PlayerCharacter* CurPlayerCharacter = new PlayerCharacter;
			memcpy((char*)CurPlayerCharacter + offsetof(BattleActor, ObjSync), rollbackdata->ObjBuffer[i], SIZEOF_BATTLEACTOR);
			memcpy((char*)CurPlayerCharacter + offsetof(PlayerCharacter, PlayerSync), rollbackdata->CharBuffer[i - 400], SIZEOF_PLAYERCHARACTER);
			CurPlayerCharacter->LogForSyncTest(fp);
			delete CurPlayerCharacter;
		}

		fprintf(fp,"RawRollbackData:\n");
		fprintf(fp, "\tObjBuffer:\n");
		for (int i = 0; i < 406; i++)
		{
			fprintf(fp, "Object %d\n", i);
			for (int x = 0; x < SIZEOF_BATTLEACTOR; x++)
			{
				fprintf(fp, " %x", rollbackdata->ObjBuffer[i][x]);	
			}
			fprintf(fp, "\n");
		}
		fprintf(fp, "\n");
		fprintf(fp, "\tObjActive:\n");
		for (int i = 0; i < 400; i++)
		{
			fprintf(fp, " %x", rollbackdata->ObjActive[i]);
		}
		fprintf(fp, "\n");
		fprintf(fp, "\tCharBuffer:\n");
		for (int i = 0; i < 6; i++)
		{
			fprintf(fp, "Character %d\n", i);
			for (int x = 0; x < SIZEOF_PLAYERCHARACTER; x++)
			{
				fprintf(fp, " %x", rollbackdata->CharBuffer[i][x]);	
			}
			fprintf(fp, "\n");
		}
		fprintf(fp, "\n");

		int checksum = fletcher32_checksum((short*)buffer, sizeof(RollbackData) / 2);
		fprintf(fp,"RawBuffer:\n");
		fprintf(fp, "\tFletcher32Checksum:%d\n", checksum);
		fprintf(fp, "\tBuffer:\n\t0: ");
		for (int i = 0; i < sizeof(RollbackData); i++)
		{
			fprintf(fp, " %x", buffer[i]);
			if((i%20)==0)
			{
				fprintf(fp, "\n\t%x: ",i);
			}
		}
		
		delete[] buffer;
		fclose(fp);
	}
	return true;
}

void FighterMultiplayerRunner::FreeBuffer(void* buffer)
{
	delete[] buffer;
}

bool FighterMultiplayerRunner::AdvanceFrameCallback(int flag)
{
	int inputs[2] = {0};
	int disconnect_flags;
	ggpo_synchronize_input(ggpo, (void*)inputs, sizeof(int) * 2, &disconnect_flags);
	GameState->Update(inputs[0], inputs[1]);
	ggpo_advance_frame(ggpo);
	return true;
}

bool FighterMultiplayerRunner::OnEventCallback(GGPOEvent* info)
{
	int progress;
	switch (info->code)
	{
	case GGPO_EVENTCODE_CONNECTED_TO_PEER:
		std::cout << "GGPO_EVENTCODE_CONNECTED_TO_PEER" << std::endl;
		break;
	case GGPO_EVENTCODE_SYNCHRONIZING_WITH_PEER:
		std::cout << "GGPO_EVENTCODE_SYNCHRONIZING_WITH_PEER" << std::endl;
	//synchronized = false;
		progress = 100 * info->u.synchronizing.count / info->u.synchronizing.total;
		break;
	case GGPO_EVENTCODE_SYNCHRONIZED_WITH_PEER:
		std::cout << "GGPO_EVENTCODE_SYNCHRONIZED_WITH_PEER" << std::endl;
	//synchronized = true;
		break;
	case GGPO_EVENTCODE_RUNNING:
		std::cout << "GGPO_EVENTCODE_RUNNING" << std::endl;
		break;
	case GGPO_EVENTCODE_CONNECTION_INTERRUPTED:
		std::cout << "GGPO_EVENTCODE_CONNECTION_INTERRUPTED" << std::endl;
	// connectionLost = true;
	// FightGameInstance->ErrorMessage = FString("Connection interrupted");
	// EndOnline(true);
		break;
	case GGPO_EVENTCODE_CONNECTION_RESUMED:
		//connectionLost = false;
		std::cout << "GGPO_EVENTCODE_CONNECTION_RESUMED" << std::endl;
		break;
	case GGPO_EVENTCODE_DISCONNECTED_FROM_PEER:
		std::cout << "GGPO_EVENTCODE_DISCONNECTED_FROM_PEER" << std::endl;
	//FightGameInstance->ErrorMessage = FString("Enemy disconnected");
	//EndOnline(true);
		break;
	case GGPO_EVENTCODE_TIMESYNC:
		std::cout << "GGPO_EVENTCODE_TIMESYNC" << std::endl;
		//if(MultipliedFramesAhead>3)
		{
			MultipliedFramesAhead=info->u.timesync.frames_ahead*TimesyncMultiplier;
		}
		break;
	}
	return true;
}


void FighterMultiplayerRunner::GgpoUpdate()
{
	GGPOErrorCode result = GGPOErrorCode::GGPO_OK;
	int disconnect_flags;
	int inputs[2] = {0};
	for (int i = 0; i < 2; ++i)
	{
		if (Players[i]->type == GGPO_PLAYERTYPE_LOCAL)
		{
			int input = 0;
			if (PlayerInputIndex[i] == -1)
			{
				for (int index = 0; index < 4; index++)
				{
					input |= GameState->GetLocalInputs(index);
				}
			}
			else
			{
				input = GameState->GetLocalInputs(PlayerInputIndex[i]);
			}

			result = ggpo_add_local_input(ggpo, PlayerHandles[i], &input, sizeof(input));
		}
	}
	if (GGPO_SUCCEEDED(result))
	{
		result = ggpo_synchronize_input(ggpo, (void*)inputs, sizeof(int) * 2, &disconnect_flags);
		if (GGPO_SUCCEEDED(result))
		{
			GameState->Update(inputs[0], inputs[1]);
			ggpo_advance_frame(ggpo);
		}
	}
}

void FighterMultiplayerRunner::Update(float DeltaTime)
{
	ElapsedTime += DeltaTime;
	int accumulatorBreaker = 0;
	
	while (ElapsedTime >= OneFrame && accumulatorBreaker < AccumulatorBreakerMax)
	{
		if(MultipliedFramesAhead>0)
		{
			int ahead = MultipliedFramesAhead;
			MultipliedFramesAhead--;
			if((ahead%TimesyncMultiplier)==0)
			{
				ElapsedTime=0;
				break;
			}
		}
		GgpoUpdate();
		ElapsedTime -= OneFrame;
		accumulatorBreaker++;
		// if(MultipliedFramesBehind>0)
		// {
		// 	MultipliedFramesBehind--;
		// 	if(MultipliedFramesBehind%TimesyncMultiplier)
		// 	{
		// 		accumulatorBreaker=0;
		// 		ElapsedTime += ONE_FRAME;
		// 	}	
		// }
		//
	}
	ggpo_idle(ggpo,1);
}


//TODO refactor to a static libaray
int
FighterMultiplayerRunner::fletcher32_checksum(short* data, size_t len)
{
	int sum1 = 0xffff, sum2 = 0xffff;

	while (len)
	{
		size_t tlen = len > 360 ? 360 : len;
		len -= tlen;
		do
		{
			sum1 += *data++;
			sum2 += sum1;
		}
		while (--tlen);
		sum1 = (sum1 & 0xffff) + (sum1 >> 16);
		sum2 = (sum2 & 0xffff) + (sum2 >> 16);
	}

	/* Second reduction step to reduce sums to 16 bits */
	sum1 = (sum1 & 0xffff) + (sum1 >> 16);
	sum2 = (sum2 & 0xffff) + (sum2 >> 16);
	return sum2 << 16 | sum1;
}
