/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  WLIB DLL version mainline.
*
****************************************************************************/


#include "wlib.h"
#include "idedll.h"
#ifdef __WATCOMC__
    #include <malloc.h> // for _heapshrink()
#endif
#include "banner.h"
#include "main.h"
#include "usage.h"

#include "clibext.h"


#define IDEFN(x)    IdeCbs->x

static IDECBHdl     IdeHdl;
static IDECallBacks *IdeCbs;
static IDEInitInfo  *ideInfo;

unsigned IDEAPI IDEGetVersion( void )
{
    return( IDE_CUR_DLL_VER );
}

IDEBool IDEAPI IDEInitDLL( IDECBHdl cbhdl, IDECallBacks *cb, IDEDllHdl *hdl )
{
    IdeHdl = cbhdl;
    IdeCbs = cb;
    *hdl = NULL;
    return( InitSubSystems() != 0 );
}

IDEBool IDEAPI IDEPassInitInfo( IDEDllHdl hdl, IDEInitInfo *info )
{
    /* unused parameters */ (void)hdl;

    ideInfo = info;
    return( false );
}

IDEBool IDEAPI IDERunYourSelf( IDEDllHdl hdl, const char *opts, IDEBool *fatalerr )
{
    char        *argv[3];

    /* unused parameters */ (void)hdl;

    *fatalerr = false;
    argv[0] = "";
    argv[1] = (char *)opts;
    argv[2] = NULL;
    return( WlibMainLine( argv ) != 0 );
}

IDEBool IDEAPI IDERunYourSelfArgv(// COMPILE A PROGRAM (ARGV ARGS)
    IDEDllHdl hdl,              // - handle for this instantiation
    int argc,                   // - # of arguments
    char **argv,                // - argument vector
    IDEBool* fatal_error )      // - addr[fatality indication]
{
    /* unused parameters */ (void)hdl; (void)argc;

    *fatal_error = false;
    return( WlibMainLine( argv ) != 0 );
}

void IDEAPI IDEStopRunning( void )
{
    if( ideInfo == NULL || ideInfo->ver <= 2 || ideInfo->console_output ) {
        exit( 1 );
    } else {
        longjmp( Env, 1 );
    }
}

void IDEAPI IDEFreeHeap( void )
{
#ifdef __WATCOMC__
    _heapshrink();
#endif
}

void IDEAPI IDEFiniDLL( IDEDllHdl hdl )
{
    /* unused parameters */ (void)hdl;

    FiniSubSystems();
}

char *WlibGetEnv( const char *name )
{
    char *env;

#if 0
    /* don't ignore environment, it is used for internal wlib setup only */
    if( !ideInfo->ignore_env ) {
#endif
        if( IdeCbs != NULL ) {
            if( !IDEFN( GetInfo )( IdeHdl, IDE_GET_ENV_VAR, (IDEGetInfoWParam)name, (IDEGetInfoLParam)&env ) ) {
                return( env );
            }
        }
#if 0
    }
#endif
    return( NULL );

}

void FatalResError( char *msg )
{
    IDEMsgInfo          msg_info;

    if( IdeCbs != NULL ) {
        IdeMsgInit( &msg_info, IDEMSGSEV_ERROR, msg );
        IDEFN( PrintWithInfo )( IdeHdl, &msg_info );
    }
    longjmp( Env, 1 );
}

void FatalError( int str, ... )
{
    va_list             arglist;
    char                buff[MAX_ERROR_SIZE];
    char                msg[512];
    IDEMsgInfo          msg_info;

    va_start( arglist, str );
    MsgGet( str, buff );
    vsnprintf( msg, 512, buff, arglist );
    if( IdeCbs != NULL ) {
        IdeMsgInit( &msg_info, IDEMSGSEV_ERROR, msg );
        IDEFN( PrintWithInfo )( IdeHdl, &msg_info );
    }
    va_end( arglist );
    longjmp( Env, 1 );
}

void Warning( int str, ... )
{
    va_list             arglist;
    char                buff[MAX_ERROR_SIZE];
    char                msg[512];
    IDEMsgInfo          msg_info;

    if( Options.quiet )
        return;
    MsgGet( str, buff );
    va_start( arglist, str );
    vsnprintf( msg, 512, buff, arglist );
    if( IdeCbs != NULL ) {
        IdeMsgInit( &msg_info, IDEMSGSEV_WARNING, msg );
        IDEFN( PrintWithInfo )( IdeHdl, &msg_info );
    }
    va_end( arglist );
}

void Message( char *buff, ... )
{
    va_list             arglist;
    char                msg[512];
    IDEMsgInfo          msg_info;

    if( Options.quiet )
        return;
    va_start( arglist, buff );
    vsnprintf( msg, 512, buff, arglist );
    if( IdeCbs != NULL ) {
        IdeMsgInit( &msg_info, IDEMSGSEV_NOTE_MSG, msg );
        IDEFN( PrintWithInfo )( IdeHdl, &msg_info );
    }
    va_end( arglist );
}

static void ConsoleMessage( const char *msgfmt, ... )
{
    va_list             arglist;
    char                msg[512];
    IDEMsgInfo          msg_info;

    va_start( arglist, msgfmt );
    vsnprintf( msg, 512, msgfmt, arglist );
    if( IdeCbs != NULL ) {
        IdeMsgInit( &msg_info, IDEMSGSEV_BANNER, msg );
        IDEFN( PrintWithInfo )( IdeHdl, &msg_info );
    }
    va_end( arglist );
}

void Usage( void )
/****************/
{
    char                buff[MAX_ERROR_SIZE];
    int                 str;
    int                 str_last;
    bool                console_tty;

    Banner();
    if( IdeCbs != NULL ) {
        console_tty = ( ideInfo != NULL && ideInfo->ver > 2 && ideInfo->console_output );
        if( console_tty && !Options.quiet && !Options.terse_listing ) {
            ConsoleMessage( "" );
        }
        if( Options.ar ) {
            str = MSG_USAGE_AR_BASE;
            str_last = MSG_USAGE_AR_BASE + MSG_USAGE_AR_COUNT;
            MsgGet( str++, buff );
            ConsoleMessage( buff, Options.ar_name );
            str_last = MSG_USAGE_AR_BASE + MSG_USAGE_AR_COUNT;
        } else {
            str = MSG_USAGE_WLIB_BASE;
            str_last = MSG_USAGE_WLIB_BASE + MSG_USAGE_WLIB_COUNT;
        }
        for( ; str < str_last; ++str ) {
            MsgGet( str, buff );
            ConsoleMessage( buff );
        }
    }
    longjmp( Env, 1 );
}

void Banner( void )
{
    static bool alreadyDone = false;

    if( !alreadyDone ) {
        alreadyDone = true;
        if( !Options.quiet && !Options.terse_listing && IdeCbs != NULL ) {
#ifndef NDEBUG
            ConsoleMessage( banner1w( "Library Manager", _WLIB_VERSION_ ) " [Internal Development]" );
#else
            ConsoleMessage( banner1w( "Library Manager", _WLIB_VERSION_ ) );
#endif
            ConsoleMessage( banner2 );
            ConsoleMessage( banner2a( 1984 ) );
            ConsoleMessage( banner3 );
            ConsoleMessage( banner3a );
        }
    }
}
