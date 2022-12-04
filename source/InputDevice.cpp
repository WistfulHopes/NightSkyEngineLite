#include "InputDevice.h"

#include <iostream>

#include "../raylib/src/raylib.h"
#include "Battle/Bitflags.h"
int ControllerInputDevice::GetInputs() {
	int inputFlags = 0;

	if (!IsGamepadAvailable(this->controller_id)) {
		// Gamepad is gone! :crab:
		// TODO: Maybe have some error code/logging for when this happens?
		return 0;
	}

	// TODO: find a better way to do this you neanderthal (kika is the neanderthal)
	if(this->IsButtonDown(this->config.InputUp)) inputFlags |= InputUp;
	if(this->IsButtonDown(this->config.InputDown)) inputFlags |= InputDown;
	if(this->IsButtonDown(this->config.InputLeft)) inputFlags |= InputLeft;
	if(this->IsButtonDown(this->config.InputRight)) inputFlags |= InputRight;
	
	if(this->IsButtonDown(this->config.InputUpAnalog)) inputFlags |= InputUp;
	if(this->IsButtonDown(this->config.InputDownAnalog)) inputFlags |= InputDown;
	if(this->IsButtonDown(this->config.InputLeftAnalog)) inputFlags |= InputLeft;
	if(this->IsButtonDown(this->config.InputRightAnalog)) inputFlags |= InputRight;

	if(this->IsButtonDown(this->config.InputL)) inputFlags |= InputL;
	if(this->IsButtonDown(this->config.InputM)) inputFlags |= InputM;
	if(this->IsButtonDown(this->config.InputH)) inputFlags |= InputH;
	if(this->IsButtonDown(this->config.InputS)) inputFlags |= InputS;
	if(this->IsButtonDown(this->config.InputA1)) inputFlags |= InputA1;
	if(this->IsButtonDown(this->config.InputA2)) inputFlags |= InputA2;
	
	return inputFlags;
}

bool ControllerInputDevice::IsButtonDown(ButtonConfig button) {
	if (button.axis) {
		float Axis = GetGamepadAxisMovement(this->controller_id, button.id);
		if (button.positive && Axis > config.deadzone)
		{
			return true;
		}
		if (!button.positive && Axis < -config.deadzone)
		{
			return true;
		}

		return false;
	}
	return IsGamepadButtonDown(this->controller_id, button.id);
}

ControllerInputDevice::ControllerInputDevice()
{
	config.InputUp.id = GAMEPAD_BUTTON_LEFT_FACE_UP;
	config.InputDown.id = GAMEPAD_BUTTON_LEFT_FACE_DOWN;
	config.InputLeft.id = GAMEPAD_BUTTON_LEFT_FACE_LEFT;
	config.InputRight.id = GAMEPAD_BUTTON_LEFT_FACE_RIGHT;
	config.InputUpAnalog.id = GAMEPAD_AXIS_LEFT_Y;
	config.InputUpAnalog.axis = true;
	config.InputDownAnalog.id = GAMEPAD_AXIS_LEFT_Y;
	config.InputDownAnalog.axis = true;
	config.InputDownAnalog.positive = true;
	config.InputLeftAnalog.id = GAMEPAD_AXIS_LEFT_X;
	config.InputLeftAnalog.axis = true;
	config.InputRightAnalog.id = GAMEPAD_AXIS_LEFT_X;
	config.InputRightAnalog.axis = true;
	config.InputRightAnalog.positive = true;
	config.InputL.id = GAMEPAD_BUTTON_RIGHT_FACE_LEFT;
	config.InputM.id = GAMEPAD_BUTTON_RIGHT_FACE_UP;
	config.InputH.id = GAMEPAD_BUTTON_RIGHT_FACE_RIGHT;
	config.InputS.id = GAMEPAD_BUTTON_RIGHT_FACE_DOWN;
}

KeyboardInputDevice::KeyboardInputDevice()
{
	config.InputUp = KEY_W;
	config.InputDown = KEY_S;
	config.InputLeft = KEY_A;
	config.InputRight = KEY_D;
	config.InputL = KEY_H;
	config.InputM = KEY_J;
	config.InputH = KEY_K;
	config.InputS = KEY_L;
}

int KeyboardInputDevice::GetInputs() {
	int inputFlags = 0;

	// TODO: find a better way to do this you neanderthal (kika is the neanderthal)
	if(IsKeyDown(this->config.InputUp)) inputFlags |= InputUp;
	if(IsKeyDown(this->config.InputDown)) inputFlags |= InputDown;
	if(IsKeyDown(this->config.InputLeft)) inputFlags |= InputLeft;
	if(IsKeyDown(this->config.InputRight)) inputFlags |= InputRight;

	if(IsKeyDown(this->config.InputL)) inputFlags |= InputL;
	if(IsKeyDown(this->config.InputM)) inputFlags |= InputM;
	if(IsKeyDown(this->config.InputH)) inputFlags |= InputH;
	if(IsKeyDown(this->config.InputS)) inputFlags |= InputS;
	if(IsKeyDown(this->config.InputA1)) inputFlags |= InputA1;
	if(IsKeyDown(this->config.InputA2)) inputFlags |= InputA2;

	return inputFlags;
}
