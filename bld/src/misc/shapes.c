#include <stdlib.h>

#define INCL_WIN
#define INCL_GPI
#include <os2.h>

int             SizeX;
int             SizeY;
HWND            FrameHandle;
HWND            WinHandle;
HMQ             hMessageQueue;
HAB             AnchorBlock;


MRESULT EXPENTRY MainDriver( HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2 )
{
    HPS         ps;
    RECTL       rcl;

    switch( msg ) {
    case WM_CREATE:
        WinHandle = hwnd;
        WinStartTimer( AnchorBlock, WinHandle, 1, 150 ) ;
        break;
    case WM_TIMER:
        DrawEllipse();
        return( 0 );
    case WM_SIZE:
        SizeX = SHORT1FROMMP( mp2 );
        SizeY = SHORT2FROMMP( mp2 );
        return( 0 );
    case WM_PAINT:
        ps = WinBeginPaint( WinHandle, NULL, NULL );
        WinQueryWindowRect( WinHandle, &rcl );
        WinFillRect( ps, &rcl, CLR_WHITE );
        WinEndPaint( ps );
        return( 0 );
    }
    return( WinDefWindowProc( WinHandle, msg, mp1, mp2 ) );
}


int     main()
{
    ULONG       style;
    QMSG        qmsg;

    AnchorBlock = WinInitialize( 0 );
    if( AnchorBlock == 0 ) return( 0 );
    hMessageQueue = WinCreateMsgQueue( AnchorBlock, 0 );
    if( hMessageQueue == 0 ) return( 0 );
    if( !WinRegisterClass( AnchorBlock, "WATCOM", (PFNWP)MainDriver,
                           CS_SIZEREDRAW, 0 ) ) {
        return( 0 );
    }
    style = FCF_TITLEBAR | FCF_SYSMENU | FCF_SIZEBORDER | FCF_MINMAX |
            FCF_SHELLPOSITION | FCF_TASKLIST;
    FrameHandle = WinCreateStdWindow( HWND_DESKTOP, WS_VISIBLE, &style,
                                      "WATCOM", "", 0, NULL, 0,
                                      &WinHandle );
    if( FrameHandle == 0 ) return( 0 );

    while( WinGetMsg( AnchorBlock, &qmsg, NULL, 0, 0 ) ) {
        WinDispatchMsg( AnchorBlock, &qmsg );
    }

    WinDestroyWindow( FrameHandle );
    WinDestroyMsgQueue( hMessageQueue );
    WinTerminate( AnchorBlock );

    return( 1 );
}


static int      Random( int high )
{
    return( ( (double)rand() / 32767 ) * high );
}


static void NewColor( HPS ps )
{
    GpiSetColor( ps, Random( 15 ) + 1 );
}


static void DrawEllipse()
{
    POINTL      ptl;
    HPS         ps;
    static int  Odd = 0;
    int         parm1,parm2;

    ps = WinGetPS( WinHandle );
    ptl.x = Random( SizeX );
    ptl.y = Random( SizeY );
    GpiMove( ps, &ptl );
    ptl.x = Random( SizeX );
    ptl.y = Random( SizeY );
    parm1 = Random( 50 );
    parm2 = Random( 50 );
    if( Random( 10 ) >= 5 ) {
        NewColor( ps );
        GpiBox( ps, DRO_FILL, &ptl, 0, 0 );
        NewColor( ps );
        GpiBox( ps, DRO_OUTLINE, &ptl, 0, 0 );
    } else {
        NewColor( ps );
        GpiBox( ps, DRO_FILL, &ptl, parm1, parm2 );
        NewColor( ps );
        GpiBox( ps, DRO_OUTLINE, &ptl, parm1, parm2 );
    }
    Odd++;
    Odd &= 1;
    WinReleasePS( ps );
}
