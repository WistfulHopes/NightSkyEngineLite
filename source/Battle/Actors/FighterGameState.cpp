#include "FighterGameState.h"
#include "InputDevice.h"

void FighterGameState::TickGameState()
{
	RemoteFrame++;
	LocalFrame++;
	UpdateLocalInput();
	Update(LocalInputs[0], LocalInputs[1]);
}

int FighterGameState::GetLocalInputs(int Index)
{
	if (Index > 2) return 0;
#ifdef RAYLIB_H
	if (Index < 2)
		if (IsGamepadAvailable(dynamic_cast<ControllerInputDevice*>(InputDevices[Index])->controller_id))
			return InputDevices[Index]->GetInputs();
#endif
	if (Index == 0)
		return InputDevices[2]->GetInputs();
	return 0;
}

void FighterGameState::UpdateLocalInput()
{
	LocalInputs[0] = GetLocalInputs(0);
	LocalInputs[1] = GetLocalInputs(1);
}

void FighterGameState::UpdateRemoteInput(int32_t RemoteInput, int32_t InFrame, int32_t InFrameAdvantage)
{
	if (CurrentNetMode == Player1)
	{
		RemoteInputs[1] = RemoteInput;
	}
	else
	{
		RemoteInputs[0] = RemoteInput;
	}
}

void FighterGameState::HandleRoundWin()
{
	if (StoredBattleState.RoundFormat < RoundFormat::TwoVsTwo)
	{
		if (Players[0]->CurrentHealth > 0 && Players[3]->CurrentHealth <= 0)
		{
			if (!Players[0]->RoundWinInputLock)
				StoredBattleState.P1RoundsWon++;
			Players[0]->RoundWinTimer--;
			Players[0]->RoundWinInputLock = true;
			StoredBattleState.PauseTimer = true;
			if (Players[0]->RoundWinTimer <= 0)
			{
				StoredBattleState.PauseTimer = false;
				HandleMatchWin();
				RoundInit();
			}
		}
		else if (Players[3]->CurrentHealth > 0 && Players[0]->CurrentHealth <= 0)
		{
			if (!Players[3]->RoundWinInputLock)
				StoredBattleState.P2RoundsWon++;
			Players[3]->RoundWinTimer--;
			Players[3]->RoundWinInputLock = true;
			StoredBattleState.PauseTimer = true;
			if (Players[3]->RoundWinTimer <= 0)
			{
				StoredBattleState.PauseTimer = false;
				HandleMatchWin();
				RoundInit();
			}
		}
		else if (Players[0]->CurrentHealth <= 0 && Players[3]->CurrentHealth <= 0)
		{
			if (!Players[0]->RoundWinInputLock)
			{
				StoredBattleState.P1RoundsWon++;
				StoredBattleState.P2RoundsWon++;
			}
			Players[0]->RoundWinInputLock = true;
			Players[0]->RoundWinTimer--;
			StoredBattleState.PauseTimer = true;
			if (Players[0]->RoundWinTimer <= 0)
			{
				StoredBattleState.PauseTimer = false;
				HandleMatchWin();
				RoundInit();
			}
		}
		else if (StoredBattleState.RoundTimer <= 0)
		{
			if (Players[0]->CurrentHealth > 0)
			{
				if (!Players[0]->RoundWinInputLock)
					StoredBattleState.P1RoundsWon++;
				Players[0]->RoundWinTimer--;
				Players[0]->RoundWinInputLock = true;
				StoredBattleState.PauseTimer = true;
				if (Players[0]->RoundWinTimer <= 0)
				{
					StoredBattleState.PauseTimer = false;
					HandleMatchWin();
					RoundInit();
				}
			}
			else if (Players[3]->CurrentHealth > 0)
			{
				if (!Players[3]->RoundWinInputLock)
					StoredBattleState.P2RoundsWon++;
				Players[3]->RoundWinTimer--;
				Players[3]->RoundWinInputLock = true;
				StoredBattleState.PauseTimer = true;
				if (Players[3]->RoundWinTimer <= 0)
				{
					StoredBattleState.PauseTimer = false;
					HandleMatchWin();
					RoundInit();
				}
			}
			else if (Players[0]->CurrentHealth == Players[3]->CurrentHealth)
			{
				if (!Players[0]->RoundWinInputLock)
				{
					StoredBattleState.P1RoundsWon++;
					StoredBattleState.P2RoundsWon++;
				}
				Players[0]->RoundWinInputLock = true;
				Players[0]->RoundWinTimer--;
				StoredBattleState.PauseTimer = true;
				if (Players[0]->RoundWinTimer <= 0)
				{
					StoredBattleState.PauseTimer = false;
					HandleMatchWin();
					RoundInit();
				}
			}
		}
	}
}

void FighterGameState::HandleMatchWin()
{
	switch (StoredBattleState.RoundFormat)
	{
	case RoundFormat::FirstToOne:
		if (StoredBattleState.P1RoundsWon > 0 && StoredBattleState.P2RoundsWon < StoredBattleState.P1RoundsWon)
		{
			//UGameplayStatics::OpenLevel(GetGameInstance(), FName(TEXT("Title")));
		}
		else if (StoredBattleState.P2RoundsWon > 0 && StoredBattleState.P1RoundsWon < StoredBattleState.P2RoundsWon)
		{
			//UGameplayStatics::OpenLevel(GetGameInstance(), FName(TEXT("Title")));
		}
		else if (StoredBattleState.P1RoundsWon == 2 && StoredBattleState.P2RoundsWon == 2)
		{
			//UGameplayStatics::OpenLevel(GetGameInstance(), FName(TEXT("Title")));
		}
		return;
	case RoundFormat::FirstToTwo:
		if (StoredBattleState.P1RoundsWon > 1 && StoredBattleState.P2RoundsWon < StoredBattleState.P1RoundsWon)
		{
			//UGameplayStatics::OpenLevel(GetGameInstance(), FName(TEXT("Title")));
		}
		else if (StoredBattleState.P2RoundsWon > 1 && StoredBattleState.P1RoundsWon < StoredBattleState.P2RoundsWon)
		{
			//UGameplayStatics::OpenLevel(GetGameInstance(), FName(TEXT("Title")));
		}
		else if (StoredBattleState.P1RoundsWon == 3 && StoredBattleState.P2RoundsWon == 3)
		{
			//UGameplayStatics::OpenLevel(GetGameInstance(), FName(TEXT("Title")));
		}
		return;
	case RoundFormat::FirstToThree:
		if (StoredBattleState.P1RoundsWon > 2 && StoredBattleState.P2RoundsWon < StoredBattleState.P1RoundsWon)
		{
			//UGameplayStatics::OpenLevel(GetGameInstance(), FName(TEXT("Title")));
		}
		else if (StoredBattleState.P2RoundsWon > 2 && StoredBattleState.P1RoundsWon < StoredBattleState.P2RoundsWon)
		{
			//UGameplayStatics::OpenLevel(GetGameInstance(), FName(TEXT("Title")));
		}
		else if (StoredBattleState.P1RoundsWon == 4 && StoredBattleState.P2RoundsWon == 4)
		{
			//UGameplayStatics::OpenLevel(GetGameInstance(), FName(TEXT("Title")));
		}
		return;
	case RoundFormat::FirstToFour:
		if (StoredBattleState.P1RoundsWon > 3 && StoredBattleState.P2RoundsWon < StoredBattleState.P1RoundsWon)
		{
			//UGameplayStatics::OpenLevel(GetGameInstance(), FName(TEXT("Title")));
		}
		else if (StoredBattleState.P2RoundsWon > 3 && StoredBattleState.P1RoundsWon < StoredBattleState.P2RoundsWon)
		{
			//UGameplayStatics::OpenLevel(GetGameInstance(), FName(TEXT("Title")));
		}
		else if (StoredBattleState.P1RoundsWon == 5 && StoredBattleState.P2RoundsWon == 5)
		{
			//UGameplayStatics::OpenLevel(GetGameInstance(), FName(TEXT("Title")));
		}
		return;
	case RoundFormat::FirstToFive:
		if (StoredBattleState.P1RoundsWon > 4 && StoredBattleState.P2RoundsWon < StoredBattleState.P1RoundsWon)
		{
			//UGameplayStatics::OpenLevel(GetGameInstance(), FName(TEXT("Title")));
		}
		else if (StoredBattleState.P2RoundsWon > 4 && StoredBattleState.P1RoundsWon < StoredBattleState.P2RoundsWon)
		{
			//UGameplayStatics::OpenLevel(GetGameInstance(), FName(TEXT("Title")));
		}
		else if (StoredBattleState.P1RoundsWon == 6 && StoredBattleState.P2RoundsWon == 6)
		{
			//UGameplayStatics::OpenLevel(GetGameInstance(), FName(TEXT("Title")));
		}
		return;
	default:
		return;
	}
}

void FighterGameState::RoundInit()
{
	for (int i = 0; i < 400; i++)
		Objects[i]->ResetObject();
	
	for (int i = 0; i < 6; i++)
		Players[i]->ResetForRound();

	Players[0]->IsOnScreen = true;
	Players[3]->IsOnScreen = true;

	StoredBattleState.RoundTimer = 99 * 60;
	StoredBattleState.CurrentScreenPos = 0;
}

void FighterGameState::SortObjects()
{
	ActiveObjectCount = 6;
	for (int i = 6; i < 406; i++)
	{
		for (int j = i + 1; j < 406; j++)
		{
			if (SortedObjects[j]->IsActive && !SortedObjects[i]->IsActive)
			{
				BattleActor* Temp = SortedObjects[i];
				SortedObjects[i] = SortedObjects[j];
				SortedObjects[j] = Temp;
			}
		}
		if (SortedObjects[i]->IsActive)
		{
			ActiveObjectCount++;
		}
	}
}

void FighterGameState::Init()
{
	for (int i = 0; i < 6; i++)
	{
		Players[i] = new PlayerCharacter();
		Players[i]->PlayerIndex = i * 3 > 6;
		SortedObjects[i] = Players[i];
		Players[i]->InitPlayer();
		Players[i]->GameState = this;
		Players[i]->ObjNumber = i + 400;
		if (i % 3 == 0)
		{
			Players[i]->IsOnScreen = true;
		}
	}
	for (int i = 0; i < 400; i++)
	{
		Objects[i] = new BattleActor();
		Objects[i]->ResetObject();
		Objects[i]->GameState = this;
		Objects[i]->ObjNumber = i;
		SortedObjects[i + 6] = Objects[i];
	}

	StoredBattleState.RoundFormat = RoundFormat::FirstToTwo;
	StoredBattleState.RoundTimer = 99 * 60;
}

void FighterGameState::Update(int Input1, int Input2)
{
	if (!StoredBattleState.PauseTimer)
		StoredBattleState.RoundTimer--;
	if (StoredBattleState.RoundTimer < 0)
		StoredBattleState.RoundTimer = 0;
	StoredBattleState.FrameNumber++;

	for (int i = 0; i < 2; i++)
	{
		if (StoredBattleState.Meter[i] > StoredBattleState.MaxMeter[i])
			StoredBattleState.Meter[i] = StoredBattleState.MaxMeter[i];
		if (StoredBattleState.Meter[i] < 0)
			StoredBattleState.Meter[i] = 0;
	}

	SortObjects();
	Players[0]->Inputs = Input1;
	Players[3]->Inputs = Input2;
	for (int i = 0; i < 6; i++)
	{
		if (Players[i]->IsOnScreen)
		{
			for (int j = 0; j < 6; j++)
			{
				if (i < 3)
				{
					if (j >= 3 && Players[j]->IsOnScreen)
					{
						Players[i]->Enemy = Players[j];
					}
				}
				else
				{
					if (j < 3 && Players[j]->IsOnScreen)
					{
						Players[i]->Enemy = Players[j];
					}
				}
			}
		}
	}
	for (int i = 0; i < 406; i++)
	{
		if (i == ActiveObjectCount)
			break;
		if (!SortedObjects[i]->IsPlayer || SortedObjects[i]->Player->IsOnScreen)
		{
			SortedObjects[i]->Update();
		}
	}
	HandlePushCollision();
	HandleHitCollision();
	CollisionView();
	SetWallCollision();
	SetScreenBounds();
	HandleRoundWin();
	ManageAudio();
}

void FighterGameState::SaveGameState()
{	int BackupFrame = LocalFrame;
	StoredRollbackData.ActiveObjectCount = ActiveObjectCount;
	memcpy(StoredRollbackData.BattleStateBuffer, &StoredBattleState.BattleStateSync, SIZEOF_BATTLESTATE);
	for (int i = 0; i < 400; i++)
	{
		if (Objects[i]->IsActive)
		{
			Objects[i]->SaveForRollback(StoredRollbackData.ObjBuffer[i]);
			StoredRollbackData.ObjActive[i] = true;
		}
		else
			StoredRollbackData.ObjActive[i] = false;
	}
	for (int i = 0; i < 6; i++)
	{
		Players[i]->SaveForRollback(StoredRollbackData.ObjBuffer[i + 400]);
		Players[i]->SaveForRollbackPlayer(StoredRollbackData.CharBuffer[i]);
	}
}

void FighterGameState::LoadGameState()
{
	ActiveObjectCount = StoredRollbackData.ActiveObjectCount;
	memcpy(&StoredBattleState.BattleStateSync, StoredRollbackData.BattleStateBuffer, SIZEOF_BATTLESTATE);
	for (int i = 0; i < 400; i++)
	{
		if (StoredRollbackData.ObjActive[i])
		{
			Objects[i]->LoadForRollback(StoredRollbackData.ObjBuffer[i]);
		}
		else
		{
			if (Objects[i]->IsActive)
				Objects[i]->ResetObject();
		}
	}
	for (int i = 0; i < 6; i++)
	{
		Players[i]->LoadForRollback(StoredRollbackData.ObjBuffer[i + 400]);
		Players[i]->LoadForRollbackPlayer(StoredRollbackData.CharBuffer[i]);
	}
	SortObjects();
	RollbackStartAudio();
}

void FighterGameState::HandlePushCollision()
{
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			if (Players[i]->PlayerIndex != Players[j]->PlayerIndex && Players[i]->IsOnScreen && Players[j]->IsOnScreen)
			{
				Players[i]->HandlePushCollision(Players[j]);
			}
		}
	}
}

void FighterGameState::HandleHitCollision()
{
	for (int i = 0; i < 406; i++)
	{
		if (i == ActiveObjectCount)
			break;
		for (int j = 0; j < 6; j++)
		{
			if (i != j && SortedObjects[j]->Player->IsOnScreen)
			{
				SortedObjects[i]->HandleHitCollision(static_cast<PlayerCharacter*>(SortedObjects[j]));
			}
		}
		for (int j = 0; j < 406; j++)
		{
			if (i != j)
			{
				SortedObjects[i]->HandleClashCollision(SortedObjects[j]);
			}
		}
	}
}

BattleActor* FighterGameState::AddBattleActor(State* InState, int PosX, int PosY, bool FacingRight, PlayerCharacter* Parent)
{
	for (int i = 0; i < 400; i++)
	{
		if (!Objects[i]->IsActive)
		{
			memcpy(Objects[i]->ObjectState, InState, sizeof(State));
			Objects[i]->ObjectState->ObjectParent = Objects[i];
			Objects[i]->IsActive = true;
			Objects[i]->FacingRight = FacingRight;
			Objects[i]->Player = Parent;
			Objects[i]->SetPosX(PosX);
			Objects[i]->SetPosY(PosY);
			Objects[i]->InitObject();
			return Objects[i];
		}
	}
	return nullptr;
}

void FighterGameState::CollisionView()
{
	if (DisplayCollision)
	{
		for (int i = 0; i < 6; i++)
		{
			if (Players[i]->IsOnScreen)
			{
				//SortedObjects[i]->CollisionView();
			}
		}
		for (int i = 6; i < 406; i++)
		{
			if (i == ActiveObjectCount)
				break;
			//SortedObjects[i]->CollisionView();
		}
	}
}

void FighterGameState::StartSuperFreeze(int Duration)
{
	for (int i = 0; i < ActiveObjectCount; i++)
		SortedObjects[i]->SuperFreezeTime = Duration;
	StoredBattleState.PauseTimer = true;
    StoredBattleState.PauseParticles = true;
}

void FighterGameState::PlayCommonAudio(char* InSoundWave, float MaxDuration)
{
	for (int i = 0; i < CommonAudioChannelCount; i++)
	{
		if (StoredBattleState.CommonAudioChannels[i].Finished)
		{
			StoredBattleState.CommonAudioChannels[i].SoundWave = InSoundWave;
			StoredBattleState.CommonAudioChannels[i].StartingFrame = StoredBattleState.FrameNumber;
			StoredBattleState.CommonAudioChannels[i].MaxDuration = MaxDuration;
			StoredBattleState.CommonAudioChannels[i].Finished = false;
			return;
		}
	}
}

void FighterGameState::PlayCharaAudio(char* InSoundWave, float MaxDuration)
{
	for (int i = 0; i < CharaAudioChannelCount; i++)
	{
		if (StoredBattleState.CharaAudioChannels[i].Finished)
		{
			StoredBattleState.CharaAudioChannels[i].SoundWave = InSoundWave;
			StoredBattleState.CharaAudioChannels[i].StartingFrame = StoredBattleState.FrameNumber;
			StoredBattleState.CharaAudioChannels[i].MaxDuration = MaxDuration;
			StoredBattleState.CharaAudioChannels[i].Finished = false;
			return;
		}
	}
}

void FighterGameState::PlayVoiceLine(char* InSoundWave, float MaxDuration, int Player)
{
	StoredBattleState.CharaVoiceChannels[Player].SoundWave = InSoundWave;
	StoredBattleState.CharaVoiceChannels[Player].StartingFrame = StoredBattleState.FrameNumber;
	StoredBattleState.CharaVoiceChannels[Player].MaxDuration = MaxDuration;
	StoredBattleState.CharaVoiceChannels[Player].Finished = false;
}

void FighterGameState::ManageAudio()
{
	for (int i = 0; i < CommonAudioChannelCount; i++)
	{
		int CurrentAudioTime = StoredBattleState.FrameNumber - StoredBattleState.CommonAudioChannels[i].StartingFrame;
		if (!StoredBattleState.CommonAudioChannels[i].Finished && static_cast<int>(StoredBattleState.CommonAudioChannels[i].MaxDuration * 60) < CurrentAudioTime)
		{
			StoredBattleState.CommonAudioChannels[i].Finished = true;
		}
	}
	for (int i = 0; i < CharaAudioChannelCount; i++)
	{
		int CurrentAudioTime = StoredBattleState.FrameNumber - StoredBattleState.CharaAudioChannels[i].StartingFrame;
		if (!StoredBattleState.CharaAudioChannels[i].Finished && static_cast<int>(StoredBattleState.CharaAudioChannels[i].MaxDuration * 60) < CurrentAudioTime)
		{
			StoredBattleState.CharaAudioChannels[i].Finished = true;
		}
	}
	for (int i = 0; i < CharaVoiceChannelCount; i++)
	{
		int CurrentAudioTime = StoredBattleState.FrameNumber - StoredBattleState.CharaVoiceChannels[i].StartingFrame;
		if (!StoredBattleState.CharaVoiceChannels[i].Finished && static_cast<int>(StoredBattleState.CharaVoiceChannels[i].MaxDuration * 60) < CurrentAudioTime)
		{
			StoredBattleState.CharaVoiceChannels[i].Finished = true;
		}
	}
	int CurrentAudioTime = StoredBattleState.FrameNumber - StoredBattleState.AnnouncerVoiceChannel.StartingFrame;
	if (!StoredBattleState.AnnouncerVoiceChannel.Finished && static_cast<int>(StoredBattleState.AnnouncerVoiceChannel.MaxDuration * 60) < CurrentAudioTime)
	{
		StoredBattleState.AnnouncerVoiceChannel.Finished = true;
	}
}

void FighterGameState::RollbackStartAudio()
{
	for (int i = 0; i < CommonAudioChannelCount; i++)
	{
	}
	for (int i = 0; i < CharaAudioChannelCount; i++)
	{
	}
	for (int i = 0; i < CharaVoiceChannelCount; i++)
	{
	}
}

void FighterGameState::SetWallCollision()
{
	for (int i = 0; i < 6; i++)
	{
		if (Players[i] != nullptr)
		{
			if (Players[i]->IsOnScreen)
			{
				Players[i]->TouchingWall = true;
				if (Players[i]->GetInternalValue(VAL_PosX) > 600000 + StoredBattleState.CurrentScreenPos)
				{
					Players[i]->SetPosX(600001 + StoredBattleState.CurrentScreenPos);
				}
				else if (Players[i]->GetInternalValue(VAL_PosX) < -600000 + StoredBattleState.CurrentScreenPos)
				{
					Players[i]->SetPosX(-600001 + StoredBattleState.CurrentScreenPos);
				}
				else if (Players[i]->GetInternalValue(VAL_PosX) < 600000 + StoredBattleState.CurrentScreenPos || Players[i]->GetInternalValue(VAL_PosX) > -1080000 + StoredBattleState.CurrentScreenPos)
				{
					Players[i]->TouchingWall = false;
				}
			}
		}
	}
}

void FighterGameState::SetScreenBounds()
{
	for (int i = 0; i < 6; i++)
	{
		if (Players[i]->PlayerIndex == 0)
		{
			for (int j = 0; j < 6; j++)
			{
				if (Players[j]->PlayerIndex == 1)
				{
					if (Players[i]->IsOnScreen && Players[j]->IsOnScreen)
					{
					    int NewScreenPos = (Players[i]->GetInternalValue(VAL_PosX) + Players[j]->GetInternalValue(VAL_PosX)) / 2;
						StoredBattleState.CurrentScreenPos = StoredBattleState.CurrentScreenPos + (NewScreenPos - StoredBattleState.CurrentScreenPos) * 10 / 100;
						if (StoredBattleState.CurrentScreenPos > 1080000)
						{
							StoredBattleState.CurrentScreenPos = 1080000;
						}
						else if (StoredBattleState.CurrentScreenPos < -1080000)
						{
							StoredBattleState.CurrentScreenPos = -1080000;
						}
					}
				}
			}
		}
	}
}
