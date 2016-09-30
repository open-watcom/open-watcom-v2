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
* Description:  Executable Strip Utility output routines.
*
****************************************************************************/


#include <string.h>
#include <stdlib.h>
#ifdef __WATCOMC__
#include <process.h>
#endif
#include "wio.h"
#include "watcom.h"
#include "wstrip.h"
#include "banner.h"

#include "clibext.h"


#define RESOURCE_MAX_SIZE       128

#if defined( INCL_MSGTEXT )

static char *StringTable[] = {
    "",                             // message ID's start at 1
    #include "incltext.gh"
};


bool Msg_Init( void )
{
    return( true );
}

static bool Msg_Get( int resourceid, char *buffer )
{
    strcpy( buffer, StringTable[resourceid] );
    return( true );
}

bool Msg_Fini( void )
{
    return( true );
}

#else

#include "wressetr.h"
#include "wresset2.h"
#include "wreslang.h"

#define NO_RES_MESSAGE "Error: could not open message resource file.\r\n"
#define NO_RES_SIZE (sizeof( NO_RES_MESSAGE ) - 1)

static  HANDLE_INFO     hInstance = { 0 };
static  unsigned        MsgShift;
static  bool            res_failure = true;

static WResFileOffset res_seek( WResFileID handle, WResFileOffset position, int where )
/* fool the resource compiler into thinking that the resource information
 * starts at offset 0 */
{
    if( where == SEEK_SET ) {
        return( lseek( handle, position + WResFileShift, where ) - WResFileShift );
    } else {
        return( lseek( handle, position, where ) );
    }
}


/* declare struct WResRoutines WResRtns {...} */
WResSetRtns( open, close, read, write, res_seek, tell, malloc, free );

static bool Msg_Get( int resourceid, char *buffer )
{
    if( res_failure || WResLoadString( &hInstance, resourceid + MsgShift, (LPSTR)buffer, RESOURCE_MAX_SIZE ) <= 0 ) {
        buffer[0] = '\0';
        return( false );
    }
    return( true );
}


bool Msg_Init( void )
{
    char        name[_MAX_PATH];

    hInstance.handle = NIL_HANDLE;
    if( _cmdname( name ) != NULL && !OpenResFile( &hInstance, name ) ) {
        res_failure = false;
        if( !FindResources( &hInstance ) && !InitResources( &hInstance ) ) {
            MsgShift = _WResLanguage() * MSG_LANG_SPACING;
            if( Msg_Get( MSG_USAGE_FIRST, name ) ) {
                return( true );
            }
        }
        CloseResFile( &hInstance );
        hInstance.handle = NIL_HANDLE;
    }
    write( STDOUT_FILENO, NO_RES_MESSAGE, NO_RES_SIZE );
    res_failure = true;
    return( false );
}


bool Msg_Fini( void )
{
    bool    retcode = true;

    if( !res_failure ) {
        if( CloseResFile( &hInstance ) ) {
            res_failure = true;
            retcode = false;
        }
        hInstance.handle = NIL_HANDLE;
    }
    return( retcode );
}

#endif

static void Outs( bool nl, const char *s )
{
    write( STDOUT_FILENO, s, strlen( s ) );
    if( nl ) {
        write( STDOUT_FILENO, "\r\n", 2 );
    }
}

static void Outc( char c )
{
    write( STDOUT_FILENO, &c, 1 );
}

void Banner( void )
{
    Outs( true, banner1w( "Executable Strip Utility", _WSTRIP_VERSION_ ) );
    Outs( true, banner2 );
    Outs( true, banner2a( "1988" ) );
    Outs( true, banner3 );
    Outs( true, banner3a );
}

void Usage( void )
{
    char        msg_buffer[RESOURCE_MAX_SIZE];
    int         i;

    for( i = MSG_USAGE_FIRST; i <= MSG_USAGE_LAST; i++ ) {
        Msg_Get( i, msg_buffer );
        Outs( true, msg_buffer );
    }
    Msg_Fini();
    exit( -1 );
}

void Fatal( int reason, const char *insert )
/* the reason doesn't have to be good */
{
    char        msg_buffer[RESOURCE_MAX_SIZE];
    size_t      i;

    Msg_Get( reason, msg_buffer );
    for( i = 0; msg_buffer[i] != '\0'; ++i ) {
        if( msg_buffer[i] == '%' ) {
            i++;
            if( msg_buffer[i] == 's' ) {
                Outs( false, insert );
            } else {
                Outc( msg_buffer[i] );
            }
        } else {
            Outc( msg_buffer[i] );
        }
    }
    Msg_Get( MSG_WSTRIP_ABORT, msg_buffer );
    Outs( true, msg_buffer );
    Msg_Fini();
    exit( -1 );
}
