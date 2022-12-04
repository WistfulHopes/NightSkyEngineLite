#include "StateMachine.h"

#include <iostream>

#include "Actors/PlayerCharacter.h"

CString<64> StateMachine::GetStateName(int Index)
{
	if (Index > 0 && Index < States.size())
	{
		return States[Index]->Name;
	}
	CString<64> Null = CString<64>();
	return Null;
}

int StateMachine::GetStateIndex(CString<64> Name)
{
	int Index = 0;
	for (CString<64> String : StateNames)
	{
		if (!strcmp(String.GetString(), Name.GetString()))
		{
			return Index;
		}
		Index++;
	}
	return -1;
}

bool StateMachine::SetState(CString<64> Name)
{
	uint32_t Index = 0;
	for (CString<64> String : StateNames)
	{
		if (!strcmp(String.GetString(), Name.GetString()))
		{
			break;
		}
		Index++;
	}
	if (Index == StateNames.size())
		return false;

	if (IsCurrentState(Name))
	{
		CurrentState = States[Index];
		return true;
	}
	
	CurrentState->OnExit();
	Parent->OnStateChange();	

	CurrentState = States[Index];

	CurrentState->OnEnter();

	return true;
}

bool StateMachine::ForceSetState(CString<64> Name)
{
	uint32_t Index = 0;
	for (CString<64> String : StateNames)
	{
		if (!strcmp(String.GetString(), Name.GetString()))
		{
			break;
		}
		Index++;
	}
	if (Index == StateNames.size())
		return false;
	if (strcmp(StateNames[Index].GetString(), Name.GetString()))
	{
		return false;
	}
	
	CurrentState->OnExit();
	Parent->OnStateChange();

	CurrentState = States[Index];

	CurrentState->OnEnter();

	return true;
}

bool StateMachine::ForceRollbackState(CString<64> Name)
{
	int Index = 0;
	for (CString<64> String : StateNames)
	{
		if (!strcmp(String.GetString(), Name.GetString()))
		{
			break;
		}
		Index++;
	}
	if (strcmp(StateNames[Index].GetString(), Name.GetString()))
	{
		return false;
	}
		
	CurrentState = States[Index];

	return true;
}

bool StateMachine::CheckStateEntryCondition(const EntryState EntryState, const int ActionFlags)
{
	if (EntryState == EntryState::Standing && ActionFlags == ACT_Standing
	|| EntryState == EntryState::Standing && ActionFlags == ACT_Crouching
	|| EntryState == EntryState::Crouching && ActionFlags == ACT_Standing
	|| EntryState == EntryState::Crouching && ActionFlags == ACT_Crouching
	|| EntryState == EntryState::Jumping && ActionFlags == ACT_Jumping
	|| EntryState == EntryState::None)
	{
		return true;
	}
	return false;
}
