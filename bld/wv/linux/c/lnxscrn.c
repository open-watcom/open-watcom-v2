/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  Screen support functions for the Linux debug client.
*
****************************************************************************/


#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#ifdef __WATCOMC__
#include <env.h>
#endif
#include <unistd.h>
#if 0
#include <sys/vt.h>
#endif
#include <sys/ioctl.h>
#include <fcntl.h>
#include <signal.h>
#include <ctype.h>
#include <errno.h>
#include "curses.h"
#define BOOL_DEFINED    // curses.h typedefs 'bool'
#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbgio.h"
#include "dbgmem.h"
#include "term.h"
#include "stdui.h"
#include "../unix/h/ctkeyb.h"
#include "dbgscrn.h"
#include "strutil.h"
#include "gui.h"
#include "guigmous.h"

extern void     StartupErr( const char * );
extern void     ReleaseProgOvlay( bool );
extern void     KillDebugger( int );
extern unsigned Lookup( const char *, const char *, unsigned );
extern void     WantEquals(void);
extern void     GetRawItem( char * );
extern char     *GetFileName( int pass );

extern char     *UITermType;

char            XConfig[2048];
char            *DbgTerminal;
unsigned        DbgConsole;
unsigned        PrevConsole;
unsigned        InitConsole = -1;
int             DbgConHandle;
int             DbgLines;
int             DbgColumns;
int             PrevLines;
int             PrevColumns;

pid_t           XTermPid;

/* definitions which should be in sys/vt.h */
#define VT_OPENQRY      0x5600  /* find available vt */
#define VT_GETSTATE     0x5603  /* get global vt state info */
#define VT_ACTIVATE     0x5606  /* make vt active */
#define VT_WAITACTIVE   0x5607  /* wait for vt active */
#define VT_DISALLOCATE  0x5608  /* free memory associated to vt */
#define VT_RESIZE       0x5609  /* set kernel's idea of screensize */

struct vt_sizes {
        unsigned short v_rows;          /* number of rows */
        unsigned short v_cols;          /* number of columns */
        unsigned short v_scrollsize;    /* number of lines of scrollback */
};

struct vt_stat {
        unsigned short v_active;        /* active vt */
        unsigned short v_signal;        /* signal to send */
        unsigned short v_state;         /* vt bitmask */
};
/* ... */

enum { C_XWIN, C_VC, C_TTY, C_CURTTY } ConMode;

void Ring_Bell( void )
{
    write( DbgConHandle, "\a", 1 );
}


/*
 * ConfigScreen -- figure out screen configuration we're going to use.
 */

unsigned ConfigScreen( void )
{
    return( 0 );
}


/*
 * InitScreen
 */
static void HupHandler( int signo )
{
    /* Xqsh has gone away -- nothing to do except die */
    signo = signo;
    ReleaseProgOvlay( TRUE );
    KillDebugger( 0 );
}

static bool TryXWindows( void )
{
    int         slavefd;
    int         masterfd;
    char        buff[64];
    char        **argv;
    char        *p;
    char        *end;
    unsigned    argc;
    char        slavename[] = "/dev/pts/xxxxxx";
    int         unlock = 0;
    char        buf;
    int         res;
    struct termios termio;

    /* we're in the X (or helper)environment */
    if ( getenv("DISPLAY") == NULL )
        return( FALSE );
    masterfd = open("/dev/ptmx", O_RDWR);
    if ( masterfd < 0 )
        return( FALSE );
    fcntl( masterfd, F_SETFD, 0 );
    ioctl( masterfd, TIOCGPTN, &slavefd ); /* slavefd = ptsname(masterfd); */
    ioctl( masterfd, TIOCSPTLCK, &unlock ); /* unlockpt(masterfd); */
    sprintf(slavename + 9, "%d", slavefd);
    slavefd = open(slavename, O_RDWR);
    DbgConHandle = slavefd;
    if( DbgConHandle == -1 ) {
        StartupErr( "unable to open debugger console" );
        return( FALSE );
    }
    tcgetattr(slavefd, &termio);
    termio.c_lflag &= ~ECHO;
    tcsetattr(slavefd, TCSANOW, &termio);
    argc = 0;
    p = XConfig;
    for( ;; ) {
        while( isspace( *p ) ) ++p;
        while( !isspace( *p ) && *p != '\0' ) ++p;
        if( *p == '\0' ) break;
        ++argc;
        *p++ = '\0';
    }
    end = p;
    _AllocA( argv, (argc + 16) * sizeof( *argv ) );

    argv[0] = "xterm";
    argv[1] = "-title";
    argv[2] = "Open Watcom Debugger";
    argv[3] = "-ut";

    argc = 4;

    if( DbgLines != 0 || DbgColumns != 0 ) {
        argv[argc++] = "-geometry";
        if( DbgLines == 0 ) DbgLines = 25;
        if( DbgColumns == 0 ) DbgColumns = 80;
        p = Format( buff, "%ux%u+0+0", DbgColumns, DbgLines ) + 1;
        argv[argc++] = buff;
    }

    for( p = XConfig; p < end; p += strlen( p ) + 1 ) {
        while( isspace( *p ) ) ++p;
        argv[argc++] = p;
    }
    Format( p, "-SXX%u", masterfd );
    argv[argc++] = p;
    argv[argc] = NULL;

    fcntl( slavefd, F_SETFD, FD_CLOEXEC );
    XTermPid = fork();
    if (XTermPid == 0) { /* child */
        setpgid( 0, 0 );
#if defined( __UNIX__ ) && !defined( __WATCOMC__ )
        execvp( argv[0], (char * const *)argv );
#else
        execvp( argv[0], (const char **)argv );
#endif
        exit( 1 );
    }
    if( XTermPid == (pid_t)-1 ) {
        StartupErr( "unable to create console helper process" );
    }
    do { /* xterm transmits a window ID -- ignore */
        res = read(slavefd, &buf, 1);
    } while ( res != -1 && buf != '\n' );
    termio.c_lflag |= ECHO;
    tcsetattr(slavefd, TCSANOW, &termio);

    /* make slavefd a controlling tty */
    setpgid( 0, XTermPid );
    setsid();
    ioctl( slavefd, TIOCSCTTY, 1 );

    signal( SIGHUP, &HupHandler );
    return( TRUE );
}

static bool TryVC( void )
{
    char                        *ptr;
    struct vt_stat vt_state;
    struct winsize winsize;
    struct vt_sizes vt_sizes;
    char tty_name[20];
    int len;

    len = readlink( "/proc/self/fd/0", tty_name, sizeof( tty_name ) - 1 );
    if ( len < 0 )
        return( FALSE );
    tty_name[ len ] = '\0';
    if( DbgConsole == 0 ) {
        DbgConHandle = open( tty_name, O_RDWR );
        if( DbgConHandle == -1 )
            return( FALSE );
        if( ioctl( DbgConHandle, VT_OPENQRY, &DbgConsole ) )
            return( FALSE );
        close( DbgConHandle );
    }
    ptr = &tty_name[ len ];
    for( ;; ) {
        --ptr;
        if( *ptr < '0' || *ptr > '9' ) break;
    }
    sprintf ( ptr + 1, "%d", DbgConsole );
    DbgConHandle = open( tty_name, O_RDWR );
    if ( DbgConHandle == -1 )
        return( FALSE );
    if( ioctl( DbgConHandle, VT_GETSTATE, &vt_state ) )
        return( FALSE );
    InitConsole = vt_state.v_active;
    ioctl( DbgConHandle, TIOCGWINSZ, &winsize );
    PrevLines = winsize.ws_row;
    PrevColumns = winsize.ws_col;
    vt_sizes.v_rows = DbgLines;
    vt_sizes.v_cols = DbgColumns;
    ioctl( DbgConHandle, VT_RESIZE, &vt_sizes );
    return( TRUE );
}

static bool TryTTY( void )
{
    unsigned long       num;
    char                *end;

    if( DbgTerminal == NULL ) return( FALSE );
    num = strtoul( DbgTerminal, &end, 10 );
    if( *end == NULLCHAR && num < 100 ) {
        DbgConsole = num;
        return( FALSE );
    }
    /* guy gave an explicit terminal name */
    end = strchr( DbgTerminal, ':' );
    if( end != NULL ) {
        /* and also told us the terminal type */
        *end = NULLCHAR;
        UITermType = strdup( end + 1 );
    }
    DbgConHandle = open( DbgTerminal, O_RDWR );
    if( DbgConHandle == -1 ) {
        StartupErr( "unable to open system console" );
    }
    return( TRUE );
}

void InitScreen( void )
{
    if( setpgid( 0, 0 ) != 0 && errno != EPERM ) {
        StartupErr( "unable to create new process group" );
    }
    if( TryTTY() ) {
        ConMode = C_TTY;
    } else if( TryVC() ) {
        ConMode = C_VC;
    } else if( TryXWindows() ) {
        ConMode = C_XWIN;
    } else {
        /* backup: just use the current terminal */
        ConMode = C_CURTTY;
        DbgConHandle = -1;
    }
    _Free( DbgTerminal );
    DbgTerminal = NULL;
    if ( DbgConHandle != -1 ) {
        fcntl( DbgConHandle, F_SETFD, (int)FD_CLOEXEC );
        UIConFile = fdopen( DbgConHandle, "w+" );
        UIConHandle = DbgConHandle;
    }
    if( !uistart() ) {
        StartupErr( "unable to initialize user interface" );
    }
    if( _IsOn( SW_USE_MOUSE ) ) GUIInitMouse( 1 );
    DebugScreen();
}


/*
 * UsrScrnMode -- setup the user screen mode
 */

bool UsrScrnMode( void )
{
    switch( ConMode ) {
    case C_TTY:
        return( TRUE );
    default:
        break;
    }
    return( FALSE );
}


void DbgScrnMode( void )
{
}


static int DebugPutc( int c )
{
    return fputc( c, UIConFile );
}

/*
 * DebugScreen -- swap/page to debugger screen
 */

bool DebugScreen( void )
{
    extern bool UserForcedTermRefresh;
    struct vt_stat vt_state;

    switch( ConMode ) {
    case C_TTY:
        return( TRUE );
    case C_CURTTY:
        _physupdate( NULL );
        UserForcedTermRefresh = TRUE;
        tputs( enter_ca_mode, 1, DebugPutc );
        break;
    case C_VC:
        ioctl( 0, VT_GETSTATE, &vt_state );
        PrevConsole = vt_state.v_active;
        ioctl( 0, VT_ACTIVATE, DbgConsole );
        ioctl( 0, VT_WAITACTIVE, DbgConsole );
        break;
    default:
        break;
    }
    return( FALSE );
}

bool DebugScreenRecover( void )
{
    return( TRUE );
}


/*
 * UserScreen -- swap/page to user screen
 */

bool UserScreen( void )
{
    switch( ConMode ) {
    case C_TTY:
        return( TRUE );
    case C_CURTTY:
        tputs( exit_ca_mode, 1, DebugPutc );
        break;
    case C_VC:
        ioctl( 0, VT_ACTIVATE, PrevConsole );
        ioctl( 0, VT_WAITACTIVE, PrevConsole );
        break;
    default:
        break;
    }
    return( FALSE );
}

void SaveMainWindowPos( void )
{
}

void FiniScreen( void )
{
    struct vt_sizes vt_sizes;

    if( _IsOn( SW_USE_MOUSE ) ) GUIFiniMouse();
    uistop();
    switch( ConMode ) {
    case C_VC:
        ioctl( 0, VT_ACTIVATE, InitConsole );
        ioctl( 0, VT_WAITACTIVE, InitConsole );
        vt_sizes.v_rows = PrevLines;
        vt_sizes.v_cols = PrevColumns;
        ioctl( 0, VT_RESIZE, &vt_sizes );
        ioctl( 0, VT_DISALLOCATE, DbgConsole );
        break;
    case C_XWIN:
        signal( SIGHUP, SIG_IGN );
        kill( XTermPid, SIGTERM );
        break;
    default:
        break;
    }
}

void ScrnSpawnStart( void )
{
    char        *term;

    if( InitConsole == -1 && UITermType != NULL ) {
        term = getenv( "TERM" );
        if( term == NULL ) term = "";
        strcpy( TxtBuff, term );
        setenv( "TERM", UITermType, 1 );
    }
}

void ScrnSpawnEnd( void )
{
    if( InitConsole == -1 && UITermType != NULL ) {
        setenv( "TERM", TxtBuff, 1 );
    }
}


/*****************************************************************************\
 *                                                                           *
 *            Replacement routines for User Interface library                *
 *                                                                           *
\*****************************************************************************/

void PopErrBox( const char *buff )
{
    WriteText( STD_ERR, buff, strlen( buff ) );
}

static const char SysOptNameTab[] = {
    "Console\0"
    "XConfig\0"
};

enum {
    OPT_CONSOLE = 1,
    OPT_XCONFIG
};

void SetNumLines( int num )
{
    if( num < 10 || num > 999 )
        num = 0;
    DbgLines = num;
}

void SetNumColumns( int num )
{
    if( num < 10 || num > 999 )
        num = 0;
    DbgColumns = num;
}

bool ScreenOption( const char *start, unsigned len, int pass )
{
    char        *p;

    pass=pass;
    switch( Lookup( SysOptNameTab, start, len ) ) {
    case OPT_CONSOLE:
        _Free( DbgTerminal );
        DbgTerminal = GetFileName( pass );
        break;
    case OPT_XCONFIG:
        WantEquals();
        p = &XConfig[ strlen( XConfig ) ];
        *p++ = ' ';
        GetRawItem( p );
        if( pass == 1 )
            XConfig[0] = NULLCHAR;
        break;
    default:
        return( false );
    }
    return( true );
}

void ScreenOptInit( void )
{
}
