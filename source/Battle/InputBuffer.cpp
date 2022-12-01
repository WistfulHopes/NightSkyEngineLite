#include "InputBuffer.h"
#include "Bitflags.h"

void InputBuffer::Tick(int32_t Input)
{
	for (int32_t i = 0; i < 89; i++)
	{
		InputBufferInternal[i] = InputBufferInternal[i + 1];
		InputDisabled[i] = InputDisabled[i + 1];
	}
	InputBufferInternal[89] = Input;
	InputDisabled[89] = 0;
}

bool InputBuffer::CheckInputCondition(const InputCondition InputCondition)
{
	Lenience = 8;
	ImpreciseInputCount = 0;
	bInputAllowDisable = true;
	switch (InputCondition)
	{
	case InputCondition::None:
		if (InputBufferInternal[89] & InputNeutral)
		{
			return true;
		}
		break;
	case InputCondition::Input_1:
		ResetInputSequence();
		InputSequence[0] = InputDownLeft;
		return CheckInputSequenceStrict();
	case InputCondition::Input_1_Press:
		ResetInputSequence();
		InputSequence[0] = InputDownLeft;
		return CheckInputSequenceOnceStrict();
	case InputCondition::Input_1_Hold:
		if (InputBufferInternal[89] & InputDown && InputBufferInternal[89] & InputLeft
			&& InputBufferInternal[88] & InputDown && InputBufferInternal[88] & InputLeft)
		{
			return true;
		}
		break;
	case InputCondition::Input_1_Release:
		if (!(InputBufferInternal[89] & InputDown && InputBufferInternal[89] & InputLeft)
			&& InputBufferInternal[88] & InputDown && InputBufferInternal[88] & InputLeft)
		{
			return true;
		}
		break;
	case InputCondition::Input_2:
		ResetInputSequence();
		InputSequence[0] = InputDown;
		return CheckInputSequenceStrict();
	case InputCondition::Input_2_Press:
		ResetInputSequence();
		InputSequence[0] = InputDown;
		return CheckInputSequenceOnceStrict();
	case InputCondition::Input_2_Hold:
		if (InputBufferInternal[89] & InputDown && !(InputBufferInternal[89] & InputLeft)
			&& !(InputBufferInternal[89] & InputRight) && InputBufferInternal[88] & InputDown
			&& !(InputBufferInternal[89] & InputUp))
		{
			return true;
		}
		break;
	case InputCondition::Input_2_Release:
		if (!(InputBufferInternal[89] & InputDown) && InputBufferInternal[86] & InputDown)
		{
			return true;
		}
		break;
	case InputCondition::Input_3:
		ResetInputSequence();
		InputSequence[0] = InputDownRight;
		return CheckInputSequenceStrict();
	case InputCondition::Input_3_Press:
		ResetInputSequence();
		InputSequence[0] = InputDownRight;
		return CheckInputSequenceOnceStrict();
	case InputCondition::Input_3_Hold:
		if (InputBufferInternal[89] & InputDown && InputBufferInternal[89] & InputRight
			&& InputBufferInternal[88] & InputDown && InputBufferInternal[88] & InputRight)
		{
			return true;
		}
		break;
	case InputCondition::Input_3_Release:
		if (!(InputBufferInternal[89] & InputDown && InputBufferInternal[89] & InputRight)
			&& InputBufferInternal[88] & InputDown && InputBufferInternal[88] & InputRight)
		{
			return true;
		}
		break;
	case InputCondition::Input_4:
		ResetInputSequence();
		InputSequence[0] = InputLeft;
		return CheckInputSequenceStrict();
	case InputCondition::Input_4_Press:
		ResetInputSequence();
		InputSequence[0] = InputLeft;
		return CheckInputSequenceOnceStrict();
	case InputCondition::Input_4_Hold:
		if (InputBufferInternal[89] & InputLeft && !(InputBufferInternal[89] & InputUp)
			&& !(InputBufferInternal[89] & InputDown) && InputBufferInternal[88] & InputLeft)
		{
			return true;
		}
		break;
	case InputCondition::Input_4_Release:
		if (!(InputBufferInternal[89] & InputLeft) && InputBufferInternal[88] & InputLeft)
		{
			return true;
		}
		break;
	case InputCondition::Input_6:
		ResetInputSequence();
		InputSequence[0] = InputRight;
		return CheckInputSequenceStrict();
	case InputCondition::Input_6_Press:
		ResetInputSequence();
		InputSequence[0] = InputRight;
		return CheckInputSequenceOnceStrict();
	case InputCondition::Input_6_Hold:
		if (InputBufferInternal[89] & InputRight && !(InputBufferInternal[89] & InputUp)
			&& !(InputBufferInternal[89] & InputDown) && InputBufferInternal[88] & InputRight)
		{
			return true;
		}
		break;
	case InputCondition::Input_6_Release:
		if (!(InputBufferInternal[89] & InputRight) && InputBufferInternal[88] & InputRight)
		{
			return true;
		}
		break;
	case InputCondition::Input_7:
		ResetInputSequence();
		InputSequence[0] = InputUpLeft;
		return CheckInputSequenceStrict();
	case InputCondition::Input_7_Press:
		ResetInputSequence();
		InputSequence[0] = InputUpLeft;
		return CheckInputSequenceOnceStrict();
	case InputCondition::Input_7_Hold:
		if (InputBufferInternal[89] & InputUp && InputBufferInternal[89] & InputLeft
			&& InputBufferInternal[88] & InputUp && InputBufferInternal[88] & InputLeft)
		{
			return true;
		}
		break;
	case InputCondition::Input_7_Release:
		if (!(InputBufferInternal[89] & InputUp && InputBufferInternal[89] & InputLeft)
			&& InputBufferInternal[88] & InputUp && InputBufferInternal[88] & InputLeft)
		{
			return true;
		}
		break;
	case InputCondition::Input_8:
		ResetInputSequence();
		InputSequence[0] = InputUp;
		return CheckInputSequenceStrict();
	case InputCondition::Input_8_Press:
		ResetInputSequence();
		InputSequence[0] = InputUp;
		return CheckInputSequenceOnceStrict();
	case InputCondition::Input_8_Hold:
		if (InputBufferInternal[89] & InputUp && !(InputBufferInternal[89] & InputLeft)
			&& !(InputBufferInternal[89] & InputRight) && InputBufferInternal[88] & InputUp)
		{
			return true;
		}
		break;
	case InputCondition::Input_8_Release:
		if (!(InputBufferInternal[89] & InputUp) && InputBufferInternal[88] & InputUp)
		{
			return true;
		}
		break;
	case InputCondition::Input_9:
		ResetInputSequence();
		InputSequence[0] = InputUpRight;
		return CheckInputSequenceStrict();
	case InputCondition::Input_9_Press:
		ResetInputSequence();
		InputSequence[0] = InputUpRight;
		return CheckInputSequenceOnceStrict();
	case InputCondition::Input_9_Hold:
		if (InputBufferInternal[89] & InputUp && InputBufferInternal[89] & InputRight
			&& InputBufferInternal[88] & InputUp && InputBufferInternal[88] & InputRight)
		{
			return true;
		}
		break;
	case InputCondition::Input_9_Release:
		if (!(InputBufferInternal[89] & InputUp && InputBufferInternal[89] & InputRight)
			&& InputBufferInternal[88] & InputUp && InputBufferInternal[88] & InputRight)
		{
			return true;
		}
		break;
	case InputCondition::Input_Down:
		ResetInputSequence();
		InputSequence[0] = InputDown;
		return CheckInputSequence();
	case InputCondition::Input_Down_Press:
		ResetInputSequence();
		InputSequence[0] = InputDown;
		return CheckInputSequenceOnce();
	case InputCondition::Input_Down_Hold:
		if (InputBufferInternal[89] & InputDown && InputBufferInternal[88] & InputDown)
		{
			return true;
		}
		break;
	case InputCondition::Input_Down_Release:
		if (!(InputBufferInternal[89] & InputDown) && InputBufferInternal[88] & InputDown)
		{
			return true;
		}
		break;
	case InputCondition::Input_Left:
		ResetInputSequence();
		InputSequence[0] = InputLeft;
		return CheckInputSequence();
	case InputCondition::Input_Left_Press:
		ResetInputSequence();
		InputSequence[0] = InputLeft;
		return CheckInputSequenceOnce();
	case InputCondition::Input_Left_Hold:
		if (InputBufferInternal[89] & InputLeft && InputBufferInternal[88] & InputLeft)
		{
			return true;
		}
		break;
	case InputCondition::Input_Left_Release:
		if (!(InputBufferInternal[89] & InputLeft) && InputBufferInternal[88] & InputLeft)
		{
			return true;
		}
		break;
	case InputCondition::Input_Right:
		ResetInputSequence();
		InputSequence[0] = InputRight;
		return CheckInputSequence();
	case InputCondition::Input_Right_Press:
		ResetInputSequence();
		InputSequence[0] = InputRight;
		return CheckInputSequenceOnce();
	case InputCondition::Input_Right_Hold:
		if (InputBufferInternal[89] & InputRight && InputBufferInternal[88] & InputRight)
		{
			return true;
		}
		break;
	case InputCondition::Input_Right_Release:
		if (!(InputBufferInternal[89] & InputRight) && InputBufferInternal[88] & InputRight)
		{
			return true;
		}
		break;
	case InputCondition::Input_Up:
		ResetInputSequence();
		InputSequence[0] = InputUp;
		return CheckInputSequence();
	case InputCondition::Input_Up_Press:
		ResetInputSequence();
		InputSequence[0] = InputUp;
		return CheckInputSequenceOnce();
	case InputCondition::Input_Up_Hold:
		if (InputBufferInternal[89] & InputUp && InputBufferInternal[88] & InputUp)
		{
			return true;
		}
		break;
	case InputCondition::Input_Up_Release:
		if (!(InputBufferInternal[89] & InputUp) && InputBufferInternal[88] & InputUp)
		{
			return true;
		}
		break;
	case InputCondition::Input_SuperJump:
		ResetInputSequence();
		InputSequence[0] = InputDown;
		InputSequence[1] = InputUp;
		return CheckInputSequence();
	case InputCondition::Input_SuperJump_Back:
		ResetInputSequence();
		InputSequence[0] = InputDown;
		InputSequence[1] = InputUpLeft;
		return CheckInputSequence();
	case InputCondition::Input_SuperJump_Forward:
		ResetInputSequence();
		InputSequence[0] = InputDown;
		InputSequence[1] = InputUpRight;
		return CheckInputSequence();
	case InputCondition::Input_DownDown:
		ResetInputSequence();
		Lenience = 6;
		InputSequence[0] = InputNeutral;
		InputSequence[1] = InputDown;
		InputSequence[2] = InputNeutral;
		InputSequence[3] = InputDown;
		return CheckInputSequence();
	case InputCondition::Input_44:
		ResetInputSequence();
		ImpreciseInputCount = 1;
		Lenience = 6;
		InputSequence[0] = InputNeutral;
		InputSequence[1] = InputLeft;
		InputSequence[2] = InputNeutral;
		InputSequence[3] = InputLeft;
		return CheckInputSequenceStrict();
	case InputCondition::Input_66:
		ResetInputSequence();
		ImpreciseInputCount = 1;
		Lenience = 6;
		InputSequence[0] = InputNeutral;
		InputSequence[1] = InputRight;
		InputSequence[2] = InputNeutral;
		InputSequence[3] = InputRight;
		return CheckInputSequenceStrict();
	case InputCondition::Input_236:
		ResetInputSequence();
		bInputAllowDisable = false;
		InputSequence[0] = InputDown;
		InputSequence[1] = InputRight;
		return CheckInputSequenceStrict();
	case InputCondition::Input_214:
		ResetInputSequence();
		bInputAllowDisable = false;
		InputSequence[0] = InputDown;
		InputSequence[1] = InputLeft;
		return CheckInputSequenceStrict();
	case InputCondition::Input_623:
		ResetInputSequence();
		bInputAllowDisable = false;
		InputSequence[0] = InputRight;
		InputSequence[1] = InputDown;
		InputSequence[2] = InputDownRight;
		return CheckInputSequenceStrict();
	case InputCondition::Input_421:
		ResetInputSequence();
		bInputAllowDisable = false;
		InputSequence[0] = InputLeft;
		InputSequence[1] = InputDown;
		InputSequence[2] = InputDownLeft;
		return CheckInputSequenceStrict();
	case InputCondition::Input_41236:
		ResetInputSequence();
		bInputAllowDisable = false;
		ImpreciseInputCount = 1;
		InputSequence[0] = InputLeft;
		InputSequence[1] = InputDown;
		InputSequence[2] = InputRight;
		return CheckInputSequenceStrict();
	case InputCondition::Input_63214:
		ResetInputSequence();
		bInputAllowDisable = false;
		ImpreciseInputCount = 1;
		InputSequence[0] = InputRight;
		InputSequence[1] = InputDown;
		InputSequence[2] = InputLeft;
		return CheckInputSequenceStrict();
	case InputCondition::Input_236236:
		ResetInputSequence();
		bInputAllowDisable = false;
		ImpreciseInputCount = 1;
		InputSequence[0] = InputDown;
		InputSequence[1] = InputRight;
		InputSequence[2] = InputDown;
		InputSequence[3] = InputRight;
		return CheckInputSequenceStrict();
	case InputCondition::Input_214214:
		ResetInputSequence();
		bInputAllowDisable = false;
		ImpreciseInputCount = 1;
		InputSequence[0] = InputDown;
		InputSequence[1] = InputLeft;
		InputSequence[2] = InputDown;
		InputSequence[3] = InputLeft;
		return CheckInputSequenceStrict();
	case InputCondition::Input_L:
		ResetInputSequence();
		InputSequence[0] = InputL;
		return CheckInputSequence();
	case InputCondition::Input_L_Press:
		ResetInputSequence();
		InputSequence[0] = InputL;
		return CheckInputSequenceOnce();
	case InputCondition::Input_L_Hold:
		if (InputBufferInternal[89] & InputL && InputBufferInternal[88] & InputL)
		{
			return true;
		}
		break;
	case InputCondition::Input_L_Release:
		if (!(InputBufferInternal[89] & InputL) && InputBufferInternal[88] & InputL)
		{
			return true;
		}
		break;
	case InputCondition::Input_M:
		ResetInputSequence();
		InputSequence[0] = InputM;
		return CheckInputSequence();
	case InputCondition::Input_M_Press:
		ResetInputSequence();
		InputSequence[0] = InputM;
		return CheckInputSequenceOnce();
	case InputCondition::Input_M_Hold:
		if (InputBufferInternal[89] & InputM && InputBufferInternal[88] & InputM)
		{
			return true;
		}
		break;
	case InputCondition::Input_M_Release:
		if (!(InputBufferInternal[89] & InputM) && InputBufferInternal[88] & InputM)
		{
			return true;
		}
		break;
	case InputCondition::Input_H:
		ResetInputSequence();
		InputSequence[0] = InputH;
		return CheckInputSequence();
	case InputCondition::Input_H_Press:
		ResetInputSequence();
		InputSequence[0] = InputH;
		return CheckInputSequenceOnce();
	case InputCondition::Input_H_Hold:
		if (InputBufferInternal[89] & InputH && InputBufferInternal[88] & InputH)
		{
			return true;
		}
		break;
	case InputCondition::Input_H_Release:
		if (!(InputBufferInternal[89] & InputH) && InputBufferInternal[88] & InputH)
		{
			return true;
		}
		break;
	case InputCondition::Input_S:
		ResetInputSequence();
		InputSequence[0] = InputS;
		return CheckInputSequence();
	case InputCondition::Input_S_Press:
		ResetInputSequence();
		InputSequence[0] = InputS;
		return CheckInputSequenceOnce();
	case InputCondition::Input_S_Hold:
		if (InputBufferInternal[89] & InputS && InputBufferInternal[88] & InputS)
		{
			return true;
		}
		break;
	case InputCondition::Input_S_Release:
		if (!(InputBufferInternal[89] & InputS) && InputBufferInternal[88] & InputS)
		{
			return true;
		}
	case InputCondition::Input_L_And_S:
		ResetInputSequence();
		InputSequence[0] = InputL;
		if (CheckInputSequenceOnce())
		{
			ResetInputSequence();
			InputSequence[0] = InputS;
			if (CheckInputSequenceOnce())
				return true;
		}
		else
		{
			ResetInputSequence();
			InputSequence[0] = InputS;
			if (CheckInputSequenceOnce())
			{
				ResetInputSequence();
				InputSequence[0] = InputL;
				if (CheckInputSequenceOnce())
					return true;
			}
		}
		break;
	case InputCondition::Input_Any_LMHS:
		if (CheckInputCondition(InputCondition::Input_L) || CheckInputCondition(InputCondition::Input_M) || CheckInputCondition(InputCondition::Input_H) || CheckInputCondition(InputCondition::Input_S))
			return true;
		break;
	default:
		return false;
	}
	return false;
}

bool InputBuffer::CheckInputSequence()
{
	int32_t InputIndex = -10;
	for (int32_t i = 19; i > -1; i--)
	{
		if (InputSequence[i] != -1)
		{
			InputIndex = i;
			break;
		}
	}
	int32_t FramesSinceLastMatch = 0; //how long it's been since last input match
	bool NoMatches = true;

	for (int32_t i = 89; i >= 0; i--)
	{
		if (InputIndex == -1) //check if input sequence has been fully read
			return true;
		
		if (NoMatches && InputDisabled[i] == InputBufferInternal[i] && bInputAllowDisable)
			return false;
		
		const int32_t NeededInput = InputSequence[InputIndex];
		if (FramesSinceLastMatch > Lenience)
			return false;
		FramesSinceLastMatch++;

		if ((InputBufferInternal[i] & NeededInput) == NeededInput) //if input matches...
		{
			NoMatches = false;
			InputIndex--; //advance sequence
			FramesSinceLastMatch = 0; //reset last match
			i--;
		}
	}

	return false;
}

bool InputBuffer::CheckInputSequenceStrict()
{
	int32_t InputIndex = -10;
	for (int32_t i = 19; i > -1; i--)
	{
		if (InputSequence[i] != -1)
		{
			InputIndex = i;
			break;
		}
	}
	int32_t FramesSinceLastMatch = 0; //how long it's been since last input match
	int32_t ImpreciseMatches = 0;
	bool NoMatches = true;
	
	for (int32_t i = 89; i >= 0; i--)
	{
		if (InputIndex == -1) //check if input sequence has been fully read
			return true;

		if (NoMatches && InputDisabled[i] == InputBufferInternal[i] && bInputAllowDisable)
			return false;
		
		const int32_t NeededInput = InputSequence[InputIndex];
		if (FramesSinceLastMatch > Lenience)
			return false;
		FramesSinceLastMatch++;

		if ((InputBufferInternal[i] ^ NeededInput) << 27 == 0) //if input matches...
		{
			NoMatches = false;
			InputIndex--; //advance sequence
			FramesSinceLastMatch = 0; //reset last match
			i--;
			continue;
		}
		if ((InputBufferInternal[i] & NeededInput) == NeededInput) //if input doesn't match precisely...
		{
			NoMatches = false;
			if (ImpreciseMatches >= ImpreciseInputCount)
				continue;
			ImpreciseMatches++;
			InputIndex--; //advance sequence
			FramesSinceLastMatch = 0; //reset last match
			i--;
		}
	}

	return false;
}

bool InputBuffer::CheckInputSequenceOnce()
{
	int32_t InputIndex = -10;
	for (int32_t i = 19; i > -1; i--)
	{
		if (InputSequence[i] != -1)
		{
			InputIndex = i;
			break;
		}
	}
	int32_t FramesSinceLastMatch = 0; //how long it's been since last input match

	for (int32_t i = 89; i >= 0; i--)
	{
		if (InputDisabled[i] == InputBufferInternal[i] && bInputAllowDisable)
			return false;

		if (InputIndex < 0) //check if input sequence has been fully read
		{
			if (InputIndex <= -Lenience)
				return false;
			if (!(InputBufferInternal[i] & InputSequence[0]))
				return true;
			InputIndex--;
			continue;
		}
		const int32_t NeededInput = InputSequence[InputIndex];

		if (FramesSinceLastMatch > Lenience)
			return false;
		FramesSinceLastMatch++;

		if ((InputBufferInternal[i] & NeededInput) == NeededInput) //if input matches...
		{
			InputIndex--; //advance sequence
			FramesSinceLastMatch = 0; //reset last match
			i--;
		}
	}

	return false;
}

bool InputBuffer::CheckInputSequenceOnceStrict()
{
	int32_t InputIndex = -10;
	for (int32_t i = 19; i > -1; i--)
	{
		if (InputSequence[i] != -1)
		{
			InputIndex = i;
			break;
		}
	}
	int32_t FramesSinceLastMatch = 0; //how long it's been since last input match
	int32_t ImpreciseMatches = 0;

	for (int32_t i = 89; i >= 0; i--)
	{
		if (InputDisabled[i] == InputBufferInternal[i] && bInputAllowDisable)
			return false;

		if (InputIndex < 0) //check if input sequence has been fully read
		{
			if (InputIndex <= -Lenience)
				return false;
			if (!(InputBufferInternal[i] & InputSequence[0]))
				return true;
			InputIndex--;
			continue;
		}
		const int32_t NeededInput = InputSequence[InputIndex];

		if (FramesSinceLastMatch > Lenience)
			return false;
		FramesSinceLastMatch++;

		if ((InputBufferInternal[i] ^ NeededInput) << 27 == 0) //if input matches...
		{
			InputIndex--; //advance sequence
			FramesSinceLastMatch = 0; //reset last match
			i--;
			continue;
		}
		if ((InputBufferInternal[i] & NeededInput) == NeededInput) //if input matches...
		{
			if (ImpreciseMatches >= ImpreciseInputCount)
				continue;
			ImpreciseMatches++;
			InputIndex--; //advance sequence
			FramesSinceLastMatch = 0; //reset last match
			i--;
		}
	}

	return false;
}

void InputBuffer::ResetInputSequence()
{
	for (int32_t i = 0; i < 20; i++)
	{
		InputSequence[i] = -1;
	}
}

void InputBuffer::FlipInputsInBuffer()
{
	for (int i = 0; i < 90; i++)
	{
		const unsigned int Bit1 = (InputBufferInternal[i] >> 2) & 1;
		const unsigned int Bit2 = (InputBufferInternal[i] >> 3) & 1;
		unsigned int x = (Bit1 ^ Bit2);

		x = x << 2 | x << 3;

		InputBufferInternal[i] = InputBufferInternal[i] ^ x;
	}
}
