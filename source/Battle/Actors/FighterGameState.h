#pragma once

#include "PlayerCharacter.h"

class InputDevice;

constexpr int32_t CommonAudioChannelCount = 50;
constexpr int32_t CharaAudioChannelCount = 50;
constexpr int32_t CharaVoiceChannelCount = 6;

enum RoundFormat
{
	FirstToOne,
	FirstToTwo,
	FirstToThree,
	FirstToFour,
	FirstToFive,
	TwoVsTwo,
	ThreeVsThree,
	TwoVsTwoKOF,
	ThreeVsThreeKOF,
};

enum NetMode
{
	Offline,
	Player1,
	Player2,
};

#pragma pack (push, 1)
struct AudioChannel
{
	char* SoundWave = nullptr;
	int32_t StartingFrame = 0;
	float MaxDuration = 1.0f;
	bool Finished = true;
};

struct BattleState
{
	char BattleStateSync = 0;
	int32_t FrameNumber = 0;
	int32_t CurrentScreenPos = 0;
	int32_t RoundTimer = 0;
	bool PauseTimer = false;
	bool PauseParticles = false;
	int32_t Meter[2] { 0 , 0 };
	int32_t MaxMeter[2] { 50000 , 50000 };
	int32_t P1RoundsWon = 0;
	int32_t P2RoundsWon = 0;
	AudioChannel CommonAudioChannels[CommonAudioChannelCount];
	AudioChannel CharaAudioChannels[CharaAudioChannelCount];
	AudioChannel CharaVoiceChannels[CharaVoiceChannelCount];
	AudioChannel AnnouncerVoiceChannel = {};
	
	char BattleStateSyncEnd = 0;

	RoundFormat RoundFormat = RoundFormat::FirstToTwo;
};
#pragma pack(pop)

#define SIZEOF_BATTLESTATE offsetof(BattleState, BattleStateSyncEnd) - offsetof(BattleState, BattleStateSync)

#pragma pack (push, 1)
struct RollbackData
{
	int32_t ActiveObjectCount = 0;
	uint8_t ObjBuffer[406][SIZEOF_BATTLEACTOR] = { { 0 } };
	bool ObjActive[400] = { false };
	uint8_t CharBuffer[6][SIZEOF_PLAYERCHARACTER] = { { 0 } };
	uint8_t BattleStateBuffer[SIZEOF_BATTLESTATE] = { 0 };
};
#pragma pack(pop)

/**
 * 
 */
class FighterGameState
{
	void HandlePushCollision(); //for each active object, handle push collision
	void HandleHitCollision(); //for each active object, handle hit collision
	void SetScreenBounds(); //sets sRemoteInputscreen bounds
	void SetWallCollision(); //forces wall collision
	void CollisionView(); //for each active object, display collision

public:
	PlayerCharacter* Players[6];
	BattleActor* Objects[400];

	RollbackData StoredRollbackData;
	BattleState StoredBattleState;
	
	BattleActor* SortedObjects[406];

	int32_t MaxOtgCount = 1;
	NetMode CurrentNetMode;

	char* CommonScript;
	uint32_t CommonScriptLength;

	void Init(); //initializes game state
	void Update(int32_t Input1, int32_t Input2); //updates game state
	void SaveGameState(); //saves game state
	void LoadGameState(); //loads game state
	
	int32_t LocalFrame;
	int32_t RemoteFrame;
	int32_t ActiveObjectCount;
	
	InputDevice* InputDevices[3] = {};

private:
	int32_t LocalInputs[2] = {0, 0};
	int32_t RemoteInputs[2] = {0, 0};
	
	void UpdateLocalInput(); //updates local input
	void HandleRoundWin();
	void HandleMatchWin();
	void RoundInit();

public:
	void UpdateRemoteInput(int32_t RemoteInput, int32_t InFrame, int32_t InFrameAdvantage); //when remote inputs are received, update inputs
	void SortObjects();

	void TickGameState(); //rollback operations, then updates game state
	
	int32_t GetLocalInputs(int32_t Index); //get local inputs from player controller
	
   	bool DisplayCollision;

	BattleActor* AddBattleActor(State* InState, int32_t PosX, int32_t PosY, bool FacingRight, PlayerCharacter* Parent); //creates object
	void StartSuperFreeze(int32_t Duration);
	void PlayCommonAudio(char* InSoundWave, float MaxDuration);
	void PlayCharaAudio(char* InSoundWave, float MaxDuration);
	void PlayVoiceLine(char* InSoundWave, float MaxDuration, int32_t Player);
	void ManageAudio();
	void RollbackStartAudio();
};
