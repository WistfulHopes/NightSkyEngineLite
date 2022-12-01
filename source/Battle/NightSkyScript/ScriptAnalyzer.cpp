#include "ScriptAnalyzer.h"
#include "../Actors/PlayerCharacter.h"
#include <stdio.h>   
#include <stdlib.h> 

void ScriptAnalyzer::Initialize(char* Addr, uint32_t Size, std::vector<State*>* States, std::vector<Subroutine*>* Subroutines)
{
    DataAddress = Addr;
    StateCount = *(int*)Addr;
    SubroutineCount = *(int*)(Addr + 4);
    StateAddresses = (StateAddress*)(Addr + 8);
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
        InitStateOffsets((char*)NewState->OffsetAddress + (uint64_t)Addr, StateSize, NewState);
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
    for (uint32_t i = 0; i < Size;)
    {
        OpCodes code = *(OpCodes*)Addr;
        switch(code)
        {
        case OpCodes::OnEnter:
            State->Offsets.OnEnterOffset = i * 4;
            break;
        case OpCodes::OnUpdate:
            State->Offsets.OnUpdateOffset = i * 4;
            break;
        case OpCodes::OnExit:
            State->Offsets.OnExitOffset = i * 4;
            break;
        case OpCodes::OnLanding:
            State->Offsets.OnLandingOffset = i * 4;
            break;
        case OpCodes::OnHit:
            State->Offsets.OnHitOffset = i * 4;
            break;
        case OpCodes::OnBlock:
            State->Offsets.OnBlockOffset = i * 4;
            break;
        case OpCodes::OnHitOrBlock:
            State->Offsets.OnHitOrBlockOffset = i * 4;
            break;
        case OpCodes::OnCounterHit:
            State->Offsets.OnCounterHitOffset = i * 4;
            break;
        case OpCodes::OnSuperFreeze:
            State->Offsets.OnSuperFreezeOffset = i * 4;
            break;
        case OpCodes::OnSuperFreezeEnd:
            State->Offsets.OnSuperFreezeEndOffset = i * 4;
            break;
        default:
            break;
        }
        i += InstructionSizes[code];
    }
}

void ScriptAnalyzer::Analyze(char* Addr, BattleActor* Actor)
{
    Addr += (uint64_t)ScriptAddress;
    bool CelExecuted = false;
    std::vector<StateAddress> Labels;
    char* BakAddr = Addr;
    GetAllLabels(BakAddr, &Labels);
    State* StateToModify = nullptr;
    while (true)
    {
        OpCodes code = *(OpCodes*)Addr;
        switch(code)
        {
        case OpCodes::SetCel:
            {
                if (CelExecuted)
                    return;
                int32_t AnimTime = *(int32_t*)(Addr + 68);
                if (Actor->AnimTime > AnimTime)
                {
                    Addr += InstructionSizes[code];
                    if (FindNextCel(Addr))
                        break;
                    return;
                }
                Actor->SetCelName(Addr + 4);
                CelExecuted = true;
                break;
            }
        case OpCodes::CallSubroutine:
            {
                Actor->Player->CallSubroutine(Addr + 4);
                break;
            }
        case OpCodes::ExitState:
        case OpCodes::EndBlock:
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
        case OpCodes::GotoLabel:
            {
                CString<64> LabelName;
                LabelName.SetString(Addr + 4);
                for (StateAddress Label : Labels)
                {
                    if (!strcmp(Label.Name.GetString(), LabelName.GetString()))
                    {   
                        Addr = (char*)Label.OffsetAddress;
                        if (FindNextCel(Addr))
                        {
                            Actor->AnimTime = *(int32_t*)(Addr + 68) - 1;
                        }
                        return;
                    }
                }
                break;
            }
        case OpCodes::EndLabel:
            {
                int32_t AnimTime = *(int32_t*)(Addr + 4);
                if (Actor->AnimTime < AnimTime)
                    return;
                break;
            }
        case OpCodes::BeginStateDefine:
            {
                CString<64> StateName;
                StateName.SetString(Addr + 4);
                int32_t Index = Actor->Player->StateMachine.GetStateIndex(StateName);
                StateToModify = Actor->Player->StateMachine.States[Index];
                break;
            }
        case OpCodes::EndStateDefine:
            StateToModify = nullptr;
            break;
        case OpCodes::SetStateType:
            if (StateToModify)
            {
                StateToModify->Type = *(StateType*)(Addr + 4);
            }
            break;
        case OpCodes::SetEntryState:
            if (StateToModify)
            {
                StateToModify->StateEntryState = *(EntryState*)(Addr + 4);
            }
            break;
        case OpCodes::AddInputCondition:
            if (StateToModify)
            {
                StateToModify->InputConditions.push_back(*(InputCondition*)(Addr + 4));
            }
            break;
        case OpCodes::AddStateCondition:
            if (StateToModify)
            {
                StateToModify->StateConditions.push_back(*(StateCondition*)(Addr + 4));
            }
            break;
        case OpCodes::IsFollowupMove:
            if (StateToModify)
            {
                StateToModify->IsFollowupState = *(bool*)(Addr + 4);
            }
            break;
        case OpCodes::SetStateObjectID:
            if (StateToModify)
            {
                StateToModify->ObjectID = *(int32_t*)(Addr + 4);
            }
            break;
        default:
            break;
        }
        Addr += InstructionSizes[code];
    }
}

bool ScriptAnalyzer::FindNextCel(char* Addr)
{
    while (true)
    {
        OpCodes code = *(OpCodes*)Addr;
        switch(code)
        {
        case OpCodes::SetCel:
            return true;
        case OpCodes::ExitState:
        case OpCodes::EndBlock:
            return false;
        default:
            break;
        }
        Addr += InstructionSizes[code];
    }
}

void ScriptAnalyzer::GetAllLabels(char* Addr, std::vector<StateAddress>* Labels)
{
    while (true)
    {
        OpCodes code = *(OpCodes*)Addr;
        switch(code)
        {
        case OpCodes::BeginLabel:
            {
                CString<64> LabelName;
                LabelName.SetString(Addr + 4);
                StateAddress Label;
                Label.Name = LabelName;
                Label.OffsetAddress = (uint64_t)Addr;
                Labels->push_back(Label);
                break;
            }
        case OpCodes::ExitState:
        case OpCodes::EndBlock:
            return;
        default:
            break;
        }
        Addr += InstructionSizes[code];
    }
}