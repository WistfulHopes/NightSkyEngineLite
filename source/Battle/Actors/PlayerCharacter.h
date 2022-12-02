// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BattleActor.h"
#include "../InputBuffer.h"
#include "../StateMachine.h"
#include "../Subroutine.h"
#pragma pack (push, 1)

constexpr int CancelArraySize = 50;
constexpr int MaxComponentCount = 80;

enum PlayerStats //player stats list
{
	PLY_FWalkSpeed,
	PLY_BWalkSpeed,
	PLY_FDashInitSpeed,
	PLY_FDashAccel,
	PLY_FDashMaxSpeed,
	PLY_FDashFriction,
	PLY_BDashSpeed,
	PLY_BDashHeight,
	PLY_BDashGravity,
	PLY_JumpHeight,
	PLY_FJumpSpeed,
	PLY_BJumpSpeed,
	PLY_JumpGravity,
	PLY_SuperJumpHeight,
	PLY_FSuperJumpSpeed,
	PLY_BSuperJumpSpeed,
	PLY_SuperJumpGravity,
	PLY_FAirDashSpeed,
	PLY_BAirDashSpeed,
};

class PlayerCharacter : public BattleActor
{
public:
	PlayerCharacter();

	//starting from this until PlayerSyncEnd, everything is saved/loaded for rollback
	unsigned char PlayerSync = 0;	
	int32_t CurrentEnableFlags = 0;
	int32_t CurrentHealth;
protected:
	//internal variables
	int32_t CurrentAirJumpCount = 0;
	int32_t CurrentAirDashCount = 0;
	int32_t AirDashTimerMax = 0;
	bool JumpCancel = false;
	bool FAirDashCancel = false;
	bool BAirDashCancel = false;
	bool SpecialCancel = true;
	bool SuperCancel = true;
	bool DefaultLandingAction = true;
	bool FarNormalForceEnable = false;
	int32_t ThrowRange = 0;
	
public:
	WallBounceEffect CurrentWallBounceEffect;
	GroundBounceEffect CurrentGroundBounceEffect;
	bool IsDead = false;
	bool ThrowActive = false;
	bool IsStunned = false;
	bool IsThrowLock = false;
	bool IsOnScreen = false;
	bool DeathCamOverride = false;
	bool IsKnockedDown = false;
	bool FlipInputs = false;
	int32_t TeamIndex;
	int32_t Inputs;
	int32_t CurrentActionFlags;
	int32_t AirDashTimer;
	int32_t AirDashNoAttackTime;
	int32_t PlayerIndex;
	int32_t Hitstun = -1;
	int32_t Blockstun = -1;
	int32_t Untech = -1;
	int32_t KnockdownTime = -1;
	int32_t TotalProration = 10000;
	int32_t ComboCounter = 0;
	int32_t ComboTimer = 0;
	int32_t LoopCounter = 0;
	int32_t ThrowTechTimer;
	int32_t HasBeenOTG;
	int32_t WallTouchTimer;
	bool TouchingWall;
	bool ChainCancelEnabled = true;
	bool WhiffCancelEnabled;
	bool StrikeInvulnerable;
	bool ThrowInvulnerable;
	bool ProjectileInvulnerable;
	bool HeadInvulnerable;
	int RoundWinTimer = 300;
	bool RoundWinInputLock = false;
	int MeterCooldownTimer = 0;
	
	//movement values
	int32_t FWalkSpeed;
	int32_t BWalkSpeed;
	int32_t FDashInitSpeed;
	int32_t FDashAccel;
	int32_t FDashMaxSpeed;
	int32_t FDashFriction;
	int32_t BDashSpeed;
	int32_t BDashHeight;
	int32_t BDashGravity;
	int32_t JumpHeight;
	int32_t FJumpSpeed;
	int32_t BJumpSpeed;
	int32_t JumpGravity;
	int32_t SuperJumpHeight;
	int32_t FSuperJumpSpeed;
	int32_t BSuperJumpSpeed;
	int32_t SuperJumpGravity;
	int32_t AirDashMinimumHeight;
	int32_t FAirDashSpeed;
	int32_t BAirDashSpeed;
	//how long until gravity kicks in
	int32_t FAirDashTime;
	//how long until gravity kicks in
	int32_t BAirDashTime;
	//how long until airdash cancellable
	int32_t FAirDashNoAttackTime;
	//how long until airdash cancellable
	int32_t BAirDashNoAttackTime; 
	int32_t AirJumpCount;
	int32_t AirDashCount;
	int32_t StandPushWidth;
	int32_t StandPushHeight;
	int32_t CrouchPushWidth;
	int32_t CrouchPushHeight;
	int32_t AirPushWidth;
	int32_t AirPushHeight;
	int32_t AirPushHeightLow;
	int32_t CloseNormalRange;
	int32_t Health;
	int32_t ComboRate = 60;
	int32_t ForwardWalkMeterGain;
	int32_t ForwardJumpMeterGain;
	int32_t ForwardDashMeterGain;
	int32_t ForwardAirDashMeterGain;
	int32_t MeterPercentOnHit = 72;
	int32_t MeterPercentOnHitGuard = 18;
	int32_t MeterPercentOnReceiveHitGuard = 10;
	int32_t MeterPercentOnReceiveHit = 40;
	std::vector<CString<64>> ThrowLockCels;

	//custom player values, reset at end of round. use to keep track of values (timers, toggles, counters, etc) that are specific to the character.
	int32_t PlayerVal1 = 0;
	int32_t PlayerVal2 = 0;
	int32_t PlayerVal3 = 0;
	int32_t PlayerVal4 = 0;
	int32_t PlayerVal5 = 0;
	int32_t PlayerVal6 = 0;
	int32_t PlayerVal7 = 0;
	int32_t PlayerVal8 = 0;
	
	//state machine
	StateMachine StateMachine;
	//input buffer
	InputBuffer InputBuffer;

	//chain cancels (copied from std::vector to static array)
	int32_t ChainCancelOptionsInternal[CancelArraySize];
	//whiff cancels (copied from std::vector to static array)
    int32_t WhiffCancelOptionsInternal[CancelArraySize]; 
	CString<64> BufferedStateName;
	CString<64> StateName;
	CString<64> ExeStateName;

	//last received hit action. clear after read
	HitAction ReceivedHitAction = HACT_None;
	//last received attack level. clear after read
	int ReceivedAttackLevel = -1; 

	//pointer to active enemy.
	PlayerCharacter* Enemy = nullptr; 

	BattleActor* ChildBattleActors[32];

	BattleActor* StoredBattleActors[16];

	bool ComponentVisible[MaxComponentCount];
	
	//anything past here isn't saved or loaded for rollback	
	int32_t PlayerSyncEnd; 

	//scripts
	ScriptAnalyzer* CharaAnalyzer;
	char* CharaScript;
	uint32_t CharaScriptLength;
	ScriptAnalyzer* ObjAnalyzer;
	char* ObjectScript;
	uint32_t ObjectScriptLength;
	ScriptAnalyzer* CommonAnalyzer;

	std::vector<Subroutine*> CommonSubroutines;
	std::vector<CString<64>> CommonSubroutineNames;
	
	std::vector<Subroutine*> Subroutines;
	std::vector<CString<64>> SubroutineNames;

	std::vector<State*> ObjectStates;
	std::vector<CString<64>> ObjectStateNames; 

	std::vector<State*> CommonStates;
	std::vector<CString<64>> CommonStateNames; 

	int32_t ColorIndex = 1;
	
private:
	//internal functions
	//update state
	void HandleStateMachine(bool Buffer);
	//buffer state
	void HandleBufferedState();
	//check state conditions
	bool HandleStateCondition(StateCondition StateCondition);
	//check if chain cancel option exists
	bool FindChainCancelOption(char* Name);
	//check if whiff cancel option exists
	bool FindWhiffCancelOption(char* Name); 
	
public:
	//initialize player for match/round start
	void InitPlayer();
	//initializes states
	void InitStates();
	//based on received hit action, choose state
	void HandleHitAction();
	//check attack against block stance
	bool IsCorrectBlock(BlockType BlockType);
	//jump to correct block state
	void HandleBlockAction();
	//called whenever state changes
	void OnStateChange(); 
	virtual void Update() override;
	void SaveForRollbackPlayer(unsigned char* Buffer);
	void LoadForRollbackPlayer(unsigned char* Buffer);
	virtual void LogForSyncTest(FILE* file) override;
	//upon successful throw, jump to state
	void ThrowExe();
	//handles throwing objects
	void HandleThrowCollision();
	//checks kara cancel
	bool CheckKaraCancel(StateType InStateType);
	//checks if a child object with a corresponding object id exists. if so, do not enter state 
	bool CheckObjectPreventingState(int InObjectID);
	//resets object for next round
	void ResetForRound();
	//handles wall bounce
	void HandleWallBounce();
	//handles ground bounce
	void HandleGroundBounce();
	
	//bp callable functions
	//add state to state machine
	void AddState(CString<64> Name, State* State); 
	//add subroutine to state machine
	void AddSubroutine(CString<64> Name, Subroutine* Subroutine);
	//calls subroutine
	void CallSubroutine(char* Name);
	//use meter
	void UseMeter(int Use);
	//add meter
	void AddMeter(int Meter);
	//sets meter gain cooldoown timer
	void SetMeterCooldownTimer(int Timer);
	//set standing/crouching/jumping
	void SetActionFlags(ActionFlags ActionFlag);
	//force set state
	void JumpToState(char* NewName);
	//gets current state name
	CString<64> GetCurrentStateName();
	//gets loop counter
	int32_t GetLoopCount();
	//gets loop counter
	void IncrementLoopCount();
	//check if state can be entered
	bool CheckStateEnabled(StateType StateType);
	//enable state type
	void EnableState(EnableFlags NewEnableFlags);
	//enable all attacks only
	void EnableAttacks();
	//disable state type
	void DisableState(EnableFlags NewEnableFlags);
	//disable ground movement only
	void DisableGroundMovement();
	//enable all states (besides tech)
	void EnableAll(); 
	//disable all states (besides tech)
	void DisableAll();
	//checks raw inputs (after side switching)
	bool CheckInputRaw(InputFlags Input);
	//checks input condition
	bool CheckInput(InputCondition Input); 
	bool CheckIsStunned();
	//temporarily adds air jump
	void AddAirJump(int32_t NewAirJump);
	//temporarily adds air dash
	void AddAirDash(int32_t NewAirDash);
	//set air dash timer (set is forward for forward airdashes)
	void SetAirDashTimer(bool IsForward);
	//add chain cancel option, use this in OnEntry
	void AddChainCancelOption(CString<64> Option);
	//add whiff cancel option, use this in OnEntry
	void AddWhiffCancelOption(CString<64> Option);
	//sets jump cancel on hit enabled
	void EnableJumpCancel(bool Enable);
	//sets forward air dash cancel on hit enabled
	void EnableFAirDashCancel(bool Enable);
	//sets back air dash cancel on hit enabled
	void EnableBAirDashCancel(bool Enable);
	//sets chain cancel options enabled. on by default 
	void EnableChainCancel(bool Enable);
	//sets whiff cancel options enabled. off by default
	void EnableWhiffCancel(bool Enable);
	//sets special cancel enabled. off by default
	void EnableSpecialCancel(bool Enable);
	//sets super cancel enabled. off by default
	void EnableSuperCancel(bool Enable);
	//toggles default landing action. if true, landing will go to JumpLanding state. if false, define your own landing.
	void SetDefaultLandingAction(bool Enable);
	//sets strike invulnerable enabled
	void SetStrikeInvulnerable(bool Invulnerable);
	//sets throw invulnerable enabled
	void SetThrowInvulnerable(bool Invulnerable);
	//sets projectile invulnerable enabled
	void SetProjectileInvulnerable(bool Invulnerable);
	//sets head attribute invulnerable enabled
	void SetHeadInvulnerable(bool Invulnerable);
	//force enables far proximity normals
	void ForceEnableFarNormal(bool Enable);
	//initiate throw
	void SetThrowActive(bool Active);
	//end throw
	void ThrowEnd(); 
	//initiate throw range
	void SetThrowRange(int32_t InThrowRange);
	//sets throw execution state
	void SetThrowExeState(char* ExeState);
	//sets grip position for throw
	void SetThrowPosition(int32_t ThrowPosX, int32_t ThrowPosY);
	//sets grip position for throw
	void SetThrowLockCel(int32_t Index);
	//plays voice line
	void PlayVoice(char* Name);
	//plays common level sequence
    void PlayCommonCameraAnim(char* Name);
	//plays character level sequence
    void PlayCharaCameraAnim(char* Name);
	//starts super freeze
	void StartSuperFreeze(int Duration);
	//toggles hud visibility
	void BattleHudVisibility(bool Visible);
	//disables last input
	void DisableLastInput();
	//creates object
	BattleActor* AddBattleActor(char* InStateName, int32_t PosXOffset, int32_t PosYOffset);
	//stores battle actor in slot
	void AddBattleActorToStorage(BattleActor* InActor, int Index);
};

#define SIZEOF_PLAYERCHARACTER offsetof(PlayerCharacter, PlayerSyncEnd) - offsetof(PlayerCharacter, PlayerSync)
#pragma pack(pop)
