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
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "wdebug.h"
#ifdef __WINDOWS__
#include <windows.h>
#endif
#include "link.h"

_dword __ConvId;

static char linkName[128];

void _farstrcpy( char __far *dest, const char __far *src )
{
    while( *dest++ = *src++ );
}

/*
 * VxDPresent - check if WGOD is present
 */
int __pascal VxDPresent( void )
{
    if( CheckWin386Debug() != WGOD_VERSION ) {
        return( 0 );
    }
    return( 1 );

} /*VxDPresent */

#ifdef __WINDOWS__
static void messageLoop( void )
{
    MSG         msg;

    Yield();
    while( PeekMessage( &msg, (HWND) NULL, (UINT) NULL, (UINT) NULL,
                    PM_NOYIELD | PM_NOREMOVE ) ) {
        GetMessage( &msg, NULL, 0, 0 );
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }
    Yield();
    TimeSlice();
    Yield();

} /* messageLoop */
#endif


#ifdef __WINDOWS__
/*
 * VxDRaiseInterrupt - raise an interrupt in the client VM
 */
void __pascal VxDRaiseInterrupt( unsigned intr )
{
    RaiseInterruptInVM( __ConvId, intr );

} /* VxDRaiseInterrupt */
#endif

/*
 * VxDGet - get some data from a client/server
 */
unsigned __pascal VxDGet( void __far *rec, unsigned len )
{
    _dword      rc;

#ifdef __WINDOWS__
    while( 1 ) {
        rc = ConvGet( __ConvId,rec, len, NO_BLOCK );
        if( (rc & 0xffff) == BLOCK ) {
            messageLoop();
        } else {
            break;
        }
    }
#else
    rc = ConvGet( __ConvId,rec, len, BLOCK );
#endif
    return( rc >> 16 );

} /* VxDGet */

/*
 * VxDPutPending - tests if a server is trying to put to us
 */
int __pascal VxDPutPending( void )
{
    return( ConvPutPending() );

} /* VxDPutPending */

/*
 * VxDPut - put some data to client/server
 */
void __pascal VxDPut( const void __far *rec, unsigned len )
{
#ifdef __WINDOWS__
    int rc;

    while( 1 ) {
        rc = ConvPut( __ConvId,rec, len, NO_BLOCK );
        if( rc == BLOCK ) {
            messageLoop();
        } else {
            break;
        }
    }
#else
    ConvPut( __ConvId,rec, len, BLOCK );
#endif

} /* VxDPut */


/*
 * VxDConnect - connect to a client/server
 */
char __pascal VxDConnect( void )
{
    int rc;
#ifdef SERVER
    rc = LookForConv( &__ConvId );
    if( rc == 1 ) {
        return( 1 );
    } else if( rc == 0 ) {
        TimeSlice();
        return( 0 );
    }
    return 0;           //added by TW (no return value otherwise)??
#else
    static int _first=1;
    if( _first ) {
        _first = 0;
        rc = StartConv( __ConvId );
    }
    while( 1 ) {
        rc = IsConvAck( __ConvId );
        if( !rc ) {
            TimeSlice();
        } else if( rc < 0 ) {
        } else {
            return( 1 );
        }
    }
#endif

} /* VxDConnect */

/*
 * VxDDisconnect - stop a conversation with a server
 */
int __pascal VxDDisconnect( void )
{
#ifndef SERVER
    return( EndConv( __ConvId ) );
#else
    return( 0 );
#endif

} /* VxDDisconnect */

/*
 * VxDLink - start a lnk
 */
const char * __pascal VxDLink( const char __far *name )
{
    int rc;

    _farstrcpy( linkName, name );
#ifdef SERVER
    rc = RegisterName( linkName );
    if( rc < 0 ) {
        return( "Could not register server!" );
    }
#else
    rc = AccessName( linkName, &__ConvId );
    if( rc < 0 ) {
        return( "Could not find server!" );
    }
#endif
    return( NULL );

} /* VxDLink */

/*
 * VxDUnLink - terminate link
 */
int __pascal VxDUnLink( void )
{
#ifdef SERVER
#ifdef __WINDOWS__
    messageLoop();
#endif
    return( UnregisterName( linkName ) );
#else
    return( UnaccessName( linkName ) );
#endif

} /* VxDUnLink */
