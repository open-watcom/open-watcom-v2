//---------------------------------------------------------------------------------------
//
// Simple Windows DirectInput Example Code
//
// Jim Shaw 15/3/2004
//
//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------
// Always include windows.h for windows programs
//---------------------------------------------------------------------------------------
#include <windows.h>

#define DIRECTINPUT_VERSION 0x800
#include <dinput.h>

#include <stdio.h>

//---------------------------------------------------------------------------------------
// Window dimensions
//---------------------------------------------------------------------------------------
#define SCREEN_X   640
#define SCREEN_Y   480

//---------------------------------------------------------------------------------------
// Variable to hold the address of the Windows window
//---------------------------------------------------------------------------------------
HWND game_window;

//---------------------------------------------------------------------------------------
// Name of our window class.  every class has a name.  every window is one of a class.
// this can be pretty much anything
//---------------------------------------------------------------------------------------
char game_class[]="DICWindow";
static int quit = 0;

//---------------------------------------------------------------------------------------
// Function prototypes
//---------------------------------------------------------------------------------------
int init_dinput(HINSTANCE);
void shutdown_dinput(void);
void show_dinput_status(HDC hdc);

//---------------------------------------------------------------------------------------
// DirectInput Variables
//---------------------------------------------------------------------------------------
IDirectInput8 *dinput;
IDirectInputDevice8 *keyboard;
IDirectInputDevice8 *mouse;
IDirectInputDevice8 *joystick;

unsigned char keymap[256];
DIMOUSESTATE2 mousedata;
DIJOYSTATE2 joydata;

//---------------------------------------------------------------------------------------
// Every window has a WindowProc which handles messages Windows sends to them.  These
// are things like key presses, mouse clicks, and thousands of other things you might
// be interested in.
//---------------------------------------------------------------------------------------
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		//this message gets sent to the window when you click the close box on it
		case WM_DESTROY:
			//tell Windows to close the app
			PostQuitMessage(0);
			quit = 1;
			break;

		case WM_PAINT:
			{
			PAINTSTRUCT ps;
			BeginPaint(hwnd, &ps);
			show_dinput_status(ps.hdc);
			EndPaint(hwnd, &ps);
			}
			break;

		case WM_TIMER:
			InvalidateRect(hwnd, 0, TRUE);
			break;

		//this message gets sent when you press a key
		case WM_KEYDOWN:
			switch (wParam)
			{
				//pressed Escape key?
				case VK_ESCAPE:
					//tell Windows to close the app
					PostQuitMessage(0);
					quit = 1;
					break;
			}
			break;

		//any message you don't process gets passed to this function inside Windows.
		default:
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	//any message you do process, return 0 (unless the message documentation says otherwise)
return 0;
}

//---------------------------------------------------------------------------------------
// WinMain
//---------------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX clas;
	MSG msg;
	int style;
	RECT rect;

	//Here we create the Class we named above
	clas.cbSize = sizeof(WNDCLASSEX);
	clas.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	clas.lpfnWndProc = WindowProc;//<- tell it where the WindowProc is
	clas.cbClsExtra = 0;
	clas.cbWndExtra = 0;
	clas.hInstance = hInstance;
	clas.hIcon = NULL;
	clas.hCursor = NULL;
	clas.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);//<- background colour of window
	clas.lpszMenuName = NULL;
	clas.lpszClassName = game_class;//<- the class name
	clas.hIconSm = 0;
	//do it!
	RegisterClassEx(&clas);

	//style of the window - what boxes do we need (close minimised etc)
	style = WS_CAPTION|WS_SYSMENU|WS_MAXIMIZEBOX|WS_MINIMIZEBOX;
	//create the window
	game_window = CreateWindowEx(0, game_class, "DirectInput", style, CW_USEDEFAULT, CW_USEDEFAULT, 1,1, NULL, NULL, hInstance, 0);

	//adjust the window size so that a SCREEN_X x SCREEN_Y window will fit inside its frame
	rect.left = rect.top = 0;
	rect.right = SCREEN_X;
	rect.bottom = SCREEN_Y;
	AdjustWindowRectEx(&rect, style , FALSE, 0);
	SetWindowPos(game_window, NULL, 0,0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOMOVE|SWP_NOZORDER);

	//show the window on the desktop
	ShowWindow(game_window, nCmdShow);

	if (init_dinput(hInstance) == 0)
	{
		MessageBox(NULL, "A problem occurred creating DirectInput.\nCheck your DirectX version.\n"
							"This demo requires DirectX9.0 or better.", "DirectX Initialisation Error", MB_ICONWARNING|MB_OK);
		shutdown_dinput();
		return 0;
	}

	// Create a timer to refresh the window
	SetTimer(game_window, 1, 1000/30, NULL);

	//message processing loop
	//all Windows programs have one of these.  It receives the messages Windows sends to the program and
	//passes them to the WindowProc in the Class we registered for the window.
	quit = 0;
	do
	{
		//Are there any messages waiting?
		while (GetMessage(&msg, game_window, 0,0) > 0 && !quit)
		{

			//pass the message to WindowProc
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

	} while (!quit);

	// Close everything down
	KillTimer(game_window, 1);

	shutdown_dinput();

	return 0;
}

//---------------------------------------------------------------------------------------
// Callback to look at DirectInput devices to see if we're interested in them
//---------------------------------------------------------------------------------------
BOOL CALLBACK dinput_enum_callback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
{
	HRESULT err;

	err = IDirectInput8_CreateDevice(dinput, &lpddi->guidInstance, &joystick, NULL);
	if (err != DI_OK)
		return DIENUM_STOP;
	err = IDirectInputDevice8_SetDataFormat(joystick, &c_dfDIJoystick2);
	if (err != DI_OK)
	{
		IDirectInputDevice8_Release(joystick);
		joystick = NULL;
		return DIENUM_STOP;
	}
	err = IDirectInputDevice8_SetCooperativeLevel(joystick, game_window, DISCL_NONEXCLUSIVE|DISCL_FOREGROUND);
	err = IDirectInputDevice8_Acquire(joystick);

	return DIENUM_STOP;
}

//---------------------------------------------------------------------------------------
// Create the DirectInput interface and access mouse, keyboard and joystick
//---------------------------------------------------------------------------------------
int init_dinput(HINSTANCE hInst)
{
	HRESULT err;

	// Create the DirectInput interface
	err = DirectInput8Create(hInst, DIRECTINPUT_VERSION, &IID_IDirectInput8, &dinput, NULL);
	if (err != DI_OK)
		return 0;

	// Create the Keyboard device
	err = IDirectInput8_CreateDevice(dinput, &GUID_SysKeyboard, &keyboard, NULL);
	err = IDirectInputDevice8_SetDataFormat(keyboard, &c_dfDIKeyboard);
	err = IDirectInputDevice8_SetCooperativeLevel(keyboard, game_window, DISCL_NONEXCLUSIVE|DISCL_FOREGROUND);
	err = IDirectInputDevice8_Acquire(keyboard);

	// Create the mouse device
	err = IDirectInput8_CreateDevice(dinput, &GUID_SysMouse, &mouse, NULL);
	err = IDirectInputDevice8_SetDataFormat(mouse, &c_dfDIMouse2);
	err = IDirectInputDevice8_SetCooperativeLevel(mouse, game_window, DISCL_NONEXCLUSIVE|DISCL_FOREGROUND);
	err = IDirectInputDevice8_Acquire(mouse);

	// Check for a joystick device, and create it if one exists
	err = IDirectInput8_EnumDevices(dinput, DI8DEVCLASS_GAMECTRL, dinput_enum_callback, 0, DIEDFL_ALLDEVICES);

	return 1;
}

//---------------------------------------------------------------------------------------
// Shutdown DirectInput
//---------------------------------------------------------------------------------------
void shutdown_dinput(void)
{
	if (joystick)
	{
		IDirectInputDevice8_Unacquire(joystick);
		IDirectInputDevice8_Release(joystick);
	}
	if (mouse)
	{
		IDirectInputDevice8_Unacquire(mouse);
		IDirectInputDevice8_Release(mouse);
	}
	if (keyboard)
	{
		IDirectInputDevice8_Unacquire(keyboard);
		IDirectInputDevice8_Release(keyboard);
	}
	if (dinput) IDirectInput8_Release(dinput);
}

//---------------------------------------------------------------------------------------
// Read data back from the controllers
//---------------------------------------------------------------------------------------
void read_dinput_status(void)
{
	HRESULT err;

	// Read the keyboard
	err = IDirectInputDevice8_Poll(keyboard);
	err = IDirectInputDevice8_GetDeviceState(keyboard, 256, keymap);

	// Read the mouse
	err = IDirectInputDevice8_Poll(mouse);
	err = IDirectInputDevice8_GetDeviceState(mouse, sizeof mousedata, &mousedata);

	// Read the joystick
	if (joystick)
	{
		err = IDirectInputDevice8_Poll(joystick);
		err = IDirectInputDevice8_GetDeviceState(joystick, sizeof joydata, &joydata);
	}
}

//---------------------------------------------------------------------------------------
// Display current device inputs
//---------------------------------------------------------------------------------------
void show_dinput_status(HDC hdc)
{
	int x, y;
	char text[256];

	read_dinput_status();

	// Keymap status
	TextOut(hdc, 0,0, "Keyboard", 8);
	for (y = 0; y < 16; y++)
	{
		for (x = 0; x < 16; x++)
		{
			if (keymap[x+16*y])
				TextOut(hdc, x*10, (y+1)*16, "1", 1);
			else
				TextOut(hdc, x*10, (y+1)*16, "0", 1);

		}
	}

	// Mouse status
	TextOut(hdc, 260,0, "Mouse", 5);
	sprintf(text, "XYZ %d,%d,%d", mousedata.lX, mousedata.lY, mousedata.lZ);
	TextOut(hdc, 260, 16, text, strlen(text));
	for (x = 0; x < 8; x++)
	{
		if (mousedata.rgbButtons[x])
			TextOut(hdc, 260+x*10,32, "1", 1);
		else
			TextOut(hdc, 260+x*10,32, "0", 1);
	}

	//Joystick status
	TextOut(hdc, 400, 0, "Joystick", 8);
	if (joystick)
	{
		sprintf(text, "XYZ %d,%d,%d", joydata.lX, joydata.lY, joydata.lZ);
		TextOut(hdc, 400, 16, text, strlen(text));
	}
	else
	{
		TextOut(hdc, 400, 16, "Unattached", 10);
	}
}
