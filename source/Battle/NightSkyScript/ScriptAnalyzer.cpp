#include "ScriptAnalyzer.h"
#include "../Actors/PlayerCharacter.h"

void ScriptAnalyzer::Initialize(char *Addr, uint32_t Size, std::vector<State *> *States, std::vector<Subroutine *> *Subroutines)
{
    DataAddress = Addr;
    StateCount = *reinterpret_cast<int *>(Addr);
    SubroutineCount = *reinterpret_cast<int *>(Addr + 4);
    StateAddresses = reinterpret_cast<StateAddress *>(Addr + 8);
    SubroutineAddresses = &StateAddresses[StateCount];
    ScriptAddress = (char *)&SubroutineAddresses[StateCount];

    for (int i = 0; i < StateCount; i++)
    {
        ScriptState *NewState = new ScriptState();
        NewState->Name = StateAddresses[i].Name;
        NewState->OffsetAddress = StateAddresses[i].OffsetAddress;
        uint32_t StateSize;
        if (i == StateCount - 1)
        {
            StateSize = Size - NewState->OffsetAddress;
        }
        else
        {
            StateSize = StateAddresses[i + 1].OffsetAddress - NewState->OffsetAddress;
        }
        InitStateOffsets(reinterpret_cast<char *>(NewState->OffsetAddress) + (uint64_t)ScriptAddress, StateSize, NewState); // NOLINT(performance-no-int-to-ptr)
        States->push_back(NewState);
    }
    for (int i = 0; i < SubroutineCount; i++)
    {
        ScriptSubroutine *NewSubroutine = new ScriptSubroutine;
        NewSubroutine->Name = SubroutineAddresses[i].Name;
        NewSubroutine->OffsetAddress = SubroutineAddresses[i].OffsetAddress;
        Subroutines->push_back(NewSubroutine);
    }
}

void ScriptAnalyzer::InitStateOffsets(char *Addr, uint32_t Size, ScriptState *State)
{
    while (true)
    {
        OpCodes code = *reinterpret_cast<OpCodes *>(Addr);
        switch (code)
        {
        case OnEnter:
            State->Offsets.OnEnterOffset = Addr - ScriptAddress;
            break;
        case OnUpdate:
            State->Offsets.OnUpdateOffset = Addr - ScriptAddress;
            break;
        case OnExit:
            State->Offsets.OnExitOffset = Addr - ScriptAddress;
            break;
        case OnLanding:
            State->Offsets.OnLandingOffset = Addr - ScriptAddress;
            break;
        case OnHit:
            State->Offsets.OnHitOffset = Addr - ScriptAddress;
            break;
        case OnBlock:
            State->Offsets.OnBlockOffset = Addr - ScriptAddress;
            break;
        case OnHitOrBlock:
            State->Offsets.OnHitOrBlockOffset = Addr - ScriptAddress;
            break;
        case OnCounterHit:
            State->Offsets.OnCounterHitOffset = Addr - ScriptAddress;
            break;
        case OnSuperFreeze:
            State->Offsets.OnSuperFreezeOffset = Addr - ScriptAddress;
            break;
        case OnSuperFreezeEnd:
            State->Offsets.OnSuperFreezeEndOffset = Addr - ScriptAddress;
            break;
        case BeginState:
            break;
        case EndState:
            return;
        case SetCel:
            break;
        case BeginSubroutine:
            break;
        case EndSubroutine:
            break;
        case CallSubroutine:
            break;
        case CallSubroutineWithArgs:
            break;
        case ExitState:
            break;
        case EndBlock:
            break;
        case BeginLabel:
            break;
        case EndLabel:
            break;
        case GotoLabel:
            break;
        case If:
            break;
        case EndIf:
            break;
        case IfOperation:
            break;
        case IfNot:
            break;
        case Else:
            break;
        case EndElse:
            break;
        case GotoLabelIf:
            break;
        case GotoLabelIfOperation:
            break;
        case GotoLabelIfNot:
            break;
        case GetPlayerStats:
            break;
        case BeginStateDefine:
            break;
        case EndStateDefine:
            break;
        case SetStateType:
            break;
        case SetEntryState:
            break;
        case AddInputCondition:
            break;
        case AddStateCondition:
            break;
        case IsFollowupMove:
            break;
        case SetStateObjectID:
            break;
        case SetPosX:
            break;
        case AddPosX:
            break;
        case AddPosXRaw:
            break;
        case SetPosY:
            break;
        case AddPosY:
            break;
        case SetSpeedX:
            break;
        case AddSpeedX:
            break;
        case SetSpeedY:
            break;
        case AddSpeedY:
            break;
        case SetSpeedXPercent:
            break;
        case SetSpeedXPercentPerFrame:
            break;
        case EnableState:
            break;
        case DisableState:
            break;
        case EnableAll:
            break;
        case DisableAll:
            break;
        case EnableFlip:
            break;
        case ForceEnableFarNormal:
            break;
        case SetGravity: break;
        default:
            break;
        }
        Addr += InstructionSizes[code];
    }
}

void ScriptAnalyzer::Analyze(char *Addr, BattleActor *Actor)
{
    Addr += (uint64_t)ScriptAddress;
    bool CelExecuted = false;
    std::vector<StateAddress> Labels;
    GetAllLabels(Addr, &Labels);
    State *StateToModify = nullptr;
    char* ElseAddr = 0;
    while (true)
    {
        OpCodes code = *reinterpret_cast<OpCodes *>(Addr);
        switch (code)
        {
        case SetCel:
        {
            if (CelExecuted)
                return;
            int32_t AnimTime = *reinterpret_cast<int32_t *>(Addr + 68);
            if (Actor->AnimTime == AnimTime)
            {
                Actor->SetCelName(Addr + 4);
                CelExecuted = true;
            }
            else if (Actor->AnimTime > AnimTime)
            {
                FindNextCel(&Addr);
                break;
            }
            break;
        }
        case CallSubroutine:
        {
            Actor->Player->CallSubroutine(Addr + 4);
            break;
        }
        case ExitState:
        case EndBlock:
        {
            return;
        }
        case GotoLabel:
        {
            CString<64> LabelName;
            LabelName.SetString(Addr + 4);
            for (StateAddress Label : Labels)
            {
                if (!strcmp(Label.Name.GetString(), LabelName.GetString()))
                {
                    Addr = ScriptAddress + Label.OffsetAddress;
                    if (FindNextCel(&Addr))
                    {
                        Actor->AnimTime = *reinterpret_cast<int32_t *>(Addr + 68) - 1;
                    }
                    return;
                }
            }
            break;
        }
        case EndLabel:
        {
            int32_t AnimTime = *reinterpret_cast<int32_t *>(Addr + 4);
            if (Actor->AnimTime < AnimTime)
                return;
            break;
        }
        case BeginStateDefine:
        {
            CString<64> StateName;
            StateName.SetString(Addr + 4);
            int32_t Index = Actor->Player->StateMachine.GetStateIndex(StateName);
            StateToModify = Actor->Player->StateMachine.States[Index];
            break;
        }
        case EndStateDefine:
            StateToModify = nullptr;
            break;
        case SetStateType:
            if (StateToModify)
            {
                StateToModify->Type = *reinterpret_cast<StateType *>(Addr + 4);
            }
            break;
        case SetEntryState:
            if (StateToModify)
            {
                StateToModify->StateEntryState = *reinterpret_cast<EntryState *>(Addr + 4);
            }
            break;
        case AddInputCondition:
            if (StateToModify)
            {
                StateToModify->InputConditions.push_back(*reinterpret_cast<InputCondition *>(Addr + 4));
            }
            break;
        case AddStateCondition:
            if (StateToModify)
            {
                StateToModify->StateConditions.push_back(*reinterpret_cast<StateCondition *>(Addr + 4));
            }
            break;
        case IsFollowupMove:
            if (StateToModify)
            {
                StateToModify->IsFollowupState = *reinterpret_cast<bool *>(Addr + 4);
            }
            break;
        case SetStateObjectID:
            if (StateToModify)
            {
                StateToModify->ObjectID = *reinterpret_cast<int32_t *>(Addr + 4);
            }
            break;
        case BeginState:
            break;
        case EndState:
            return;
        case BeginSubroutine:
            break;
        case EndSubroutine:
            return;
        case CallSubroutineWithArgs:
            break;
        case OnEnter:
            break;
        case OnUpdate:
            break;
        case OnExit:
            break;
        case OnLanding:
            break;
        case OnHit:
            break;
        case OnBlock:
            break;
        case OnHitOrBlock:
            break;
        case OnCounterHit:
            break;
        case OnSuperFreeze:
            break;
        case OnSuperFreezeEnd:
            break;
        case BeginLabel:
            break;
        case If:
        {
            int32_t Operand = *reinterpret_cast<int32_t *>(Addr + 8);
            if (*reinterpret_cast<int32_t *>(Addr + 4) > 0)
            {
                Operand = Actor->GetInternalValue((InternalValue)Operand);
            }
            if (Operand != 0)
            {
                break;
            }
            else
            {
                FindMatchingEnd(&Addr, EndIf);
                code = EndIf;
                break;
            }
        }
        case EndIf:
            break;
        case IfOperation:
        {
            int32_t Operand1 = *reinterpret_cast<int32_t *>(Addr + 12);
            if (*reinterpret_cast<int32_t *>(Addr + 8) > 0)
            {
                Operand1 = Actor->GetInternalValue((InternalValue)Operand1);
            }
            int32_t Operand2 = *reinterpret_cast<int32_t *>(Addr + 20);
            if (*reinterpret_cast<int32_t *>(Addr + 16) > 0)
            {
                Operand2 = Actor->GetInternalValue((InternalValue)Operand1);
            }
            Operation Op = *reinterpret_cast<Operation *>(Addr + 4);
            CheckOperation(Op, Operand1, Operand2, &Actor->StoredRegister);
            if (Actor->StoredRegister != 0)
            {
                break;
            }
            else
            {
                FindMatchingEnd(&Addr, EndIf);
                ElseAddr = Addr;
                FindElse(&ElseAddr);
                code = EndIf;
                break;
            }
        }
        case IfNot:
        {
            int32_t Operand = *reinterpret_cast<int32_t *>(Addr + 8);
            if (*reinterpret_cast<int32_t *>(Addr + 4) > 0)
            {
                Operand = Actor->GetInternalValue((InternalValue)Operand);
            }
            if (Operand == 0)
            {
                break;
            }
            else
            {
                FindMatchingEnd(&Addr, EndIf);
                ElseAddr = Addr;
                FindElse(&ElseAddr);
                code = EndIf;
                break;
            }
        };
        case Else:
            if (ElseAddr == Addr)
            {
                ElseAddr = 0;
                break;
            }
            else
            {
                FindMatchingEnd(&Addr, EndElse);
                code = EndElse;
                break;
            }
        case EndElse:
            break;
        case GotoLabelIf:
            break;
        case GotoLabelIfOperation:
            break;
        case GotoLabelIfNot:
            break;
        case GetPlayerStats:
        {
            PlayerStats Stat = *reinterpret_cast<PlayerStats*>(Addr + 4);
            int32_t Val = 0;
            switch(Stat)
            {
            case PLY_FWalkSpeed:
                Val = Actor->Player->FWalkSpeed;
                break;
            case PLY_BWalkSpeed:
                Val = Actor->Player->BWalkSpeed;
                break;
            case PLY_FDashInitSpeed:
                Val = Actor->Player->FDashInitSpeed;
                break;
            case PLY_FDashAccel:
                Val = Actor->Player->FDashAccel;
                break;
            case PLY_FDashMaxSpeed:
                Val = Actor->Player->FDashMaxSpeed;
                break;
            case PLY_FDashFriction:
                Val = Actor->Player->FDashFriction;
                break;
            case PLY_BDashSpeed:
                Val = Actor->Player->BDashSpeed;
                break;
            case PLY_BDashHeight:
                Val = Actor->Player->BDashHeight;
                break;
            case PLY_BDashGravity:
                Val = Actor->Player->BDashGravity;
                break;
            case PLY_JumpHeight:
                Val = Actor->Player->JumpHeight;
                break;
            case PLY_FJumpSpeed:
                Val = Actor->Player->FJumpSpeed;
                break;
            case PLY_BJumpSpeed:
                Val = Actor->Player->BJumpSpeed;
                break;
            case PLY_JumpGravity:
                Val = Actor->Player->JumpGravity;
                break;
            case PLY_SuperJumpHeight:
                Val = Actor->Player->JumpHeight;
                break;
            case PLY_FSuperJumpSpeed:
                Val = Actor->Player->FJumpSpeed;
                break;
            case PLY_BSuperJumpSpeed:
                Val = Actor->Player->BJumpSpeed;
                break;
            case PLY_SuperJumpGravity:
                Val = Actor->Player->JumpGravity;
                break;
            case PLY_FAirDashSpeed:
                Val = Actor->Player->FAirDashSpeed;
                break;
            case PLY_BAirDashSpeed:
                Val = Actor->Player->BAirDashSpeed;
                break;
            }
            Actor->StoredRegister = Val;
            break;
        }
        case SetPosX:
        {
            int32_t Operand = *reinterpret_cast<int32_t *>(Addr + 8);
            if (*reinterpret_cast<int32_t *>(Addr + 4) > 0)
            {
                Operand = Actor->GetInternalValue((InternalValue)Operand);
            }
            Actor->SetPosX(Operand);
            break;
        }
        case AddPosX:
        {
            int32_t Operand = *reinterpret_cast<int32_t *>(Addr + 8);
            if (*reinterpret_cast<int32_t *>(Addr + 4) > 0)
            {
                Operand = Actor->GetInternalValue((InternalValue)Operand);
            }
            Actor->AddPosX(Operand);
            break;
        }
        case AddPosXRaw:
        {
            int32_t Operand = *reinterpret_cast<int32_t *>(Addr + 8);
            if (*reinterpret_cast<int32_t *>(Addr + 4) > 0)
            {
                Operand = Actor->GetInternalValue((InternalValue)Operand);
            }
            Actor->AddPosXRaw(Operand);
            break;
        }
        case SetPosY:
        {
            int32_t Operand = *reinterpret_cast<int32_t *>(Addr + 8);
            if (*reinterpret_cast<int32_t *>(Addr + 4) > 0)
            {
                Operand = Actor->GetInternalValue((InternalValue)Operand);
            }
            Actor->SetPosY(Operand);
            break;
        }
        case AddPosY:
        {
            int32_t Operand = *reinterpret_cast<int32_t *>(Addr + 8);
            if (*reinterpret_cast<int32_t *>(Addr + 4) > 0)
            {
                Operand = Actor->GetInternalValue((InternalValue)Operand);
            }
            Actor->AddPosY(Operand);
            break;
        }
        case SetSpeedX:
        {
            int32_t Operand = *reinterpret_cast<int32_t *>(Addr + 8);
            if (*reinterpret_cast<int32_t *>(Addr + 4) > 0)
            {
                Operand = Actor->GetInternalValue((InternalValue)Operand);
            }
            Actor->SetSpeedX(Operand);
            break;
        }
        case AddSpeedX:
        {
            int32_t Operand = *reinterpret_cast<int32_t *>(Addr + 8);
            if (*reinterpret_cast<int32_t *>(Addr + 4) > 0)
            {
                Operand = Actor->GetInternalValue((InternalValue)Operand);
            }
            Actor->AddSpeedX(Operand);
            break;
        }
        case SetSpeedY:
        {
            int32_t Operand = *reinterpret_cast<int32_t *>(Addr + 8);
            if (*reinterpret_cast<int32_t *>(Addr + 4) > 0)
            {
                Operand = Actor->GetInternalValue((InternalValue)Operand);
            }
            Actor->SetSpeedY(Operand);
            break;
        }
        case AddSpeedY:
        {
            int32_t Operand = *reinterpret_cast<int32_t *>(Addr + 8);
            if (*reinterpret_cast<int32_t *>(Addr + 4) > 0)
            {
                Operand = Actor->GetInternalValue((InternalValue)Operand);
            }
            Actor->AddSpeedY(Operand);
            break;
        }
        case SetSpeedXPercent:
        {
            int32_t Operand = *reinterpret_cast<int32_t *>(Addr + 8);
            if (*reinterpret_cast<int32_t *>(Addr + 4) > 0)
            {
                Operand = Actor->GetInternalValue((InternalValue)Operand);
            }
            Actor->SetSpeedXPercent(Operand);
            break;
        }
        case SetSpeedXPercentPerFrame:
        {
            int32_t Operand = *reinterpret_cast<int32_t *>(Addr + 8);
            if (*reinterpret_cast<int32_t *>(Addr + 4) > 0)
            {
                Operand = Actor->GetInternalValue((InternalValue)Operand);
            }
            Actor->SetSpeedXPercentPerFrame(Operand);
            break;
        }
        case SetGravity:
        {
            int32_t Operand = *reinterpret_cast<int32_t *>(Addr + 8);
            if (*reinterpret_cast<int32_t *>(Addr + 4) > 0)
            {
                Operand = Actor->GetInternalValue((InternalValue)Operand);
            }
            Actor->SetGravity(Operand);
            break;
        }
        case EnableState:
        {
            if (Actor->IsPlayer)
            {
                Actor->Player->EnableState(*reinterpret_cast<EnableFlags *>(Addr + 4));
            }
            break;
        }
        case DisableState:
        {
            if (Actor->IsPlayer)
            {
                Actor->Player->DisableState(*reinterpret_cast<EnableFlags *>(Addr + 4));
            }
            break;
        }
        case EnableAll:
        {
            if (Actor->IsPlayer)
            {
                Actor->Player->EnableAll();
            }
            break;
        }
        case DisableAll:
        {
            if (Actor->IsPlayer)
            {
                Actor->Player->DisableAll();
            }
            break;
        }
        case EnableFlip:
            Actor->EnableFlip(*reinterpret_cast<bool *>(Addr + 4));
            break;
        case ForceEnableFarNormal:
        {
            if (Actor->IsPlayer)
            {
                Actor->Player->ForceEnableFarNormal(*reinterpret_cast<bool *>(Addr + 4));
            }
            break;
        }
        default:
            break;
        }
        Addr += InstructionSizes[code];
    }
}

bool ScriptAnalyzer::FindNextCel(char **Addr)
{
    while (true)
    {
        OpCodes code = *reinterpret_cast<OpCodes *>(*Addr);
        switch (code)
        {
        case SetCel:
        case EndLabel:
            return true;
        case ExitState:
        case EndBlock:
            return false;
        case BeginState:
            break;
        case EndState:
            return false;
        case BeginSubroutine:
            break;
        case EndSubroutine:
            return false;
        case CallSubroutine:
            break;
        case CallSubroutineWithArgs:
            break;
        case OnEnter:
            break;
        case OnUpdate:
            break;
        case OnExit:
            break;
        case OnLanding:
            break;
        case OnHit:
            break;
        case OnBlock:
            break;
        case OnHitOrBlock:
            break;
        case OnCounterHit:
            break;
        case OnSuperFreeze:
            break;
        case OnSuperFreezeEnd:
            break;
        case BeginLabel:
            break;
        case GotoLabel:
            break;
        case If:
            break;
        case EndIf:
            break;
        case IfOperation:
            break;
        case IfNot:
            break;
        case Else:
            break;
        case EndElse:
            break;
        case GotoLabelIf:
            break;
        case GotoLabelIfOperation:
            break;
        case GotoLabelIfNot:
            break;
        case GetPlayerStats:
            break;
        case BeginStateDefine:
            break;
        case EndStateDefine:
            break;
        case SetStateType:
            break;
        case SetEntryState:
            break;
        case AddInputCondition:
            break;
        case AddStateCondition:
            break;
        case IsFollowupMove:
            break;
        case SetStateObjectID:
            break;
        case SetPosX:
            break;
        case AddPosX:
            break;
        case AddPosXRaw:
            break;
        case SetPosY:
            break;
        case AddPosY:
            break;
        case SetSpeedX:
            break;
        case AddSpeedX:
            break;
        case SetSpeedY:
            break;
        case AddSpeedY:
            break;
        case SetSpeedXPercent:
            break;
        case SetSpeedXPercentPerFrame:
            break;
        case EnableState:
            break;
        case DisableState:
            break;
        case EnableAll:
            break;
        case DisableAll:
            break;
        case EnableFlip:
            break;
        case ForceEnableFarNormal:
            break;
        case SetGravity: break;
        default:
            break;
        }
        *Addr += InstructionSizes[code];
    }
}

void ScriptAnalyzer::FindMatchingEnd(char **Addr, OpCodes EndCode)
{
    while (true)
    {
        OpCodes code = *reinterpret_cast<OpCodes*>(*Addr);
        if (code == EndCode)
            return;
        switch (code)
        {
        case BeginLabel:
            break;
        case ExitState:
        case EndBlock:
            return;
        case BeginState:
            break;
        case EndState:
            return;
        case SetCel:
            break;
        case BeginSubroutine:
            break;
        case EndSubroutine:
            return;
        case CallSubroutine:
            break;
        case CallSubroutineWithArgs:
            break;
        case OnEnter:
            break;
        case OnUpdate:
            break;
        case OnExit:
            break;
        case OnLanding:
            break;
        case OnHit:
            break;
        case OnBlock:
            break;
        case OnHitOrBlock:
            break;
        case OnCounterHit:
            break;
        case OnSuperFreeze:
            break;
        case OnSuperFreezeEnd:
            break;
        case EndLabel:
            return;
        case GotoLabel:
            break;
        case If:
            break;
        case EndIf:
            break;
        case IfOperation:
            break;
        case IfNot:
            break;
        case Else:
            break;
        case EndElse:
            break;
        case GotoLabelIf:
            break;
        case GotoLabelIfOperation:
            break;
        case GotoLabelIfNot:
            break;
        case GetPlayerStats:
            break;
        case BeginStateDefine:
            break;
        case EndStateDefine:
            break;
        case SetStateType:
            break;
        case SetEntryState:
            break;
        case AddInputCondition:
            break;
        case AddStateCondition:
            break;
        case IsFollowupMove:
            break;
        case SetStateObjectID:
            break;
        case SetPosX:
            break;
        case AddPosX:
            break;
        case AddPosXRaw:
            break;
        case SetPosY:
            break;
        case AddPosY:
            break;
        case SetSpeedX:
            break;
        case AddSpeedX:
            break;
        case SetSpeedY:
            break;
        case AddSpeedY:
            break;
        case SetSpeedXPercent:
            break;
        case SetSpeedXPercentPerFrame:
            break;
        case EnableState:
            break;
        case DisableState:
            break;
        case EnableAll:
            break;
        case DisableAll:
            break;
        case EnableFlip:
            break;
        case ForceEnableFarNormal:
            break;
        case SetGravity: break;
        default:
            break;
        }
        *Addr += InstructionSizes[code];
    }
}

void ScriptAnalyzer::FindElse(char **Addr)
{
    while (true)
    {
        OpCodes code = *reinterpret_cast<OpCodes *>(*Addr);
        switch (code)
        {
        case BeginLabel:
            break;
        case ExitState:
        case EndBlock:
            *Addr = 0;
            return;
        case BeginState:
            break;
        case EndState:
            *Addr = 0;
            return;
        case SetCel:
            break;
        case BeginSubroutine:
            break;
        case EndSubroutine:
            *Addr = 0;
            return;
        case CallSubroutine:
            break;
        case CallSubroutineWithArgs:
            break;
        case OnEnter:
            break;
        case OnUpdate:
            break;
        case OnExit:
            break;
        case OnLanding:
            break;
        case OnHit:
            break;
        case OnBlock:
            break;
        case OnHitOrBlock:
            break;
        case OnCounterHit:
            break;
        case OnSuperFreeze:
            break;
        case OnSuperFreezeEnd:
            break;
        case EndLabel:
            *Addr = 0;
            return;
        case GotoLabel:
            break;
        case If:
            break;
        case EndIf:
            break;
        case IfOperation:
            break;
        case IfNot:
            break;
        case Else:
            return;
        case EndElse:
            break;
        case GotoLabelIf:
            break;
        case GotoLabelIfOperation:
            break;
        case GotoLabelIfNot:
            break;
        case GetPlayerStats:
            break;
        case BeginStateDefine:
            break;
        case EndStateDefine:
            break;
        case SetStateType:
            break;
        case SetEntryState:
            break;
        case AddInputCondition:
            break;
        case AddStateCondition:
            break;
        case IsFollowupMove:
            break;
        case SetStateObjectID:
            break;
        case SetPosX:
            break;
        case AddPosX:
            break;
        case AddPosXRaw:
            break;
        case SetPosY:
            break;
        case AddPosY:
            break;
        case SetSpeedX:
            break;
        case AddSpeedX:
            break;
        case SetSpeedY:
            break;
        case AddSpeedY:
            break;
        case SetSpeedXPercent:
            break;
        case SetSpeedXPercentPerFrame:
            break;
        case EnableState:
            break;
        case DisableState:
            break;
        case EnableAll:
            break;
        case DisableAll:
            break;
        case EnableFlip:
            break;
        case ForceEnableFarNormal:
            break;
        case SetGravity: break;
        default:
            break;
        }
        *Addr += InstructionSizes[code];
    }
}

void ScriptAnalyzer::GetAllLabels(char *Addr, std::vector<StateAddress> *Labels)
{
    while (true)
    {
        OpCodes code = *reinterpret_cast<OpCodes *>(Addr);
        switch (code)
        {
        case BeginLabel:
        {
            CString<64> LabelName;
            LabelName.SetString(Addr + 4);
            StateAddress Label;
            Label.Name = LabelName;
            Label.OffsetAddress = Addr - ScriptAddress;
            Labels->push_back(Label);
            break;
        }
        case ExitState:
        case EndBlock:
            return;
        case BeginState:
            break;
        case EndState:
            return;
        case SetCel:
            break;
        case BeginSubroutine:
            break;
        case EndSubroutine:
            return;
        case CallSubroutine:
            break;
        case CallSubroutineWithArgs:
            break;
        case OnEnter:
            break;
        case OnUpdate:
            break;
        case OnExit:
            break;
        case OnLanding:
            break;
        case OnHit:
            break;
        case OnBlock:
            break;
        case OnHitOrBlock:
            break;
        case OnCounterHit:
            break;
        case OnSuperFreeze:
            break;
        case OnSuperFreezeEnd:
            break;
        case EndLabel:
            break;
        case GotoLabel:
            break;
        case If:
            break;
        case EndIf:
            break;
        case IfOperation:
            break;
        case IfNot:
            break;
        case Else:
            break;
        case EndElse:
            break;
        case GotoLabelIf:
            break;
        case GotoLabelIfOperation:
            break;
        case GotoLabelIfNot:
            break;
        case GetPlayerStats:
            break;
        case BeginStateDefine:
            break;
        case EndStateDefine:
            break;
        case SetStateType:
            break;
        case SetEntryState:
            break;
        case AddInputCondition:
            break;
        case AddStateCondition:
            break;
        case IsFollowupMove:
            break;
        case SetStateObjectID:
            break;
        case SetPosX:
            break;
        case AddPosX:
            break;
        case AddPosXRaw:
            break;
        case SetPosY:
            break;
        case AddPosY:
            break;
        case SetSpeedX:
            break;
        case AddSpeedX:
            break;
        case SetSpeedY:
            break;
        case AddSpeedY:
            break;
        case SetSpeedXPercent:
            break;
        case SetSpeedXPercentPerFrame:
            break;
        case EnableState:
            break;
        case DisableState:
            break;
        case EnableAll:
            break;
        case DisableAll:
            break;
        case EnableFlip:
            break;
        case ForceEnableFarNormal:
            break;
        case SetGravity: break;
        default:
            break;
        }
        Addr += InstructionSizes[code];
    }
}

void ScriptAnalyzer::CheckOperation(Operation Op, int32_t Operand1, int32_t Operand2, int32_t *Return)
{
    switch (Op)
    {
    case OP_Add:
    {
        *Return = Operand1 + Operand2;
        break;
    }
    case OP_Sub:
    {
        *Return = Operand1 - Operand2;
        break;
    }
    case OP_Mul:
    {
        *Return = Operand1 * Operand2;
        break;
    }
    case OP_Div:
    {
        *Return = Operand1 / Operand2;
        break;
    }
    case OP_Mod:
    {
        *Return = Operand1 % Operand2;
        break;
    }
    case OP_And:
    {
        *Return = Operand1 && Operand2;
        break;
    }
    case OP_Or:
    {
        *Return = Operand1 || Operand2;
        break;
    }
    case OP_BitwiseAnd:
    {
        *Return = Operand1 & Operand2;
        break;
    }
    case OP_BitwiseOr:
    {
        *Return = Operand1 | Operand2;
        break;
    }
    case OP_IsEqual:
    {
        *Return = Operand1 == Operand2;
        break;
    }
    case OP_IsGreater:
    {
        *Return = Operand1 > Operand2;
        break;
    }
    case OP_IsLesser:
    {
        *Return = Operand1 < Operand2;
        break;
    }
    case OP_IsGreaterOrEqual:
    {
        *Return = Operand1 >= Operand2;
        break;
    }
    case OP_IsLesserOrEqual:
    {
        *Return = Operand1 <= Operand2;
        break;
    }
    case OP_BitDelete:
    {
        *Return = Operand1 & ~Operand2;
        break;
    }
    case OP_IsNotEqual:
    {
        *Return = Operand1 != Operand2;
        break;
    }
    }
}