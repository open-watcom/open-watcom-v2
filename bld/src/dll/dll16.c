/*
 *  DLL16.C
 *
 *  Compile:        wcc dll16 /mc /zu /zc /bd /bt=windows /d2
 *  C++ Compile:    wpp dll16 /mc /zu /zc /bd /bt=windows /d2
 *  Link:           wlink @dll16
 *  DLL16.LNK:      debug all
 *                  system windows dll initinstance memory
 *                  file dll16
 *                  option oneautodata
 *                  option map
 *                  option heapsize=32K
 *                  libfile libentry
 */
#include <stddef.h>
#ifdef __cplusplus
  #include <iostream.h>
  #include <strstrea.h>
#else
  #include <stdio.h>
#endif
#include <windows.h>

extern void BreakPoint( void );
#pragma aux BreakPoint = 0xcc;

extern short GetSS( void );
#pragma aux GetSS = 0x8C 0xD0 value[ax];

#pragma off (unreferenced);
BOOL FAR PASCAL LibMain( HANDLE hInstance, WORD wDataSegment,
                         WORD wHeapSize, LPSTR lpszCmdLine )
#pragma on (unreferenced);
{
  short i;
  char buf[128];

  //BreakPoint();
  i = GetSS();
  #ifdef __cplusplus
  {
    ostrstream sout( buf, sizeof( buf ) );
    sout << "DLL16 Started, SS=" << hex << i << ends;
    MessageBox( NULL, sout.str(), "DLL16", MB_OK | MB_TASKMODAL );
  }
  #else
    sprintf( buf, "DLL16 Started, SS=%hx", i );
    MessageBox( NULL, buf, "DLL16", MB_OK | MB_TASKMODAL );
  #endif
  return( 1 );
}

#ifdef __cplusplus
extern "C"
#endif
long __export FAR PASCAL Lib1( WORD a, LONG b, WORD c, WORD d, LONG e )
{
  char buf[128];

  //BreakPoint();
  #ifdef __cplusplus
  {
    ostrstream sout( buf, sizeof( buf ) );
    sout << hex << "Lib1: a=" << a <<
                       ", b=" << b <<
                       ", c=" << c <<
                       ", d=" << d <<
                       ", e=" << e << ends;
    MessageBox( NULL, sout.str(), "DLL16", MB_OK | MB_TASKMODAL );
  }
  #else
    sprintf( buf, "Lib1: a=%hx, b=%lx, c=%hx, d=%hx, e=%lx",
                  a, b, c, d, e );
    MessageBox( NULL, buf, "DLL16", MB_OK | MB_TASKMODAL );
  #endif
  return( a + b + c + d + e );
}

#ifdef __cplusplus
extern "C"
#endif
long __export FAR CDECL Lib2( WORD a, LONG b, WORD c, WORD d, LONG e )
{
  char buf[128];

  //BreakPoint();
  #ifdef __cplusplus
  {
    ostrstream sout( buf, sizeof( buf ) );
    sout << hex << "Lib2: a=" << a <<
                       ", b=" << b <<
                       ", c=" << c <<
                       ", d=" << d <<
                       ", e=" << e << ends;
    MessageBox( NULL, sout.str(), "DLL16", MB_OK | MB_TASKMODAL );
  }
  #else
    sprintf( buf, "Lib2: a=%hx, b=%lx, c=%hx, d=%hx, e=%lx",
                  a, b, c, d, e );
    MessageBox( NULL, buf, "DLL16", MB_OK | MB_TASKMODAL );
  #endif
  return( a + b + c + d + e );
}

#pragma off (unreferenced);
int __export FAR PASCAL WEP( int nParameter )
#pragma on (unreferenced);
{
  //BreakPoint();
  return( 1 );
}

