class InputDevice {
	public:
		int GetInputs();
}

struct ButtonConfig {
	int id;
	bool axis;
}

struct ControllerConfig {
	ButtonConfig InputUp;
	ButtonConfig InputDown;
	ButtonConfig InputLeft;
	ButtonConfig InputRight;

	ButtonConfig InputL;
	ButtonConfig InputM;
	ButtonConfig InputH;
	ButtonConfig InputS;
	ButtonConfig InputA1;
	ButtonConfig InputA2;

	float deadzone;
}

class ControllerInputDevice : public InputDevice {
	private:
		int controller_id;
		ControllerConfig config;

		bool IsButtonDown(ButtonConfig button);

	public:
		int GetInputs();
}

struct KeyboardConfig {
	int InputUp;
	int InputDown;
	int InputLeft;
	int InputRight;

	int InputL;
	int InputM;
	int InputH;
	int InputS;
	int InputA1;
	int InputA2;
}

class KeyboardInputDevice : public InputDevice {
	private:
		KeyboardConfig config;

	public:
		int GetInputs();
}