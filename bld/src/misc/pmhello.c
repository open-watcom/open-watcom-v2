//
// For OS/2 2.x:
//      wcl386 pmhello.c -bt=os2 -l=os2v2_pm
//
// For OS/2 1.x:
//      wcl pmhello.c -bt=os2 -l=os2_pm -"op stack=8k"
//

#define INCL_WIN
#include <os2.h>

HMQ             hMessageQueue;
HAB             AnchorBlock;

int main( void )
{
    AnchorBlock = WinInitialize( 0 );
    if( AnchorBlock == 0 ) return( 0 );

    hMessageQueue = WinCreateMsgQueue( AnchorBlock, 0 );
    if( hMessageQueue == 0 ) return( 0 );

    WinMessageBox( HWND_DESKTOP,
                   0,
                   "Hello World",
                   "Open Watcom Example",
                   0,
                   MB_NOICON | MB_OK );

    WinDestroyMsgQueue( hMessageQueue );
    WinTerminate( AnchorBlock );

    return( 1 );
}
