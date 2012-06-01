#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Xinput.h>
#pragma comment(lib, "XInput.lib")

class XboxController {
private:
	XINPUT_STATE _controllerState;
	int _controllerNum;
public:

	virtual ~XboxController (void){
		if(isConnected())
			Vibrate(0,0);
	}

	XboxController(int player);
	XINPUT_STATE GetState();
	bool isConnected();
	void Vibrate(int left=0, int right=0);
};