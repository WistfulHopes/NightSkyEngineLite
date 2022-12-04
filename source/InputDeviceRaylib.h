#pragma once
#include "InputDevice.h"

struct ButtonConfig {
	int id = 0;
	bool axis = false;
	bool positive = false;
};

struct ControllerConfig {
	ButtonConfig InputUp;
	ButtonConfig InputDown;
	ButtonConfig InputLeft;
	ButtonConfig InputRight;
	ButtonConfig InputUpAnalog;
	ButtonConfig InputDownAnalog;
	ButtonConfig InputLeftAnalog;
	ButtonConfig InputRightAnalog;

	ButtonConfig InputL;
	ButtonConfig InputM;
	ButtonConfig InputH;
	ButtonConfig InputS;
	ButtonConfig InputA1;
	ButtonConfig InputA2;

	float deadzone = 0.2f;
};

class ControllerInputDevice : public InputDevice {
	ControllerConfig config;

	bool IsButtonDown(ButtonConfig button);

public:
	ControllerInputDevice();
	~ControllerInputDevice() override = default;
	int controller_id = 0;

	int GetInputs() override;
};

struct KeyboardConfig {
	int InputUp = 0;
	int InputDown = 0;
	int InputLeft = 0;
	int InputRight = 0;

	int InputL = 0;
	int InputM = 0;
	int InputH = 0;
	int InputS = 0;
	int InputA1 = 0;
	int InputA2 = 0;
};

class KeyboardInputDevice : public InputDevice {
	KeyboardConfig config = {};

public:
	KeyboardInputDevice();
	~KeyboardInputDevice() override = default;
	int GetInputs() override;
};