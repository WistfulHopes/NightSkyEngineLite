#pragma once

#include <stdint.h>
#include "../CString.h"
#include <vector>

class State;
class Subroutine;
class BattleActor;

constexpr int InstructionSizes[] = {
    72,
    68,
    100,
    4,
    0,
    0,
    0,
    0,
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
    4,
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
};

enum OpCodes
{
    SetCel = 0,
    CallSubroutine = 1,
    CallSubroutineWithArgs = 2,
    ExitState = 3,
    BlockEnd = 10,
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
};

struct StateAddress
{
    uint32_t OffsetAddress;
    CString<64> Name;
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
    void Initialize(char* Addr, uint32_t Size, std::vector<State*> States, std::vector<Subroutine*> Subroutines);
    void InitStateOffsets(char* Addr, uint32_t Size, ScriptState* State);
    void Analyze(char* Addr, BattleActor* Actor);
    bool FindNextCel(char* Addr);
    void GetAllLabels(char* Addr, std::vector<StateAddress>* Labels);
};