#pragma once

#include "../CString.h"

class BattleActor;

class Subroutine
{
public:
	BattleActor* Parent;
	CString<64> Name;

	virtual void OnCall(); //executes on call. write in script
};

class ScriptSubroutine : public Subroutine
{
public:
	uint32_t OffsetAddress;

	virtual void OnCall() override; //executes on call. write in script
};
