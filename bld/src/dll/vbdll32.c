/*
 *  VBDLL32.C
 */
#include <stdio.h>
#include <windows.h>    /* required for all Windows applications */

long FAR PASCAL Add3( short var1, long varlong, short var2 )
{
  char buf[128];

  sprintf( buf, "Add3: var1=%d, varlong=%ld, var2=%d",
                var1, varlong, var2 );
  MessageBox( NULL, buf, "VBDLL32", MB_OK | MB_TASKMODAL );
  return( var1 + varlong + var2 );
}

long FAR PASCAL Add2( long varlong, short var2 )
{
  char buf[128];

  sprintf( buf, "Add2: varlong=%ld, var2=%d", varlong, var2 );
  MessageBox( NULL, buf, "VBDLL32", MB_OK | MB_TASKMODAL );
  return( varlong + var2 );
}

#pragma off (unreferenced);
int PASCAL WinMain(HANDLE hInstance, HANDLE x1, LPSTR lpCmdLine, int x2)
#pragma on (unreferenced);
{
  DefineDLLEntry( 1, (void *) Add3, DLL_WORD, DLL_DWORD, DLL_WORD,
                      DLL_ENDLIST );
  DefineDLLEntry( 2, (void *) Add2, DLL_DWORD, DLL_WORD, DLL_ENDLIST );
  return( 1 );
}
