#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <commctrl.h>
#include <commdlg.h>
#include <process.h>
#include <dsound.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>

#include "mp3dec.h"

#define NUMEVENTS 8

char filename[256];

char message[50] = "OK";

char hostaddrs[16][30];

signed short SB_Buf[16384], SB_Buf2[16384], SB_BufHalf;

signed short SB_BufP[32768], SB_BufPPos, SB_BufPPos2, SB_NumPBufs;
int SB_Block = 4*1152, SB_Block2 = 1152, SB_Freq = 44100;
int SB_Delay = 80, SB_Timer1;

int echohist[16], echopos, echohistlen = 4;

int SB_notifyplay = 0;

LPDIRECTSOUND               lpds;
DSBUFFERDESC                dsbdesc;
LPDIRECTSOUNDBUFFER         lpdsb;
WAVEFORMATEX                pwfx;
LPDIRECTSOUNDNOTIFY         lpdsn;

GUID DriverGUID[32];
GUID* lpDriverGUID[32];
char* DriverDesc[32];
int DriverIndex = 0, PlaybackDriverNum;
int PlaybackDriver = 0;

int SB_thread, SB_BufPos1, SB_BufPos2, SB_lasttime, SB_NumBufs, SB_NumBufs2;
int SB_Playing, SB_ToStopPlayback, SB_Timeout;
int SB_PTT, SB_PTTMaster, SB_Pushed, SB_UnPush = 0;
volatile int SB_endthread, SB_Mixing, SB_suspend = 0;

HWND mainhWnd, oschWnd, hosthWnd, joinhWnd, stophWnd, testhWnd, dlghWnd;
HWND playerhWnd;
HWND hWndpool[100];

HMENU Menu1, ChannelMenu, ServerMenu, PlayerMenu;

HFONT hFont;

HANDLE events[16];

FILE *outf;
int save = 0, cnt, suspend = 0;


void StartPlayback(void);
void StopPlayback(void);
void ProcessPlayback(int buf);


void ProcessBuffer(void)
{
   if (suspend) return;

   SB_Mixing = 1;

   if (save) fwrite(&SB_Buf,2,SB_Block,outf);
   SB_Mixing = 0;
}

void SB_ThreadFunc(void *arg)
{
   int t, t2, i;
   HRESULT r;

   SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_ABOVE_NORMAL);
   Sleep(300);
   t2 = GetTickCount();
   while (!SB_endthread) {
      r = WaitForMultipleObjects(NUMEVENTS,events,FALSE,15);
      if (SB_suspend) continue;

      if (r != WAIT_TIMEOUT) {
         i = r - WAIT_OBJECT_0;
         ProcessPlayback(i);
      }

      t = GetTickCount();
      if (t - t2 >= 1000) {
         t2 += 1000;
      }

      if (SB_Playing && !SB_notifyplay) {
         static DWORD numbytes, playcur, writecur;

         numbytes = SB_Block * 4;
         if (IDirectSoundBuffer_GetCurrentPosition(lpdsb,&writecur,&playcur) != DS_OK) {
            DWORD stat;
            IDirectSoundBuffer_GetStatus(lpdsb,&stat);
            if (stat & DSBSTATUS_BUFFERLOST) {
               IDirectSoundBuffer_Restore(lpdsb);
               IDirectSoundBuffer_Play(lpdsb,0,0,DSBPLAY_LOOPING);
            } else
               strcpy(message,"GetPosition");
         } else if (playcur / numbytes != SB_BufPos1) {
            SB_BufPos1 = playcur / numbytes;
            ProcessPlayback((SB_BufPos1 + 1) % SB_NumBufs2);
         }
      }
   }
   SB_endthread = 2;
}

void ProcessPlayback(int buf)
{
   int numbytes,posinbuf;
   unsigned char *ptr1, *ptr2;
   DWORD len1,len2, i, freq;
   HRESULT r;

   numbytes = SB_Block*4;

   /*SB_BufPos1 = buf;*/

   posinbuf = buf * numbytes;
   r = IDirectSoundBuffer_Lock(lpdsb,posinbuf,numbytes,&ptr1,&len1,&ptr2,&len2,0);
   if (r != DS_OK) strcpy(message,"Lock");
   if (len1 != numbytes) strcpy(message,"Len1");
   for (i = 0; i < SB_Block; i += SB_Block2) {
      if (MPG_Get_Filepos () != C_MPG_EOF) {

         if (MPG_Read_Frame () == MPG_OK) {
            /* Decode the compressed frame into 1152 mono/stereo PCM audio samples */
            MPG_Decode_L3 ();
            freq = g_sampling_frequency[g_frame_header.id][g_frame_header.sampling_frequency];
         }
         memcpy(ptr1 + i * 4, &mp3_outdata, SB_Block2 * 4);
      } else {
         memset(ptr1 + i * 4, 0, SB_Block2 * 4);
      }
   }

   //memcpy(ptr1,&SB_BufP[SB_BufPPos2 * SB_Block],numbytes);
   IDirectSoundBuffer_Unlock(lpdsb,ptr1,len1,ptr2,len2);
   if (freq != SB_Freq) {
      IDirectSoundBuffer_SetFrequency(lpdsb, freq);
      SB_Freq = freq;
   }
}

INT_PTR CALLBACK DSoundEnumCallback(GUID* pGUID, LPSTR strDesc,
   LPSTR strDrvName, VOID* pContext)
{
   if (pGUID)
   {
      if (DriverIndex >= 32)
         return FALSE;

      lpDriverGUID[DriverIndex] = &DriverGUID[DriverIndex];
      memcpy(lpDriverGUID[DriverIndex], pGUID, sizeof(GUID));
   } else {
      lpDriverGUID[DriverIndex] = NULL;
   }
   DriverDesc[DriverIndex] = malloc(strlen(strDesc));
   strcpy(DriverDesc[DriverIndex],strDesc);
   DriverIndex++;

   return TRUE;
}

void EnumerateDevices(void)
{
   DirectSoundEnumerate((LPDSENUMCALLBACK) DSoundEnumCallback, NULL);
   PlaybackDriverNum = DriverIndex;
}


int InitDSound(HWND hWnd, int flag)
{
   if (PlaybackDriver >= PlaybackDriverNum) PlaybackDriver = 0;
   if (DirectSoundCreate(lpDriverGUID[PlaybackDriver],&lpds,NULL) != DS_OK)
      return 10;
   if (IDirectSound_SetCooperativeLevel(lpds,mainhWnd,DSSCL_NORMAL) != DS_OK) return 11;

   return 0;
}

int InitDSoundBuffers(void)
{
   DSBPOSITIONNOTIFY dsbpnotify[16];
   int i;

   pwfx.wFormatTag = WAVE_FORMAT_PCM;
   pwfx.nChannels = 2;
   pwfx.nSamplesPerSec = SB_Freq;
   pwfx.wBitsPerSample = 16;
   pwfx.nBlockAlign = pwfx.nChannels * pwfx.wBitsPerSample / 8;
   pwfx.nAvgBytesPerSec = pwfx.nBlockAlign * pwfx.nSamplesPerSec;
   pwfx.cbSize = 0;

   if (lpds) {
      memset(&dsbdesc,0,sizeof(DSBUFFERDESC));
      dsbdesc.dwSize = sizeof(DSBUFFERDESC);
      dsbdesc.dwFlags = DSBCAPS_CTRLFREQUENCY | DSBCAPS_GLOBALFOCUS;
      if (SB_notifyplay)
         dsbdesc.dwFlags |= DSBCAPS_CTRLPOSITIONNOTIFY;
      else
         dsbdesc.dwFlags |= DSBCAPS_GETCURRENTPOSITION2;
      dsbdesc.dwBufferBytes = 4 * SB_Block*4;
      dsbdesc.lpwfxFormat = &pwfx;
      if (IDirectSound_CreateSoundBuffer(lpds,&dsbdesc,&lpdsb,NULL) != DS_OK)
         return 12;

      SB_NumBufs2 = dsbdesc.dwBufferBytes / SB_Block / 4;
      SB_NumPBufs = 32768 / SB_Block / 4;

      if (SB_notifyplay) {
         if (IDirectSoundBuffer_QueryInterface(lpdsb,&IID_IDirectSoundNotify,&lpdsn) != DS_OK)
            return 13;
         for (i = 0; i < SB_NumBufs; i++) {
            if (i == 0)
               dsbpnotify[i].dwOffset = (SB_NumBufs-1) * SB_Block;
            else
               dsbpnotify[i].dwOffset = (i-1) * SB_Block;
            dsbpnotify[i].dwOffset += SB_Block / 2;
            dsbpnotify[i].hEventNotify = events[i];
         }
         IDirectSoundNotify_SetNotificationPositions(lpdsn,SB_NumBufs,&dsbpnotify);
      }
   }

   return 0;
}

void ReleaseDSoundBuffers(void)
{
   if (lpdsn) IDirectSoundNotify_Release(lpdsn);
   if (lpdsb) IDirectSoundBuffer_Release(lpdsb);
   lpdsn = NULL;
   lpdsb = NULL;
}


void StartPlayback(void)
{
   int i;

   if (!lpdsb) return;
   if (SB_Playing) StopPlayback();
   {
      unsigned char *ptr1, *ptr2;
      DWORD len1,len2;
      IDirectSoundBuffer_Lock(lpdsb,0,8*SB_Block,&ptr1,&len1,&ptr2,&len2,0);
      /*for (i = 0; i < len1; i++) ptr1[i] = 128;*/
      memset(ptr1,0,len1);
      IDirectSoundBuffer_Unlock(lpdsb,ptr1,len1,ptr2,len2);
   }
   i = IDirectSoundBuffer_Play(lpdsb,0,0,DSBPLAY_LOOPING);
   if (i != DS_OK) strcpy(message,"Can't start playback");
   if (i == DSERR_BUFFERLOST) strcpy(message,"BUFFERLOST");
   if (i == DSERR_INVALIDCALL) strcpy(message,"INVALIDCALL");
   if (i == DSERR_INVALIDPARAM) strcpy(message,"INVALIDPARAM");
   if (i == DSERR_PRIOLEVELNEEDED) strcpy(message,"PRIOLEVELNEEDED");
   SB_BufPos1 = 1;
   SB_Playing = 1;
}

void StopPlayback(void)
{
   if (!SB_Playing) return;
   SB_Playing = 0;
   while (SB_Mixing) ;
   if (lpdsb) IDirectSoundBuffer_Stop(lpdsb);
}

void FlushPlayBuffer(void)
{
   SB_BufPPos = SB_BufPPos2;
   SB_Timer1 = 0;
}


void EndDSound(void)
{
   ReleaseDSoundBuffers();
   if (lpds) IDirectSound_Release(lpds);
}

void ReinitDSound(void)
{
   int i;

   SB_suspend = 1;

   //StopAll();
   EndDSound();

   if (i = InitDSound(mainhWnd,0)) {
      sprintf(message,"FAIL DSOUND: %d",i);
      if (i = InitDSound(mainhWnd,1)) {
         sprintf(message,"%s,%d",message,i);
      }
   }
   //if (j) StartAll(codec);

   SB_suspend = 0;
}

LONG WINAPI WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static PAINTSTRUCT ps;
    HDC hDC;

    switch(uMsg) {
    case WM_PAINT:
        hDC = BeginPaint(hWnd, &ps);
        /*display(hDC);*/
        EndPaint(hWnd, &ps);
        return 0;

    case WM_SIZE:
        return 0;

    case WM_COMMAND:
        /*switch(LOWORD(wParam)) {
        }*/
        return 0;

    case WM_NOTIFY:
        return 0;

    /*case DM_GETDEFID:
        return (DC_HASDEFID << 16) + IDM_JOIN;*/

    case WM_CLOSE:
        DestroyWindow(hWnd);
        return 0;

    case WM_DESTROY:
    PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

HWND CreateMainWindow(void)
{
    HWND        hWnd;
    WNDCLASS    wc;
    static HINSTANCE hInstance = 0;
    char fontname[32] = "Helv";

    hFont = CreateFont(8,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,
       OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
       VARIABLE_PITCH | FF_SWISS,fontname);

    /* only register the window class once - use hInstance as a flag. */
    if (!hInstance) {
    hInstance = GetModuleHandle(NULL);
    wc.style         = CS_PARENTDC;
    wc.lpfnWndProc   = (WNDPROC)WindowProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_WINLOGO);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) (COLOR_BTNFACE + 1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "MP3PLAY";

    if (!RegisterClass(&wc)) {
        MessageBox(NULL, "RegisterClass() failed:  "
               "Cannot register window class.", "Error", MB_OK);
        return NULL;
    }

    }

    hWnd = CreateWindowEx(WS_EX_DLGMODALFRAME,
       "MP3PLAY", "MP3 Player",
       WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU |
       WS_CLIPSIBLINGS | DS_3DLOOK,
       CW_USEDEFAULT, CW_USEDEFAULT, 300, 200,
       NULL, NULL, hInstance, NULL);

    if (hWnd == NULL) {
    MessageBox(NULL, "CreateWindow() failed:  Cannot create a window.",
           "Error", MB_OK);
    return NULL;
    }

    return hWnd;
}

void ProcessArgs(const char *arg)
{
   int i, j, l;
   char ch;

   l = strlen(arg);
   for (i = 0; i < l; i++) {
      if (arg[i] == ' ') continue;
      if ((arg[i] == '-') || (arg[i] == '/')) {
         i++;
         //if (toupper(arg[i]) == 'D') Dedicated = 1;
      } else {
         if (arg[i] == '"') {
            i++;
            ch = '"';
         } else
            ch = ' ';
         j = 0;
         while ((i < l) && (arg[i] != ch))
            filename[j++] = arg[i++];
         filename[j] = 0;
         if (arg[i] == '"') i++;
      }
      while (arg[i] == ' ') i++;
   }
}

int APIENTRY
WinMain(HINSTANCE hCurrentInst, HINSTANCE hPreviousInst,
    LPSTR lpszCmdLine, int nCmdShow)
{
    HWND  hWnd;             /* window */
    MSG   msg;              /* message */
    int i;

    ProcessArgs(lpszCmdLine);

    InitCommonControls();

    SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_ABOVE_NORMAL);

    for (i = 0; i < NUMEVENTS; i++)
       events[i] = CreateEvent(NULL, FALSE, FALSE, NULL);

    hWnd = CreateMainWindow();
    if (hWnd == NULL)
    exit(1);
    mainhWnd = hWnd;

    ShowWindow(hWnd, nCmdShow);

    EnumerateDevices();
    if (i = InitDSound(hWnd,0)) {
       sprintf(message,"FAIL DSOUND: %d",i);
       if (i = InitDSound(hWnd,1)) {
          sprintf(message,"%s,%d",message,i);
       }
    }
    InitDSoundBuffers();
    SB_endthread = 0;
    SB_thread = _beginthread(SB_ThreadFunc,65536,NULL);
    StartPlayback();

    if (save) outf = fopen("out.raw","wb");

    UpdateWindow(hWnd);

    while (GetMessage(&msg,NULL,0,0)) {
       if (!IsDialogMessage(hWnd, &msg)) {
          TranslateMessage(&msg);
          DispatchMessage(&msg);
       }
    }

    SB_endthread = 1;
    while (SB_endthread != 2) ;

    if (save) fclose(outf);

    StopPlayback();
    EndDSound();

    return msg.wParam;
}
