/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "wdebug.h"
#include "trptypes.h"
#include "trperr.h"
#include "packet.h"
#include "osidle.h"


#define LINKNAME_SIZE   80

#define DEFAULT_LINK_NAME   "WinLink"

char        LinkName[LINKNAME_SIZE + 1];

static _dword _id;
#ifdef DEBUG_ME
static int _aa = 10, _bb = 12;
static int _a = 0, _b = 2;

void Blip( int *a, int *b, char a1 )
{
    char __far *s1 = (char __far *)0xb8000000;
    char __far *s2 = (char __far *)0xb0000000;
    int c;

    s1[*a] = a1;
    s1[*b] = ' ';
    s2[*a] = a1;
    s2[*b] = ' ';
    c = *a;
    *a = *b;
    *b = c;

}
#endif

trap_retval RemoteGet( void *data, trap_elen len )
{
    unsigned long rc;

#ifdef __WINDOWS__
    for( ;; ) {
        rc = ConvGet( _id, data, len, NO_BLOCK );
        if( (rc & 0xffff) == BLOCK ) {
            SetExecutionFocus( _id );
        } else {
            break;
        }
    }
#else
#ifdef DEBUG_ME
    Blip( &_a, &_b, 'G' );
#endif
    rc = ConvGet( _id, data, len, BLOCK );
#ifdef DEBUG_ME
    Blip( &_a, &_b, 'g' );
#endif
#endif
    return( rc >> 16 );
}

trap_retval RemotePut( void *data, trap_elen len )
{
#ifdef __WINDOWS__
    int rc;

    for( ;; ) {
        rc = ConvPut( _id, data, len, NO_BLOCK );
        if( rc == BLOCK ) {
            SetExecutionFocus( _id );
        } else {
            break;
        }
    }
#else
#ifdef DEBUG_ME
    Blip( &_aa, &_bb, 'T' );
#endif
    ConvPut( _id, data, len, BLOCK );
#ifdef DEBUG_ME
    Blip( &_aa, &_bb, 't' );
#endif
#endif
    return( len );
}


bool RemoteConnect( void )
{
    int rc;

#ifdef SERVER
    rc = LookForConv( &_id );
    if( rc == 1 ) {
        return( true );
    } else if( rc == 0 ) {
        ReleaseVMTimeSlice();
    }
    return( false );
#else
    static bool _first = true;

    if( _first ) {

        _first = false;
        rc = StartConv( _id );
        if( rc != 0 ) {
        }
    }
    for( ;; ) {
        rc = IsConvAck( _id );
        if( !rc ) {
            ReleaseVMTimeSlice();
        } else if( rc < 0 ) {
        } else {
            return( true );
        }
    }
#endif
}

void RemoteDisco( void )
{
#ifdef SERVER
#else
    EndConv( _id );
#endif
}

const char *RemoteLink( const char *parms, bool server )
{
    int     i;
    int     rc;

    /* unused parameters */ (void)server;

    if( *parms == '\0' )
        parms = DEFAULT_LINK_NAME;
    for( i = 0; i < LINKNAME_SIZE; i++ ) {
        if( *parms == '\0' )
            break;
        LinkName[i] = *parms++;
    }
    LinkName[i] = '\0';
#ifdef SERVER
    rc = RegisterName( LinkName );
    if( rc < 0 ) {
        UnregisterName( LinkName );
        rc = RegisterName( LinkName );
        if( rc < 0 ) {
            return( TRP_ERR_CANT_REGISTER_SERVER );
        }
    }
#else
    rc = AccessName( LinkName, &_id );
    if( rc < 0 )
        return( TRP_ERR_CANT_FIND_SERVER );
#endif
    return( NULL );
}


void RemoteUnLink( void )
{
#ifdef SERVER
    UnregisterName( LinkName );
#else
    UnaccessName( LinkName );
#endif
}
