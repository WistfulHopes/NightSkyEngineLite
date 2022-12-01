#pragma once

#include "State.h"
#include <stdint.h>

#pragma pack (push, 1)

/**
 * 
 */
struct InputBuffer
{
private:
	int32_t InputSequence[20] = { -1 };
	int32_t Lenience = 8; //how much time is allowed between inputs
	int32_t ImpreciseInputCount = 0; //how much time is allowed between inputs
	bool bInputAllowDisable = true;
public:
	int32_t InputBufferInternal[90] = { 16 };
	int32_t InputDisabled[90] = { 0 };
	
	void Tick(int32_t Input);
	bool CheckInputCondition(InputCondition InputCondition);
	bool CheckInputSequence();
	bool CheckInputSequenceStrict(); //directional inputs must be exact
	bool CheckInputSequenceOnce();
	bool CheckInputSequenceOnceStrict();
	void ResetInputSequence();
	void FlipInputsInBuffer();
};
#pragma pack(pop)

