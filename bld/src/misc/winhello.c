// For Windows 3.1 16 bit:
//          wcl winhello.c -bt=windows -l=windows
// For Windows 3.1 16 bit:
//          wcl386 winhello.c -bt=windows -l=win386
//          wbind -n winhello
// For Windows NT 3.1:
//          wcl386 winhello.c -bt=nt -l=nt_win

#include <windows.h>

int PASCAL WinMain( HANDLE currinst, HANDLE previnst, LPSTR cmdline, int cmdshow )
{
    currinst = currinst;
    previnst = previnst;
    cmdline = cmdline;
    cmdshow = cmdshow;
    MessageBox( NULL, "Hello World", "WATCOM Example", MB_OK );
    return( 0 );
}
