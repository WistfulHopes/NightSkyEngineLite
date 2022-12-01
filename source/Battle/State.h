﻿#pragma once

#include "../CString.h"
#include "NightSkyScript/ScriptAnalyzer.h"
#include <vector>

class PlayerCharacter;
class BattleActor;

enum EntryState //character state required to enter state
{
	None, //any
	Standing,
	Jumping,
	Crouching,
};

enum InputCondition //input required to enter state
{
	None, //no input
	Input_1,
	Input_1_Press,
	Input_1_Hold,
	Input_1_Release,
	Input_2,
	Input_2_Press,
	Input_2_Hold,
	Input_2_Release,
	Input_3,
	Input_3_Press,
	Input_3_Hold,
	Input_3_Release,
	Input_4,
	Input_4_Press,
	Input_4_Hold,
	Input_4_Release,
	Input_6,
	Input_6_Press,
	Input_6_Hold,
	Input_6_Release,
	Input_7,
	Input_7_Press,
	Input_7_Hold,
	Input_7_Release,
	Input_8,
	Input_8_Press,
	Input_8_Hold,
	Input_8_Release,
	Input_9,
	Input_9_Press,
	Input_9_Hold,
	Input_9_Release,
	Input_Down,
	Input_Down_Press,
	Input_Down_Hold,
	Input_Down_Release,
	Input_Left,
	Input_Left_Press,
	Input_Left_Hold,
	Input_Left_Release,
	Input_Right,
	Input_Right_Press,
	Input_Right_Hold,
	Input_Right_Release,
	Input_Up,
	Input_Up_Press,
	Input_Up_Hold,
	Input_Up_Release,
	Input_SuperJump,
	Input_SuperJump_Back,
	Input_SuperJump_Forward,
	Input_DownDown,
	Input_44,
	Input_66,
	Input_236,
	Input_214,
	Input_623,
	Input_421,
	Input_41236,
	Input_63214,
	Input_236236,
	Input_214214,
	Input_L,
	Input_L_Press,
	Input_L_Hold,
	Input_L_Release,
	Input_M,
	Input_M_Press,
	Input_M_Hold,
	Input_M_Release,
	Input_H,
	Input_H_Press,
	Input_H_Hold,
	Input_H_Release,
	Input_S,
	Input_S_Press,
	Input_S_Hold,
	Input_S_Release,
	Input_L_And_S,
	Input_Any_LMHS,
};

enum StateType
{
	Standing,
	Crouching,
	NeutralJump,
	ForwardJump,
	BackwardJump,
	ForwardWalk,
	BackwardWalk,
	ForwardDash,
	BackwardDash,
	ForwardAirDash,
	BackwardAirDash,
	NormalAttack,
	NormalThrow,
	SpecialAttack,
	SuperAttack,
	Hitstun,
	Blockstun,
	Tech,
};

enum StateCondition
{
	None,
	AirJumpOk,
	AirJumpMinimumHeight,
	AirDashOk,
	AirDashMinimumHeight,
	CloseNormal,
	FarNormal,
	MeterNotZero,
	MeterQuarterBar,
	MeterHalfBar,
	MeterOneBar,
	MeterTwoBars,
	MeterThreeBars,
	MeterFourBars,
	MeterFiveBars,
};

class State
{
public:
	PlayerCharacter* Parent;
	BattleActor* ObjectParent;
	CString<64> Name;
	EntryState StateEntryState;
	std::vector<InputCondition> InputConditions;
	StateType Type;
	std::vector<StateCondition> StateConditions;
	bool IsFollowupState;
	int32_t ObjectID;
	
	virtual void OnEnter(); //executes on enter. write in script
	virtual void OnUpdate(float DeltaTime); //executes every frame. write in script
	virtual void OnExit(); //executes on exit. write in script
	virtual void OnLanding(); //executes on landing. write in script
	virtual void OnHit(); //executes on hit. write in script
	virtual void OnBlock(); //executes on hit. write in script
	virtual void OnHitOrBlock(); //executes on hit. write in script
    virtual void OnCounterHit(); //executes on counter hit. write in script
	virtual void OnSuperFreeze(); //executes on super freeze. write in script
	virtual void OnSuperFreezeEnd(); //executes on super freeze. write in script
};

struct ScriptBlockOffsets 
{
	uint32_t OnEnterOffset;
	uint32_t OnUpdateOffset;
	uint32_t OnExitOffset;
	uint32_t OnLandingOffset;
	uint32_t OnHitOffset;
	uint32_t OnBlockOffset;
	uint32_t OnHitOrBlockOffset;
	uint32_t OnCounterHitOffset;
	uint32_t OnSuperFreezeOffset;
	uint32_t OnSuperFreezeEndOffset;
};

class ScriptState : public State
{
public:
	ScriptState* ParentState;
	uint32_t OffsetAddress;
	uint32_t Size;
	ScriptBlockOffsets Offsets;

	virtual void OnEnter() override; //executes on enter. write in script
	virtual void OnUpdate(float DeltaTime) override; //executes every frame. write in script
	virtual void OnExit() override; //executes on exit. write in script
	virtual void OnLanding() override; //executes on landing. write in script
	virtual void OnHit() override; //executes on hit. write in script
	virtual void OnBlock() override; //executes on hit. write in script
	virtual void OnHitOrBlock() override; //executes on hit. write in script
    virtual void OnCounterHit() override; //executes on counter hit. write in script
	virtual void OnSuperFreeze() override; //executes on super freeze. write in script
	virtual void OnSuperFreezeEnd() override; //executes on super freeze. write in script
};
