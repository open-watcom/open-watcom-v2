/*
 *  DLL32.C
 *
 *  C Compile:      wcc386 dll32 /bd /bt=windows /d2
 *  C++ Compile:    wpp386 dll32 /bd /bt=windows /d2
 *  Link:           wlink @dll32
 *  Bind:           wbind dll32 -d -n
 *  DLL32.LNK       debug all
 *                  system win386
 *                  file dll32
 *                  option map
 *                  option stack=12K
 *                  option maxdata=8K
 *                  option mindata=4K
 */
#include <windows.h>    /* required for all Windows applications */
#include <dos.h>
#include <stdio.h>
#ifdef __cplusplus
  #include <iostream.h>
  #include <strstrea.h>
#endif

extern void BreakPoint( void );
#pragma aux BreakPoint = 0xcc;

extern short GetSS( void );
#pragma aux GetSS = 0x8C 0xD0 value[ax];

int FAR PASCAL Lib1( WORD w1, DWORD w2, WORD w3 )
{
  char buf[128];

  #ifdef __cplusplus
  {
    ostrstream sout( buf, sizeof( buf ) );
    sout << hex << "Lib1: w1=" << w1 <<
                       ", w2=" << w2 <<
                       ", w3=" << w3 << ends;
    MessageBox( NULL, sout.str(), "DLL32", MB_OK | MB_TASKMODAL );
  }
  #else
    sprintf( buf, "Lib1: w1=%hx, w2=%lx, w3=%hx", w1, w2, w3 );
    MessageBox( NULL, buf, "DLL32", MB_OK | MB_TASKMODAL );
  #endif
  return( w1 + w2);
}

int FAR PASCAL Lib2( DWORD w1, WORD w2 )
{
  char buf[128];

  #ifdef __cplusplus
  {
    ostrstream sout( buf, sizeof( buf ) );
    sout << hex << "Lib2: w1=" << w1 <<
                       ", w2=" << w2 << ends;
    MessageBox( NULL, sout.str(), "DLL32", MB_OK | MB_TASKMODAL );
  }
  #else
    sprintf( buf, "Lib2: w1=%lx, w2=%hx", w1, w2 );
    MessageBox( NULL, buf, "DLL32", MB_OK | MB_TASKMODAL );
  #endif
  return( w1 + 1 );
}

int FAR PASCAL Lib3( LPSTR data, WORD a, WORD b,
                     DWORD d1, WORD c, DWORD d2 )
{
  char far *str;
  char buf[128];

  str = (char far *)MK_FP32( data );
  sprintf( buf, "Lib3: string ==>%Fs<==", str );
  MessageBox( NULL, buf, "DLL32", MB_OK | MB_TASKMODAL );
  #ifdef __cplusplus
  {
    ostrstream sout( buf, sizeof( buf ) );
    sout << hex << "Lib3: a=" << a <<
                       ", b=" << b <<
                       ", c=" << c << ends;
    MessageBox( NULL, sout.str(), "DLL32", MB_OK | MB_TASKMODAL );
  }
  #else
    sprintf( buf, "Lib3: a=%hd, b=%hd, c=%hd", a, b, c );
    MessageBox( NULL, buf, "DLL32", MB_OK | MB_TASKMODAL );
  #endif
  #ifdef __cplusplus
  {
    ostrstream sout( buf, sizeof( buf ) );
    sout << hex << "Lib3: d1=" << d1 <<
                       ", d2=" << d2 << ends;
    MessageBox( NULL, sout.str(), "DLL32", MB_OK | MB_TASKMODAL );
  }
  #else
    sprintf( buf, "Lib3: d1=%lx, d2=%lx", d1, d2 );
    MessageBox( NULL, buf, "DLL32", MB_OK | MB_TASKMODAL );
  #endif
  return( d1 + d2 );
}

int FAR PASCAL WEP( int nParameter )
{
  nParameter = nParameter;
  //BreakPoint();
  return( 1 );
}

int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE x1, LPSTR lpCmdLine, int x2 )
{
  short i;
  char buf[128];

  //BreakPoint();
  DefineDLLEntry( 1, (void *) Lib1, DLL_WORD, DLL_DWORD, DLL_WORD,
                      DLL_ENDLIST );
  DefineDLLEntry( 2, (void *) Lib2, DLL_DWORD, DLL_WORD, DLL_ENDLIST );
  DefineDLLEntry( 3, (void *) Lib3, DLL_PTR, DLL_WORD, DLL_WORD,
                      DLL_DWORD, DLL_WORD, DLL_DWORD, DLL_ENDLIST );
  DefineDLLEntry( 4, (void *) WEP, DLL_WORD,
                      DLL_ENDLIST );
  i = GetSS();
  #ifdef __cplusplus
  {
    ostrstream sout( buf, sizeof( buf ) );
    sout << "DLL32 Started, SS=" << hex << i << ends;
    MessageBox( NULL, sout.str(), "DLL32", MB_OK | MB_TASKMODAL );
  }
  #else
    sprintf( buf, "DLL32 Started, SS=%hx", i );
    MessageBox( NULL, buf, "DLL32", MB_OK | MB_TASKMODAL );
  #endif
  return( 1 );
}
