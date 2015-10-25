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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/console.h>
#include <sys/dev.h>
#include <sys/psinfo.h>
#include <sys/sidinfo.h>
#include <sys/kernel.h>
#include <sys/vc.h>
#include <fcntl.h>
#include <process.h>
#include <signal.h>
#include <ctype.h>
#include <errno.h>
#include <env.h>
#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbgio.h"
#include "dbgmem.h"
#include "stdui.h"
#include "dbgscrn.h"
#include "strutil.h"
#include "guigmous.h"
#include "dbgcmdln.h"
#include "dbgprog.h"
#include "dbginit.h"


extern void         StartupErr( const char * );
extern int          Lookup( const char *, const char *, size_t );

extern unsigned     UIConHandle;
extern char         *UITermType;

char                XConfig[2048];
char                *DbgTerminal;
unsigned            DbgConsole;
unsigned            PrevConsole;
unsigned            InitConsole;
int                 DbgConHandle;
int                 DbgLines;
int                 DbgColumns;
int                 PrevLines;
int                 PrevColumns;
struct _console_ctrl *ConCtrl;

pid_t                   XQshPid;

enum { C_XWIN, C_QCON, C_TTY } ConMode;

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
    char        xqsh_name[CMD_LEN];
    int         pip[2];
    char        buff[64];
    char        **argv;
    int         len;
    char        *p;
    char        *end;
    unsigned    argc;

    /* we're in the X Windows (or helper)environment */
    if( pipe( pip ) != 0 ) {
        StartupErr( "unable to create console control channel" );
    }
    fcntl( pip[0], F_SETFD, (int)FD_CLOEXEC );
    searchenv( "qnxterm", "PATH", xqsh_name );
    if( xqsh_name[0] == '\0' ) {
        StartupErr( "qnxterm executable not in PATH" );
    }
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
    _AllocA( argv, (argc + 10) * sizeof( *argv ) );

    argv[0] = xqsh_name;
    argv[1] = "-T";
    argv[2] = "WATCOM Debugger";

    argc = 3;

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
    argv[argc++] = "-tty";
    Format( p, "%u", pip[1] );
    argv[argc++] = p;
    argv[argc] = NULL;

    XQshPid = qnx_spawn( 0, 0, 0, -1, -1, _SPAWN_NEWPGRP,
                argv[0], argv, environ, NULL, 0 );
    if( XQshPid == (pid_t)-1 ) {
        StartupErr( "unable to create console helper process" );
    }
    /* close the write pipe here so that the read fails if xqsh aborts */
    close( pip[1] );
    len = read( pip[0], buff, sizeof( buff ) );
    if( len == -1 ) {
        StartupErr( "console helper process unable to initialize" );
    }
    close( pip[0] );
    buff[len] = '\0';
    DbgConHandle = open( buff, O_RDWR );
    if( DbgConHandle == -1 ) {
        StartupErr( "unable to open debugger console" );
    }
    UITermType = "qnx";
    tcsetct( DbgConHandle, getpid() );
    signal( SIGHUP, &HupHandler );
    return( TRUE );
}


static bool TryQConsole( void )
{
    struct _psinfo              psinfo;
    struct _sidinfo             info;
    struct _dev_info_entry      dev;
    char                        *ptr;
    char                        *term;

    if( qnx_psinfo( PROC_PID, getpid(), &psinfo, 0, 0 ) != getpid() ) {
        StartupErr( "unable to obtain process information" );
    }
    if( qnx_sid_query( PROC_PID, psinfo.sid, &info ) != psinfo.sid ) {
        StartupErr( "unable to obtain console name" );
    }
    ptr = &info.tty_name[ strlen( info.tty_name ) ];
    for( ;; ) {
        --ptr;
        if( *ptr < '0' || *ptr > '9' ) break;
    }
    if( DbgConsole != 0 ) {
        ptr[ 1 ] = '0' + DbgConsole / 10;
        ptr[ 2 ] = '0' + DbgConsole % 10;
        ptr += 2;
    }
    ptr[1] = NULLCHAR;
    DbgConHandle = open( info.tty_name, O_RDWR );
    if( DbgConHandle == -1 ) {
        StartupErr( "unable to open system console" );
    }
    term = getenv( "TERM" );
    if( term != NULL && strcmp( term, "qnxw" ) == 0 ) {
        /* in QNX windows */
#define PROP_STRING     "\033\"pwd\""
        write( DbgConHandle, PROP_STRING, sizeof( PROP_STRING ) - 1 );
    }

    ConCtrl = console_open( DbgConHandle, O_WRONLY );
    if( ConCtrl == NULL ) {
        close( DbgConHandle );
        return( FALSE );
    }
    if( dev_info( DbgConHandle, &dev ) == -1 ) {
        StartupErr( "unable to obtain console information" );
    }
    DbgConsole = dev.unit;
    console_size( ConCtrl, DbgConsole, 0, 0, &PrevLines, &PrevColumns );
    console_size( ConCtrl, DbgConsole, DbgLines, DbgColumns, 0, 0 );
    InitConsole = console_active( ConCtrl, -1 );
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
    } else if( TryQConsole() ) {
        ConMode = C_QCON;
    } else if( TryXWindows() ) {
        ConMode = C_XWIN;
    } else {
        StartupErr( "unable to initialize debugger screen" );
    }
    _Free( DbgTerminal );
    DbgTerminal = NULL;
    fcntl( DbgConHandle, F_SETFD, (int)FD_CLOEXEC );
    UIConHandle = DbgConHandle;
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
    }
    return( FALSE );
}


void DbgScrnMode( void )
{
}


/*
 * DebugScreen -- swap/page to debugger screen
 */

bool DebugScreen( void )
{
    switch( ConMode ) {
    case C_TTY:
        return( TRUE );
    case C_QCON:
        PrevConsole = console_active( ConCtrl, DbgConsole );
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
    case C_QCON:
        console_active( ConCtrl, PrevConsole );
        break;
    }
    return( FALSE );
}

void SaveMainWindowPos( void )
{
}

void FiniScreen( void )
{
    if( _IsOn( SW_USE_MOUSE ) ) GUIFiniMouse();
    uistop();
    switch( ConMode ) {
    case C_QCON:
        console_active( ConCtrl, InitConsole );
        console_size( ConCtrl, DbgConsole, PrevLines, PrevColumns, NULL, NULL );
        console_close( ConCtrl );
        break;
    case C_XWIN:
        signal( SIGHUP, SIG_IGN );
        kill( XQshPid, SIGTERM );
        break;
    }
}

void ScrnSpawnStart( void )
{
    char        *term;

    if( ConCtrl == NULL && UITermType != NULL ) {
        term = getenv( "TERM" );
        if( term == NULL ) term = "";
        strcpy( TxtBuff, term );
        setenv( "TERM", UITermType, 1 );
    }
}

void ScrnSpawnEnd( void )
{
    if( ConCtrl == NULL && UITermType != NULL ) {
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
    ""
};

enum { OPT_CONSOLE, OPT_XCONFIG };


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
        return( FALSE );
    }
    return( TRUE );
}

void ScreenOptInit( void )
{
}
