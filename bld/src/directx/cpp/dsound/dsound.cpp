//---------------------------------------------------------------------------------------
//
// Simple Windows DirectSound Example Code
//
// Jim Shaw 12/3/2004
//
//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------
// Always include windows.h for windows programs
//---------------------------------------------------------------------------------------
#include <windows.h>

#include <dsound.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

//---------------------------------------------------------------------------------------
// Windows variables
//---------------------------------------------------------------------------------------
HWND game_window;
char game_class[]="DSNDCPPWindow";

//---------------------------------------------------------------------------------------
// DirectSound variables
//---------------------------------------------------------------------------------------
IDirectSound8 *dsound=NULL;
IDirectSoundBuffer *dsprimarybuffer=NULL;
IDirectSoundBuffer8 *sfxbuffer8=NULL;

//---------------------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------------------
int init_dsound(void);
int load_wav(char *);
void shutdown_dsound(void);

//---------------------------------------------------------------------------------------
// Main Windows function
//---------------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	char filename[_MAX_PATH];
	OPENFILENAME ofn;
	BOOL res;
	WNDCLASSEX clas;
	int style;
	RECT rect;
	MSG msg;
	HDC hdc;

	//Here we create the Class we named above
	clas.cbSize = sizeof(WNDCLASSEX);
	clas.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	clas.lpfnWndProc = DefWindowProc;
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
	game_window = CreateWindowEx(0, game_class, "DirectSound", style, CW_USEDEFAULT, CW_USEDEFAULT, 1,1, NULL, NULL, hInstance, 0);

	//adjust the window size so that a 200 x 40 window will fit inside its frame
	rect.left = rect.top = 0;
	rect.right = 200;
	rect.bottom = 40;
	AdjustWindowRectEx(&rect, style , FALSE, 0);
	SetWindowPos(game_window, NULL, 0,0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOMOVE|SWP_NOZORDER);

	//show the window on the desktop
	ShowWindow(game_window, nCmdShow);

	filename[0] = '\0';
	ofn.lStructSize = sizeof(OPENFILENAME); 
	ofn.hwndOwner = NULL; 
	ofn.hInstance = NULL;
	ofn.lpstrFilter = "WAV Files\0*.wav\0"; 
	ofn.lpstrCustomFilter = NULL; 
	ofn.nMaxCustFilter = 0; 
	ofn.nFilterIndex = 0; 
	ofn.lpstrFile = filename; 
	ofn.nMaxFile = _MAX_PATH; 
	ofn.lpstrFileTitle = NULL; 
	ofn.nMaxFileTitle = 0; 
	ofn.lpstrInitialDir = NULL; 
	ofn.lpstrTitle = "Select Audio File"; 
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

	if (init_dsound() != 0)
	{
		MessageBox(NULL, "A problem occurred initialising DirectSound.\nCheck your sound drivers and "
							"DirectX version.\nThis demo requires DirectX9.0 or better.", "DirectX Initialisation Error", MB_ICONWARNING|MB_OK);
		shutdown_dsound();
		return 0;
	}

	if (load_wav(filename) != 0)
	{
		MessageBox(NULL, "A problem occurred loading the WAV file\n",
							"File Loading Error", MB_ICONWARNING|MB_OK);
		shutdown_dsound();
		return 0;
	}

	hdc = GetDC(game_window);
	TextOut(hdc, 10,10, "Close me to stop sound!", 23);
	ReleaseDC(game_window, hdc);

	while (GetMessage(&msg, game_window, 0,0) >= 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	shutdown_dsound();

	return 0;
}

//---------------------------------------------------------------------------------------
// Initialise DirectSound
//---------------------------------------------------------------------------------------
int init_dsound(void)
{
	HRESULT err;
	DSCAPS dsoundcaps;
	DSBUFFERDESC dsbdesc;
	WAVEFORMATEX wfx;

	err = DirectSoundCreate8(NULL, &dsound, NULL);
	if (err != DS_OK)
		return 1;
	
	err = dsound->SetCooperativeLevel(game_window, DSSCL_EXCLUSIVE);
	if (err != DS_OK)
		return 1;

	memset(&dsoundcaps, 0, sizeof dsoundcaps);
	dsoundcaps.dwSize = sizeof dsoundcaps;

	err = IDirectSound8_GetCaps(dsound, &dsoundcaps);
	if (err != DS_OK)
		return 1;

	// Create and set primary buffer format
	memset(&dsbdesc, 0, sizeof dsbdesc);

	dsbdesc.dwSize = sizeof dsbdesc;
	dsbdesc.dwFlags = DSBCAPS_PRIMARYBUFFER;

	err = dsound->CreateSoundBuffer(&dsbdesc, &dsprimarybuffer, NULL);
	if (err != DS_OK)
		return 1;
 
	memset(&wfx, 0, sizeof wfx);
	wfx.cbSize = sizeof wfx;
	wfx.wFormatTag = WAVE_FORMAT_PCM; 
	wfx.nChannels = 2; 
	wfx.nSamplesPerSec = 44100; 
	wfx.wBitsPerSample = 16; 
	wfx.nBlockAlign = wfx.wBitsPerSample / 8 * wfx.nChannels;
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
 
	err = dsprimarybuffer->SetFormat(&wfx);
	if (err != DS_OK)
		return 1;

	//force dsound to ALWAYS keep DMA active
	err = dsprimarybuffer->Play(0, 0, DSBPLAY_LOOPING);
	if (err != DS_OK)
		return 1;

	return 0;
}

//---------------------------------------------------------------------------------------
// Shutdown DirectSound
//---------------------------------------------------------------------------------------
void shutdown_dsound()
{

	if (sfxbuffer8)
	{
		sfxbuffer8->Stop();
		sfxbuffer8->Release();
	}
	if (dsprimarybuffer)
	{
		dsprimarybuffer->Stop();
		dsprimarybuffer->Release();
	}
	if (dsound) dsound->Release();

}

//---------------------------------------------------------------------------------------
// Convert linear volume to DirectSound db scale
//---------------------------------------------------------------------------------------
int vol_to_db(int vol)
{
	if (vol)
		return (int)(20.0 * 100.0 * log10((double)vol/127.0));
	return -10000;
}


//---------------------------------------------------------------------------------------
// Make a WAV FOURCC
//---------------------------------------------------------------------------------------
#define MRIFFID(a,b,c,d) (((d)<<24)|((c)<<16)|((b)<<8)|(a))

//---------------------------------------------------------------------------------------
// Load and play the given wav file
//---------------------------------------------------------------------------------------
int load_wav(char *filename)
{
	unsigned int *riff;
	FILE *f;
	long len;
	void *sfx;

	// Read in the supplied file
	f = fopen(filename, "rb");
	if (!f)
		return 1;
	fseek(f, 0, SEEK_END);
	len = ftell(f);
	fseek(f, 0, SEEK_SET);

	sfx = malloc(len);
	if (!sfx)
	{
		fclose(f);
		return 1;
	}

	fread(sfx, 1, len, f);
	fclose(f);

	riff = (unsigned int *)sfx;

	// Check it's a RIFF WAV file
	if (*riff == MRIFFID('R','I','F','F'))
	{
		IDirectSoundBuffer *sfxbuffer;
		DSBUFFERDESC dsbufferdesc;
		void *data1, *data2;
		DWORD size1, size2;
		unsigned char *wav_data;
		unsigned int wav_size;
		WAVEFORMATEX *wav_hdr;
		HRESULT err;

		// Strip through the WAV header looking for the WAVEFORMAT data

		riff += 3;//skip length and id
		while (*riff != MRIFFID('f','m','t',' '))
			riff = (unsigned int *)((unsigned char *)riff + 8 + *(riff+1));

		wav_hdr = (WAVEFORMATEX *)(riff+2);

		// Strip through the WAV header looking for the sample data

		riff = (unsigned int *)((unsigned char *)riff + 8 + *(riff+1));
		while (*riff != MRIFFID('d','a','t','a'))
			riff = (unsigned int *)((unsigned char *)riff + 8 + *(riff+1));

		wav_data = (unsigned char *)(riff+2);
		wav_size = *(riff+1);

		// Create a suitable buffer
		memset(&dsbufferdesc, 0, sizeof dsbufferdesc);
		dsbufferdesc.dwSize = sizeof dsbufferdesc;
		dsbufferdesc.dwFlags = DSBCAPS_STATIC|DSBCAPS_CTRLVOLUME|DSBCAPS_CTRLFREQUENCY|DSBCAPS_CTRLPAN;
		dsbufferdesc.dwBufferBytes = wav_size;
		dsbufferdesc.lpwfxFormat = wav_hdr;

		err = dsound->CreateSoundBuffer(&dsbufferdesc, &sfxbuffer, NULL);
		if (err != DS_OK)
		{
			free(sfx);
			return 1;
		}
		// Get the DirectSound8 interface to the buffer
		err = sfxbuffer->QueryInterface(IID_IDirectSoundBuffer8, (void **)&sfxbuffer8);
		if (sfxbuffer8 == NULL)
		{
			free(sfx);
			return 1;
		}
		// Release the base object
		err = sfxbuffer->Release();

		// Lock the buffer
		err = sfxbuffer8->Lock(0, 0, (void **)&data1, &size1, (void **)&data2, &size2, DSBLOCK_ENTIREBUFFER);
		if (err != DS_OK)
		{
			free(sfx);
			return 1;
		}

		// Copy wav data from file into DirectSound buffer
		memcpy(data1, wav_data, wav_size);

		err = sfxbuffer8->Unlock(data1, size1, data2, size2);

		// Set the sfx volume
		err = sfxbuffer8->SetVolume(vol_to_db(127));

		// Play the sound
		err = sfxbuffer8->Play(0,0,0);
		if (err != DS_OK)
		{
			free(sfx);
			return 1;
		}
	}

	// Tidy up
	free(sfx);

return 0;
}

