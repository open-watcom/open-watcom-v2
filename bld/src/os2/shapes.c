#include <stdlib.h>

#define INCL_WIN
#define INCL_GPI
#include <os2.h>

int             SizeX;
int             SizeY;
HWND            FrameHandle;
HMQ             hMessageQueue;
HAB             AnchorBlock;


static int      Random( int high )
{
    return( ( (double)rand() / 32767 ) * high );
}

static void NewColor( HPS ps )
{
    GpiSetColor( ps, Random( 15 ) + 1 );
}

/* Draw a rectangular shape of random size and color at random position */
static void DrawEllipse(HWND hwndWindow)
{
    POINTL      ptl;
    HPS         ps;
    static int  Odd = 0;
    int         parm1,parm2;

    ps = WinGetPS( hwndWindow );
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

/* Client window procedure */
MRESULT EXPENTRY MainDriver( HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2 )
{
    HPS         ps;
    RECTL       rcl;

    switch( msg ) {
    case WM_CREATE:
        /* Start a 150ms timer on window creation */
        WinStartTimer( AnchorBlock, hwnd, 1, 150 ) ;
        break;
    case WM_TIMER:
        /* Draw another ellipse on each timer tick */
        DrawEllipse( hwnd );
        return( 0 );
    case WM_SIZE:
        /* Remember new dimensions when window is resized */
        SizeX = SHORT1FROMMP( mp2 );
        SizeY = SHORT2FROMMP( mp2 );
        return( 0 );
    case WM_PAINT:
        /* Handle paint events */
        ps = WinBeginPaint( hwnd, NULL, NULL );
        WinQueryWindowRect( hwnd, &rcl );
        WinFillRect( ps, &rcl, CLR_WHITE );
        WinEndPaint( ps );
        return( 0 );
    }
    /* Let the default window procedure handle all other messages */
    return( WinDefWindowProc( hwnd, msg, mp1, mp2 ) );
}

int     main()
{
    ULONG       style;
    QMSG        qmsg;
    HWND        WinHandle;

    /* Initialize windowing and create message queue */
    AnchorBlock = WinInitialize( 0 );
    if( AnchorBlock == 0 ) return( 0 );
    hMessageQueue = WinCreateMsgQueue( AnchorBlock, 0 );
    if( hMessageQueue == 0 ) return( 0 );

    /* Register window class */
    if( !WinRegisterClass( AnchorBlock, "Watcom", (PFNWP)MainDriver,
                           CS_SIZEREDRAW, 0 ) ) {
        return( 0 );
    }

    /* Create frame and client windows */
    style = FCF_TITLEBAR | FCF_SYSMENU | FCF_SIZEBORDER | FCF_MINMAX |
            FCF_SHELLPOSITION | FCF_TASKLIST;
    FrameHandle = WinCreateStdWindow( HWND_DESKTOP, WS_VISIBLE, &style,
                                      "Watcom",
                                      "Shapes - C sample",
                                      0, NULL, 0, &WinHandle );

    /* If window creation failed, exit immediately! */
    if( FrameHandle == 0 ) return( 0 );

    /* Message loop */
    while( WinGetMsg( AnchorBlock, &qmsg, NULL, 0, 0 ) ) {
        WinDispatchMsg( AnchorBlock, &qmsg );
    }

    /* Shut down and clean up */
    WinDestroyWindow( FrameHandle );
    WinDestroyMsgQueue( hMessageQueue );
    WinTerminate( AnchorBlock );

    return( 1 );
}
