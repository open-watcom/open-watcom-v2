/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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


static  HANDLE_INFO     hInstance = { 0 };
static  unsigned        MsgShift;

static bool Msg_Get( int resourceid, char *buffer )
{
    if( hInstance.status == 0 || WResLoadString( &hInstance, resourceid + MsgShift, (lpstr)buffer, RESOURCE_MAX_SIZE ) <= 0 ) {
        buffer[0] = '\0';
        return( false );
    }
    return( true );
}

bool Msg_Init( void )
{
    char        name[_MAX_PATH];

    hInstance.status = 0;
    if( _cmdname( name ) != NULL && OpenResFile( &hInstance, name ) ) {
        MsgShift = _WResLanguage() * MSG_LANG_SPACING;
        if( Msg_Get( MSG_USAGE_FIRST, name ) ) {
            return( true );
        }
    }
    CloseResFile( &hInstance );
    printf( NO_RES_MESSAGE );
    return( false );
}


bool Msg_Fini( void )
{
    return( CloseResFile( &hInstance ) );
}

#endif

void Banner( void )
{
    printf( banner1w( "Executable Strip Utility", _WSTRIP_VERSION_ ) "\n" );
    printf( banner2 "\n" );
    printf( banner2a( 1988 ) "\n" );
    printf( banner3 "\n" );
    printf( banner3a "\n" );
}

void Usage( void )
{
    char        msg_buffer[RESOURCE_MAX_SIZE];
    int         i;

    for( i = MSG_USAGE_FIRST; i <= MSG_USAGE_LAST; i++ ) {
        Msg_Get( i, msg_buffer );
        printf( "%s\n", msg_buffer );
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
