#include "MyForm.h"
#pragma comment(lib, "Xinput9_1_0.lib")	///link to Xinput library 
#include <cstdlib>
#include <iostream> 
#include <stdio.h>
#include <cmath>
#include <Windows.h>
#include <Xinput.h>	///toolbox for communicationg with Xbox Controller

//global vars
 float Rthrottle = 0;
 float RleftFlap = 0;
 float RrightFlap = 0;
 bool connected = false;
 bool updateSignal = false;

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Threading;


class controller
{
private:
	int cId; /// controller ID : the port that the controller is connected to (0|1|2|3)

public:
	///constructor
	controller() : cycleTime(250) {}///initialize cycleTime (AKA refresh rate) as 250 mS

	XINPUT_STATE state; //raw signals from controller
	/*typedef struct _XINPUT_STATE
	{
	DWORD                               dwPacketNumber; //increments when connection changes
	XINPUT_GAMEPAD                      Gamepad;
	} XINPUT_STATE, *PXINPUT_STATE;
	typedef struct _XINPUT_GAMEPAD
	{
	WORD                                wButtons;
	BYTE                                bLeftTrigger;
	BYTE                                bRightTrigger;
	SHORT                               sThumbLX;
	SHORT                               sThumbLY;
	SHORT                               sThumbRX;
	SHORT                               sThumbRY; 
	} XINPUT_GAMEPAD, *PXINPUT_GAMEPAD; */


	///normalized analog signals on range 0 to 1
	float leftStickX;
	float leftStickY;
	float rightStickX;
	float rightStickY;
	float leftTrigger;
	float rightTrigger;

	//final results
	float throttle;
	float leftFlap;
	float rightFlap;

	WORD wButtons; ///WORD = microsoft version of unsigned short int
	int cycleTime;
	int vibrateTime;

	//Class methods AKA member functions
	int GetPort() {
		//pretty useless. returns cId = (0|1|2|3)
		return cId;
	}
	XINPUT_GAMEPAD *GetState()
	{
			/* gets the current controller state (big structure of type 'XINPUT_GAMEPAD')
	passes the old structure by pointer, completely overwrites it. */
		return &state.Gamepad;
	}
	bool CheckConnection()
		// scans over ports until it encounters a successful connection
	{
		int controllerId = -1;

		for (DWORD i = 0; i < XUSER_MAX_COUNT && controllerId == -1; i++) //DWORD is windows version of unsigned integer
		{
			XINPUT_STATE state;
			ZeroMemory(&state, sizeof(XINPUT_STATE));

			if (XInputGetState(i, &state) == ERROR_SUCCESS)
				controllerId = i;
		}

		cId = controllerId;

		return controllerId != -1;
	}
	bool Refresh()
		// Returns boolean true if there is a solid connection
		// updates the controller state (of type XINPUT_STATE) and updates the controller class variables accordingly
		// normalizes the readings and applies deadzones
	{
		if (cId == -1)
			CheckConnection();

		if (cId != -1) // valid connection on port (0|1|2|3) != -1
		{
			ZeroMemory(&state, sizeof(XINPUT_STATE));
			if (XInputGetState(cId, &state) != ERROR_SUCCESS)
			{
				cId = -1;
				return false;
			}

			//NORMALIZING STICK INPUT////Thumbstick response : short on range(-32768 to 32767)
			//#define XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE  7849
			//#define XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE 8689

			//weird stuff happening -> bigger deadzone
			#define XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE  7849+500
			#define XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE 8689+500

			//left thumbstick
			short leftX = state.Gamepad.sThumbLX; ///range of -32768 to 32767
			short leftY = state.Gamepad.sThumbLY;
			float leftMagn = sqrt(leftY*leftY + leftX*leftX);

			if (leftMagn < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) {
				///inside deadzone
				leftStickX = 0;
				leftStickY = 0;
			}
			else {///outside deadzone
				if (leftX >= 0)
				{
					leftStickX = (leftX / leftMagn)*((leftMagn - XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) / (32767 - XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE));	///positive normalization
				}
				else
				{
					leftStickX = (leftX / leftMagn)*((leftMagn - XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) / (32768 - XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)); ///negative normalization
				}
				if (leftY >= 0)
				{
					leftStickY = (leftY / leftMagn)*((leftMagn - XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) / (32767 - XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE));	///positive normalization
				}
				else
				{
					leftStickY = (leftY / leftMagn)*((leftMagn - XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) / (32768 - XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)); ///negative normalization
				}
			}//end of left stick normalization
					
			//right thumbstick
			short rightX = state.Gamepad.sThumbRX; ///range of -32768 to 32767
			short rightY = state.Gamepad.sThumbRY;
			float rightMagn = sqrt(rightY*rightY + rightX*rightX);

			if (rightMagn < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE/2) {
				///inside deadzone
				rightStickX = 0;
				rightStickY = 0;
			}else {///outside deadzone
				if (rightX >= 0) 
				{
					rightStickX = (rightX / rightMagn)*((rightMagn - XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) / (32767 - XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE));	///positive normalization
				}
				else 
				{
					rightStickX = (rightX / rightMagn)*((rightMagn - XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) / (32768 - XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)); ///negative normalization
				}
				if (rightY >= 0) 
				{
					rightStickY = (rightY / rightMagn)*((rightMagn - XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) / (32767 - XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE));	///positive normalization
				}
				else 
				{ 
					rightStickY = (rightY / rightMagn)*((rightMagn - XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) / (32768 - XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)); ///negative normalization
				} 
			}//end of right stick normalization

			//NORMALIZING TRIGGER INPUT//
			//#define XINPUT_GAMEPAD_TRIGGER_THRESHOLD    30
			//trigger response : byte on range(0 to 255)
			short leftT = state.Gamepad.bLeftTrigger;
			short rightT = state.Gamepad.bRightTrigger;

			if (leftT< XINPUT_GAMEPAD_TRIGGER_THRESHOLD) {
				leftTrigger = 0;
			}
			else {
				leftTrigger = (leftT - XINPUT_GAMEPAD_TRIGGER_THRESHOLD) / (255.0f - XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
			}

			if (rightT < XINPUT_GAMEPAD_TRIGGER_THRESHOLD) {
				rightTrigger = 0;
			}
			else {
				rightTrigger = (rightT - XINPUT_GAMEPAD_TRIGGER_THRESHOLD) / (255.0f - XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
			}

			return true;
		}
		return false;
	}
	bool IsPressed(WORD button)
		//checks if a certain button has been pressed
	{
		return (state.Gamepad.wButtons & button) != 0;
	}
	WORD getButtons()
	{//returns the wButtons value from state
		wButtons = state.Gamepad.wButtons;
		return wButtons;
	}
	void vibrate(char type, int time)
		// select left or right motor (l,L or r,R) and specify a time in ms ie (R,50)
		/*typedef struct _XINPUT_VIBRATION
		{
		WORD                                wLeftMotorSpeed;
		WORD                                wRightMotorSpeed;
		} XINPUT_VIBRATION, *PXINPUT_VIBRATION; */

	{
		if (type == 'L' || type == 'l')
		{//rumble left motor (low frequency)
		 //range 0 to 65,535 for motor speed (0-100%)

			XINPUT_VIBRATION vibeState;
			ZeroMemory(&vibeState, sizeof(XINPUT_VIBRATION));
			vibeState.wLeftMotorSpeed = 15000;
			XInputSetState(cId, &vibeState);

			vibrateTime = time;


		}
		if (type == 'R' || type == 'r')
		{//rumble right motor (high frequency)

			XINPUT_VIBRATION vibeState;
			ZeroMemory(&vibeState, sizeof(XINPUT_VIBRATION));
			vibeState.wRightMotorSpeed = 25000;
			XInputSetState(cId, &vibeState);

			vibrateTime = time;
		}
		if (type == '#')
		{// special case has been called: stop vibrating now
			XINPUT_VIBRATION vibeState;
			ZeroMemory(&vibeState, sizeof(XINPUT_VIBRATION));
			vibeState.wLeftMotorSpeed = 0;
			vibeState.wRightMotorSpeed = 0;
			XInputSetState(cId, &vibeState);
		}

	}
};


void XboxControl() {
	//	pass by reference via pointer. 
	// this allows the XBOX class to be created as a global variable which is constantly updated.

	controller XBOX;
	bool wasConnected = true;
	double PI = 3.14159265;
	float throttle, leftFlap, rightFlap, flapAvg, flapDelta, avgTrim = 0, deltaTrim = 0, throttleTrim = 0;;

	while (1)
	{
		Sleep(XBOX.cycleTime); /// 250 miliseconds by default, faster after 1st successful connection

		if (!XBOX.Refresh()) ///no controller connected
		{
			if (wasConnected)
			{ //enters once when newly disconnected
				wasConnected = false;
				std::cout << "Please connect an Xbox 360 controller." << std::endl;
				XBOX.cycleTime = 250;
				std::printf("refresh rate is %dms pending connection\n", XBOX.cycleTime);
				::connected = false;
			}
		}
		else ///connected & new state has been read
		{
			if (!wasConnected) ///enters once when newly connected.
			{
				wasConnected = true;
				std::cout << "Xbox 360 controller detected on port  " << XBOX.GetPort() << std::endl;
				Sleep(XBOX.cycleTime); ///wait a second so it is readable
				XBOX.cycleTime = 25; //controller was connected: start refreshing faster 
				std::printf("refresh rate is now %dms\n", XBOX.cycleTime);
				::connected = true;
			}


			/*           - BUTTON READING TIME -
			XINPUT_GAMEPAD_DPAD_UP			0x0001 =1
			XINPUT_GAMEPAD_DPAD_DOWN		0x0002 =2
			XINPUT_GAMEPAD_DPAD_LEFT		0x0004 =4
			XINPUT_GAMEPAD_DPAD_RIGHT		0x0008 =8
			XINPUT_GAMEPAD_START			0x0010 =16
			XINPUT_GAMEPAD_BACK				0x0020 =32
			XINPUT_GAMEPAD_LEFT_THUMB		0x0040 =64
			XINPUT_GAMEPAD_RIGHT_THUMB		0x0080 =128
			XINPUT_GAMEPAD_LEFT_SHOULDER	0x0100 =256
			XINPUT_GAMEPAD_RIGHT_SHOULDER	0x0200 =512
			XINPUT_GAMEPAD_A				0x1000 =4096
			XINPUT_GAMEPAD_B				0x2000 =8192
			XINPUT_GAMEPAD_X				0x4000 =16384
			XINPUT_GAMEPAD_Y				0x8000 =32768 */

			switch (XBOX.getButtons()) {
			case 1: ///DPAD UP :more throttle
				if (throttleTrim < 1.0) {
					throttleTrim += 0.0005f * XBOX.cycleTime;
					XBOX.vibrate('L', XBOX.cycleTime);
				}
				break;
			case 2: ///DPAD DOWN :less throttle
				if (throttleTrim > -1.0)
				{
					throttleTrim -= 0.0007f * XBOX.cycleTime;
					XBOX.vibrate('L', XBOX.cycleTime);
				}
				break;
			case 16384: ///X button: trim more left
				if (deltaTrim > -0.5)
				{
					deltaTrim -= 0.00007f * XBOX.cycleTime;
					XBOX.vibrate('R', XBOX.cycleTime);
				}
				break;
			case 8192: ///B button: trim more right
				if (deltaTrim < 0.5)
				{
					deltaTrim += 0.00007f * XBOX.cycleTime;
					XBOX.vibrate('R', XBOX.cycleTime);
				}
				break;
			case 32768: ///Y button: trim up
				if (avgTrim > -0.7)
				{
					avgTrim -= 0.00007f * XBOX.cycleTime;
					XBOX.vibrate('R', XBOX.cycleTime);
				}
				break;
			case 4096: ///A button: trim down
				if (avgTrim < 0.7)
				{
					avgTrim += 0.00007f * XBOX.cycleTime;
					XBOX.vibrate('R', XBOX.cycleTime);
				}
				break;
			case 4: /// LEFT DPAD: reset throttle trim
				throttleTrim = 0;
				XBOX.vibrate('L', 500);
				break;
			case 8: /// RIGHT DPAD: reset aileron trims
				avgTrim = 0;
				deltaTrim = 0;
				XBOX.vibrate('R', 500);
				break;
			case 768: ///both shoulders are held down at once! reset all trim
				throttleTrim = 0;
				avgTrim = 0;
				deltaTrim = 0;
				XBOX.vibrate('L', 500);
				XBOX.vibrate('R', 500);
				break;
			}


			/*  - READ STICK POSITION, FIND THE DESIRED FLAP POSITION -
			(+) upwards deflected flap
			(-) downwards deflected flap						*/

			flapDelta = XBOX.rightStickX;
			flapAvg = -XBOX.rightStickY; //inverted

			leftFlap = flapAvg + avgTrim - flapDelta - deltaTrim;
			rightFlap = flapAvg + avgTrim + flapDelta + deltaTrim;
			throttle = XBOX.leftStickY + throttleTrim;

			///Limit outputs between -1 and 1 (only useful when some trim is in effect)
			if (leftFlap > 1.0) leftFlap = 1.0;
			if (leftFlap < -1.0) leftFlap = -1.0;
			if (rightFlap > 1.0) rightFlap = 1.0;
			if (rightFlap < -1.0) rightFlap = -1.0;
			if (throttle > 1.0) throttle = 1.0;
			if (throttle < -1.0) throttle = -1.0;

			//End of loop//
			XBOX.vibrateTime -= XBOX.cycleTime; /// decrement the vibrate time 
			if (XBOX.vibrateTime < 0)///ran out of vibration time
			{
				XBOX.vibrate('#', 0); ///special sign '#': stop vibration.
			}

			// Update the XBOX class results:
			XBOX.throttle = throttle;
			XBOX.leftFlap = leftFlap;
			XBOX.rightFlap = rightFlap;

			// use global variables to store and pass the results to the UI part of the code
		    ::Rthrottle = XBOX.throttle;
			::RleftFlap = XBOX.leftFlap;
			::RrightFlap = XBOX.rightFlap;
			::updateSignal = true;


			//TO FIX: annoying console text flicker

			//building the text string
			//char debugText1[100], debugText2[200], debugText[400]; //buffers
			//sprintf_s(debugText1, "Thrust %+05.2f, Lflap at %+05.2f, Rflap at %+05.2f \n", throttle, leftFlap, rightFlap);
			//sprintf_s(debugText2, "Right stick X: %+05.2f, right stick Y: %+05.2f, left Trigger %+05.2f, right Trigger %+05.2f, ButtonID %d \n", XBOX.rightStickX, XBOX.rightStickY, XBOX.leftTrigger, XBOX.rightTrigger, XBOX.wButtons);
			//sprintf_s(debugText, "%s%s", debugText1, debugText2);
			//std::printf(debugText); 			// text is printed with a single command but still flickers?
		}
	}
}


[STAThread] // absolutely no idea 
void Main(array<String^>^ args)
{
	//creates a console window!
	AllocConsole();
	std::freopen("conin$", "r", stdin);
	std::freopen("conout$", "w", stdout);
	std::freopen("conout$", "w", stderr);
	std::printf("Debugging Window:\n");

	//Thread^ thread2 = gcnew Thread(gcnew ParameterizedThreadStart(this, &XboxControl));
	//thread2->Start(results);

	//continuously reads the 360 controller using a second thread
	Thread^ thread2 = gcnew Thread(gcnew ThreadStart(XboxControl));
	thread2->Name = "thread2";
	thread2->Start();
	

	//opens the windows form UI
	Application::EnableVisualStyles(); //forces use of windows style, set by user preferrences
	Application::SetCompatibleTextRenderingDefault(false);
	Control_Tower::MyForm form;
	Application::Run(%form);
	//code placed here will never run: Application::Run(%form) is an infinite message loop
}
