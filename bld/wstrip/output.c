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
* Description:  Executable Strip Utility output routines.
*
****************************************************************************/


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#ifdef __WATCOMC__
#include <process.h>
#endif
#include "watcom.h"
#include "wstrip.h"
#include "banner.h"
#include "usage.h"

#include "clibext.h"


#define RESOURCE_MAX_SIZE       128

#if defined( INCL_MSGTEXT )

static char *StringTable[] = {
    #define pick(c,e,j) e,
    #include "wstrip.msg"
    #include "usage.gh"
    #undef pick
};

#else

#include "wressetr.h"
#include "wresset2.h"
#include "wreslang.h"

static  HANDLE_INFO     hInstance = { 0 };
static  unsigned        MsgShift;

#endif

static bool Msg_Get( int resourceid, char *buffer )
{
#if defined( INCL_MSGTEXT )
    strcpy( buffer, StringTable[resourceid] );
#else
    if( hInstance.status == 0 || WResLoadString( &hInstance, resourceid + MsgShift, (lpstr)buffer, RESOURCE_MAX_SIZE ) <= 0 ) {
        buffer[0] = '\0';
        return( false );
    }
#endif
    return( true );
}

bool Msg_Init( void )
{
#if defined( INCL_MSGTEXT )
    return( true );
#else
    char        name[_MAX_PATH];

    hInstance.status = 0;
    if( _cmdname( name ) != NULL && OpenResFile( &hInstance, name ) ) {
        MsgShift = _WResLanguage() * MSG_LANG_SPACING;
        if( Msg_Get( MSG_USAGE_BASE, name ) ) {
            return( true );
        }
    }
    CloseResFile( &hInstance );
    puts( NO_RES_MESSAGE );
    return( false );
#endif
}


bool Msg_Fini( void )
{
#if defined( INCL_MSGTEXT )
    return( true );
#else
    return( CloseResFile( &hInstance ) );
#endif
}

void Banner( void )
{
    puts( banner1w( "Executable Strip Utility", _WSTRIP_VERSION_ ) );
    puts( banner2 );
    puts( banner2a( 1988 ) );
    puts( banner3 );
    puts( banner3a );
}

void Usage( void )
{
    char        msg_buffer[RESOURCE_MAX_SIZE];
    int         i;

    puts( "" );
    for( i = MSG_USAGE_BASE; i < MSG_USAGE_BASE + MSG_USAGE_COUNT; i++ ) {
        Msg_Get( i, msg_buffer );
        puts( msg_buffer );
    }
    Msg_Fini();
    exit( -1 );
}

void Fatal( int reason, const char *insert )
/* the reason doesn't have to be good */
{
    char        msg_buffer[RESOURCE_MAX_SIZE];

    Msg_Get( reason, msg_buffer );
    printf( msg_buffer, insert );
    Msg_Get( MSG_WSTRIP_ABORT, msg_buffer );
    printf( "%s", msg_buffer );
    Msg_Fini();
    exit( -1 );
}
