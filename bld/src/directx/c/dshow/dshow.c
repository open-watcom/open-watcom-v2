//---------------------------------------------------------------------------------------
//
// Simple Windows DirectShow Example Code
//
// Jim Shaw 12/3/2004
//
//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------
// Always include windows.h for windows programs
//---------------------------------------------------------------------------------------
#define _WIN32_DCOM
#include <windows.h>

#ifdef __WATCOMC__
#define _WINGDI_
#endif

#define COBJMACROS
#include <dshow.h>

#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include <assert.h>

//---------------------------------------------------------------------------------------
// Screen dimensions
//---------------------------------------------------------------------------------------
#define SCREEN_X   640
#define SCREEN_Y   480
#define IN_OWN_WINDOW 1

//---------------------------------------------------------------------------------------
// Variable to hold the address of the Windows window
//---------------------------------------------------------------------------------------
HWND game_window;

//---------------------------------------------------------------------------------------
// Name of our window class.  every class has a name.  every window is one of a class.
// this can be pretty much anything
//---------------------------------------------------------------------------------------
char game_class[]="DSCWindow";
static int quit = 0;

//---------------------------------------------------------------------------------------
// Function prototypes
//---------------------------------------------------------------------------------------
void draw(void);
void flip(void);
int init_dshow(wchar_t *, int);
void shutdown_dshow(void);
int handle_dshow_event(void);

//---------------------------------------------------------------------------------------
// DirectShow Variables
//---------------------------------------------------------------------------------------
IGraphBuilder *pGB=NULL;
IMediaControl *pMC=NULL;
IMediaEventEx *pME=NULL;
IMediaSeeking *pMS=NULL;
IVMRWindowlessControl *pWC=NULL;

#define WM_GRAPHNOTIFY0  (WM_USER+13)

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

		case WM_GRAPHNOTIFY0:
			if (handle_dshow_event())
			{
				PostQuitMessage(0);
				quit = 1;
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
// Convert char string to wchar_t string
//---------------------------------------------------------------------------------------
static wchar_t c[_MAX_PATH];
static wchar_t *ctowc(char *p)
{
#ifdef __WATCOMC__
	swprintf(c, _MAX_PATH, L"%hs", p);
#else
	swprintf(c, L"%S", p);
#endif
	return c;
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
	char filename[_MAX_PATH];
	OPENFILENAME ofn;
	BOOL res;

	//need COM for DirectShow
	CoInitializeEx(NULL, COINIT_MULTITHREADED); 

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
	game_window = CreateWindowEx(0, game_class, "DirectShow", style, CW_USEDEFAULT, CW_USEDEFAULT, 1,1, NULL, NULL, hInstance, 0);

	//adjust the window size so that a SCREEN_X x SCREEN_Y window will fit inside its frame
	rect.left = rect.top = 0;
	rect.right = SCREEN_X;
	rect.bottom = SCREEN_Y;
	AdjustWindowRectEx(&rect, style , FALSE, 0);
	SetWindowPos(game_window, NULL, 0,0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOMOVE|SWP_NOZORDER);

	//show the window on the desktop
	ShowWindow(game_window, nCmdShow);

	filename[0] = '\0';
	ofn.lStructSize = sizeof(OPENFILENAME); 
	ofn.hwndOwner = NULL; 
	ofn.hInstance = NULL;
	ofn.lpstrFilter = "AVI Files\0*.avi\0"; 
	ofn.lpstrCustomFilter = NULL; 
	ofn.nMaxCustFilter = 0; 
	ofn.nFilterIndex = 0; 
	ofn.lpstrFile = filename; 
	ofn.nMaxFile = _MAX_PATH; 
	ofn.lpstrFileTitle = NULL; 
	ofn.nMaxFileTitle = 0; 
	ofn.lpstrInitialDir = NULL; 
	ofn.lpstrTitle = "Select Video File"; 
	ofn.Flags = OFN_FILEMUSTEXIST|OFN_EXPLORER; 
	ofn.nFileOffset=0; 
	ofn.nFileExtension=0; 
	ofn.lpstrDefExt="bin"; 
	ofn.lCustData=0; 
	ofn.lpfnHook=NULL; 
	ofn.lpTemplateName=""; 

	res = GetOpenFileName(&ofn);
	if (res == 0)
		return 0;

	if (init_dshow(ctowc(filename), IN_OWN_WINDOW)==0)
	{
		MessageBox(NULL, "A problem occurred creating the DirectShow FilterGraph.\nCheck your video CODECs and "
							"DirectX version.\nThis demo requires DirectX9.0 or better.", "DirectX Initialisation Error", MB_ICONWARNING|MB_OK);
		shutdown_dshow();
		return 0;
	}

	//message processing loop
	//all Windows programs have one of these.  It receives the messages Windows sends to the program and
	//passes them to the WindowProc in the Class we registered for the window.
	quit = 0;
	do
	{
		//Are there any messages waiting?
		while (PeekMessage(&msg, game_window, 0, 0, PM_NOREMOVE))
		{
			//yes!  read it.
			if (GetMessage(&msg, game_window, 0,0) < 0)
				break;

			//pass the message to WindowProc
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

	} while (!quit);

	shutdown_dshow();

	CoUninitialize();

	return 0;
}

//---------------------------------------------------------------------------------------
// Initialise all the DirectShow structures we need
//---------------------------------------------------------------------------------------
int init_dshow(wchar_t *FileName, int render_in_own_window)
{
	HRESULT err;
	FILTER_STATE state = State_Stopped;

	// Create a new graph
    err = CoCreateInstance(&CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, &IID_IGraphBuilder, (void **)&pGB);
	if (err != S_OK)
		return 0;

	// Get access to the video controls
	err = IGraphBuilder_QueryInterface(pGB, &IID_IMediaControl, (void **)&pMC);
	if (err != S_OK)
		return 0;

	err = IGraphBuilder_QueryInterface(pGB, &IID_IMediaSeeking, (void **)&pMS);
	if (err != S_OK)
		return 0;

	err = IGraphBuilder_QueryInterface(pGB, &IID_IMediaEventEx, (void **)&pME);
	if (err != S_OK)
		return 0;

	if (render_in_own_window)
	{
		IBaseFilter *pVMR;
		IVMRFilterConfig *pFC; 
		long lWidth, lHeight; 
		RECT rcSrc, rcDest;

		err = CoCreateInstance(&CLSID_VideoMixingRenderer, NULL, CLSCTX_INPROC, &IID_IBaseFilter, (void**)&pVMR); 
		if (err != S_OK)
			return 0;

		// Add the VMR to the filter graph.
		err = IGraphBuilder_AddFilter(pGB, pVMR, L"VMR");
		if (err != S_OK)
			return 0;

		// Set the rendering mode.  
		err = IBaseFilter_QueryInterface(pVMR, &IID_IVMRFilterConfig, (void**)&pFC); 
		if (err != S_OK)
			return 0;

		err = IVMRFilterConfig_SetRenderingMode(pFC, VMRMode_Windowless); 
		if (err != S_OK)
			return 0;

		IVMRFilterConfig_Release(pFC); 

		// Set the window. 
		err = IBaseFilter_QueryInterface(pVMR, &IID_IVMRWindowlessControl, (void**)&pWC);
		if (err != S_OK)
			return 0;

		err = IVMRWindowlessControl_SetVideoClippingWindow(pWC, game_window); 
		if (err != S_OK)
			return 0;

		IBaseFilter_Release(pVMR); 

		// Find the native video size.
		err = IVMRWindowlessControl_GetNativeVideoSize(pWC, &lWidth, &lHeight, NULL, NULL); 
			// Set the source rectangle.
		SetRect(&rcSrc, 0, 0, lWidth/2, lHeight/2); 
    
			// Get the window client area.
		GetClientRect(game_window, &rcDest); 
		// Set the destination rectangle.
		SetRect(&rcDest, 0, 0, rcDest.right, rcDest.bottom); 
   
			// Set the video position.
		err = IVMRWindowlessControl_SetVideoPosition(pWC, &rcSrc, &rcDest); 
	}

	// Add the source file
	err = IGraphBuilder_RenderFile(pGB, FileName, NULL);
	if (err != S_OK)
		return 0;

	// Have the graph signal event via window callbacks for performance
	err = IMediaEventEx_SetNotifyWindow(pME, (OAHWND)game_window, WM_GRAPHNOTIFY0, 0);
	if (err != S_OK)
		return 0;

	err = IMediaSeeking_SetTimeFormat(pMS, &TIME_FORMAT_FRAME);
	if(err != S_OK)
	{
		err = IMediaSeeking_SetTimeFormat(pMS, &TIME_FORMAT_MEDIA_TIME);
	}

	err = IMediaControl_Run(pMC);
	do
	{
		err = IMediaControl_GetState(pMC, 0, (long *)&state);
	} while (state != State_Running);

	return 1;
}

//---------------------------------------------------------------------------------------
// Shutdown DirectShow
//---------------------------------------------------------------------------------------
void shutdown_dshow(void)
{
	if (pWC) IVMRWindowlessControl_Release(pWC);
	if (pMS) IMediaSeeking_Release(pMS);
	if (pME) IMediaEventEx_Release(pME);
	if (pMC) IMediaControl_Release(pMC);
	if (pGB) IGraphBuilder_Release(pGB);
}

//---------------------------------------------------------------------------------------
// Handle DirectShow Event
//---------------------------------------------------------------------------------------
int handle_dshow_event(void)
{
    LONG evCode, evParam1, evParam2;
	int stopping = 0;

    // Make sure that we don't access the MediaEventEx interface after it has already been released.
    if (!pME)
	    return S_OK;

    // Process all queued events
    while (IMediaEventEx_GetEvent(pME, &evCode, &evParam1, &evParam2, 0) == S_OK)
    {
        switch (evCode)
        {
			case EC_COMPLETE:
				stopping = 1;
				break;

			default:
				break;
       }

        // Free memory associated with callback, since we're not using it
        IMediaEventEx_FreeEventParams(pME, evCode, evParam1, evParam2);
    }

	return stopping;
}
