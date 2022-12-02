#pragma once

#include <cstdint>

#include "../CString.h"

class BattleActor;

class Subroutine
{
public:
	BattleActor* Parent;
	CString<64> Name;

	virtual void OnCall() = 0; //executes on call. write in script
};

class ScriptSubroutine : public Subroutine
{
public:
	uint32_t OffsetAddress;
	bool CommonSubroutine = false;

	virtual void OnCall() override; //executes on call. write in script
};
