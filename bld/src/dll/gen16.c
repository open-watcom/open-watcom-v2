/*
 *  GEN16.C
 *
 *  C Compile:      wcc gen16 /bt=windows
 *  C++ Compile:    wpp gen16 /bt=windows
 *  Link:           wlink @gen16
 *  GEN16.LNK       debug all
 *                  system windows memory font
 *                  file gen16
 *                  option map
 *                  option stack=8K
 *                  option heapsize=32K
 */
#include <windows.h>
#ifdef __cplusplus
  #include <iostream.h>
  #include <strstrea.h>
#else
  #include <stdio.h>
#endif

#define DLL_1 1
#define DLL_2 2
#define DLL_3 3

/*
 *  Comment out this line if you do not want
 *  to do the 16-bit DLL call test.
 */
#define DO_16BIT_TEST

typedef long (FAR PASCAL *FARPROC2)();
typedef long (FAR PASCAL *FARPROC2_Lib1)( WORD, LONG, WORD, WORD, LONG );
typedef long (FAR PASCAL *FARPROC2_WIN386_Lib1)( WORD, DWORD, WORD, int );
typedef long (FAR PASCAL *FARPROC2_WIN386_Lib2)( DWORD, WORD, int );
typedef long (FAR PASCAL *FARPROC2_WIN386_Lib3)( LPSTR, WORD, WORD, DWORD, WORD, DWORD, int );
typedef long (FAR PASCAL *FARPROC2_WIN386_WEP)( int );
#ifdef __cplusplus
  typedef long (FAR CDECL *FARPROC2C)(...);
#else
  typedef long (FAR CDECL *FARPROC2C)();
#endif

int PASCAL WinMain( HANDLE hInstance,
                    HANDLE hPrevInstance,
                    LPSTR lpCmdLine,
                    int nCmdShow )
{
  FARPROC2 fp;
  FARPROC2_WIN386_WEP fp32WEP;
  FARPROC2C fpc;
  HANDLE hlib16, hlib32;
  DWORD cb;
  char buf[128];

#ifdef DO_16BIT_TEST
  /*
   * 16-bit DLL test, calling with Microsoft C calling conventions
   */
  hlib16 = LoadLibrary( "dll16.dll" );
  fp = (FARPROC2) GetProcAddress( hlib16, "Lib1" );
  fpc = (FARPROC2C) GetProcAddress( hlib16, "_Lib2" );

  cb = (*(FARPROC2_Lib1)fp)( 0x1, 0x11110000, 0x1100, 0x10, 0x22222222 );
  #ifdef __cplusplus
  {
    ostrstream sout( buf, sizeof( buf ) );
    sout << "RC = " << hex << cb << ends;
    MessageBox( NULL, sout.str(), "Gen16", MB_OK | MB_TASKMODAL );
  }
  #else
    sprintf( buf, "RC = %lx", cb );
    MessageBox( NULL, buf, "Gen16", MB_OK | MB_TASKMODAL );
  #endif

  cb = fpc( 0x1, 0x11110000, 0x1100, 0x10, 0x22222222 );
  #ifdef __cplusplus
  {
    ostrstream sout( buf, sizeof( buf ) );
    sout << "RC (cdecl) = " << hex << cb << ends;
    MessageBox( NULL, sout.str(), "Gen16", MB_OK | MB_TASKMODAL );
  }
  #else
    sprintf( buf, "RC (cdecl) = %lx", cb );
    MessageBox( NULL, buf, "Gen16", MB_OK | MB_TASKMODAL );
  #endif
  FreeLibrary( hlib16 );
#endif

  /*
   * 32-bit DLL test
   */
  hlib32 = LoadLibrary( "dll32.dll" );
  fp = (FARPROC2) GetProcAddress( hlib32, "Win386LibEntry" );
  fp32WEP = (FARPROC2_WIN386_WEP) GetProcAddress( hlib32, "WEP" );

  cb = (*(FARPROC2_WIN386_Lib1)fp)( 0x666, 0x77777111, 0x6969, DLL_1 );
  #ifdef __cplusplus
  {
    ostrstream sout( buf, sizeof( buf ) );
    sout << "RC1 = " << hex << cb << ends;
    MessageBox( NULL, sout.str(), "Gen16", MB_OK | MB_TASKMODAL );
  }
  #else
    sprintf( buf, "RC1 = %lx", cb );
    MessageBox( NULL, buf, "Gen16", MB_OK | MB_TASKMODAL );
  #endif

  cb = (*(FARPROC2_WIN386_Lib2)fp)( 0x12345678, 0x8888, DLL_2 );
  #ifdef __cplusplus
  {
    ostrstream sout( buf, sizeof( buf ) );
    sout << "RC2 = " << hex << cb << ends;
    MessageBox( NULL, sout.str(), "Gen16", MB_OK | MB_TASKMODAL );
  }
  #else
    sprintf( buf, "RC2 = %lx", cb );
    MessageBox( NULL, buf, "Gen16", MB_OK | MB_TASKMODAL );
  #endif

  cb = (*(FARPROC2_WIN386_Lib3)fp)( (char far *) "A Test String", 1, 2 , 0xabcddcba, 3,
              0x12344321, DLL_3 );
  #ifdef __cplusplus
  {
    ostrstream sout( buf, sizeof( buf ) );
    sout << "RC3 = " << hex << cb << ends;
    MessageBox( NULL, sout.str(), "Gen16", MB_OK | MB_TASKMODAL );
  }
  #else
    sprintf( buf, "RC3 = %lx", cb );
    MessageBox( NULL, buf, "Gen16", MB_OK | MB_TASKMODAL );
  #endif

  /*
   * this function call invokes the Supervisor WEP function
   */
  cb = fp32WEP( 1 );
  #ifdef __cplusplus
  {
    ostrstream sout( buf, sizeof( buf ) );
    sout << "RC WEP = " << hex << cb << ends;
    MessageBox( NULL, sout.str(), "Gen16", MB_OK | MB_TASKMODAL );
  }
  #else
    sprintf( buf, "RC WEP = %lx", cb );
    MessageBox( NULL, buf, "Gen16", MB_OK | MB_TASKMODAL );
  #endif

  FreeLibrary( hlib32 );

  return( 0 );
}
