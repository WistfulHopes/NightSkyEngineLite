#pragma once

#include "../CString.h"

class BattleActor;

/**
 * 
 */
class Subroutine
{
public:
	BattleActor* Parent;
	CString<64> Name;

	void OnCall(); //executes on call. write in bp
};
