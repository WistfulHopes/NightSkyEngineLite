// Fill out your copyright notice in the Description page of Project Settings.

#include <cstdlib>
#include "BattleActor.h"
#include "FighterGameState.h"
#include "PlayerCharacter.h"
#include "../State.h"
#include "../../Utilities/CreateSpriteList.h"

BattleActor::BattleActor()
{
	ObjSync = 0;
	NormalHitEffect = HitEffect();
	CounterHitEffect = HitEffect();
	HitPosX = 0;
	HitPosY = 0;
	for (int i = 0; i < CollisionArraySize; i++)
	{
		CollisionBoxes[i] = CollisionBox();
	}
	ObjectID = 0;
	Player = nullptr;
	ObjSyncEnd = 0;
	ObjNumber = 0;
	GameState = nullptr;
	ObjectState = nullptr;
}

void BattleActor::InitObject()
{
	RoundStart = false;
	ObjectState->ObjectParent = this;
	ObjectState->Parent = Player;
	Hitstop = 0;
	if (PosY == 0 && PrevPosY != 0)
		Gravity = 1900;
	ObjectState->OnEnter();
}

void BattleActor::Update()
{
	if (DeactivateOnNextUpdate)
	{
		ResetObject();
		return;
	}

	L = PosX - PushWidth / 2; //sets pushboxes
	R = PosX + PushWidth / 2;
	if (FacingRight)
		R += PushWidthExpand;
	else
		L -= PushWidthExpand;
	T = PosY + PushHeight;
	B = PosY - PushHeightLow;
	
	if (MiscFlags & MISC_FlipEnable)
		HandleFlip();
	
	if (SuperFreezeTime > 0)
	{
		SuperFreezeTime--;
		return;
	}
	if (SuperFreezeTime == 0)
	{
		if (ObjectState)
			ObjectState->OnSuperFreezeEnd();
		PauseRoundTimer(false);
		GameState->StoredBattleState.PauseParticles = false;
	}
	SuperFreezeTime--;
	
	if (Hitstop > 0) //break if hitstop active.
	{
		Hitstop--;
		return;
	}
	Hitstop--;
	GetBoxes(); //get boxes from cel name
	if (IsPlayer && Player->IsThrowLock) //if locked by throw, return
		return;
	
	Move(); //handle movement
	
	AnimTime++; //increments counters
	ActionTime++;
	ActiveTime++;
	
	if (IsPlayer) //initializes player only values
	{
		if (Player->CurrentActionFlags == ACT_Standing) //set pushbox values based on state
		{
			PushWidth = Player->StandPushWidth;
			PushHeight = Player->StandPushHeight;
			PushHeightLow = 0;
		}
		else if (Player->CurrentActionFlags == ACT_Crouching)
		{
			PushWidth = Player->CrouchPushWidth;
			PushHeight = Player->CrouchPushHeight;
			PushHeightLow = 0;
		}
		else if (Player->CurrentActionFlags == ACT_Jumping)
		{
			PushWidth = Player->AirPushWidth;
			PushHeight = Player->AirPushHeight;
			PushHeightLow = Player->AirPushHeightLow;
		}
		if (RoundStart)
		{
			if (Player->PlayerIndex == 0)
			{
				FacingRight = true;
				PosX = -200000;
			}
			else
			{
				FacingRight = false;
				PosX = 200000;
			}
		}
	}
	
	if (!IsPlayer)
	{
		ObjectState->OnUpdate(1/60);
	}
	RoundStart = false; //round has started
}

void BattleActor::Move()
{
	PrevPosX = PosX; //set previous pos values
	PrevPosY = PosY;
	
	if (IsPlayer && Player != nullptr)
	{
		if (Player->AirDashTimer <= 0) //only set gravity if air dash timer isn't active
		{
			AddPosY(SpeedY);
			if (PosY > 0)
				SpeedY -= Gravity;
		}
	}
	else
	{
		AddPosY(SpeedY);
		if (PosY > 0)
			SpeedY -= Gravity;
	}
	
	SpeedX = SpeedX * SpeedXPercent / 100;
	if (!SpeedXPercentPerFrame)
		SpeedXPercent = 100;
	
	AddPosX(SpeedX); //apply speed
	
	if (MiscFlags & MISC_InertiaEnable) //only use inertia if enabled
	{
		if (PosY <= 0) //only decrease inertia if grounded
		{
			Inertia = Inertia - Inertia / 10;
		}
		if (Inertia > -875 && Inertia < 875) //if inertia small enough, set to zero
		{
			ClearInertia();
		}
		AddPosX(Inertia);
	}
	if (PosY < 0) //if on ground, force y values to zero
	{
		PosY = 0;
		SpeedY = 0;
	}
	if (ScreenCollisionActive)
	{
		if (PosX < -1200000)
		{
			PosX = -1200001;
		}
		else if (PosX > 1200000)
		{
			PosX = 1200001;
		}
	}
}

void BattleActor::SetPosX(int32_t InPosX)
{
	PosX = InPosX;
}

void BattleActor::SetPosY(int32_t InPosY)
{
	PosY = InPosY;
}

void BattleActor::AddPosX(int32_t InPosX)
{
	if (FacingRight)
	{
		PosX += InPosX;
	}
	else
	{
		PosX -= InPosX;
	}
}

void BattleActor::AddPosXRaw(int32_t InPosX)
{
	PosX += InPosX;
}

void BattleActor::AddPosY(int32_t InPosY)
{
	PosY += InPosY;
}

void BattleActor::SetSpeedX(int32_t InSpeedX)
{
	SpeedX = InSpeedX;
}

void BattleActor::SetSpeedY(int32_t InSpeedY)
{
	SpeedY = InSpeedY;
}

void BattleActor::SetGravity(int32_t InGravity)
{
	Gravity = InGravity;
}

void BattleActor::HaltMomentum()
{
	SpeedX = 0;
	SpeedY = 0;
	Gravity = 0;
	ClearInertia();
}

void BattleActor::SetPushWidthExpand(int32_t Expand)
{
	PushWidthExpand = Expand;
}

int32_t BattleActor::GetInternalValue(InternalValue InternalValue, ObjType ObjType)
{
	BattleActor* Obj;
	switch (ObjType)
	{
	case OBJ_Self:
		Obj = this;
		break;
	case OBJ_Enemy:
		Obj = Player->Enemy;
		break;
	case OBJ_Parent:
		Obj = Player;
		break;
	default:
		Obj = this;
		break;
	}
	switch (InternalValue)
	{
	case VAL_StoredRegister:
		return Obj->StoredRegister;
	case VAL_ActionFlag:
		if (Obj->IsPlayer && Obj->Player != nullptr) //only available as player character
			return Obj->Player->CurrentActionFlags;
		break;
	case VAL_PosX:
		return Obj->PosX;
	case VAL_PosY:
		return Obj->PosY;
	case VAL_SpeedX:
		return Obj->SpeedX;
	case VAL_SpeedY:
		return Obj->SpeedY;
	case VAL_ActionTime:
		return Obj->ActionTime;
	case VAL_Inertia:
		return Obj->Inertia;
	case VAL_FacingRight:
		return Obj->FacingRight;
	case VAL_DistanceToBackWall:
		return -1440000 + Obj->PosX;
	case VAL_DistanceToFrontWall:
		return 1440000 + Obj->PosX;
	case VAL_IsAir:
		return Obj-> PosY > 0;
	case VAL_IsLand:
		return Obj->PosY <= 0;
	case VAL_IsStunned:
		if (Obj->IsPlayer && Obj->Player != nullptr) //only available as player character
			return Obj->Player->IsStunned;
		break;
	case VAL_Health:
		if (Obj->IsPlayer && Obj->Player != nullptr) //only available as player character
			return Obj->Player->CurrentHealth;
		break;
	case VAL_Meter:
		if (Obj->IsPlayer && Obj->Player != nullptr) //only available as player character
			return GameState->StoredBattleState.Meter[Obj->Player->PlayerIndex];
		break;
	case VAL_Angle: break;
	case VAL_PlayerVal1: 
		if (Obj->IsPlayer && Obj->Player != nullptr) //only available as player character
			return Obj->Player->PlayerVal1;
		break;
	case VAL_PlayerVal2: 
		if (Obj->IsPlayer && Obj->Player != nullptr) //only available as player character
			return Obj->Player->PlayerVal2;
		break;
	case VAL_PlayerVal3: 
		if (Obj->IsPlayer && Obj->Player != nullptr) //only available as player character
			return Obj->Player->PlayerVal3;
		break;
	case VAL_PlayerVal4: 
		if (Obj->IsPlayer && Obj->Player != nullptr) //only available as player character
			return Obj->Player->PlayerVal4;
		break;
	case VAL_PlayerVal5: 
		if (Obj->IsPlayer && Obj->Player != nullptr) //only available as player character
			return Obj->Player->PlayerVal5;
		break;
	case VAL_PlayerVal6: 
		if (Obj->IsPlayer && Obj->Player != nullptr) //only available as player character
			return Obj->Player->PlayerVal6;
		break;
	case VAL_PlayerVal7: 
		if (Obj->IsPlayer && Obj->Player != nullptr) //only available as player character
			return Obj->Player->PlayerVal7;
		break;
	case VAL_PlayerVal8: 
		if (Obj->IsPlayer && Obj->Player != nullptr) //only available as player character
			return Obj->Player->PlayerVal8;
		break;
	case VAL_DefaultCommonAction:
		return Obj->DefaultCommonAction;
	default:
		return 0;
	}
	return 0;
}

void BattleActor::SetInternalValue(InternalValue InternalValue, int32_t Val, ObjType ObjType)
{
	BattleActor* Obj;
	switch (ObjType)
	{
	case OBJ_Self:
		Obj = this;
		break;
	case OBJ_Enemy:
		Obj = Player->Enemy;
		break;
	case OBJ_Parent:
		Obj = Player;
		break;
	default:
		Obj = this;
		break;
	}
	switch (InternalValue)
	{
	case VAL_StoredRegister:
		Obj->StoredRegister = Val;
	case VAL_ActionFlag:
		if (Obj->IsPlayer && Obj->Player != nullptr) //only available as player character
			Obj->Player->CurrentActionFlags = Val;
		break;
	case VAL_PosX:
		Obj->PosX = Val;
	case VAL_PosY:
		Obj->PosY = Val;
	case VAL_SpeedX:
		Obj->SpeedX = Val;
	case VAL_SpeedY:
		Obj->SpeedY = Val;
	case VAL_ActionTime:
		Obj->ActionTime = Val;
	case VAL_Inertia:
		Obj->Inertia = Val;
	case VAL_FacingRight:
		Obj->FacingRight = Val;
	case VAL_DistanceToBackWall:
	case VAL_DistanceToFrontWall:
	case VAL_IsAir:
	case VAL_IsLand:
	case VAL_IsStunned:
		break;
	case VAL_Health:
		if (Obj->IsPlayer && Obj->Player != nullptr) //only available as player character
			Obj->Player->CurrentHealth = Val;
		break;
	case VAL_Meter:
		if (Obj->IsPlayer && Obj->Player != nullptr) //only available as player character
			GameState->StoredBattleState.Meter[Obj->Player->PlayerIndex] = Val;
		break;
	case VAL_Angle: break;
	case VAL_PlayerVal1: 
		if (Obj->IsPlayer && Obj->Player != nullptr) //only available as player character
			Obj->Player->PlayerVal1 = Val;
		break;
	case VAL_PlayerVal2: 
		if (Obj->IsPlayer && Obj->Player != nullptr) //only available as player character
			Obj->Player->PlayerVal2 = Val;
		break;
	case VAL_PlayerVal3: 
		if (Obj->IsPlayer && Obj->Player != nullptr) //only available as player character
			Obj->Player->PlayerVal3 = Val;
		break;
	case VAL_PlayerVal4: 
		if (Obj->IsPlayer && Obj->Player != nullptr) //only available as player character
			Obj->Player->PlayerVal4 = Val;
		break;
	case VAL_PlayerVal5: 
		if (Obj->IsPlayer && Obj->Player != nullptr) //only available as player character
			Obj->Player->PlayerVal5 = Val;
		break;
	case VAL_PlayerVal6: 
		if (Obj->IsPlayer && Obj->Player != nullptr) //only available as player character
			Obj->Player->PlayerVal6 = Val;
		break;
	case VAL_PlayerVal7: 
		if (Obj->IsPlayer && Obj->Player != nullptr) //only available as player character
			Obj->Player->PlayerVal7 = Val;
		break;
	case VAL_PlayerVal8: 
		if (Obj->IsPlayer && Obj->Player != nullptr) //only available as player character
			Obj->Player->PlayerVal8 = Val;
		break;
	case VAL_DefaultCommonAction:
		Obj->DefaultCommonAction = Val;
	default:
		return;
	}
	return;
}

bool BattleActor::IsOnFrame(int32_t Frame)
{
	if (Frame == AnimTime)
	{
		return true;
	}
	return false;
}

bool BattleActor::IsStopped()
{
	return SuperFreezeTime > 0 || Hitstop > 0 || IsPlayer && Player->IsThrowLock;
}

void BattleActor::SetCelName(char* InCelName)
{
	CelNameInternal.SetString(InCelName);
}

void BattleActor::SetHitEffectName(char* InHitEffectName)
{
	HitEffectName.SetString(InHitEffectName);
}

void BattleActor::AddSpeedX(int32_t InSpeedX)
{
	SpeedX += InSpeedX;
}

void BattleActor::AddSpeedY(int32_t InSpeedY)
{
	SpeedY += InSpeedY;
}

void BattleActor::SetSpeedXPercent(int32_t Percent)
{
	SpeedXPercent = Percent;
}

void BattleActor::SetSpeedXPercentPerFrame(int32_t Percent)
{
	SpeedXPercent = Percent;
	SpeedXPercentPerFrame = true;
}

void BattleActor::SetInertia(int32_t InInertia)
{
	Inertia = InInertia;
}

void BattleActor::ClearInertia()
{
	Inertia = 0;
}

void BattleActor::EnableInertia()
{
	MiscFlags |= MISC_InertiaEnable;
}

void BattleActor::DisableInertia()
{
	MiscFlags = MiscFlags & ~MISC_InertiaEnable;
}

void BattleActor::HandlePushCollision(BattleActor* OtherObj)
{
	if (PushCollisionActive && OtherObj->PushCollisionActive)
	{
		if (Hitstop <= 0 && (!OtherObj->IsPlayer || !OtherObj->Player->IsThrowLock) || (!IsPlayer || !Player->IsThrowLock))
		{
			if (T >= OtherObj->B && B <= OtherObj->T && R >= OtherObj->L && L <= OtherObj->R)
			{
				bool IsPushLeft;
				int32_t CollisionDepth;
				int32_t PosXOffset;
				if(PosX == OtherObj->PosX)
				{
					if (PrevPosX == OtherObj->PrevPosX)
					{
						if (IsPlayer == OtherObj->IsPlayer)
						{
							if (Player->WallTouchTimer == OtherObj->Player->WallTouchTimer)
							{
								IsPushLeft = Player->TeamIndex > 0;
							}
							else
							{
								IsPushLeft = Player->WallTouchTimer > OtherObj->Player->WallTouchTimer;
								if (PosX > 0)
								{
									IsPushLeft = Player->WallTouchTimer <= OtherObj->Player->WallTouchTimer;
								}
							}
						}
						else
						{
							IsPushLeft = IsPlayer > OtherObj->IsPlayer;
						}
					}
					else
					{
						IsPushLeft = PrevPosX < OtherObj->PrevPosX;
					}
				}
				else
				{
					IsPushLeft = PosX < OtherObj->PosX;
				}
				if (IsPushLeft)
				{
					CollisionDepth = OtherObj->L - R;
				}
				else
				{
					CollisionDepth = OtherObj->R - L;
				}
				PosXOffset = CollisionDepth / 2;
				AddPosXRaw(PosXOffset);
			}
		}
	}
}

void BattleActor::SetFacing(bool NewFacingRight)
{
	FacingRight = NewFacingRight;
}

void BattleActor::FlipCharacter()
{
	FacingRight = !FacingRight;
}

void BattleActor::EnableFlip(bool Enabled)
{
	if (Enabled)
	{
		MiscFlags |= MISC_FlipEnable;
	}
	else
	{
		MiscFlags = MiscFlags & ~MISC_FlipEnable;
	}
}

void BattleActor::GetBoxes()
{
	//reimplement for new engine
}

void BattleActor::HandleHitCollision(PlayerCharacter* OtherChar)
{
	if (IsAttacking && HitActive && !OtherChar->StrikeInvulnerable && OtherChar != Player)
	{
		if (!(AttackHeadAttribute && OtherChar->HeadInvulnerable) && !(AttackProjectileAttribute && OtherChar->ProjectileInvulnerable))
		{
			for (int32_t i = 0; i < CollisionArraySize; i++)
			{
				if (CollisionBoxes[i].Type == Hitbox)
				{
					for (int32_t j = 0; j < CollisionArraySize; j++)
					{
						if (OtherChar->CollisionBoxes[j].Type == Hurtbox)
						{
							CollisionBox Hitbox = CollisionBoxes[i];

							CollisionBox Hurtbox = OtherChar->CollisionBoxes[j];

							if (FacingRight)
							{
								Hitbox.PosX += PosX;
							}
							else
							{
								Hitbox.PosX = -Hitbox.PosX + PosX;  
							}
							Hitbox.PosY += PosY;
							if (OtherChar->FacingRight)
							{
								Hurtbox.PosX += OtherChar->PosX;
							}
							else
							{
								Hurtbox.PosX = -Hurtbox.PosX + OtherChar->PosX;  
							}
							Hurtbox.PosY += OtherChar->PosY;
							
							if (Hitbox.PosY + Hitbox.SizeY / 2 >= Hurtbox.PosY - Hurtbox.SizeY / 2
								&& Hitbox.PosY - Hitbox.SizeY / 2 <= Hurtbox.PosY + Hurtbox.SizeY / 2
								&& Hitbox.PosX + Hitbox.SizeX / 2 >= Hurtbox.PosX - Hurtbox.SizeX / 2
								&& Hitbox.PosX - Hitbox.SizeX / 2 <= Hurtbox.PosX + Hurtbox.SizeX / 2)
							{
								OtherChar->HandleFlip();
								OtherChar->IsStunned = true;
								OtherChar->HaltMomentum();
								HitActive = false;
								HasHit = true;
								int32_t CollisionDepthX;
								if (Hitbox.PosX < Hurtbox.PosX)
									CollisionDepthX = Hurtbox.PosX - Hurtbox.SizeX / 2 - (Hitbox.PosX + Hitbox.SizeX / 2);
								else
									CollisionDepthX = Hitbox.PosX - Hitbox.SizeX / 2 - (Hurtbox.PosX + Hurtbox.SizeX / 2);
								int32_t CollisionDepthY;
								if (Hitbox.PosY < Hurtbox.PosY)
									CollisionDepthY = Hurtbox.PosY - Hurtbox.SizeY / 2 - (Hitbox.PosY + Hitbox.SizeY / 2);
								else
									CollisionDepthY = Hitbox.PosY - Hitbox.SizeY / 2 - (Hurtbox.PosY + Hurtbox.SizeY / 2);
								HitPosX = Hitbox.PosX - CollisionDepthX / 2;
								HitPosY = Hitbox.PosY - CollisionDepthY / 2;
								
								if (IsPlayer)
									Player->StateMachine.CurrentState->OnHitOrBlock();
								else
									ObjectState->OnHitOrBlock();
								
								if ((OtherChar->CurrentEnableFlags & ENB_Block || OtherChar->Blockstun > 0) && OtherChar->IsCorrectBlock(NormalHitEffect.BlockType)) //check blocking
								{
									CreateCommonParticle("cmn_guard", POS_Hit, Vector(0, 0), -NormalHitEffect.HitAngle);
									if (NormalHitEffect.AttackLevel < 1)
									{
										switch (NormalHitEffect.SFXType)
										{
										case HitSFXType::SFX_Kick:
											PlayCommonSound("GuardMeleeAltS");
											break;
										case HitSFXType::SFX_Slash:
											PlayCommonSound("GuardSlashS");
											break;
										case HitSFXType::SFX_Punch:
										default:
											PlayCommonSound("GuardMeleeS");
											break;
										}
									}
									else if (NormalHitEffect.AttackLevel < 3)
									{
										switch (NormalHitEffect.SFXType)
										{
										case HitSFXType::SFX_Kick:
											PlayCommonSound("GuardMeleeAltM");
											break;
										case HitSFXType::SFX_Slash:
											PlayCommonSound("GuardSlashM");
											break;
										case HitSFXType::SFX_Punch:
										default:
											PlayCommonSound("GuardMeleeM");
											break;
										}
									}
									else
									{
										switch (NormalHitEffect.SFXType)
										{
										case HitSFXType::SFX_Kick:
											PlayCommonSound("GuardMeleeAltL");
											break;
										case HitSFXType::SFX_Slash:
											PlayCommonSound("GuardSlashL");
											break;
										case HitSFXType::SFX_Punch:
										default:
											PlayCommonSound("GuardMeleeL");
											break;
										}
									}
									if (IsPlayer)
										Player->StateMachine.CurrentState->OnBlock();
									else
										ObjectState->OnBlock();
									OtherChar->Hitstop = NormalHitEffect.Hitstop;
									OtherChar->Blockstun = NormalHitEffect.Blockstun;
									Hitstop = NormalHitEffect.Hitstop;
									const int32_t ChipDamage = NormalHitEffect.HitDamage * NormalHitEffect.ChipDamagePercent / 100;
									OtherChar->CurrentHealth -= ChipDamage;
									if (OtherChar->CurrentHealth <= 0)
									{
										OtherChar->Blockstun = -1;
										OtherChar->Hitstun = 999;
										OtherChar->Untech = 999;
										if (OtherChar->PosY == 0 && OtherChar->KnockdownTime < 0)
										{
											switch (NormalHitEffect.GroundHitAction)
											{
											case HACT_GroundNormal:
											case HACT_ForceCrouch:
											case HACT_ForceStand:
												OtherChar->Hitstun = NormalHitEffect.Hitstun;
												OtherChar->Untech = -1;
												OtherChar->SetInertia(-NormalHitEffect.HitPushbackX);
												if (OtherChar->TouchingWall)
												{
													if (IsPlayer && Player != nullptr)
													{
														if (PosY > 0)
														{
															ClearInertia();
															AddSpeedX(-NormalHitEffect.HitPushbackX / 2);
														}
														else
														{
															SetInertia(-NormalHitEffect.HitPushbackX);
														}
													}
												}
												break;
											case HACT_AirNormal:
											case HACT_AirFaceUp:
											case HACT_AirVertical:
											case HACT_AirFaceDown:
												OtherChar->CurrentGroundBounceEffect = NormalHitEffect.GroundBounceEffect;
												OtherChar->CurrentWallBounceEffect = NormalHitEffect.WallBounceEffect;
												OtherChar->Untech = NormalHitEffect.Untech;
												OtherChar->Hitstun = -1;
												OtherChar->KnockdownTime = NormalHitEffect.KnockdownTime;
												OtherChar->ClearInertia();
												OtherChar->SetSpeedX(-NormalHitEffect.AirHitPushbackX);
												if (OtherChar->TouchingWall)
												{
													if (IsPlayer && Player != nullptr)
													{
														if (PosY > 0)
														{
															ClearInertia();
															AddSpeedX(-NormalHitEffect.HitPushbackX / 2);
														}
														else
														{
															SetInertia(-NormalHitEffect.HitPushbackX);
														}
													}
												}
												OtherChar->SetSpeedY(15000);
												break;
											case HACT_Blowback:
												OtherChar->CurrentGroundBounceEffect = NormalHitEffect.GroundBounceEffect;
												OtherChar->CurrentWallBounceEffect = NormalHitEffect.WallBounceEffect;
												OtherChar->Untech = NormalHitEffect.Untech;
												OtherChar->Hitstun = -1;
												OtherChar->KnockdownTime = NormalHitEffect.KnockdownTime;
												OtherChar->ClearInertia();
												OtherChar->SetSpeedX(-NormalHitEffect.AirHitPushbackX);
												if (OtherChar->TouchingWall)
												{
													if (IsPlayer && Player != nullptr)
													{
														if (PosY > 0)
														{
															ClearInertia();
															AddSpeedX(-NormalHitEffect.HitPushbackX / 2);
														}
														else
														{
															SetInertia(-NormalHitEffect.HitPushbackX);
														}
													}
												}
												OtherChar->SetSpeedY(30000);
											default:
												break;
											}
											OtherChar->ReceivedHitAction = NormalHitEffect.GroundHitAction;
											OtherChar->ReceivedAttackLevel = NormalHitEffect.AttackLevel;
										}
										else
										{
											switch (NormalHitEffect.AirHitAction)
											{
											case HACT_GroundNormal:
											case HACT_ForceCrouch:
											case HACT_ForceStand:
												OtherChar->Hitstun = NormalHitEffect.Hitstun;
												OtherChar->Untech = -1;
												OtherChar->SetInertia(-NormalHitEffect.HitPushbackX);
												if (OtherChar->TouchingWall)
												{
													if (IsPlayer && Player != nullptr)
													{
														if (PosY > 0)
														{
															ClearInertia();
															AddSpeedX(-NormalHitEffect.HitPushbackX / 2);
														}
														else
														{
															SetInertia(-NormalHitEffect.HitPushbackX);
														}
													}
												}
												break;
											case HACT_AirNormal:
											case HACT_AirFaceUp:
											case HACT_AirVertical:
											case HACT_AirFaceDown:
												OtherChar->CurrentGroundBounceEffect = NormalHitEffect.GroundBounceEffect;
												OtherChar->CurrentWallBounceEffect = NormalHitEffect.WallBounceEffect;
												OtherChar->Untech = NormalHitEffect.Untech;
												OtherChar->Hitstun = -1;
												OtherChar->KnockdownTime = NormalHitEffect.KnockdownTime;
												OtherChar->ClearInertia();
												OtherChar->SetSpeedX(-NormalHitEffect.AirHitPushbackX);
												if (OtherChar->TouchingWall)
												{
													if (IsPlayer && Player != nullptr)
													{
														if (PosY > 0)
														{
															ClearInertia();
															AddSpeedX(-NormalHitEffect.HitPushbackX / 2);
														}
														else
														{
															SetInertia(-NormalHitEffect.HitPushbackX);
														}
													}
												}
												OtherChar->SetSpeedY(15000);
												break;
											case HACT_Blowback:
												OtherChar->CurrentGroundBounceEffect = NormalHitEffect.GroundBounceEffect;
												OtherChar->CurrentWallBounceEffect = NormalHitEffect.WallBounceEffect;
												OtherChar->Untech = NormalHitEffect.Untech;
												OtherChar->Hitstun = -1;
												OtherChar->KnockdownTime = NormalHitEffect.KnockdownTime;
												OtherChar->ClearInertia();
												OtherChar->SetSpeedX(-NormalHitEffect.AirHitPushbackX);
												if (OtherChar->TouchingWall)
												{
													if (IsPlayer && Player != nullptr)
													{
														if (PosY > 0)
														{
															ClearInertia();
															AddSpeedX(-NormalHitEffect.HitPushbackX / 2);
														}
														else
														{
															SetInertia(-NormalHitEffect.HitPushbackX);
														}
													}
												}
												OtherChar->SetSpeedY(30000);
											default:
												break;
											}
											OtherChar->ReceivedHitAction = NormalHitEffect.AirHitAction;
											OtherChar->ReceivedAttackLevel = NormalHitEffect.AttackLevel;
											OtherChar->AirDashTimer = 0;
										}
									}
									else
									{
										if (OtherChar->PosY <= 0)
										{
											OtherChar->SetInertia(-NormalHitEffect.HitPushbackX);
											if (OtherChar->TouchingWall)
											{
												if (IsPlayer && Player != nullptr)
												{
													SetInertia(-NormalHitEffect.HitPushbackX);
												}
											}
										}
										else
										{
											OtherChar->SetInertia(-12000);
											if (OtherChar->TouchingWall)
											{
												if (IsPlayer && Player != nullptr)
												{
													SetInertia(-NormalHitEffect.HitPushbackX);
												}
											}
											OtherChar->SetSpeedY(20000);
											OtherChar->AirDashTimer = 0;
										}
										OtherChar->HandleBlockAction();
									}
									OtherChar->AddMeter(NormalHitEffect.HitDamage * OtherChar->MeterPercentOnReceiveHitGuard / 100);
									Player->AddMeter(NormalHitEffect.HitDamage * Player->MeterPercentOnHitGuard / 100);
								}
								else if (!OtherChar->IsAttacking)
								{
									OtherChar->DeathCamOverride = NormalHitEffect.DeathCamOverride;
									if (IsPlayer)
										Player->StateMachine.CurrentState->OnHit();
									else
										ObjectState->OnHit();
									HandleHitEffect(OtherChar, NormalHitEffect);
								}
								else
								{
									OtherChar->DeathCamOverride = CounterHitEffect.DeathCamOverride;
									if (IsPlayer)
										Player->StateMachine.CurrentState->OnCounterHit();
									else
										ObjectState->OnCounterHit();
									HandleHitEffect(OtherChar, CounterHitEffect);
								}
								if (OtherChar->PosX < PosX)
								{
									OtherChar->SetFacing(true);
								}
								else if (OtherChar->PosX > PosX)
								{
									OtherChar->SetFacing(false);
								}
								OtherChar->Move();
								OtherChar->DisableAll();
								if (OtherChar->CurrentHealth <= 0 && !OtherChar->DeathCamOverride && !OtherChar->IsDead)
								{
									if (Player->CurrentHealth == 0)
									{
										return;
									}
									Player->BattleHudVisibility(false);
									if (Player->Enemy->ReceivedAttackLevel < 2)
									{
										Player->StartSuperFreeze(40);
										Player->PlayCommonCameraAnim("KO_Shake");
									}
									else if (Player->Enemy->ReceivedAttackLevel < 4)
									{
										Player->StartSuperFreeze(70);
										Player->PlayCommonCameraAnim("KO_Zoom");
									}
									else
									{
										Player->StartSuperFreeze(110);
										Player->PlayCommonCameraAnim("KO_Turnaround");
									}
									Player->Hitstop = 0;
									Player->Enemy->Hitstop = 0;
								}
								return;
							}
						}
					}
				}
			}
		}
	}
}

void BattleActor::HandleHitEffect(PlayerCharacter* OtherChar, HitEffect InHitEffect)
{
	int32_t Proration = InHitEffect.ForcedProration;
	if (Player->ComboCounter == 0)
		Proration *= InHitEffect.InitialProration;
	else
		Proration *= 100;
	if (Player->ComboCounter == 0)
		OtherChar->TotalProration = 10000;
	Proration = Proration * OtherChar->TotalProration / 10000;
	OtherChar->TotalProration = OtherChar->TotalProration * InHitEffect.ForcedProration / 100;
	int32_t FinalDamage;
	if (Player->ComboCounter == 0)
		FinalDamage = InHitEffect.HitDamage;
	else
		FinalDamage = InHitEffect.HitDamage * Proration * Player->ComboRate / 1000000;

	if (FinalDamage < InHitEffect.MinimumDamagePercent * InHitEffect.HitDamage / 100)
	FinalDamage = InHitEffect.HitDamage * InHitEffect.MinimumDamagePercent / 100;

	const int32_t FinalHitPushbackX = InHitEffect.HitPushbackX + Player->ComboCounter * InHitEffect.HitPushbackX / 60;
	const int32_t FinalAirHitPushbackX = InHitEffect.AirHitPushbackX + Player->ComboCounter * InHitEffect.AirHitPushbackX / 60;
	const int32_t FinalAirHitPushbackY = InHitEffect.AirHitPushbackY - Player->ComboCounter * InHitEffect.AirHitPushbackY / 120;
	const int32_t FinalGravity = InHitEffect.HitGravity - Player->ComboCounter * InHitEffect.HitGravity / 60;

	OtherChar->CurrentHealth -= FinalDamage;
	OtherChar->AddMeter(FinalDamage * OtherChar->MeterPercentOnReceiveHit / 100);
	Player->AddMeter(FinalDamage * OtherChar->MeterPercentOnHit / 100);
	Player->ComboCounter++;
	if (OtherChar->CurrentHealth < 0)
		OtherChar->CurrentHealth = 0;
	
	OtherChar->Hitstop = InHitEffect.Hitstop;
	Hitstop = NormalHitEffect.Hitstop;
	OtherChar->Blockstun = -1;
	OtherChar->Gravity = FinalGravity;

	int32_t FinalUntech = InHitEffect.Untech;
	if (Player->ComboTimer >= 14 * 60)
	{
		FinalUntech = FinalUntech * 60 / 100;
	}
	else if (Player->ComboTimer >= 10 * 60)
	{
		FinalUntech = FinalUntech * 70 / 100;
	}
	else if (Player->ComboTimer >= 7 * 60)
	{
		FinalUntech = FinalUntech * 80 / 100;
	}
	else if (Player->ComboTimer >= 5 * 60)
	{
		FinalUntech = FinalUntech * 90 / 100;
	}
	else if (Player->ComboTimer >= 3 * 60)
	{
		FinalUntech = FinalUntech * 95 / 100;
	}

	if (OtherChar->PosY == 0 && !OtherChar->IsKnockedDown)
	{
		switch (InHitEffect.GroundHitAction)
		{
		case HACT_GroundNormal:
		case HACT_ForceCrouch:
		case HACT_ForceStand:
			OtherChar->Hitstun = InHitEffect.Hitstun;
			OtherChar->Untech = -1;
			OtherChar->SetInertia(-FinalHitPushbackX);
			if (OtherChar->TouchingWall)
			{
				if (IsPlayer && Player != nullptr)
				{
					if (PosY > 0)
					{
						ClearInertia();
						AddSpeedX(-FinalHitPushbackX / 2);
					}
					else
					{
						SetInertia(-FinalHitPushbackX);
					}
				}
			}
			break;
		case HACT_Crumple:
			OtherChar->Untech = FinalUntech;
			OtherChar->Hitstun = -1;
			OtherChar->KnockdownTime = -1;
			OtherChar->SetInertia(-FinalHitPushbackX);
			if (OtherChar->TouchingWall)
			{
				if (IsPlayer && Player != nullptr)
				{
					if (PosY > 0)
					{
						ClearInertia();
						AddSpeedX(-FinalHitPushbackX / 2);
					}
					else
					{
						SetInertia(-FinalHitPushbackX);
					}
				}
			}
			break;
		case HACT_AirNormal:
		case HACT_AirFaceUp:
		case HACT_AirVertical:
		case HACT_AirFaceDown:
			OtherChar->CurrentGroundBounceEffect = InHitEffect.GroundBounceEffect;
			OtherChar->CurrentWallBounceEffect = InHitEffect.WallBounceEffect;
			OtherChar->Untech = FinalUntech;
			OtherChar->Hitstun = -1;
			OtherChar->KnockdownTime = InHitEffect.KnockdownTime;
			OtherChar->ClearInertia();
			OtherChar->SetSpeedX(-FinalAirHitPushbackX);
			if (OtherChar->TouchingWall)
			{
				if (IsPlayer && Player != nullptr)
				{
					if (PosY > 0)
					{
						ClearInertia();
						AddSpeedX(-FinalHitPushbackX / 2);
					}
					else
					{
						SetInertia(-FinalHitPushbackX);
					}
				}
			}
			OtherChar->SetSpeedY(FinalAirHitPushbackY);
			if (FinalAirHitPushbackY <= 0 && OtherChar->PosY <= 0)
				OtherChar->PosY = 1;
			break;
		case HACT_Blowback:
			OtherChar->CurrentGroundBounceEffect = InHitEffect.GroundBounceEffect;
			OtherChar->CurrentWallBounceEffect = InHitEffect.WallBounceEffect;
			OtherChar->Untech = FinalUntech;
			OtherChar->Hitstun = -1;
			OtherChar->KnockdownTime = InHitEffect.KnockdownTime;
			OtherChar->ClearInertia();
			OtherChar->SetSpeedX(-FinalAirHitPushbackX * 2);
			if (OtherChar->TouchingWall)
			{
				if (IsPlayer && Player != nullptr)
				{
					if (PosY > 0)
					{
						ClearInertia();
						AddSpeedX(-FinalHitPushbackX / 2);
					}
					else
					{
						SetInertia(-FinalHitPushbackX);
					}
				}
			}
			OtherChar->SetSpeedY(FinalAirHitPushbackY * 2);
			if (FinalAirHitPushbackY != 0 && OtherChar->PosY <= 0)
				OtherChar->PosY = 1000;
		default:
			break;
		}
		OtherChar->ReceivedHitAction = InHitEffect.GroundHitAction;
		OtherChar->ReceivedAttackLevel = InHitEffect.AttackLevel;
	}
	else
	{
		switch (InHitEffect.AirHitAction)
		{
		case HACT_GroundNormal:
		case HACT_ForceCrouch:
		case HACT_ForceStand:
			OtherChar->Hitstun = InHitEffect.Hitstun;
			OtherChar->Untech = -1;
			OtherChar->SetInertia(-FinalHitPushbackX);
			if (OtherChar->TouchingWall)
			{
				if (IsPlayer && Player != nullptr)
				{
					if (PosY > 0)
					{
						ClearInertia();
						AddSpeedX(-FinalHitPushbackX / 2);
					}
					else
					{
						SetInertia(-FinalHitPushbackX);
					}
				}
			}
			break;
		case HACT_Crumple:
			OtherChar->Untech = FinalUntech;
			OtherChar->Hitstun = -1;
			OtherChar->KnockdownTime = -1;
			OtherChar->SetInertia(-FinalHitPushbackX);
			if (OtherChar->TouchingWall)
			{
				if (IsPlayer && Player != nullptr)
				{
					if (PosY > 0)
					{
						ClearInertia();
						AddSpeedX(-FinalHitPushbackX / 2);
					}
					else
					{
						SetInertia(-FinalHitPushbackX);
					}
				}
			}
			break;
		case HACT_AirNormal:
		case HACT_AirFaceUp:
		case HACT_AirVertical:
		case HACT_AirFaceDown:
			OtherChar->CurrentGroundBounceEffect = InHitEffect.GroundBounceEffect;
			OtherChar->CurrentWallBounceEffect = InHitEffect.WallBounceEffect;
			OtherChar->Untech = FinalUntech;
			OtherChar->Hitstun = -1;
			OtherChar->KnockdownTime = InHitEffect.KnockdownTime;
			OtherChar->ClearInertia();
			OtherChar->SetSpeedX(-FinalAirHitPushbackX);
			if (OtherChar->TouchingWall)
			{
				if (IsPlayer && Player != nullptr)
				{
					if (PosY > 0)
					{
						ClearInertia();
						AddSpeedX(-FinalHitPushbackX / 2);
					}
					else
					{
						SetInertia(-FinalHitPushbackX);
					}
				}
			}
			OtherChar->SetSpeedY(FinalAirHitPushbackY);
			if (NormalHitEffect.AirHitPushbackY <= 0 && OtherChar->PosY <= 0)
				OtherChar->PosY = 1;
			break;
		case HACT_Blowback:
			OtherChar->CurrentGroundBounceEffect = InHitEffect.GroundBounceEffect;
			OtherChar->CurrentWallBounceEffect = InHitEffect.WallBounceEffect;
			OtherChar->Untech = FinalUntech;
			OtherChar->Hitstun = -1;
			OtherChar->KnockdownTime = InHitEffect.KnockdownTime;
			OtherChar->ClearInertia();
			OtherChar->SetSpeedX(-FinalAirHitPushbackX * 2);
			if (OtherChar->TouchingWall)
			{
				if (IsPlayer && Player != nullptr)
				{
					if (PosY > 0)
					{
						ClearInertia();
						AddSpeedX(-FinalHitPushbackX / 2);
					}
					else
					{
						SetInertia(-FinalHitPushbackX);
					}
				}
			}
			OtherChar->SetSpeedY(FinalAirHitPushbackY * 2);
			if (FinalAirHitPushbackY <= 0 && OtherChar->PosY <= 0)
				OtherChar->PosY = 1;
		default:
			break;
		}
		OtherChar->ReceivedHitAction = InHitEffect.AirHitAction;
		OtherChar->ReceivedAttackLevel = InHitEffect.AttackLevel;
		OtherChar->AirDashTimer = 0;
	}
									
	if (OtherChar->PosY <= 0 && OtherChar->HasBeenOTG > GameState->MaxOtgCount)
	{
		OtherChar->SetStrikeInvulnerable(true);
		OtherChar->SetProjectileInvulnerable(true);
		OtherChar->Untech = 6;
		OtherChar->SetSpeedY(15000);
		OtherChar->SetInertia(-35000);
	}
	if (OtherChar->PosY <= 0 && OtherChar->KnockdownTime > 0)
	{
		OtherChar->IsKnockedDown = false;
		OtherChar->HasBeenOTG++;
	}
									
	if (strcmp(HitEffectName.GetString(), ""))
	{
		CreateCharaParticle(HitEffectName.GetString(), POS_Hit, Vector(0, 0), -InHitEffect.HitAngle);
		if (InHitEffect.AttackLevel < 1)
		{
			switch (InHitEffect.SFXType)
			{
			case HitSFXType::SFX_Kick:
				PlayCommonSound("HitMeleeAltS");
				break;
			case HitSFXType::SFX_Slash:
				PlayCommonSound("HitSlashS");
				break;
			case HitSFXType::SFX_Punch:
			default:
				PlayCommonSound("HitMeleeS");
				break;
			}
		}
		else if (InHitEffect.AttackLevel < 3)
		{
			switch (InHitEffect.SFXType)
			{
			case HitSFXType::SFX_Kick:
				PlayCommonSound("HitMeleeAltM");
				break;
			case HitSFXType::SFX_Slash:
				PlayCommonSound("HitSlashM");
				break;
			case HitSFXType::SFX_Punch:
			default:
				PlayCommonSound("HitMeleeM");
				break;
			}
		}
		else if (InHitEffect.AttackLevel < 4)
		{
			switch (InHitEffect.SFXType)
			{
			case HitSFXType::SFX_Kick:
				PlayCommonSound("HitMeleeAltL");
				break;
			case HitSFXType::SFX_Slash:
				PlayCommonSound("HitSlashL");
				break;
			case HitSFXType::SFX_Punch:
			default:
				PlayCommonSound("HitMeleeL");
				break;
			}
		}
		else 
		{
			switch (InHitEffect.SFXType)
			{
			case HitSFXType::SFX_Kick:
				PlayCommonSound("HitMeleeAltXL");
				break;
			case HitSFXType::SFX_Slash:
				PlayCommonSound("HitSlashL");
				break;
			case HitSFXType::SFX_Punch:
			default:
				PlayCommonSound("HitMeleeXL");
				break;
			}
		}								    
	}
	else if (ObjectState != nullptr)
	{
		if (ObjectState->Type == StateType::SpecialAttack || ObjectState->Type == StateType::SuperAttack)
		{
			CreateCommonParticle("cmn_hit_sp", POS_Hit, Vector(0, 0), -InHitEffect.HitAngle);
			if (InHitEffect.AttackLevel < 1)
			{
				switch (InHitEffect.SFXType)
				{
				case HitSFXType::SFX_Kick:
					PlayCommonSound("HitMeleeAltS");
					break;
				case HitSFXType::SFX_Slash:
					PlayCommonSound("HitSlashS");
					break;
				case HitSFXType::SFX_Punch:
				default:
					PlayCommonSound("HitMeleeS");
					break;
				}
			}
			else if (InHitEffect.AttackLevel < 3)
			{
				switch (InHitEffect.SFXType)
				{
				case HitSFXType::SFX_Kick:
					PlayCommonSound("HitMeleeAltM");
					break;
				case HitSFXType::SFX_Slash:
					PlayCommonSound("HitSlashM");
					break;
				case HitSFXType::SFX_Punch:
				default:
					PlayCommonSound("HitMeleeM");
					break;
				}
			}
			else if (InHitEffect.AttackLevel < 4)
			{
				switch (InHitEffect.SFXType)
				{
				case HitSFXType::SFX_Kick:
					PlayCommonSound("HitMeleeAltL");
					break;
				case HitSFXType::SFX_Slash:
					PlayCommonSound("HitSlashL");
					break;
				case HitSFXType::SFX_Punch:
				default:
					PlayCommonSound("HitMeleeL");
					break;
				}
			}
			else 
			{
				switch (InHitEffect.SFXType)
				{
				case HitSFXType::SFX_Kick:
					PlayCommonSound("HitMeleeAltXL");
					break;
				case HitSFXType::SFX_Slash:
					PlayCommonSound("HitSlashL");
					break;
				case HitSFXType::SFX_Punch:
				default:
					PlayCommonSound("HitMeleeXL");
					break;
				}
			}
		}
		else
		{
			if (InHitEffect.AttackLevel < 1)
			{
				CreateCommonParticle("cmn_hit_s", POS_Hit, Vector(0, 0), -InHitEffect.HitAngle);
				switch (InHitEffect.SFXType)
				{
				case HitSFXType::SFX_Kick:
					PlayCommonSound("HitMeleeAltS");
					break;
				case HitSFXType::SFX_Slash:
					PlayCommonSound("HitSlashS");
					break;
				case HitSFXType::SFX_Punch:
				default:
					PlayCommonSound("HitMeleeS");
					break;
				}
			}
			else if (InHitEffect.AttackLevel < 3)
			{
				CreateCommonParticle("cmn_hit_m", POS_Hit, Vector(0, 0), -InHitEffect.HitAngle);
				switch (InHitEffect.SFXType)
				{
				case HitSFXType::SFX_Kick:
					PlayCommonSound("HitMeleeAltM");
					break;
				case HitSFXType::SFX_Slash:
					PlayCommonSound("HitSlashM");
					break;
				case HitSFXType::SFX_Punch:
				default:
					PlayCommonSound("HitMeleeM");
					break;
				}
			}
			else if (InHitEffect.AttackLevel < 4)
			{
				CreateCommonParticle("cmn_hit_l", POS_Hit, Vector(0, 0), -InHitEffect.HitAngle);
				switch (InHitEffect.SFXType)
				{
				case HitSFXType::SFX_Kick:
					PlayCommonSound("HitMeleeAltL");
					break;
				case HitSFXType::SFX_Slash:
					PlayCommonSound("HitSlashL");
					break;
				case HitSFXType::SFX_Punch:
				default:
					PlayCommonSound("HitMeleeL");
					break;
				}
			}
			else 
			{
				CreateCommonParticle("cmn_hit_l", POS_Hit, Vector(0, 0), -InHitEffect.HitAngle);
				switch (InHitEffect.SFXType)
				{
				case HitSFXType::SFX_Kick:
					PlayCommonSound("HitMeleeAltXL");
					break;
				case HitSFXType::SFX_Slash:
					PlayCommonSound("HitSlashL");
					break;
				case HitSFXType::SFX_Punch:
				default:
					PlayCommonSound("HitMeleeXL");
					break;
				}
			}								    
		}
	}
	else if (IsPlayer)
	{
		if (Player->StateMachine.CurrentState->Type == StateType::SpecialAttack || Player->StateMachine.CurrentState->Type == StateType::SuperAttack)
		{
			CreateCommonParticle("cmn_hit_sp", POS_Hit, Vector(0, 0), -InHitEffect.HitAngle);
			if (InHitEffect.AttackLevel < 1)
			{
				switch (InHitEffect.SFXType)
				{
				case HitSFXType::SFX_Kick:
					PlayCommonSound("HitMeleeAltS");
					break;
				case HitSFXType::SFX_Slash:
					PlayCommonSound("HitSlashS");
					break;
				case HitSFXType::SFX_Punch:
				default:
					PlayCommonSound("HitMeleeS");
					break;
				}
			}
			else if (InHitEffect.AttackLevel < 3)
			{
				switch (InHitEffect.SFXType)
				{
				case HitSFXType::SFX_Kick:
					PlayCommonSound("HitMeleeAltM");
					break;
				case HitSFXType::SFX_Slash:
					PlayCommonSound("HitSlashM");
					break;
				case HitSFXType::SFX_Punch:
				default:
					PlayCommonSound("HitMeleeM");
					break;
				}
			}
			else if (InHitEffect.AttackLevel < 4)
			{
				CreateCommonParticle("cmn_hit_l", POS_Hit, Vector(0, 0), -InHitEffect.HitAngle);
				switch (InHitEffect.SFXType)
				{
				case HitSFXType::SFX_Kick:
					PlayCommonSound("HitMeleeAltL");
					break;
				case HitSFXType::SFX_Slash:
					PlayCommonSound("HitSlashL");
					break;
				case HitSFXType::SFX_Punch:
				default:
					PlayCommonSound("HitMeleeL");
					break;
				}
			}
			else 
			{
				switch (NormalHitEffect.SFXType)
				{
				case HitSFXType::SFX_Kick:
					PlayCommonSound("HitMeleeAltXL");
					break;
				case HitSFXType::SFX_Slash:
					PlayCommonSound("HitSlashL");
					break;
				case HitSFXType::SFX_Punch:
				default:
					PlayCommonSound("HitMeleeXL");
					break;
				}
			}								    
		}
		else
		{
			if (InHitEffect.AttackLevel < 1)
			{
				CreateCommonParticle("cmn_hit_s", POS_Hit, Vector(0, 0), -InHitEffect.HitAngle);
				switch (InHitEffect.SFXType)
				{
				case HitSFXType::SFX_Kick:
					PlayCommonSound("HitMeleeAltS");
					break;
				case HitSFXType::SFX_Slash:
					PlayCommonSound("HitSlashS");
					break;
				case HitSFXType::SFX_Punch:
				default:
					PlayCommonSound("HitMeleeS");
					break;
				}
			}
			else if (InHitEffect.AttackLevel < 3)
			{
				CreateCommonParticle("cmn_hit_m", POS_Hit, Vector(0, 0), -InHitEffect.HitAngle);
				switch (InHitEffect.SFXType)
				{
				case HitSFXType::SFX_Kick:
					PlayCommonSound("HitMeleeAltM");
					break;
				case HitSFXType::SFX_Slash:
					PlayCommonSound("HitSlashM");
					break;
				case HitSFXType::SFX_Punch:
				default:
					PlayCommonSound("HitMeleeM");
					break;
				}
			}
			else if (InHitEffect.AttackLevel < 4)
			{
				CreateCommonParticle("cmn_hit_l", POS_Hit, Vector(0, 0), -InHitEffect.HitAngle);
				switch (InHitEffect.SFXType)
				{
				case HitSFXType::SFX_Kick:
					PlayCommonSound("HitMeleeAltL");
					break;
				case HitSFXType::SFX_Slash:
					PlayCommonSound("HitSlashL");
					break;
				case HitSFXType::SFX_Punch:
				default:
					PlayCommonSound("HitMeleeL");
					break;
				}
			}
			else 
			{
				CreateCommonParticle("cmn_hit_l", POS_Hit, Vector(0, 0), -InHitEffect.HitAngle);
				switch (InHitEffect.SFXType)
				{
				case HitSFXType::SFX_Kick:
					PlayCommonSound("HitMeleeAltXL");
					break;
				case HitSFXType::SFX_Slash:
					PlayCommonSound("HitSlashL");
					break;
				case HitSFXType::SFX_Punch:
				default:
					PlayCommonSound("HitMeleeXL");
					break;
				}
			}								    
		}
	}
}

void BattleActor::HandleClashCollision(BattleActor* OtherObj)
{
	if (IsAttacking && HitActive && OtherObj != Player && OtherObj->IsAttacking && OtherObj->HitActive)
	{
		for (int32_t i = 0; i < CollisionArraySize; i++)
		{
			if (CollisionBoxes[i].Type == Hitbox)
			{
				for (int32_t j = 0; j < CollisionArraySize; j++)
				{
					if (OtherObj->CollisionBoxes[j].Type == Hitbox)
					{
						CollisionBox Hitbox = CollisionBoxes[i];

						CollisionBox OtherHitbox = OtherObj->CollisionBoxes[j];

						if (FacingRight)
						{
							Hitbox.PosX += PosX;
						}
						else
						{
							Hitbox.PosX = -Hitbox.PosX + PosX;  
						}
						Hitbox.PosY += PosY;
						if (OtherObj->FacingRight)
						{
							OtherHitbox.PosX += OtherObj->PosX;
						}
						else
						{
							OtherHitbox.PosX = -OtherHitbox.PosX + OtherObj->PosX;  
						}
						OtherHitbox.PosY += OtherObj->PosY;
							
						if (Hitbox.PosY + Hitbox.SizeY / 2 >= OtherHitbox.PosY - OtherHitbox.SizeY / 2
							&& Hitbox.PosY - Hitbox.SizeY / 2 <= OtherHitbox.PosY + OtherHitbox.SizeY / 2
							&& Hitbox.PosX + Hitbox.SizeX / 2 >= OtherHitbox.PosX - OtherHitbox.SizeX / 2
							&& Hitbox.PosX - Hitbox.SizeX / 2 <= OtherHitbox.PosX + OtherHitbox.SizeX / 2)
						{
							int32_t CollisionDepthX;
							if (Hitbox.PosX < OtherHitbox.PosX)
								CollisionDepthX = OtherHitbox.PosX - OtherHitbox.SizeX / 2 - (Hitbox.PosX + Hitbox.SizeX / 2);
							else
								CollisionDepthX = Hitbox.PosX - Hitbox.SizeX / 2 - (OtherHitbox.PosX + OtherHitbox.SizeX / 2);
							int32_t CollisionDepthY;
							if (Hitbox.PosY < OtherHitbox.PosY)
								CollisionDepthY = OtherHitbox.PosY - OtherHitbox.SizeY / 2 - (Hitbox.PosY + Hitbox.SizeY / 2);
							else
								CollisionDepthY = Hitbox.PosY - Hitbox.SizeY / 2 - (OtherHitbox.PosY + OtherHitbox.SizeY / 2);
							HitPosX = Hitbox.PosX - CollisionDepthX / 2;
							HitPosY = Hitbox.PosY - CollisionDepthY / 2;
							
							if (IsPlayer && OtherObj->IsPlayer)
							{
								Hitstop = 16;
								OtherObj->Hitstop = 16;
								HitActive = false;
								OtherObj->HitActive = false;
								OtherObj->HitPosX = HitPosX;
								OtherObj->HitPosY = HitPosY;
								Player->EnableAttacks();
								OtherObj->Player->EnableAttacks();
								OtherObj->Player->StateMachine.CurrentState->OnHitOrBlock();
								Player->StateMachine.CurrentState->OnHitOrBlock();
								return;
							}
							if (!IsPlayer && !OtherObj->IsPlayer)
							{
								OtherObj->Hitstop = 16;
								Hitstop = 16;
								OtherObj->HitActive = false;
								HitActive = false;
								OtherObj->HitPosX = HitPosX;
								OtherObj->HitPosY = HitPosY;
								OtherObj->ObjectState->OnHitOrBlock();
								ObjectState->OnHitOrBlock();
								return;
							}
							return;
						}
					}
				}
			}
		}
	}
}

void BattleActor::HandleFlip()
{
	bool CurrentFacing = FacingRight;
	if (!Player->Enemy) return;
	if (PosX < Player->Enemy->PosX)
	{
		SetFacing(true);
	}
	else if (PosX > Player->Enemy->PosX)
	{
		SetFacing(false);
	}
	if (CurrentFacing != FacingRight)
	{
		SpeedX = -SpeedX;
		Inertia = -Inertia;
		if (IsPlayer)
		{
			Player->InputBuffer.FlipInputsInBuffer();
			if (Player->CurrentActionFlags == ACT_Standing)
				Player->JumpToState("StandFlip");
			else if (Player->CurrentActionFlags == ACT_Crouching)
				Player->JumpToState("CrouchFlip");
			else
				Player->JumpToState("JumpFlip");
		}
	}
}

void BattleActor::EnableHit(bool Enabled)
{
	HitActive = Enabled;
}

void BattleActor::SetPushCollisionActive(bool Active)
{
	PushCollisionActive = Active;
}

void BattleActor::SetAttacking(bool Attacking)
{
	IsAttacking = Attacking;
}

void BattleActor::SetHeadAttribute(bool HeadAttribute)
{
	AttackHeadAttribute = HeadAttribute;
}

void BattleActor::SetProjectileAttribute(bool ProjectileAttribute)
{
	AttackProjectileAttribute = ProjectileAttribute;
}

void BattleActor::SetHitEffect(HitEffect InHitEffect)
{
	NormalHitEffect = InHitEffect;
}

void BattleActor::SetCounterHitEffect(HitEffect InHitEffect)
{
	CounterHitEffect = InHitEffect;
}

void BattleActor::CreateCommonParticle(char* Name, PosType PosType, Vector Offset, int32_t Angle)
{
	/*if (Player->CommonParticleData != nullptr)
	{
		for (FParticleStruct ParticleStruct : Player->CommonParticleData->ParticleDatas)
		{
			if (ParticleStruct.Name == Name)
			{
				FVector FinalLocation;
				if (!FacingRight)
					Offset = FVector(Offset.X, -Offset.Y, Offset.Z);
				switch (PosType)
				{
				case POS_Player:
					FinalLocation = Offset + GetActorLocation();
					break;
				case POS_Enemy:
					FinalLocation = Offset + Player->Enemy->GetActorLocation();
					break;
				case POS_Hit:
					FinalLocation = Offset + FVector(0, HitPosX / COORD_SCALE, HitPosY / COORD_SCALE);
					break;
				default:
					FinalLocation = Offset + GetActorLocation();
					break;
				}
				GameState->ParticleManager->NiagaraComponents.Add(UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ParticleStruct.ParticleSystem, FinalLocation, Rotation, GetActorScale()));
				GameState->ParticleManager->NiagaraComponents.Last()->SetAgeUpdateMode(ENiagaraAgeUpdateMode::DesiredAge);
				GameState->ParticleManager->NiagaraComponents.Last()->SetNiagaraVariableBool("NeedsRollback", true);
				GameState->ParticleManager->NiagaraComponents.Last()->SetNiagaraVariableFloat("SpriteRotate", Rotation.Pitch);
				if (!FacingRight)
				{
					GameState->ParticleManager->NiagaraComponents.Last()->SetNiagaraVariableVec2("UVScale", Vector(-1, 1));
					GameState->ParticleManager->NiagaraComponents.Last()->SetNiagaraVariableVec2("PivotOffset", Vector(0, 0.5));
					GameState->ParticleManager->NiagaraComponents.Last()->SetNiagaraVariableFloat("SpriteRotate", -Rotation.Pitch);
				}
				break;
			}
		}
	}*/
}

void BattleActor::CreateCharaParticle(char* Name, PosType PosType, Vector Offset, int32_t Angle)
{
	/*if (Player->ParticleData != nullptr)
	{
		for (FParticleStruct ParticleStruct : Player->ParticleData->ParticleDatas)
		{
			if (ParticleStruct.Name == Name)
			{
				FVector FinalLocation;
				if (!FacingRight)
					Offset = FVector(Offset.X, -Offset.Y, Offset.Z);
				switch (PosType)
				{
				case POS_Player:
					FinalLocation = Offset + GetActorLocation();
					break;
				case POS_Enemy:
					FinalLocation = Offset + Player->Enemy->GetActorLocation();
					break;
				case POS_Hit:
					FinalLocation = Offset + FVector(0, HitPosX / COORD_SCALE, HitPosY / COORD_SCALE);
					break;
				default:
					FinalLocation = Offset + GetActorLocation();
					break;
				}
				GameState->ParticleManager->NiagaraComponents.Add(UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ParticleStruct.ParticleSystem, FinalLocation, Rotation, GetActorScale()));
				GameState->ParticleManager->NiagaraComponents.Last()->SetAgeUpdateMode(ENiagaraAgeUpdateMode::DesiredAge);
				GameState->ParticleManager->NiagaraComponents.Last()->SetNiagaraVariableBool("NeedsRollback", true);
				GameState->ParticleManager->NiagaraComponents.Last()->SetNiagaraVariableFloat("SpriteRotate", Rotation.Pitch);
				if (!FacingRight)
				{
					GameState->ParticleManager->NiagaraComponents.Last()->SetNiagaraVariableVec2("UVScale", Vector(-1, 1));
					GameState->ParticleManager->NiagaraComponents.Last()->SetNiagaraVariableVec2("PivotOffset", Vector(0, 1));
					GameState->ParticleManager->NiagaraComponents.Last()->SetNiagaraVariableFloat("SpriteRotate", -Rotation.Pitch);
				}
				break;
			}
		}
	}*/
}

void BattleActor::LinkCharaParticle(char* Name)
{
	/*if (Player->ParticleData != nullptr)
	{
		for (FParticleStruct ParticleStruct : Player->ParticleData->ParticleDatas)
		{
			if (ParticleStruct.Name == Name)
			{
				FVector Scale;
				if (!FacingRight)
				{
					Scale = FVector(1, -1, 1);
				}
				else
				{
					Scale = FVector(1, 1, 1);
				}
				GameState->ParticleManager->NiagaraComponents.Add(UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ParticleStruct.ParticleSystem, GetActorLocation(), FRotator::ZeroRotator, Scale));
				GameState->ParticleManager->NiagaraComponents.Last()->SetAgeUpdateMode(ENiagaraAgeUpdateMode::DesiredAge);
				GameState->ParticleManager->NiagaraComponents.Last()->SetNiagaraVariableBool("NeedsRollback", true);
				LinkedParticle = GameState->ParticleManager->NiagaraComponents.Last();
				if (!FacingRight)
					GameState->ParticleManager->NiagaraComponents.Last()->SetNiagaraVariableVec2("UVScale", Vector(-1, 1));
				break;
			}
		}
	}*/
}

void BattleActor::PlayCommonSound(char* Name)
{
}

void BattleActor::PlayCharaSound(char* Name)
{
}

void BattleActor::PauseRoundTimer(bool Pause)
{
	GameState->StoredBattleState.PauseTimer = Pause;
}

void BattleActor::SetObjectID(int32_t InObjectID)
{
	ObjectID = InObjectID;
}

void BattleActor::DeactivateIfBeyondBounds()
{
	if (IsPlayer)
		return;
	if (PosX > 1200000 + GameState->StoredBattleState.CurrentScreenPos || PosX < -1200000 + GameState->StoredBattleState.CurrentScreenPos)
		DeactivateObject();
}

void BattleActor::DeactivateObject()
{
	if (IsPlayer)
		return;
	ObjectState->OnExit();
	for (int32_t i = 0; i < 32; i++)
	{
		if (this == Player->ChildBattleActors[i])
		{
			Player->ChildBattleActors[i] = nullptr;
			break;
		}
	}
	for (int32_t i = 0; i < 16; i++)
	{
		if (this == Player->StoredBattleActors[i])
		{
			Player->StoredBattleActors[i] = nullptr;
			break;
		}
	}
	DeactivateOnNextUpdate = true;
}

void BattleActor::ResetObject()
{
	if (IsPlayer)
		return;
	DeactivateOnNextUpdate = false;
	IsActive = false;
	PosX = 0;
	PosY = 0;
	PrevPosX = 0;
	PrevPosY = 0;
	SpeedX = 0;
	SpeedY = 0;
	Gravity = 1900;
	Inertia = 0;
	ActionTime = -1;
	ActiveTime = -1;
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
	AttackProjectileAttribute = true;
	RoundStart = false;
	FacingRight = false;
	HasHit = false;
	SpeedXPercent = 100;
	SpeedXPercentPerFrame = false;
	ScreenCollisionActive = false;
	PushCollisionActive = false;
	StateVal1 = 0;
	StateVal2 = 0;
	StateVal3 = 0;
	StateVal4 = 0;
	StateVal5 = 0;
	StateVal6 = 0;
	StateVal7 = 0;
	StateVal8 = 0;
	FacingRight = false;
	MiscFlags = 0;
	IsPlayer = false;
	SuperFreezeTime = -1;
	CelNameInternal.SetString("");
	HitEffectName.SetString("");
	SocketName.SetString("");
	AnimTime = -1;
	HitPosX = 0;
	HitPosY = 0;
	DefaultCommonAction = true;
	for (int32_t i = 0; i < CollisionArraySize; i++)
	{
		CollisionBoxes[i] = CollisionBox();
	}
	ObjectStateName.SetString("");
	ObjectID = 0;
	Player = nullptr;
}

void BattleActor::SaveForRollback(unsigned char* Buffer)
{
	memcpy(Buffer, &ObjSync, SIZEOF_BATTLEACTOR);
}

void BattleActor::LoadForRollback(unsigned char* Buffer)
{
	memcpy(&ObjSync, Buffer, SIZEOF_BATTLEACTOR);
	if (!IsPlayer)
	{
		int Index = 0;
		for (CString<64> String : Player->ObjectStateNames)
		{
			if (!strcmp(String.GetString(), ObjectStateName.GetString()))
			{
				break;
			}
			Index++;
		}
		if (strcmp(Player->ObjectStateNames[Index].GetString(), ObjectStateName.GetString()))
		{
			memcpy(ObjectState, Player->ObjectStates[Index], sizeof(State));
			ObjectState->ObjectParent = this;
		}
	}
}

void BattleActor::LogForSyncTest(FILE* file)
{
	if(file)
	{
		fprintf(file,"BattleActor:\n");
		fprintf(file,"\tPosX: %d\n", PosX);
		fprintf(file,"\tPosY: %d\n", PosY);
		fprintf(file,"\tPrevPosX: %d\n", PrevPosX);
		fprintf(file,"\tPrevPosY: %d\n", PrevPosY);
		fprintf(file,"\tSpeedX: %d\n", SpeedX);
		fprintf(file,"\tSpeedY: %d\n", SpeedY);
		fprintf(file,"\tGravity: %d\n", Gravity);
		fprintf(file,"\tInertia: %d\n", Inertia);
		fprintf(file,"\tActionTime: %d\n", ActionTime);
		fprintf(file,"\tPushHeight: %d\n", PushHeight);
		fprintf(file,"\tPushHeightLow: %d\n", PushHeightLow);
		fprintf(file,"\tPushWidth: %d\n", PushWidth);
		fprintf(file,"\tHitstop: %d\n", Hitstop);
		fprintf(file,"\tCelName: %s\n", CelNameInternal.GetString());
		fprintf(file,"\tHitActive: %d\n", HitActive);
		fprintf(file,"\tIsAttacking: %d\n", IsAttacking);
		fprintf(file,"\tFacingRight: %d\n", FacingRight);
		fprintf(file,"\tHasHit: %d\n", HasHit);
		fprintf(file,"\tMiscFlags: %d\n", MiscFlags);
		fprintf(file,"\tAnimTime: %d\n", AnimTime);
		fprintf(file,"\tDefaultCommonAction: %d\n", DefaultCommonAction);
	}
}

BattleActor* BattleActor::GetBattleActor(ObjType Type)
{
	switch (Type)
	{
	case OBJ_Self:
		return this;
	case OBJ_Enemy:
		return Player->Enemy;
	case OBJ_Parent:
		return Player;
	case OBJ_Child0:
		if (IsPlayer && Player->StoredBattleActors[0])
			if (Player->StoredBattleActors[0]->IsActive)
				return Player->StoredBattleActors[0];
		return nullptr;
	case OBJ_Child1:
		if (IsPlayer && Player->StoredBattleActors[1])
			if (Player->StoredBattleActors[1]->IsActive)
				return Player->StoredBattleActors[1];
		return nullptr;
	case OBJ_Child2:
		if (IsPlayer && Player->StoredBattleActors[2])
			if (Player->StoredBattleActors[2]->IsActive)
				return Player->StoredBattleActors[2];
		return nullptr;
	case OBJ_Child3:
		if (IsPlayer && Player->StoredBattleActors[3])
			if (Player->StoredBattleActors[3]->IsActive)
				return Player->StoredBattleActors[3];
		return nullptr;
	case OBJ_Child4:
		if (IsPlayer && Player->StoredBattleActors[4])
			if (Player->StoredBattleActors[4]->IsActive)
				return Player->StoredBattleActors[4];
		return nullptr;
	case OBJ_Child5:
		if (IsPlayer && Player->StoredBattleActors[5])
			if (Player->StoredBattleActors[5]->IsActive)
				return Player->StoredBattleActors[5];
		return nullptr;
	case OBJ_Child6:
		if (IsPlayer && Player->StoredBattleActors[6])
			if (Player->StoredBattleActors[6]->IsActive)
				return Player->StoredBattleActors[6];
		return nullptr;
	case OBJ_Child7:
		if (IsPlayer && Player->StoredBattleActors[7])
			if (Player->StoredBattleActors[7]->IsActive)
				return Player->StoredBattleActors[7];
		return nullptr;
	case OBJ_Child8:
		if (IsPlayer && Player->StoredBattleActors[8])
			if (Player->StoredBattleActors[8]->IsActive)
				return Player->StoredBattleActors[8];
		return nullptr;
	case OBJ_Child9:
		if (IsPlayer && Player->StoredBattleActors[9])
			if (Player->StoredBattleActors[9]->IsActive)
				return Player->StoredBattleActors[9];
		return nullptr;
	case OBJ_Child10:
		if (IsPlayer && Player->StoredBattleActors[10])
			if (Player->StoredBattleActors[10]->IsActive)
				return Player->StoredBattleActors[10];
		return nullptr;
	case OBJ_Child11:
		if (IsPlayer && Player->StoredBattleActors[11])
			if (Player->StoredBattleActors[11]->IsActive)
				return Player->StoredBattleActors[11];
		return nullptr;
	case OBJ_Child12:
		if (IsPlayer && Player->StoredBattleActors[12])
			if (Player->StoredBattleActors[12]->IsActive)
				return Player->StoredBattleActors[12];
		return nullptr;
	case OBJ_Child13:
		if (IsPlayer && Player->StoredBattleActors[13])
			if (Player->StoredBattleActors[13]->IsActive)
				return Player->StoredBattleActors[13];
		return nullptr;
	case OBJ_Child14:
		if (IsPlayer && Player->StoredBattleActors[14])
			if (Player->StoredBattleActors[14]->IsActive)
				return Player->StoredBattleActors[14];
		return nullptr;
	case OBJ_Child15:
		if (IsPlayer && Player->StoredBattleActors[15])
			if (Player->StoredBattleActors[15]->IsActive)
				return Player->StoredBattleActors[15];
		return nullptr;
	default:
		return nullptr;
	}
}


