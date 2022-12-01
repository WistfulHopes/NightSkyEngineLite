#include "ScriptAnalyzer.h"
#include "../Actors/PlayerCharacter.h"

void ScriptAnalyzer::Initialize(char* Addr, uint32_t Size, std::vector<State*>* States, std::vector<Subroutine*>* Subroutines)
{
    DataAddress = Addr;
    StateCount = *reinterpret_cast<int*>(Addr);
    SubroutineCount = *reinterpret_cast<int*>(Addr + 4);
    StateAddresses = reinterpret_cast<StateAddress*>(Addr + 8);
    SubroutineAddresses = &StateAddresses[StateCount];
    ScriptAddress = (char*)&SubroutineAddresses[StateCount];
    
    for (int i = 0; i < StateCount; i++)
    {
        ScriptState* NewState = new ScriptState();
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
        InitStateOffsets(reinterpret_cast<char*>(NewState->OffsetAddress) + (uint64_t)ScriptAddress, StateSize, NewState);  // NOLINT(performance-no-int-to-ptr)
        States->push_back(NewState);
    }
    for (int i = 0; i < SubroutineCount; i++)
    {
        ScriptSubroutine* NewSubroutine = new ScriptSubroutine;
        NewSubroutine->Name = SubroutineAddresses[i].Name;
        NewSubroutine->OffsetAddress = SubroutineAddresses[i].OffsetAddress;
        Subroutines->push_back(NewSubroutine);
    }
}

void ScriptAnalyzer::InitStateOffsets(char* Addr, uint32_t Size, ScriptState* State)
{
    while (true)
    {
        OpCodes code = *reinterpret_cast<OpCodes*>(Addr);
        switch(code)
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
        case BeginState: break;
        case EndState: return;
        case SetCel: break;
        case BeginSubroutine: break;
        case EndSubroutine: break;
        case CallSubroutine: break;
        case CallSubroutineWithArgs: break;
        case ExitState: break;
        case EndBlock: break;
        case BeginLabel: break;
        case EndLabel: break;
        case GotoLabel: break;
        case If: break;
        case EndIf: break;
        case IfOperation: break;
        case IfNot: break;
        case Else: break;
        case EndElse: break;
        case GotoLabelIf: break;
        case GotoLabelIfOperation: break;
        case GotoLabelIfNot: break;
        case BeginStateDefine: break;
        case EndStateDefine: break;
        case SetStateType: break;
        case SetEntryState: break;
        case AddInputCondition: break;
        case AddStateCondition: break;
        case IsFollowupMove: break;
        case SetStateObjectID: break;
        case SetPosX: break;
        case AddPosX: break;
        case AddPosXRaw: break;
        case SetPosY: break;
        case AddPosY: break;
        case SetSpeedX: break;
        case AddSpeedX: break;
        case SetSpeedY: break;
        case AddSpeedY: break;
        case SetSpeedXPercent: break;
        case SetSpeedXPercentPerFrame: break;
        case EnableState: break;
        case DisableState: break;
        case EnableAll: break;
        case DisableAll: break;
        case EnableFlip: break;
        case ForceEnableFarNormal: break;
        default:
            break;
        }
        Addr += InstructionSizes[code];
    }
}

void ScriptAnalyzer::Analyze(char* Addr, BattleActor* Actor)
{
    Addr += (uint64_t)ScriptAddress;
    bool CelExecuted = false;
    std::vector<StateAddress> Labels;
    GetAllLabels(Addr, &Labels);
    State* StateToModify = nullptr;
    while (true)
    {
        OpCodes code = *reinterpret_cast<OpCodes*>(Addr);
        switch(code)
        {
        case SetCel:
            {
                if (CelExecuted)
                    return;
                int32_t AnimTime = *reinterpret_cast<int32_t*>(Addr + 68);
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
                if (!Actor->IsPlayer)
                {
                    Actor->DeactivateObject();
                }
                else
                {
                    switch(Actor->Player->CurrentActionFlags)
                    {
                    case ACT_Standing:
                        Actor->Player->JumpToState("Stand");
                        break;
                    case ACT_Crouching:
                        Actor->Player->JumpToState("Crouch");
                        break;
                    case ACT_Jumping:
                        Actor->Player->JumpToState("VJump");
                        break;
                    }
                }
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
                            Actor->AnimTime = *reinterpret_cast<int32_t*>(Addr + 68) - 1;
                        }
                        return;
                    }
                }
                break;
            }
        case EndLabel:
            {
                int32_t AnimTime = *reinterpret_cast<int32_t*>(Addr + 4);
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
                StateToModify->Type = *reinterpret_cast<StateType*>(Addr + 4);
            }
            break;
        case SetEntryState:
            if (StateToModify)
            {
                StateToModify->StateEntryState = *reinterpret_cast<EntryState*>(Addr + 4);
            }
            break;
        case AddInputCondition:
            if (StateToModify)
            {
                StateToModify->InputConditions.push_back(*reinterpret_cast<InputCondition*>(Addr + 4));
            }
            break;
        case AddStateCondition:
            if (StateToModify)
            {
                StateToModify->StateConditions.push_back(*reinterpret_cast<StateCondition*>(Addr + 4));
            }
            break;
        case IsFollowupMove:
            if (StateToModify)
            {
                StateToModify->IsFollowupState = *reinterpret_cast<bool*>(Addr + 4);
            }
            break;
        case SetStateObjectID:
            if (StateToModify)
            {
                StateToModify->ObjectID = *reinterpret_cast<int32_t*>(Addr + 4);
            }
            break;
        case BeginState: break;
        case EndState: return;
        case BeginSubroutine: break;
        case EndSubroutine: return;
        case CallSubroutineWithArgs: break;
        case OnEnter: break;
        case OnUpdate: break;
        case OnExit: break;
        case OnLanding: break;
        case OnHit: break;
        case OnBlock: break;
        case OnHitOrBlock: break;
        case OnCounterHit: break;
        case OnSuperFreeze: break;
        case OnSuperFreezeEnd: break;
        case BeginLabel: break;
        case If: break;
        case EndIf: break;
        case IfOperation: break;
        case IfNot: break;
        case Else: break;
        case EndElse: break;
        case GotoLabelIf: break;
        case GotoLabelIfOperation: break;
        case GotoLabelIfNot: break;
        case SetPosX: 
            Actor->SetPosX(*reinterpret_cast<int32_t*>(Addr + 4));
        case AddPosX: 
            Actor->AddPosX(*reinterpret_cast<int32_t*>(Addr + 4));
        case AddPosXRaw:
            Actor->AddPosXRaw(*reinterpret_cast<int32_t*>(Addr + 4));
        case SetPosY: 
            Actor->SetPosY(*reinterpret_cast<int32_t*>(Addr + 4));
        case AddPosY: 
            Actor->AddPosY(*reinterpret_cast<int32_t*>(Addr + 4));
        case SetSpeedX: 
            Actor->SetSpeedX(*reinterpret_cast<int32_t*>(Addr + 4));
        case AddSpeedX: 
            Actor->AddSpeedX(*reinterpret_cast<int32_t*>(Addr + 4));
        case SetSpeedY: 
            Actor->SetSpeedY(*reinterpret_cast<int32_t*>(Addr + 4));
        case AddSpeedY:
            Actor->AddSpeedY(*reinterpret_cast<int32_t*>(Addr + 4));
        case SetSpeedXPercent:
            Actor->SetSpeedXPercent(*reinterpret_cast<int32_t*>(Addr + 4));
        case SetSpeedXPercentPerFrame:
            Actor->SetSpeedXPercentPerFrame(*reinterpret_cast<int32_t*>(Addr + 4));
        case EnableState:
            {
                if (Actor->IsPlayer)
                {
                    Actor->Player->EnableState(*reinterpret_cast<EnableFlags*>(Addr + 4));
                }
            }
        case DisableState: 
            {
                if (Actor->IsPlayer)
                {
                    Actor->Player->DisableState(*reinterpret_cast<EnableFlags*>(Addr + 4));
                }
            }
        case EnableAll: 
            {
                if (Actor->IsPlayer)
                {
                    Actor->Player->EnableAll();
                }
            }
        case DisableAll: 
            {
                if (Actor->IsPlayer)
                {
                    Actor->Player->DisableAll();
                }
            }
        case EnableFlip:
            Actor->EnableFlip(*reinterpret_cast<bool*>(Addr + 4));
        case ForceEnableFarNormal:
            {
                if (Actor->IsPlayer)
                {
                    Actor->Player->ForceEnableFarNormal(*reinterpret_cast<bool*>(Addr + 4));
                }
            }
        default:
            break;
        }
        Addr += InstructionSizes[code];
    }
}

bool ScriptAnalyzer::FindNextCel(char** Addr)
{
    while (true)
    {
        OpCodes code = *reinterpret_cast<OpCodes*>(*Addr);
        switch(code)
        {
        case SetCel:
        case EndLabel:
            return true;
        case ExitState:
        case EndBlock:
            return false;
        case BeginState: break;
        case EndState: return false;
        case BeginSubroutine: break;
        case EndSubroutine: return false;
        case CallSubroutine: break;
        case CallSubroutineWithArgs: break;
        case OnEnter: break;
        case OnUpdate: break;
        case OnExit: break;
        case OnLanding: break;
        case OnHit: break;
        case OnBlock: break;
        case OnHitOrBlock: break;
        case OnCounterHit: break;
        case OnSuperFreeze: break;
        case OnSuperFreezeEnd: break;
        case BeginLabel: break;
        case GotoLabel: break;
        case If: break;
        case EndIf: break;
        case IfOperation: break;
        case IfNot: break;
        case Else: break;
        case EndElse: break;
        case GotoLabelIf: break;
        case GotoLabelIfOperation: break;
        case GotoLabelIfNot: break;
        case BeginStateDefine: break;
        case EndStateDefine: break;
        case SetStateType: break;
        case SetEntryState: break;
        case AddInputCondition: break;
        case AddStateCondition: break;
        case IsFollowupMove: break;
        case SetStateObjectID: break;
        case SetPosX: break;
        case AddPosX: break;
        case AddPosXRaw: break;
        case SetPosY: break;
        case AddPosY: break;
        case SetSpeedX: break;
        case AddSpeedX: break;
        case SetSpeedY: break;
        case AddSpeedY: break;
        case SetSpeedXPercent: break;
        case SetSpeedXPercentPerFrame: break;
        case EnableState: break;
        case DisableState: break;
        case EnableAll: break;
        case DisableAll: break;
        case EnableFlip: break;
        case ForceEnableFarNormal: break;
        default:
            break;
        }
        *Addr += InstructionSizes[code];
    }
}

void ScriptAnalyzer::GetAllLabels(char* Addr, std::vector<StateAddress>* Labels)
{
    while (true)
    {
        OpCodes code = *reinterpret_cast<OpCodes*>(Addr);
        switch(code)
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
        case BeginState: break;
        case EndState: return;
        case SetCel: break;
        case BeginSubroutine: break;
        case EndSubroutine: return;
        case CallSubroutine: break;
        case CallSubroutineWithArgs: break;
        case OnEnter: break;
        case OnUpdate: break;
        case OnExit: break;
        case OnLanding: break;
        case OnHit: break;
        case OnBlock: break;
        case OnHitOrBlock: break;
        case OnCounterHit: break;
        case OnSuperFreeze: break;
        case OnSuperFreezeEnd: break;
        case EndLabel: return;
        case GotoLabel: break;
        case If: break;
        case EndIf: break;
        case IfOperation: break;
        case IfNot: break;
        case Else: break;
        case EndElse: break;
        case GotoLabelIf: break;
        case GotoLabelIfOperation: break;
        case GotoLabelIfNot: break;
        case BeginStateDefine: break;
        case EndStateDefine: break;
        case SetStateType: break;
        case SetEntryState: break;
        case AddInputCondition: break;
        case AddStateCondition: break;
        case IsFollowupMove: break;
        case SetStateObjectID: break;
        case SetPosX: break;
        case AddPosX: break;
        case AddPosXRaw: break;
        case SetPosY: break;
        case AddPosY: break;
        case SetSpeedX: break;
        case AddSpeedX: break;
        case SetSpeedY: break;
        case AddSpeedY: break;
        case SetSpeedXPercent: break;
        case SetSpeedXPercentPerFrame: break;
        case EnableState: break;
        case DisableState: break;
        case EnableAll: break;
        case DisableAll: break;
        case EnableFlip: break;
        case ForceEnableFarNormal: break;
        default:
            break;
        }
        Addr += InstructionSizes[code];
    }
}