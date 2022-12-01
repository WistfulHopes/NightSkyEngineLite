#pragma once

#include "State.h"
#include "Bitflags.h"

class PlayerCharacter; //forward declaration

/**
 * 
 */
struct StateMachine
{
	State* CurrentState;
	std::vector<State*> States;
	std::vector<CString<64>> StateNames;

	PlayerCharacter* Parent;

	void AddState(const CString<64> Name, State* Config)
	{
		Config->Parent = Parent;
		States.push_back(Config);
		StateNames.push_back(Name);
		if (CurrentState == nullptr)
		{
			CurrentState = Config;
			CurrentState->OnEnter();
		}
	}

	bool IsCurrentState(CString<64> Name)
	{
		return !(strcmp(CurrentState->Name.GetString(), Name.GetString()));
	}

	CString<64> GetStateName(int Index);
	int GetStateIndex(CString<64> Name);
	
	bool SetState(CString<64> Name);
	bool ForceSetState(CString<64> Name);
	bool ForceRollbackState(CString<64> Name);
		
	static bool CheckStateEntryCondition(const EntryState EntryState, const int ActionFlags);
	
	void Tick(const float DeltaTime)
	{
		if (CurrentState != nullptr)
		{
			CurrentState->OnUpdate(DeltaTime);
		}
	}
};
