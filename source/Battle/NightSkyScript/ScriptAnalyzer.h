#pragma once

#include <stdint.h>
#include "../../CString.h"
#include <vector>

class ScriptState;
class State;
class Subroutine;
class BattleActor;

constexpr int InstructionSizes[] = {
    68,
    4,
    72,
    68,
    4,
    68,
    100,
    4,
    0,
    0,
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    68,
    8,
    68,
    12,
    4,
    24,
    12,
    4,
    4,
    76,
    88,
    76,
    4,
    0,
    0,
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    4,
    0,
    0,
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    4,
    0,
    0,
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    4,
    0,
    0,
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    4,
    0,
    0,
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    4,
    0,
    0,
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    4,
    0,
    0,
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    4,
    0,
    0,
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    4,
    0,
    0,
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    4,
    0,
    0,
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    4,
    0,
    0,
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    4,
    0,
    0,
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    4,
    0,
    0,
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    4,
    0,
    0,
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    4,
    0,
    0,
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    4,
    0,
    0,
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    4,
    0,
    0,
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    4,
    0,
    0,
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    4,
    0,
    0,
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    4,
    0,
    0,
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    4,
    0,
    0,
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    4,
    0,
    0,
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4, 
    4,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    68,
    4,
    8,
    8,
    8,
    8,
    8,
    8,
};

enum OpCodes
{
    BeginState = 0,
    EndState = 1,
    SetCel = 2,
    BeginSubroutine = 3,
    EndSubroutine = 4,
    CallSubroutine = 5,
    CallSubroutineWithArgs = 6,
    ExitState = 7,
    EndBlock = 10,
    OnEnter = 11,
    OnUpdate = 12,
    OnExit = 13,
    OnLanding = 14,
    OnHit = 15,
    OnBlock = 16,
    OnHitOrBlock = 17,
    OnCounterHit = 18,
    OnSuperFreeze = 19,
    OnSuperFreezeEnd = 20,
    BeginLabel = 50,
    EndLabel = 51,
    GotoLabel = 52,
    If = 53,
    EndIf = 54,
    IfOperation = 55,
    IfNot = 56,
    Else = 57,
    EndElse = 58,
    GotoLabelIf = 59,
    GotoLabelIfOperation = 60,
    GotoLabelIfNot = 61,
    BeginStateDefine = 1000,
    EndStateDefine = 1001,
    SetStateType = 1002,
    SetEntryState = 1003,
    AddInputCondition = 1004,
    AddStateCondition = 1005,
    IsFollowupMove = 1006,
    SetStateObjectID = 1007,
};

struct StateAddress
{
    CString<64> Name;
    uint32_t OffsetAddress;
};

class ScriptAnalyzer
{
private:
    char* DataAddress;
    char* ScriptAddress;
    int32_t StateCount;
    StateAddress* StateAddresses;
    int32_t SubroutineCount;
    StateAddress* SubroutineAddresses;

public:
    void Initialize(char* Addr, uint32_t Size, std::vector<State*>* States, std::vector<Subroutine*>* Subroutines);
    void InitStateOffsets(char* Addr, uint32_t Size, ScriptState* State);
    void Analyze(char* Addr, BattleActor* Actor);
    bool FindNextCel(char* Addr);
    void GetAllLabels(char* Addr, std::vector<StateAddress>* Labels);
};