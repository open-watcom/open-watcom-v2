.chap *refid=os2pm Programming for OS/2 Presentation Manager
.*
:set symbol="winsys" value="Presentation Manager".
:set symbol="oprompt"    value=&prompt.
:set symbol="prompt"     value="[C:\]".
.*
.np
.ix 'OS/2 Presentation Manager'
Basically, there are two classes of &lang applications that can run in
a windowed environment.
.np
The first are those &lang applications that do not use any of the
&winsys API functions; they are strictly &lang applications that do
not rely on the features of a particular operating system.
.np
The second class of &lang applications are those that actually call
&winsys API functions directly.
These are applications that have been tailored for the &winsys
operating environment.
.np
It is assumed that the reader is familiar with the concepts of &winsys
programming.
.*
.section Porting Existing &lang Applications
.*
.np
.ix 'OS/2 PM' 'non-GUI applications'
Suppose you have a set of &lang applications that previously ran under
DOS and you now wish to run them under OS/2.
To achieve this, simply recompile your application and link with the
appropriate libraries.
Depending on the method with which you linked your application, it can
run in an OS/2 fullscreen environment, a PM-compatible window, or as a
Presentation Manager application.
An OS/2 fullscreen application runs in its own screen group.
A PM-compatible application will run in an OS/2 fullscreen environment
or in a window in the Presentation Manager screen group but does not
take direct advantage of menus, mouse or other features available in
the Presentation Manager.
A Presentation Manager application has full access to the complete set
of user-interface tools such as menus, icons, scroll bars, etc. However,
porting a console oriented application to Presentation Manager often
requires significant effort and a substantial redesign of the application.
.*
.if '&defwinos2' eq '1' .do begin
An application that was not designed as a windowed application (such
as a DOS application) can run as a Presentation Manager application.
This is achieved by a default windowing system that is optionally
linked with your application.
The following sections describe the default windowing system.
.do end
.*
.beglevel
.*
.if '&defwinos2' eq '1' .do begin
.im winio
.do end
.*
.section An Example
.*
.np
.ix 'OS/2 PM' 'non-GUI example'
Very little effort is required to port an existing &lang application
to OS/2.
Let us try to run the following sample program (contained in the file
.fi hello.&langsuff
.ct ).
.if '&lang' eq 'FORTRAN 77' .do begin
.millust begin
    print *, 'Hello world!'
    end
.millust end
.do end
.if '&lang' eq 'C' .do begin
.millust begin
#include <stdio.h>

int main( void )
/**************/
{
    printf( "Hello world!\n" );
    return( 0 );
}
.millust end
.do end
.if '&lang' eq 'C/C++' .do begin
.millust begin
#include <stdio.h>

int main( void )
/**************/
{
    printf( "Hello world!\n" );
    return( 0 );
}
.millust end
.np
An equivalent C++ program follows:
.millust begin
#include <iostream.h>

int main( void )
{
    cout << "Hello world" << endl;
    return( 0 );
}
.millust end
.do end
.np
First we must compile the file
.fi hello.&langsuff
by issuing the following command.
.millust begin
&prompt.&ccmd32 hello
.millust end
.np
Once we have successfully compiled the file, we can link it by issuing
the following command.
.millust begin
&prompt.&lnkcmd sys os2v2 file hello
.millust end
.np
It is also possible to compile and link in one step, by issuing the
following command.
.millust begin
&prompt.&wclcmd32 &sw.l=os2v2 hello
.millust end
.np
This will create a PM-compatible application.
If you wish to create a fullscreen application, link with the
following command.
.millust begin
&prompt.&lnkcmd sys os2v2 fullscreen file hello
.millust end
.*
.if '&defwinos2' eq '1' .do begin
.np
If you wish to use the default windowing system, you must recompile
your application and specify a special option, namely "bw".
.millust begin
&prompt.&ccmd32 -bw hello
.millust end
.np
We now link our application with the following command.
.millust begin
&prompt.&lnkcmd sys os2v2_pm file hello
.millust end
.endlevel
.*
.im pgdwlib
.do end
.el .do begin
.endlevel
.do end
.*
.section Calling Presentation Manager API Functions
.*
.np
.ix 'OS/2 PM' 'API calls'
It is also possible for a &lang application to create its own
windowing environment.
This is achieved by calling PM API functions directly from your &lang
program.
The techniques for developing these applications can be found in the
.book OS/2 Technical Library.
.*
.if '&lang' eq 'C/C++' .do begin
.np
A number of &lang include files (files with extension
.fi &hxt
.ct ) are
provided which define Presentation Manager data structures and
constants.
They are located in the
.fi &pathnam.&pc.h&pc.os2
directory.
These include files are roughly equivalent to the C/C++ header files that
are available with the IBM OS/2 Developer's Toolkit.
.np
A sample C/C++ Presentation Manager application is also located in the
.fi &pathnam.&pc.samples&pc.os2
directory.
It is contained in the files
.fi shapes.c
(C variant) and
.fi shapes.cpp
(C++ variant, nearly identical).
The file
.fi shapes.c
contains the following.
.code begin
#include <stdlib.h>

#define INCL_WIN
#define INCL_GPI
#include <os2.h>

.code break
int             SizeX;
int             SizeY;
HWND            FrameHandle;
HMQ             hMessageQueue;
HAB             AnchorBlock;

.code break
static int      Random( int high )
{
    return( ( (double)rand() / 32767 ) * high );
}

static void NewColor( HPS ps )
{
    GpiSetColor( ps, Random( 15 ) + 1 );
}

.code break
/* Draw a rectangular shape of random size and color at random position */
static void DrawEllipse(HWND hwndWindow)
{
    POINTL      ptl;
    HPS         ps;
    static int  Odd = 0;
    int         parm1,parm2;

.code break
    ps = WinGetPS( hwndWindow );
    ptl.x = Random( SizeX );
    ptl.y = Random( SizeY );
    GpiMove( ps, &ptl );
    ptl.x = Random( SizeX );
    ptl.y = Random( SizeY );
    parm1 = Random( 50 );
    parm2 = Random( 50 );
.code break
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
.code break
    Odd++;
    Odd &= 1;
    WinReleasePS( ps );
}

.code break
/* Client window procedure */
MRESULT EXPENTRY MainDriver( HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2 )
{
    HPS         ps;
    RECTL       rcl;

    switch( msg ) {
.code break
    case WM_CREATE:
        /* Start a 150ms timer on window creation */
        WinStartTimer( AnchorBlock, hwnd, 1, 150 ) ;
        break;
    case WM_TIMER:
        /* Draw another ellipse on each timer tick */
        DrawEllipse( hwnd );
        return( 0 );
.code break
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
.code break
    }
    /* Let the default window procedure handle all other messages */
    return( WinDefWindowProc( hwnd, msg, mp1, mp2 ) );
}

.code break
int     main()
{
    ULONG       style;
    QMSG        qmsg;
    HWND        WinHandle;

.code break
    /* Initialize windowing and create message queue */
    AnchorBlock = WinInitialize( 0 );
    if( AnchorBlock == 0 ) return( 0 );
    hMessageQueue = WinCreateMsgQueue( AnchorBlock, 0 );
    if( hMessageQueue == 0 ) return( 0 );

.code break
    /* Register window class */
    if( !WinRegisterClass( AnchorBlock, "Watcom", (PFNWP)MainDriver,
                           CS_SIZEREDRAW, 0 ) ) {
        return( 0 );
    }

.code break
    /* Create frame and client windows */
    style = FCF_TITLEBAR | FCF_SYSMENU | FCF_SIZEBORDER | FCF_MINMAX |
            FCF_SHELLPOSITION | FCF_TASKLIST;
    FrameHandle = WinCreateStdWindow( HWND_DESKTOP, WS_VISIBLE, &style,
                                      "Watcom",
                                      "Shapes - C sample",
                                      0, NULL, 0, &WinHandle );

.code break
    /* If window creation failed, exit immediately! */
    if( FrameHandle == 0 ) return( 0 );

    /* Message loop */
    while( WinGetMsg( AnchorBlock, &qmsg, NULL, 0, 0 ) ) {
        WinDispatchMsg( AnchorBlock, &qmsg );
    }

.code break
    /* Shut down and clean up */
    WinDestroyWindow( FrameHandle );
    WinDestroyMsgQueue( hMessageQueue );
    WinTerminate( AnchorBlock );

    return( 1 );
}
.code end
.np
You can compile, link and run this demonstration by issuing the following
commands.
.millust begin
&prompt.&wclcmd32 &sw.l=os2v2_pm shapes
&prompt.shapes
.millust end
.do end
.*
.if '&lang' eq 'FORTRAN 77' .do begin
.np
A number of &lang include files (files with extension
.fi &hxt
or
.fi ~.fap
.ct ) are
provided which define Presentation Manager data structures and
constants.
They are located in the
.fi &pathnam.&pc.src&pc.fortran&pc.os2
directory.
These include files are equivalent to the C header files that are
available with the IBM OS/2 Developer's Toolkit.
.np
A sample FORTRAN 77 Presentation Manager application is also located in the
.fi &pathnam.&pc.samples&pc.fortran&pc.os2
directory.
It is contained in the files
.fi fshapes.for
and
.fi fshapes.fi.
The file
.fi fshapes.for
contains the following.
.code begin
c$define INCL_WINFRAMEMGR
c$define INCL_WINMESSAGEMGR
c$define INCL_WINWINDOWMGR
c$define INCL_WINTIMER
c$define INCL_GPIPRIMITIVES
c$include os2.fap

.code break
        program fshapes

        integer         style
        record /QMSG/   qmsg

        character*7     watcom
        parameter       (watcom='WATCOM'c)

        include 'fshapes.fi'

.code break
        AnchorBlock = WinInitialize( 0 )
        if( AnchorBlock .eq. 0 ) stop
        hMessageQueue = WinCreateMsgQueue( AnchorBlock, 0 )
        if( hMessageQueue .eq. 0 ) stop
        if( WinRegisterClass( AnchorBlock, watcom, MainDriver,
     +                        CS_SIZEREDRAW, 0 ) .eq. 0 ) stop
        style = FCF_TITLEBAR .or. FCF_SYSMENU .or. FCF_SIZEBORDER .or.
     +          FCF_MINMAX .or. FCF_SHELLPOSITION .or. FCF_TASKLIST
        FrameHandle = WinCreateStdWindow( HWND_DESKTOP, WS_VISIBLE,
     +                                    style, watcom,
     +                                    char(0), 0, NULL,
     +                                    0, WinHandle )
.code break
        if( FrameHandle .eq. 0 ) stop

        while( WinGetMsg( AnchorBlock, qmsg, NULL, 0, 0 ) ) do
            call WinDispatchMsg( AnchorBlock, qmsg )
        end while

        call WinDestroyWindow( FrameHandle )
        call WinDestroyMsgQueue( hMessageQueue )
        call WinTerminate( AnchorBlock )

        end

.code break
        function MainDriver( hwnd, msg, mp1, mp2 )

        integer hwnd
        integer msg
        integer mp1
        integer mp2

.code break
        include 'fshapes.fi'

        integer         ps
        record /RECTL/  rcl

        select case ( msg )
        case ( WM_CREATE )
            WinHandle = hwnd
            call WinStartTimer( AnchorBlock, WinHandle, 1, 150 )
        case ( WM_TIMER )
            call DrawEllipse()
            MainDriver = 0
            return
.code break
        case ( WM_SIZE )
            SizeX = SHORT1FROMMP( mp2 )
            SizeY = SHORT2FROMMP( mp2 )
            MainDriver = 0
            return
        case ( WM_PAINT )
            ps = WinBeginPaint( WinHandle, NULL, NULL_POINTER )
            call WinQueryWindowRect( WinHandle, rcl )
            call WinFillRect( ps, rcl, CLR_WHITE )
            call WinEndPaint( ps )
            MainDriver = 0
            return
        end select

.code break
        MainDriver = WinDefWindowProc( WinHandle, msg, mp1, mp2 )
        return

        end

.code break
        subroutine DrawEllipse

        record /POINTL/         ptl
        integer                 ps
        integer                 Odd /0/
        integer                 parm1
        integer                 parm2

        include 'fshapes.fi'

.code break
        ps = WinGetPS( WinHandle )
        ptl.x = Random( SizeX )
        ptl.y = Random( SizeY )
        call GpiMove( ps, ptl )
        ptl.x = Random( SizeX )
        ptl.y = Random( SizeY )
        parm1 = Random( 32767 )
        parm2 = Random( 32767 )
.code break
        if( Random( 10 ) .ge. 5 ) then
            execute NewColor
            call GpiBox( ps, DRO_FILL, ptl, 0, 0 )
            execute NewColor
            call GpiBox( ps, DRO_OUTLINE, ptl, 0, 0 )
        else
            execute NewColor
            call GpiBox( ps, DRO_FILL, ptl, parm1, parm2 )
            execute NewColor
            call GpiBox( ps, DRO_OUTLINE, ptl, parm1, parm2 )
        end if

.code break
        Odd = Odd + 1
        Odd = Odd .and. 1
        call WinReleasePS( ps )

        remote block NewColor
        call GpiSetColor( ps, Random( 15 ) + 1 )
        end block

        end

.code break
        integer function Random( high )
        integer         high

        external        urand
        real            urand
        integer         seed /75347/
        Random = urand( seed ) * high

        end
.code end
.np
The include file
.fi fshapes.fi
contains the following.
.code begin
        include 'os2.fi'

        integer         SizeX
        integer         SizeY
        integer         FrameHandle
        integer         WinHandle
        integer         hMessageQueue
        integer         AnchorBlock

.code break
        common /globals/
     +  SizeX,
     +  SizeY,
     +  FrameHandle,
     +  WinHandle,
     +  hMessageQueue,
     +  AnchorBlock

.code break
        external                Random
        integer                 Random

        external                MainDriver
        integer                 MainDriver
c$pragma aux (FNWP)             MainDriver
.code end
.np
.autonote Notes:
.note
Include files with extension
.fi ~.fap
define the calling conventions for each of the OS/2 API functions.
These files must be included at the top of each FORTRAN 77 source module.
.note
Include files with extension
.fi &hxt
define the data structures and constants
used by the OS/2 API functions.
These files must be included in each subprogram that requires them.
.note
Each call-back function (i.e. window procedure) must be defined using the
following pragma.
.millust begin
c$pragma aux (FNWP) WindowProc
.millust end
.note
The include file
.fi os2.fap
is included at the beginning
of the source file and
.fi os2.fi
is included in each subprogram.
Also note that a number of macros were defined at the top of the file.
By defining these macros, only those components of the OS/2 API required
by the module will be compiled.
.endnote
.np
You can compile, link and run this demonstration by issuing the following
commands.
.millust begin
&prompt.&setcmd finclude=&pathnam.&pc.src&pc.fortran&pc.os2
&prompt.&wclcmd32 &sw.l=os2v2_pm fshapes
&prompt.fshapes
.millust end
.do end
:set symbol="prompt"     value=&oprompt.
