#include "Xbox.h"

XboxController::XboxController(int player) {
	//This will set the controller number
	_controllerNum = player;
}

XINPUT_STATE XboxController::GetState() {
	//Zero the controller memory state
	ZeroMemory(&_controllerState, sizeof(XINPUT_STATE));

	//Get the state of input of the controller
	XInputGetState(_controllerNum, &_controllerState);

	return _controllerState;
}

bool XboxController::isConnected()
{
	//Zero the controller memory state
	ZeroMemory(&_controllerState, sizeof(XINPUT_STATE));

	// Get the state
	DWORD Result = XInputGetState(_controllerNum, &_controllerState);

	return Result == ERROR_SUCCESS;
}

void XboxController::Vibrate(int left, int right)
{
	// Create a Vibraton State
	XINPUT_VIBRATION Vibration;

	// Zeroise the Vibration
	ZeroMemory(&Vibration, sizeof(XINPUT_VIBRATION));

	// Set the Vibration Values
	Vibration.wLeftMotorSpeed = left;
	Vibration.wRightMotorSpeed = right;

	// Vibrate the controller
	XInputSetState(_controllerNum, &Vibration);
}