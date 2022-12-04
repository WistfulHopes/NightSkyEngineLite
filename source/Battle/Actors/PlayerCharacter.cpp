// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "FighterGameState.h"
#include <cstdlib>

PlayerCharacter::PlayerCharacter()
{
	InitPlayer();
	for (int i = 0; i < 32; i++)
		ChildBattleActors[i] = nullptr;
	for (int i = 0; i < 16; i++)
		StoredBattleActors[i] = nullptr;
	CharaAnalyzer = new ScriptAnalyzer();
	ObjAnalyzer = new ScriptAnalyzer();
	CommonAnalyzer = new ScriptAnalyzer();
	Player = this;
	StateMachine.Parent = this;
	ScreenCollisionActive = true;
	PushCollisionActive = true;
	FWalkSpeed = 7800;
	BWalkSpeed = 4800;
	FDashInitSpeed = 13000;
	FDashAccel = 600;
	FDashFriction = 95;
	FDashMaxSpeed = 20000;
	BDashSpeed = 14000;
	BDashHeight = 5200;
	BDashGravity = 700;
	JumpHeight = 35000;
	FJumpSpeed = 7900;
	BJumpSpeed = 5200;
	JumpGravity = 1900;
	SuperJumpHeight = 43700;
	FSuperJumpSpeed = 7900;
	BSuperJumpSpeed = 5200;
	SuperJumpGravity = 1900;
	AirDashMinimumHeight = 165000;
	FAirDashSpeed = 23000;
	BAirDashSpeed = 17500;
	FAirDashTime = 20;
	BAirDashTime = 12;
	FAirDashNoAttackTime = 5;
	BAirDashNoAttackTime = 5;
	AirJumpCount = 1;
	AirDashCount = 1;
	CurrentActionFlags = ACT_Standing;
	StandPushWidth = 110000;
	StandPushHeight = 240000;
	CrouchPushWidth = 120000;
	CrouchPushHeight = 180000;
	AirPushWidth = 100000;
	AirPushHeight = 275000;
	AirPushHeightLow = -135000;
	DefaultCommonAction = true;
	IsPlayer = true;
	IsActive = true;
	Health = 10000;
	ForwardWalkMeterGain = 12;
	ForwardJumpMeterGain = 10;
	ForwardDashMeterGain = 25;
	ForwardAirDashMeterGain = 25;
}

void PlayerCharacter::InitPlayer()
{
	CurrentHealth = Health;
	AttackProjectileAttribute = false;
	DefaultLandingAction = true;
	EnableAll();
	EnableFlip(true);
	StateName.SetString("Stand");
}

void PlayerCharacter::InitStates()
{
	CommonAnalyzer->Initialize(GameState->CommonScript, GameState->CommonScriptLength, &CommonStates, &CommonSubroutines);
	for (auto State : CommonStates)
	{
		State->Parent = this;
		dynamic_cast<ScriptState*>(State)->CommonState = true;
	}
	for (auto Subroutine : CommonSubroutines)
	{
		CommonSubroutineNames.push_back(Subroutine->Name);
		Subroutine->Parent = this;
		dynamic_cast<ScriptSubroutine*>(Subroutine)->CommonSubroutine = true;
	}
	CharaAnalyzer->Initialize(CharaScript, CharaScriptLength, &StateMachine.States, &Subroutines);
	ObjAnalyzer->Initialize(ObjectScript, ObjectScriptLength, &ObjectStates, &Subroutines);
	for (auto Subroutine : Subroutines)
	{
		SubroutineNames.push_back(Subroutine->Name);
		Subroutine->Parent = this;
	}
	for (auto State : ObjectStates)
	{
		ObjectStateNames.push_back(State->Name);
	}
	StateMachine.Initialize();
	StateMachine.ParentStates(CommonStates);
	CallSubroutine("CmnMatchInit");
	CallSubroutine("MatchInit");
	StateMachine.CurrentState->OnEnter();
}

void PlayerCharacter::Update()
{
	BattleActor::Update();
	CallSubroutine("CmnOnUpdate");
	CallSubroutine("OnUpdate");
	
	//run input buffer before checking hitstop
	if (!FacingRight && !FlipInputs || FlipInputs && FacingRight) //flip inputs with direction
	{
		const unsigned int Bit1 = (Inputs >> 2) & 1;
		const unsigned int Bit2 = (Inputs >> 3) & 1;
		unsigned int x = (Bit1 ^ Bit2);

		x = x << 2 | x << 3;

		Inputs = Inputs ^ x;
	}
	if (!IsStunned)
	{
		Enemy->ComboCounter = 0;
		Enemy->ComboTimer = 0;
		TotalProration = 10000;
	}
	if (!Inputs << 27) //if no direction, set neutral input
	{
		Inputs |= (int)InputNeutral;
	}
	else
	{
		Inputs = Inputs & ~(int)InputNeutral; //remove neutral input if directional input
	}
	if (IsThrowLock)
	{
		InputBuffer.Tick(Inputs);
		HandleStateMachine(true); //handle state transitions
		if (ThrowTechTimer > 0)
		{
			if (CheckInput(InputCondition::Input_L_And_S))
			{
				ThrowTechTimer = 0;
				JumpToState("GuardBreak");
				Enemy->JumpToState("GuardBreak");
				IsThrowLock = false;
				SetInertia(-15000);
				Enemy->SetInertia(-15000);
				if (TeamIndex == 0)
					Hitstop = 1;
				else
					Enemy->Hitstop = 1;
				return;
			}
		}
		ThrowTechTimer--;
		return;
	}
	
	if (ReceivedAttackLevel != -1)
		HandleHitAction();
	
	if (SuperFreezeTime > 0)
	{
		InputBuffer.Tick(Inputs);
		HandleStateMachine(true); //handle state transitions
		return;
	}
	if (SuperFreezeTime == 0)
	{
		BattleHudVisibility(true);
		StateMachine.CurrentState->OnSuperFreezeEnd();
		AnimTime++;
	}
	
	if (Hitstop > 0)
	{
		InputBuffer.Tick(Inputs);
		HandleStateMachine(true); //handle state transitions
		return;
	}

	HandleBufferedState();

	if (TouchingWall)
		WallTouchTimer++;
	else
		WallTouchTimer = 0;
	
	if (ComboCounter > 0)
		ComboTimer++;
		
	if (StateMachine.CurrentState->Type == StateType::ForwardWalk)
		AddMeter(ForwardWalkMeterGain);
	else if (StateMachine.CurrentState->Type == StateType::ForwardJump)
		AddMeter(ForwardJumpMeterGain);
	if (StateMachine.CurrentState->Type == StateType::ForwardDash)
		AddMeter(ForwardDashMeterGain);
	else if (StateMachine.CurrentState->Type == StateType::ForwardAirDash)
		AddMeter(ForwardAirDashMeterGain);
	MeterCooldownTimer--;
	
	if (!RoundWinInputLock)
		InputBuffer.Tick(Inputs);
	else
		InputBuffer.Tick(InputNeutral);
	
	AirDashTimer--;
	AirDashNoAttackTime--;
	
	Hitstun--;
	if (Hitstun == 0 && !IsDead)
	{
		EnableAll();
		if (CurrentActionFlags == ACT_Standing)
		{
			JumpToState("Stand");
		}
		else if (CurrentActionFlags == ACT_Crouching)
		{
			JumpToState("Crouch");
		}
		TotalProration = 10000;
	}

	if (PosY > 0) //set jumping if above ground
	{
		SetActionFlags(ACT_Jumping);
	}
	
	Untech--;
	if (Untech == 0 && !IsKnockedDown && !IsDead)
		EnableState(ENB_Tech);

	if (StateMachine.CurrentState->Type == StateType::Tech)
	{
		HasBeenOTG = 0;
		CurrentWallBounceEffect = WallBounceEffect();
		CurrentGroundBounceEffect = GroundBounceEffect();
	}
	
	if (StateMachine.CurrentState->Type == StateType::Hitstun && PosY <= 0 && PrevPosY > 0)
	{
		HaltMomentum();
		if (!strcmp(StateMachine.CurrentState->Name.GetString(), "BLaunch") || !strcmp(StateMachine.CurrentState->Name.GetString(), "Blowback"))
			JumpToState("FaceUpBounce");
		else if (!strcmp(StateMachine.CurrentState->Name.GetString(), "FLaunch"))
			JumpToState("FaceDownBounce");
	}

	if (PosY <= 0 && !IsDead && CurrentGroundBounceEffect.GroundBounceCount == 0)
	{
		KnockdownTime--;
		if (Untech > 0 && PrevPosY > 0)
		{
			Untech = -1;
			IsKnockedDown = true;
			DisableState(ENB_Tech);
		}
	}

	if (StateMachine.CurrentState->Type != StateType::Hitstun)
	{
		KnockdownTime = -1;
		IsKnockedDown = false;
	}

	if (KnockdownTime < 0 && Blockstun < 0 && (Untech < 0 && StateMachine.CurrentState->Type != StateType::Hitstun) && Hitstun < 0)
		IsStunned = false;

	if (KnockdownTime == 0 && PosY <= 0 && !IsDead)
	{
		Enemy->ComboCounter = 0;
		Enemy->ComboTimer = 0;
		HasBeenOTG = 0;
		if (!strcmp(StateMachine.CurrentState->Name.GetString(), "FaceDown") || !strcmp(StateMachine.CurrentState->Name.GetString(), "FaceDownBounce"))
			JumpToState("WakeUpFaceDown");
		else if (!strcmp(StateMachine.CurrentState->Name.GetString(), "FaceUp") || !strcmp(StateMachine.CurrentState->Name.GetString(), "FaceUpBounce"))
			JumpToState("WakeUpFaceUp");
		TotalProration = 10000;
	}
	
	if (IsDead)
		DisableState(ENB_Tech);
	Blockstun--;
	if (Blockstun == 0)
	{
		if (CurrentActionFlags & ACT_Standing)
		{
			JumpToState("Stand");
		}
		else if (CurrentActionFlags & ACT_Crouching)
		{
			JumpToState("Crouch");
		}
		else
		{
			JumpToState("VJump");
		}
	}
	
	HandleWallBounce();
	
	if (PosY == 0 && PrevPosY != 0) //reset air move counts on ground
	{
		CurrentAirJumpCount = AirJumpCount;
		CurrentAirDashCount = AirDashCount;
		if (DefaultLandingAction)
		{
			JumpToState("JumpLanding");
		}
		else
		{
			StateMachine.CurrentState->OnLanding();
		}
		CreateCommonParticle("cmn_jumpland_smoke", POS_Player);
		HandleGroundBounce();
	}
	HandleThrowCollision();
	if (Hitstop != 0)
		StateMachine.Tick(0.0166666); //update current state

	HandleStateMachine(false); //handle state transitions
}

void PlayerCharacter::HandleStateMachine(bool Buffer)
{
	for (int i = StateMachine.States.size() - 1; i >= 0; i--)
	{
        if (!(CheckStateEnabled(StateMachine.States[i]->Type) && !StateMachine.States[i]->IsFollowupState
            || FindChainCancelOption(StateMachine.States[i]->Name.GetString())
            || FindWhiffCancelOption(StateMachine.States[i]->Name.GetString())
            || CheckKaraCancel(StateMachine.States[i]->Type) && !StateMachine.States[i]->IsFollowupState
            )) //check if the state is enabled, continue if not
        {
            continue;
        }
		if (CheckObjectPreventingState(StateMachine.States[i]->ObjectID)) //check if an object is preventing state entry, continue if so
		{
			continue;
		}
        //check current character state against entry state condition, continue if not entry state
		if (!StateMachine.CheckStateEntryCondition(StateMachine.States[i]->StateEntryState, CurrentActionFlags))
        {
            continue;
        }
		if (StateMachine.States[i]->StateConditions.size() != 0) //only check state conditions if there are any
		{
			for (int j = 0; j < StateMachine.States[i]->StateConditions.size(); j++) //iterate over state conditions
			{
                if (!(HandleStateCondition(StateMachine.States[i]->StateConditions[j]))) //check state condition
                {
                    break;
                }
                if (!(j == StateMachine.States[i]->StateConditions.size() - 1)) //have all conditions been met?
                {
                    continue;
                }
				for (int v = 0; v < StateMachine.States[i]->InputConditions.size(); v++) //iterate over input conditions
				{
					//check input condition against input buffer, if not met break.
                    if (!InputBuffer.CheckInputCondition(StateMachine.States[i]->InputConditions[v]))
                    {
                        break;
                    }
					if (v == StateMachine.States[i]->InputConditions.size() - 1) //have all conditions been met?
					{
						if (FindChainCancelOption(StateMachine.States[i]->Name.GetString())
							|| FindWhiffCancelOption(StateMachine.States[i]->Name.GetString())) //if cancel option, allow resetting state
						{
							if (Buffer)
							{
								BufferedStateName.SetString(StateMachine.States[i]->Name.GetString());
								return;
							}
							if (StateMachine.ForceSetState(StateMachine.States[i]->Name)) //if state set successful...
							{
								StateName.SetString(StateMachine.States[i]->Name.GetString());
								switch (StateMachine.States[i]->StateEntryState)
								{
								case EntryState::Standing:
									CurrentActionFlags = ACT_Standing;
									break;
								case EntryState::Crouching:
									CurrentActionFlags = ACT_Crouching;
									break;
								case EntryState::Jumping:
									CurrentActionFlags = ACT_Jumping;
									break;
								default:
									break;
								}
								return; //don't try to enter another state
							}
						}
						else
						{
							if (Buffer)
							{
								BufferedStateName.SetString(StateMachine.States[i]->Name.GetString());
								return;
							}
							if (StateMachine.SetState(StateMachine.States[i]->Name)) //if state set successful...
							{
								StateName.SetString(StateMachine.States[i]->Name.GetString());
								switch (StateMachine.States[i]->StateEntryState)
								{
								case EntryState::Standing:
									CurrentActionFlags = ACT_Standing;
									break;
								case EntryState::Crouching:
									CurrentActionFlags = ACT_Crouching;
									break;
								case EntryState::Jumping:
									CurrentActionFlags = ACT_Jumping;
									break;
								default:
									break;
								}
								return; //don't try to enter another state
							}
						}
					}
				}
				if (StateMachine.States[i]->InputConditions.size() == 0) //if no input condtions, set state
				{
					if (Buffer)
					{
						BufferedStateName.SetString(StateMachine.States[i]->Name.GetString());
						return;
					}
					if (StateMachine.SetState(StateMachine.States[i]->Name)) //if state set successful...
					{
						StateName.SetString(StateMachine.States[i]->Name.GetString());
						switch (StateMachine.States[i]->StateEntryState)
						{
						case EntryState::Standing:
							CurrentActionFlags = ACT_Standing;
							break;
						case EntryState::Crouching:
							CurrentActionFlags = ACT_Crouching;
							break;
						case EntryState::Jumping:
							CurrentActionFlags = ACT_Jumping;
							break;
						default:
							break;
						}
						return; //don't try to enter another state
					}
				}
                continue; //this is unneeded but here for clarity.
			}
		}
		else
		{
			for (int v = 0; v < StateMachine.States[i]->InputConditions.size(); v++) //iterate over input conditions
			{
				//check input condition against input buffer, if not met break.
				if (!InputBuffer.CheckInputCondition(StateMachine.States[i]->InputConditions[v]))
				{
					break;
				}
				if (v == StateMachine.States[i]->InputConditions.size() - 1) //have all conditions been met?
				{
					if (FindChainCancelOption(StateMachine.States[i]->Name.GetString())
						|| FindWhiffCancelOption(StateMachine.States[i]->Name.GetString())) //if cancel option, allow resetting state
					{
						if (Buffer)
						{
							BufferedStateName.SetString(StateMachine.States[i]->Name.GetString());
							return;
						}
						if (StateMachine.ForceSetState(StateMachine.States[i]->Name)) //if state set successful...
						{
							StateName.SetString(StateMachine.States[i]->Name.GetString());
							switch (StateMachine.States[i]->StateEntryState)
							{
							case EntryState::Standing:
								CurrentActionFlags = ACT_Standing;
								break;
							case EntryState::Crouching:
								CurrentActionFlags = ACT_Crouching;
								break;
							case EntryState::Jumping:
								CurrentActionFlags = ACT_Jumping;
								break;
							default:
								break;
							}
							return; //don't try to enter another state
						}
					}
					else
					{
						if (Buffer)
						{
							BufferedStateName.SetString(StateMachine.States[i]->Name.GetString());
							return;
						}
						if (StateMachine.SetState(StateMachine.States[i]->Name)) //if state set successful...
						{
							StateName.SetString(StateMachine.States[i]->Name.GetString());
							switch (StateMachine.States[i]->StateEntryState)
							{
							case EntryState::Standing:
								CurrentActionFlags = ACT_Standing;
								break;
							case EntryState::Crouching:
								CurrentActionFlags = ACT_Crouching;
								break;
							case EntryState::Jumping:
								CurrentActionFlags = ACT_Jumping;
								break;
							default:
								break;
							}
							return; //don't try to enter another state
						}
					}
				}
			}
			if (StateMachine.States[i]->InputConditions.size() == 0) //if no input condtions, set state
			{
				if (Buffer)
				{
					BufferedStateName.SetString(StateMachine.States[i]->Name.GetString());
					return;
				}
				if (StateMachine.SetState(StateMachine.States[i]->Name)) //if state set successful...
				{
					StateName.SetString(StateMachine.States[i]->Name.GetString());
					switch (StateMachine.States[i]->StateEntryState)
					{
					case EntryState::Standing:
						CurrentActionFlags = ACT_Standing;
						break;
					case EntryState::Crouching:
						CurrentActionFlags = ACT_Crouching;
						break;
					case EntryState::Jumping:
						CurrentActionFlags = ACT_Jumping;
						break;
					default:
						break;
					}
					return; //don't try to enter another state
				}
			}
		}
	}
}


void PlayerCharacter::HandleBufferedState()
{
	if (strcmp(BufferedStateName.GetString(), ""))
	{
		if (FindChainCancelOption(BufferedStateName.GetString())
			|| FindWhiffCancelOption(BufferedStateName.GetString())) //if cancel option, allow resetting state
		{
			if (StateMachine.ForceSetState(BufferedStateName))
			{
				StateName.SetString(BufferedStateName.GetString());
				switch (StateMachine.CurrentState->StateEntryState)
				{
				case EntryState::Standing:
					CurrentActionFlags = ACT_Standing;
					break;
				case EntryState::Crouching:
					CurrentActionFlags = ACT_Crouching;
					break;
				case EntryState::Jumping:
					CurrentActionFlags = ACT_Jumping;
					break;
				default:
					break;
				}
			}
			BufferedStateName.SetString("");
		}
		else
		{
			if (StateMachine.SetState(BufferedStateName))
			{
				StateName.SetString(BufferedStateName.GetString());
				switch (StateMachine.CurrentState->StateEntryState)
				{
				case EntryState::Standing:
					CurrentActionFlags = ACT_Standing;
					break;
				case EntryState::Crouching:
					CurrentActionFlags = ACT_Crouching;
					break;
				case EntryState::Jumping:
					CurrentActionFlags = ACT_Jumping;
					break;
				default:
					break;
				}
			}
			BufferedStateName.SetString("");
		}
	}
}

void PlayerCharacter::SetActionFlags(ActionFlags ActionFlag)
{
	CurrentActionFlags = ActionFlag;
}

void PlayerCharacter::AddState(CString<64> Name, State* State)
{
	StateMachine.AddState(Name, State);
}

void PlayerCharacter::AddSubroutine(CString<64> Name, Subroutine* Subroutine)
{
	Subroutine->Parent = this;
	Subroutines.push_back(Subroutine);
	SubroutineNames.push_back(Name);
}

void PlayerCharacter::CallSubroutine(char* Name)
{
	int Index = 0;
	for (CString<64> String : CommonSubroutineNames)
	{
		if (!strcmp(String.GetString(), Name))
		{
			CommonSubroutines[Index]->OnCall();
			return;
		}
		Index++;
	}
	Index = 0;
	for (CString<64> String : SubroutineNames)
	{
		if (!strcmp(String.GetString(), Name))
		{
			Subroutines[Index]->OnCall();
			return;
		}
		Index++;
	}
}

void PlayerCharacter::UseMeter(int Use)
{
	GameState->StoredBattleState.Meter[PlayerIndex] -= Use;
}

void PlayerCharacter::AddMeter(int Meter)
{
	if (MeterCooldownTimer > 0)
		Meter /= 10;
	GameState->StoredBattleState.Meter[PlayerIndex] += Meter;
}

void PlayerCharacter::SetMeterCooldownTimer(int Timer)
{
	MeterCooldownTimer = Timer;
}

void PlayerCharacter::JumpToState(char* NewName)
{
	CString<64> Name;
	Name.SetString(NewName);
	if (StateMachine.ForceSetState(Name))
		StateName.SetString(NewName);
	if (StateMachine.CurrentState != nullptr)
	{
		switch (StateMachine.CurrentState->StateEntryState)
		{
		case EntryState::Standing:
			CurrentActionFlags = ACT_Standing;
			break;
		case EntryState::Crouching:
			CurrentActionFlags = ACT_Crouching;
			break;
		case EntryState::Jumping:
			CurrentActionFlags = ACT_Jumping;
			break;
		case EntryState::None: break;
		}
	}
}

CString<64> PlayerCharacter::GetCurrentStateName()
{
	return StateMachine.CurrentState->Name;
}

int32_t PlayerCharacter::GetLoopCount()
{
	return LoopCounter;
}

void PlayerCharacter::IncrementLoopCount()
{
	LoopCounter += 1;
}

bool PlayerCharacter::CheckStateEnabled(StateType StateType)
{
	if (StateType < StateType::NormalAttack && AirDashNoAttackTime > 0)
		return false;
	switch (StateType)
	{
	case StateType::Standing:
		if (CurrentEnableFlags & ENB_Standing)
			return true;
		break;
	case StateType::Crouching:
		if (CurrentEnableFlags & ENB_Crouching)
			return true;
		break;
	case StateType::NeutralJump:
	case StateType::ForwardJump:
	case StateType::BackwardJump:
		if (CurrentEnableFlags & ENB_Jumping || JumpCancel && HasHit && IsAttacking)
			return true;
		break;
	case StateType::ForwardWalk:
		if (CurrentEnableFlags & ENB_ForwardWalk)
			return true;
		break;
	case StateType::BackwardWalk:
		if (CurrentEnableFlags & ENB_BackWalk)
			return true;
		break;
	case StateType::ForwardDash:
		if (CurrentEnableFlags & ENB_ForwardDash)
			return true;
		break;
	case StateType::BackwardDash:
		if (CurrentEnableFlags & ENB_BackDash)
			return true;
		break;
	case StateType::ForwardAirDash:
		if (CurrentEnableFlags & ENB_ForwardAirDash || FAirDashCancel && HasHit && IsAttacking)
			return true;
		break;
	case StateType::BackwardAirDash:
		if (CurrentEnableFlags & ENB_BackAirDash || BAirDashCancel && HasHit && IsAttacking)
			return true;
		break;
	case StateType::NormalAttack:
	case StateType::NormalThrow:
		if (CurrentEnableFlags & ENB_NormalAttack)
			return true;
		break;
	case StateType::SpecialAttack:
		if (CurrentEnableFlags & ENB_SpecialAttack || SpecialCancel && HasHit && IsAttacking)
			return true;
		break;
	case StateType::SuperAttack:
		if (CurrentEnableFlags & ENB_SuperAttack || SuperCancel && HasHit && IsAttacking)
			return true;
		break;
	case StateType::Tech:
		if (CurrentEnableFlags & ENB_Tech)
			return true;
		break;
	default:
		return false;
	}
	return false;
}

void PlayerCharacter::EnableState(EnableFlags NewEnableFlags)
{
	CurrentEnableFlags |= (int)NewEnableFlags;	
}

void PlayerCharacter::DisableState(EnableFlags NewEnableFlags)
{
	CurrentEnableFlags = CurrentEnableFlags & ~(int)NewEnableFlags;
}

void PlayerCharacter::EnableAll()
{
	EnableState(ENB_Standing);
	EnableState(ENB_Crouching);
	EnableState(ENB_Jumping);
	EnableState(ENB_ForwardWalk);
	EnableState(ENB_BackWalk);
	EnableState(ENB_ForwardDash);
	EnableState(ENB_BackDash);
	EnableState(ENB_ForwardAirDash);
	EnableState(ENB_BackAirDash);
	EnableState(ENB_NormalAttack);
	EnableState(ENB_SpecialAttack);
	EnableState(ENB_SuperAttack);
	EnableState(ENB_Block);
	DisableState(ENB_Tech);
}

void PlayerCharacter::DisableGroundMovement()
{
	DisableState(ENB_Standing);
	DisableState(ENB_Crouching);
	DisableState(ENB_ForwardWalk);
	DisableState(ENB_BackWalk);
	DisableState(ENB_ForwardDash);
	DisableState(ENB_BackDash);
	DisableState(ENB_Tech);
}

void PlayerCharacter::DisableAll()
{
	DisableState(ENB_Standing);
	DisableState(ENB_Crouching);
	DisableState(ENB_Jumping);
	DisableState(ENB_ForwardWalk);
	DisableState(ENB_BackWalk);
	DisableState(ENB_ForwardDash);
	DisableState(ENB_BackDash);
	DisableState(ENB_ForwardAirDash);
	DisableState(ENB_BackAirDash);
	DisableState(ENB_NormalAttack);
	DisableState(ENB_SpecialAttack);
	DisableState(ENB_SuperAttack);
	DisableState(ENB_Block);
	DisableState(ENB_Tech);
}

bool PlayerCharacter::CheckInputRaw(InputFlags Input)
{
	if (Inputs & Input)
		return true;
	return false;
}

bool PlayerCharacter::CheckIsStunned()
{
	return IsStunned;
}


void PlayerCharacter::AddAirJump(int NewAirJump)
{
	CurrentAirJumpCount += NewAirJump;
}

void PlayerCharacter::AddAirDash(int NewAirDash)
{
	CurrentAirDashCount += NewAirDash;
}

bool PlayerCharacter::HandleStateCondition(StateCondition StateCondition)
{
	switch(StateCondition)
	{
	case StateCondition::None:
		return true;
	case StateCondition::AirJumpOk:
		if (CurrentAirJumpCount > 0)
			return true;
		break;
	case StateCondition::AirJumpMinimumHeight:
		if (SpeedY <= 0 || PosY >= 122500)
			return true;
		break;
	case StateCondition::AirDashOk:
		if (CurrentAirDashCount > 0)
			return true;
		break;
	case StateCondition::AirDashMinimumHeight:
		if (PosY > AirDashMinimumHeight && SpeedY > 0)
			return true;
		if (PosY > 100000 && SpeedY <= 0)
			return true;
		break;
	case StateCondition::CloseNormal:
		if (abs(PosX - Enemy->PosX) < CloseNormalRange && !FarNormalForceEnable)
			return true;
		break;
	case StateCondition::FarNormal:
		if (abs(PosX - Enemy->PosX) > CloseNormalRange || FarNormalForceEnable)
			return true;
		break;
	case StateCondition::MeterNotZero:
		if (GameState->StoredBattleState.Meter[PlayerIndex] > 0)
			return true;
		break;
	case StateCondition::MeterQuarterBar:
		if (GameState->StoredBattleState.Meter[PlayerIndex] >= 2500)
			return true;
		break;
	case StateCondition::MeterHalfBar:
		if (GameState->StoredBattleState.Meter[PlayerIndex] >= 5000)
			return true;
		break;
	case StateCondition::MeterOneBar:
		if (GameState->StoredBattleState.Meter[PlayerIndex] >= 10000)
			return true;
		break;
	case StateCondition::MeterTwoBars:
		if (GameState->StoredBattleState.Meter[PlayerIndex] >= 20000)
			return true;
		break;
	case StateCondition::MeterThreeBars:
		if (GameState->StoredBattleState.Meter[PlayerIndex] >= 30000)
			return true;
		break;
	case StateCondition::MeterFourBars:
		if (GameState->StoredBattleState.Meter[PlayerIndex] >= 40000)
			return true;
		break;
	case StateCondition::MeterFiveBars:
		if (GameState->StoredBattleState.Meter[PlayerIndex] >= 50000)
			return true;
		break;
	default:
		return false;
	}
	return false;
}

void PlayerCharacter::SetAirDashTimer(bool IsForward)
{
	if (IsForward)
	{
		AirDashTimer = AirDashTimerMax = FAirDashTime;
		AirDashNoAttackTime = FAirDashNoAttackTime;
	}
	else
	{
		AirDashTimer = AirDashTimerMax = BAirDashTime;
		AirDashNoAttackTime = BAirDashNoAttackTime;
	}
}

bool PlayerCharacter::CheckInput(InputCondition Input)
{
	return InputBuffer.CheckInputCondition(Input);
}

void PlayerCharacter::EnableAttacks()
{
	EnableState(ENB_NormalAttack);
	EnableState(ENB_SpecialAttack);
	EnableState(ENB_SuperAttack);
}

void PlayerCharacter::HandleHitAction()
{
	if (CurrentHealth <= 0)
	{
		IsDead = true;
		if (PosY <= 0)
		{
			JumpToState("Crumple");
		}
		else
		{
			if (ReceivedHitAction == HACT_AirFaceUp)
				JumpToState("BLaunch");
			else if (ReceivedHitAction == HACT_AirVertical)
				JumpToState("VLaunch");
			else if (ReceivedHitAction == HACT_AirFaceDown)
				JumpToState("FLaunch");
			else if (ReceivedHitAction == HACT_Blowback)
				JumpToState("Blowback");
			else
				JumpToState("BLaunch");
		}
		ReceivedHitAction = HACT_None;
		ReceivedAttackLevel = -1;
		return;
	}
	switch (ReceivedHitAction)
	{
	case HACT_GroundNormal:
		if (CurrentActionFlags == ACT_Standing)
		{
			if (ReceivedAttackLevel == 0)
				JumpToState("Hitstun0");
			else if (ReceivedAttackLevel == 1)
				JumpToState("Hitstun1");
			else if (ReceivedAttackLevel == 2)
				JumpToState("Hitstun2");
			else if (ReceivedAttackLevel == 3)
				JumpToState("Hitstun3");
			else if (ReceivedAttackLevel == 4)
				JumpToState("Hitstun4");
		}
		else if (CurrentActionFlags == ACT_Crouching)
		{
			if (ReceivedAttackLevel == 0)
				JumpToState("CrouchHitstun0");
			else if (ReceivedAttackLevel == 1)
				JumpToState("CrouchHitstun1");
			else if (ReceivedAttackLevel == 2)
				JumpToState("CrouchHitstun2");
			else if (ReceivedAttackLevel == 3)
				JumpToState("CrouchHitstun3");
			else if (ReceivedAttackLevel == 4)
				JumpToState("CrouchHitstun4");
			Hitstun += 2;
		}
		break;
	case HACT_AirNormal:
		JumpToState("BLaunch");
		break;
	case HACT_Crumple:
		JumpToState("Crumple");
		break;
	case HACT_ForceCrouch:
		CurrentActionFlags = ACT_Crouching;
		if (ReceivedAttackLevel == 0)
			JumpToState("CrouchHitstun0");
		else if (ReceivedAttackLevel == 1)
			JumpToState("CrouchHitstun1");
		else if (ReceivedAttackLevel == 2)
			JumpToState("CrouchHitstun2");
		else if (ReceivedAttackLevel == 3)
			JumpToState("CrouchHitstun3");
		else if (ReceivedAttackLevel == 4)
			JumpToState("CrouchHitstun4");
		Hitstun += 2;
		break;
	case HACT_ForceStand:
		CurrentActionFlags = ACT_Standing;
		if (ReceivedAttackLevel == 0)
			JumpToState("Hitstun0");
		else if (ReceivedAttackLevel == 1)
			JumpToState("Hitstun1");
		else if (ReceivedAttackLevel == 2)
			JumpToState("Hitstun2");
		else if (ReceivedAttackLevel == 3)
			JumpToState("Hitstun3");
		else if (ReceivedAttackLevel == 4)
			JumpToState("Hitstun4");
		break;
	case HACT_GuardBreakCrouch:
		JumpToState("GuardBreakCrouch");
		break;
	case HACT_GuardBreakStand:
		JumpToState("GuardBreak");
		break;
	case HACT_AirFaceUp:
		JumpToState("BLaunch");
		break;
	case HACT_AirVertical:
		JumpToState("VLaunch");
		break;
	case HACT_AirFaceDown:
		JumpToState("FLaunch");
		break;
	case HACT_Blowback:
		JumpToState("Blowback");
		break;
	case HACT_None: break;
	default: ;
	}
	EnableInertia();
	DisableAll();
	ReceivedHitAction = HACT_None;
	ReceivedAttackLevel = -1;
}

bool PlayerCharacter::IsCorrectBlock(BlockType BlockType)
{
	if (BlockType != BLK_None)
	{
		if (CheckInput(InputCondition::Input_Left) && PosY > 0 && !CheckInput(InputCondition::Input_Right))
		{
			return true;
		}
		if (CheckInput(InputCondition::Input_4) && BlockType != BLK_Low && !CheckInput(InputCondition::Input_Right))
		{
			return true;
		}
		if (CheckInput(InputCondition::Input_1) && BlockType != BLK_High && !CheckInput(InputCondition::Input_Right))
		{
			return true;
		}
		if (CheckInput(InputCondition::Input_Left) && BlockType == BLK_Mid && !CheckInput(InputCondition::Input_Right))
		{
			return true;
		}
	}
	return false;
}

void PlayerCharacter::HandleBlockAction()
{
	EnableInertia();
	if (CheckInputRaw(InputLeft) && !CheckInputRaw(InputDown) && PosY <= 0)
	{
		JumpToState("Block");
		CurrentActionFlags = ACT_Standing;
	}
	else if (CheckInputRaw(InputDownLeft) && PosY <= 0)
	{
		JumpToState("CrouchBlock");
		CurrentActionFlags = ACT_Crouching;
	}
	else
	{
		JumpToState("AirBlock");
	}
}

void PlayerCharacter::EnableJumpCancel(bool Enable)
{
	JumpCancel = Enable;
}

void PlayerCharacter::EnableBAirDashCancel(bool Enable)
{
	BAirDashCancel = Enable;
}

void PlayerCharacter::EnableChainCancel(bool Enable)
{
	ChainCancelEnabled = Enable;
}

void PlayerCharacter::EnableWhiffCancel(bool Enable)
{
	WhiffCancelEnabled = Enable;
}

void PlayerCharacter::EnableSpecialCancel(bool Enable)
{
	SpecialCancel = Enable;
	SuperCancel = Enable;
}

void PlayerCharacter::EnableSuperCancel(bool Enable)
{
	SuperCancel = Enable;
}

void PlayerCharacter::SetDefaultLandingAction(bool Enable)
{
	DefaultLandingAction = Enable;
}

void PlayerCharacter::SetStrikeInvulnerable(bool Invulnerable)
{
	StrikeInvulnerable = Invulnerable;
}

void PlayerCharacter::SetThrowInvulnerable(bool Invulnerable)
{
	ThrowInvulnerable = Invulnerable;
}

void PlayerCharacter::SetProjectileInvulnerable(bool Invulnerable)
{
	ProjectileInvulnerable = Invulnerable;
}


void PlayerCharacter::SetHeadInvulnerable(bool Invulnerable)
{
	HeadInvulnerable = Invulnerable;
}

void PlayerCharacter::ForceEnableFarNormal(bool Enable)
{
	FarNormalForceEnable = Enable;
}

void PlayerCharacter::SetThrowActive(bool Active)
{
	ThrowActive = Active;
}

void PlayerCharacter::ThrowExe()
{
	JumpToState(ExeStateName.GetString());
	ThrowActive = false;
}

void PlayerCharacter::ThrowEnd()
{
	if (!Enemy) return;
	Enemy->IsThrowLock = false;
}

void PlayerCharacter::SetThrowRange(int32_t InThrowRange)
{
	ThrowRange = InThrowRange;
}

void PlayerCharacter::SetThrowExeState(char* ExeState)
{
	ExeStateName.SetString(ExeState);
}

void PlayerCharacter::SetThrowPosition(int32_t ThrowPosX, int32_t ThrowPosY)
{
	if (!Enemy) return;
	if (FacingRight)
		Enemy->PosX = R + ThrowPosX;
	else
		Enemy->PosX = L - ThrowPosX;
	Enemy->PosY = PosY + ThrowPosY;
}

void PlayerCharacter::SetThrowLockCel(int32_t Index)
{
	if (Index < Enemy->ThrowLockCels.size())
	{
		Enemy->SetCelName(Enemy->ThrowLockCels[Index].GetString());
	}
}

void PlayerCharacter::PlayVoice(char* Name)
{
	/*if (VoiceData != nullptr)
	{
		for (FSoundDataStruct SoundStruct : VoiceData->SoundDatas)
		{
			if (SoundStruct.Name == Name)
			{
				GameState->PlayVoiceLine(SoundStruct.SoundWave, SoundStruct.MaxDuration, ObjNumber - 400);
				break;
			}
		}
	}*/
}

BattleActor* PlayerCharacter::AddBattleActor(char* InStateName, int PosXOffset, int PosYOffset)
{
	int Index = 0;
	for (CString<64> String : ObjectStateNames)
	{
		if (!strcmp(String.GetString(), ObjectStateName.GetString()))
		{
			break;
		}
		Index++;
	}
	if (Index < ObjectStateNames.size())
	{
		if (!FacingRight)
			PosXOffset = -PosXOffset;
		for (int i = 0; i < 32; i++)
		{
			if (ChildBattleActors[i] == nullptr)
			{
				ChildBattleActors[i] = GameState->AddBattleActor(ObjectStates[Index],
					PosX + PosXOffset, PosY + PosYOffset, FacingRight, this);
				return ChildBattleActors[i];
			}
			if (!ChildBattleActors[i]->IsActive)
			{
				ChildBattleActors[i] = GameState->AddBattleActor(ObjectStates[Index],
					PosX + PosXOffset, PosY + PosYOffset, FacingRight, this);
				return ChildBattleActors[i];
			}
		}
	}
	return nullptr;
}

void PlayerCharacter::AddBattleActorToStorage(BattleActor* InActor, int Index)
{
	if (Index < 16)
	{
		StoredBattleActors[Index] = InActor;
	}
}

void PlayerCharacter::PlayCommonCameraAnim(char* Name)
{
}

void PlayerCharacter::PlayCharaCameraAnim(char* Name)
{
}

void PlayerCharacter::EnableFAirDashCancel(bool Enable)
{
	FAirDashCancel = Enable;
}

void PlayerCharacter::AddChainCancelOption(CString<64> Option)
{
	for (int i = 0; i < CancelArraySize; i++)
	{
		if (ChainCancelOptionsInternal[i] == -1)
		{
			ChainCancelOptionsInternal[i] = StateMachine.GetStateIndex(Option);
		}
	}
}

void PlayerCharacter::AddWhiffCancelOption(CString<64> Option)
{
	for (int i = 0; i < CancelArraySize; i++)
	{
		if (WhiffCancelOptionsInternal[i] == -1)
		{
			WhiffCancelOptionsInternal[i] = StateMachine.GetStateIndex(Option);
		}
	}
}

bool PlayerCharacter::FindChainCancelOption(char* Name)
{
	if (HasHit && IsAttacking && ChainCancelEnabled)
	{
		for (int i = 0; i < CancelArraySize; i++)
		{
			CString<64> CName;
			CName.SetString(Name);
			if (ChainCancelOptionsInternal[i] == StateMachine.GetStateIndex(CName) && ChainCancelOptionsInternal[i] != -1)
			{
				return true;
			}
		}
	}
	return false;
}

bool PlayerCharacter::FindWhiffCancelOption(char* Name)
{
	if (WhiffCancelEnabled)
	{
		for (int i = 0; i < CancelArraySize; i++)
		{
			CString<64> CName;
			CName.SetString(Name);
			if (WhiffCancelOptionsInternal[i] == StateMachine.GetStateIndex(CName) && WhiffCancelOptionsInternal[i] != -1)
			{
				return true;
			}
		}
	}
	return false;
}

void PlayerCharacter::StartSuperFreeze(int Duration)
{
	GameState->StartSuperFreeze(Duration);
	StateMachine.CurrentState->OnSuperFreeze();
}

void PlayerCharacter::BattleHudVisibility(bool Visible)
{
	//GameState->BattleHudVisibility(Visible);
}

void PlayerCharacter::DisableLastInput()
{
	InputBuffer.InputDisabled[89] = InputBuffer.InputBufferInternal[89];
}

void PlayerCharacter::OnStateChange()
{
	if (MiscFlags & MISC_FlipEnable)
		HandleFlip();
	DisableLastInput();
	StateName.SetString("");
	HitEffectName.SetString("");
	Gravity = JumpGravity;
	for (int i = 0; i < CancelArraySize; i++)
	{
		ChainCancelOptionsInternal[i] = -1;
		WhiffCancelOptionsInternal[i] = -1;
	}
	ChainCancelEnabled = true;
	WhiffCancelEnabled = false;
	JumpCancel = false;
	FAirDashCancel = false;
	BAirDashCancel = false;
	HasHit = false;
	AnimTime = -1; //reset anim time
	ActionTime = -1; //reset action time
	DefaultLandingAction = true;
	DefaultCommonAction = true;
	FarNormalForceEnable = false;
	SpeedXPercent = 100;
	SpeedXPercentPerFrame = false;
	IsAttacking = false;
	ThrowActive = false;
	StrikeInvulnerable = false;
	ThrowInvulnerable = false;
	ProjectileInvulnerable = false;
	HeadInvulnerable = false;
	AttackHeadAttribute = false;
	AttackProjectileAttribute = false;
	PushWidthExpand = 0;
	LoopCounter = 0;
	StateVal1 = 0;
	StateVal2 = 0;
	StateVal3 = 0;
	StateVal4 = 0;
	StateVal5 = 0;
	StateVal6 = 0;
	StateVal7 = 0;
	StateVal8 = 0;
	FlipInputs = false;
	PushCollisionActive = true;
}

void PlayerCharacter::SaveForRollbackPlayer(unsigned char* Buffer)
{
	memcpy(Buffer, &PlayerSync, SIZEOF_PLAYERCHARACTER);
}

void PlayerCharacter::LoadForRollbackPlayer(unsigned char* Buffer)
{
	memcpy(&PlayerSync, Buffer, SIZEOF_PLAYERCHARACTER);
}

void PlayerCharacter::HandleThrowCollision()
{
	if (IsAttacking && ThrowActive && !Enemy->ThrowInvulnerable && !Enemy->GetInternalValue(VAL_IsStunned) &&
		(Enemy->PosY <= 0 && PosY <= 0 && Enemy->KnockdownTime < 0 || Enemy->PosY > 0 && PosY > 0))
	{
		int ThrowPosX;
		if (FacingRight)
			ThrowPosX = R + ThrowRange;
		else
			ThrowPosX = L - ThrowRange;
		if ((PosX <= Enemy->PosX && ThrowPosX >= Enemy->L || PosX > Enemy->PosX && ThrowPosX <= Enemy->R)
			&& T >= Enemy->B && T <= Enemy->T)
		{
			if (Enemy->CheckInput(InputCondition::Input_L_And_S))
			{
				JumpToState("GuardBreak");
				Enemy->JumpToState("GuardBreak");
				SetInertia(-15000);
				Enemy->SetInertia(-15000);
				if (TeamIndex == 0)
					Hitstop = 1;
				else
					Enemy->Hitstop = 1;
			}
			else
			{
				Enemy->JumpToState("Hitstun0");
				Enemy->IsThrowLock = true;
				Enemy->ThrowTechTimer = 10;
				ThrowExe();
			}
		}
	}
}

bool PlayerCharacter::CheckKaraCancel(StateType InStateType)
{
	//two checks: if it's an attack, and if the given state type has a higher or equal priority to the current state
	if (InStateType == StateType::NormalThrow && StateMachine.CurrentState->Type < InStateType && StateMachine.CurrentState->Type >= StateType::NormalAttack && ActionTime < 3)
	{
		return true;
	}
	if (InStateType == StateType::SpecialAttack && StateMachine.CurrentState->Type < InStateType && StateMachine.CurrentState->Type >= StateType::NormalAttack && ActionTime < 3)
	{
		return true;
	}
	if (InStateType == StateType::SuperAttack && StateMachine.CurrentState->Type < InStateType && StateMachine.CurrentState->Type >= StateType::NormalAttack && ActionTime < 3)
	{
		return true;
	}	
	return false;
}

bool PlayerCharacter::CheckObjectPreventingState(int InObjectID)
{
	for (int i = 0; i < 32; i++)
	{
		if (ChildBattleActors[i] != nullptr)
		{
			if (ChildBattleActors[i]->IsActive)
			{
				if (ChildBattleActors[i]->ObjectID == InObjectID && ChildBattleActors[i]->ObjectID != 0)
					return true;
			}
		}
	}
	return false;
}

void PlayerCharacter::ResetForRound()
{
	PosX = 0;
	PosY = 0;
	PrevPosX = 0;
	PrevPosY = 0;
	SpeedX = 0;
	SpeedY = 0;
	Gravity = 1900;
	Inertia = 0;
	ActionTime = -1;
	PushHeight = 0;
	PushHeightLow = 0;
	PushWidth = 0;
	PushWidthExpand = 0;
	Hitstop = 0;
	L = 0;
	R = 0;
	T = 0;
	B = 0;
	NormalHitEffect = HitEffect();
	CounterHitEffect = HitEffect();
	HitActive = false;
	IsAttacking = false;
	AttackHeadAttribute = false;
	AttackProjectileAttribute = false;
	RoundStart = true;
	HasHit = false;
	SpeedXPercent = 100;
	SpeedXPercentPerFrame = false;
	FacingRight = false;
	ScreenCollisionActive = false;
	StateVal1 = 0;
	StateVal2 = 0;
	StateVal3 = 0;
	StateVal4 = 0;
	StateVal5 = 0;
	StateVal6 = 0;
	StateVal7 = 0;
	StateVal8 = 0;
	MiscFlags = 0;
	SuperFreezeTime = -1;
	CelNameInternal.SetString("");
	HitEffectName.SetString("");
	AnimTime = -1;
	HitPosX = 0;
	HitPosY = 0;
	ObjectID = 0;
	CurrentEnableFlags = 0;
	CurrentHealth = 0;
	CurrentAirJumpCount = 0;
	CurrentAirDashCount = 0;
	AirDashTimerMax = 0;
	JumpCancel = false;
	FAirDashCancel = false;
	BAirDashCancel = false;
	SpecialCancel = false;
	SuperCancel = false;
	DefaultLandingAction = false;
	FarNormalForceEnable = false;
	IsDead = false;
	ThrowRange = 0;
	CurrentWallBounceEffect = WallBounceEffect();
	CurrentGroundBounceEffect = GroundBounceEffect();
	ThrowActive = false;
	IsThrowLock = false;
	IsOnScreen = false;
	DeathCamOverride = false;
	IsKnockedDown = false;
	FlipInputs = false;
	Inputs = 0;
	CurrentActionFlags = 0;
	AirDashTimer = 0;
	Hitstun = -1;
	Blockstun = -1;
	Untech = -1;
	TotalProration = 10000;
	ComboCounter = 0;
	ComboTimer = 0;
	LoopCounter = 0;
	ThrowTechTimer = 0;
	HasBeenOTG = 0;
	WallTouchTimer = 0;
	TouchingWall = false;
	ChainCancelEnabled = true;
	WhiffCancelEnabled = false;
	StrikeInvulnerable = false;
	WhiffCancelEnabled = false;
	StrikeInvulnerable = false;
	ThrowInvulnerable = false;
	ProjectileInvulnerable = false;
	HeadInvulnerable = false;
	RoundWinTimer = 300;
	RoundWinInputLock = false;
	MeterCooldownTimer = 0;
	PlayerVal1 = 0;
	PlayerVal2 = 0;
	PlayerVal3 = 0;
	PlayerVal4 = 0;
	PlayerVal5 = 0;
	PlayerVal6 = 0;
	PlayerVal7 = 0;
	PlayerVal8 = 0;
	for (int i = 0; i < CancelArraySize; i++)
	{
		ChainCancelOptionsInternal[i] = -1;
		WhiffCancelOptionsInternal[i] = -1;
	}
	JumpToState("Stand");
	ExeStateName.SetString("");
	ReceivedHitAction = HACT_None;
	ReceivedAttackLevel = -1;
	for (int i = 0; i < 90; i++)
		InputBuffer.InputBufferInternal[i] = InputNeutral;
	InitPlayer();
}

void PlayerCharacter::HandleWallBounce()
{
	if (Untech > 0)
	{
		if (CurrentWallBounceEffect.WallBounceInCornerOnly)
		{
			if (PosX > 1200000 || PosX < -1200000)
			{
				if (CurrentWallBounceEffect.WallBounceCount > 0)
				{
					TouchingWall = false;
					CurrentWallBounceEffect.WallBounceCount--;
					SetSpeedX(CurrentWallBounceEffect.WallBounceXSpeed);
					SetSpeedY(CurrentWallBounceEffect.WallBounceYSpeed);
					SetGravity(CurrentWallBounceEffect.WallBounceGravity);
					if (CurrentWallBounceEffect.WallBounceUntech > 0)
						Untech = CurrentWallBounceEffect.WallBounceUntech;
					JumpToState("FLaunch");
				}
			}
			return;
		}
		if (TouchingWall)
		{
			if (CurrentWallBounceEffect.WallBounceCount > 0)
			{
				TouchingWall = false;
				CurrentWallBounceEffect.WallBounceCount--;
				SetSpeedX(CurrentWallBounceEffect.WallBounceXSpeed);
				SetSpeedY(CurrentWallBounceEffect.WallBounceYSpeed);
				SetGravity(CurrentWallBounceEffect.WallBounceGravity);
				if (CurrentWallBounceEffect.WallBounceUntech > 0)
					Untech = CurrentWallBounceEffect.WallBounceUntech;
				JumpToState("FLaunch");
			}
		}
	}
}

void PlayerCharacter::HandleGroundBounce()
{
	if (KnockdownTime > 0 || Untech > 0)
	{
		if (CurrentGroundBounceEffect.GroundBounceCount > 0)
		{
			CurrentGroundBounceEffect.GroundBounceCount--;
			SetInertia(CurrentGroundBounceEffect.GroundBounceXSpeed);
			SetSpeedY(CurrentGroundBounceEffect.GroundBounceYSpeed);
			SetGravity(CurrentGroundBounceEffect.GroundBounceGravity);
			if (CurrentGroundBounceEffect.GroundBounceUntech > 0)
				Untech = CurrentGroundBounceEffect.GroundBounceUntech;
			JumpToState("BLaunch");
		}
	}
}
void PlayerCharacter::LogForSyncTest(FILE* file)
{
	BattleActor::LogForSyncTest(file);
	if(file)
	{
		fprintf(file,"PlayerCharacter:\n");
		fprintf(file,"\tEnableFlags: %d\n", CurrentEnableFlags);
		fprintf(file,"\tCurrentAirJumpCount: %d\n", CurrentAirJumpCount);
		fprintf(file,"\tCurrentAirDashCount: %d\n", CurrentAirDashCount);
		fprintf(file,"\tAirDashTimerMax: %d\n", AirDashTimerMax);
		fprintf(file,"\tCurrentHealth: %d\n", CurrentHealth);
		fprintf(file,"\tJumpCancel: %d\n", JumpCancel);
		fprintf(file,"\tFAirDashCancel: %d\n", FAirDashCancel);
		fprintf(file,"\tBAirDashCancel: %d\n", BAirDashCancel);
		fprintf(file,"\tSpecialCancel: %d\n", SpecialCancel);
		fprintf(file,"\tSuperCancel: %d\n", SuperCancel);
		fprintf(file,"\tBAirDashCancel: %d\n", DefaultLandingAction);
		fprintf(file,"\tInputs: %d\n", InputBuffer.InputBufferInternal[89]);
		fprintf(file,"\tActionFlags: %d\n", CurrentActionFlags);
		fprintf(file,"\tAirDashTimer: %d\n", AirDashTimer);
		fprintf(file,"\tHitstun: %d\n", Hitstun);
		fprintf(file,"\tUntech: %d\n", Untech);
		fprintf(file,"\tUntech: %d\n", TouchingWall);
		fprintf(file,"\tChainCancelEnabled: %d\n", ChainCancelEnabled);
		fprintf(file,"\tWhiffCancelEnabled: %d\n", WhiffCancelEnabled);
		fprintf(file,"\tStrikeInvulnerable: %d\n", StrikeInvulnerable);
		fprintf(file,"\tThrowInvulnerable: %d\n", ThrowInvulnerable);
		int ChainCancelChecksum = 0;
		for (int i = 0; i < 0x20; i++)
		{
			ChainCancelChecksum += ChainCancelOptionsInternal[i];
		}
		fprintf(file,"\tChainCancelOptions: %d\n", ChainCancelChecksum);
		int WhiffCancelChecksum = 0;
		for (int i = 0; i < 0x20; i++)
		{
			WhiffCancelChecksum += WhiffCancelOptionsInternal[i];
		}
		fprintf(file,"\tChainCancelOptions: %d\n", WhiffCancelChecksum);
		if (StateMachine.States.size() != 0)
			fprintf(file,"\tStateName: %s\n", StateName.GetString());
		fprintf(file,"\tEnemy: %p\n", Enemy);
	}
}
