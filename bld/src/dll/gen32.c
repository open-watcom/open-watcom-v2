/*
 *  GEN32.C
 *
 *  Compile:        wcc386 gen32 /bt=windows
 *  C++ Compile:    wpp386 gen32 /bt=windows
 *  Link:           wlink @gen32
 *  Bind:           wbind gen32 -n
 *  GEN32.LNK       debug all
 *                  system win386
 *                  file gen32
 *                  option map
 *                  option stack=12K
 *                  option maxdata=8K
 *                  option mindata=4K
 *                  # option symfile=gen32.sym
 */
#include <windows.h>
#ifdef __cplusplus
  #include <iostream.h>
  #include <strstrea.h>
#else
  #include <stdio.h>
#endif

/*
 *  Comment out this line if you do not want
 *  to do the 16-bit DLL call test.
 */
#define DO_16BIT_TEST

#define DLL_1 1
#define DLL_2 2
#define DLL_3 3

int PASCAL WinMain( HANDLE hInstance,
                    HANDLE hPrevInstance,
                    LPSTR lpCmdLine,
                    int nCmdShow )
{
  FARPROC fp, fp1, fp2, fpWEP;
  HANDLE hlib16, hlib32;
  HINDIR hIndira, hIndirb;
  HINDIR hIndir1, hIndir2, hIndir3, hIndirWEP;
  DWORD cb;
  char buf[128];

#ifdef DO_16BIT_TEST
  /*
   * 16-bit DLL test, calling with Microsoft C calling conventions
   */
  hlib16 = LoadLibrary( "dll16.dll" );
  fp1 = GetProcAddress( hlib16, PASS_WORD_AS_POINTER( 1 ) );
  fp2 = GetProcAddress( hlib16, PASS_WORD_AS_POINTER( 2 ) );

  hIndira = GetIndirectFunctionHandle( fp1, INDIR_WORD,
              INDIR_DWORD, INDIR_WORD, INDIR_WORD,
              INDIR_DWORD, INDIR_ENDLIST );
  hIndirb = GetIndirectFunctionHandle( fp2, INDIR_CDECL, INDIR_WORD,
              INDIR_DWORD, INDIR_WORD, INDIR_WORD,
              INDIR_DWORD, INDIR_ENDLIST );

  cb = InvokeIndirectFunction( hIndira,
                               0x1, 0x11110000, 0x1100, 0x10,
                               0x22222222 );
  #ifdef __cplusplus
  {
    ostrstream sout( buf, sizeof( buf ) );
    sout << "RC = " << hex << cb << ends;
    MessageBox( NULL, sout.str(), "Gen32", MB_OK | MB_TASKMODAL );
  }
  #else
    sprintf( buf, "RC = %lx", cb );
    MessageBox( NULL, buf, "Gen32", MB_OK | MB_TASKMODAL );
  #endif

  cb = InvokeIndirectFunction( hIndirb,
                               0x1, 0x11110000, 0x1100, 0x10,
                               0x22222222 );
  #ifdef __cplusplus
  {
    ostrstream sout( buf, sizeof( buf ) );
    sout << "RC (cdecl) = " << hex << cb << ends;
    MessageBox( NULL, sout.str(), "Gen32", MB_OK | MB_TASKMODAL );
  }
  #else
    sprintf( buf, "RC (cdecl) = %lx", cb );
    MessageBox( NULL, buf, "Gen32", MB_OK | MB_TASKMODAL );
  #endif
  FreeLibrary( hlib16 );
#endif

  /*
   * 32-bit DLL test
   */
  hlib32 = LoadLibrary( "dll32.dll" );
  fp = GetProcAddress( hlib32, "Win386LibEntry" );
  fpWEP = GetProcAddress( hlib32, "WEP" );

  hIndir1 = GetIndirectFunctionHandle( fp, INDIR_WORD, INDIR_DWORD,
              INDIR_WORD, INDIR_WORD, INDIR_ENDLIST );
  hIndir2 = GetIndirectFunctionHandle( fp, INDIR_DWORD, INDIR_WORD,
              INDIR_WORD, INDIR_ENDLIST );
  hIndir3 = GetIndirectFunctionHandle( fp, INDIR_PTR, INDIR_WORD, INDIR_WORD,
          INDIR_DWORD, INDIR_WORD, INDIR_DWORD, INDIR_WORD, INDIR_ENDLIST );
  hIndirWEP = GetIndirectFunctionHandle( fpWEP, INDIR_WORD,
              INDIR_ENDLIST );

  cb = InvokeIndirectFunction( hIndir1, 0x666, 0x77777111, 0x6969, DLL_1 );
  #ifdef __cplusplus
  {
    ostrstream sout( buf, sizeof( buf ) );
    sout << "RC1 = " << hex << cb << ends;
    MessageBox( NULL, sout.str(), "Gen32", MB_OK | MB_TASKMODAL );
  }
  #else
    sprintf( buf, "RC1 = %lx", cb );
    MessageBox( NULL, buf, "Gen32", MB_OK | MB_TASKMODAL );
  #endif

  cb = InvokeIndirectFunction( hIndir2, 0x12345678, 0x8888, DLL_2 );
  #ifdef __cplusplus
  {
    ostrstream sout( buf, sizeof( buf ) );
    sout << "RC2 = " << hex << cb << ends;
    MessageBox( NULL, sout.str(), "Gen32", MB_OK | MB_TASKMODAL );
  }
  #else
    sprintf( buf, "RC2 = %lx", cb );
    MessageBox( NULL, buf, "Gen32", MB_OK | MB_TASKMODAL );
  #endif

  cb = InvokeIndirectFunction( hIndir3, "A Test String", 1, 2,
                               0xabcddcba, 3, 0x12344321, DLL_3 );
  #ifdef __cplusplus
  {
    ostrstream sout( buf, sizeof( buf ) );
    sout << "RC3 = " << hex << cb << ends;
    MessageBox( NULL, sout.str(), "Gen32", MB_OK | MB_TASKMODAL );
  }
  #else
    sprintf( buf, "RC3 = %lx", cb );
    MessageBox( NULL, buf, "Gen32", MB_OK | MB_TASKMODAL );
  #endif

  /*
   * this function call invokes the Supervisor WEP function
   */
  cb = InvokeIndirectFunction( hIndirWEP, 1 );
  #ifdef __cplusplus
  {
    ostrstream sout( buf, sizeof( buf ) );
    sout << "RC WEP = " << hex << cb << ends;
    MessageBox( NULL, sout.str(), "Gen32", MB_OK | MB_TASKMODAL );
  }
  #else
    sprintf( buf, "RC WEP = %lx", cb );
    MessageBox( NULL, buf, "Gen32", MB_OK | MB_TASKMODAL );
  #endif

  FreeLibrary( hlib32 );

  return( 0 );
}
