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
#include <conio.h>
#include <string.h>
#include <dos.h>
#include "tinyio.h"
#include "packet.h"
#define uint mumblyschwartz
#include "dvapi.h"
#undef uint

typedef unsigned long   a_handle;

static a_handle         GetHandle;
static a_handle         PutHandle;

#define PATTERN 0xA5


static int fstrlen( char far *str )
{
    int         i;

    for( i=0; *str; ++i, ++str );
    return( i );
}

#pragma off(unreferenced);
unsigned RemoteGet( char far *rec, unsigned len )
#pragma on(unreferenced);
{
    char        far *buffer;
    int         buflen;
    int         status;

    status = mal_read( GetHandle, &buffer, &buflen );
    movedata( FP_SEG(buffer), FP_OFF(buffer),
              FP_SEG(rec), FP_OFF(rec),
              buflen );
    return( buflen );
}

void RemotePut( char far *snd, unsigned len )
{
    mal_write( PutHandle, snd, len );
}


char RemoteConnect( void )
{
    int                 status;
    char                far *buffer;
    int                 buflen;
    char                pattern;

#ifdef SERVER
    status = mal_sizeof( GetHandle );
    if( status == 0 ) return( 0 );
    status = mal_read( GetHandle, &buffer, &buflen );
    if( *buffer == PATTERN ){
        PutHandle = mal_of( mal_addr( GetHandle ) );
        pattern = PATTERN;
        mal_write( PutHandle, &pattern, sizeof(pattern) );
        return( 1 );
    }
    return( 0 );
#else
    pattern = PATTERN;
    mal_write( PutHandle, &pattern, sizeof( pattern ) );
    status = mal_read( GetHandle, &buffer, &buflen );
    if( *buffer != PATTERN ){
        return( 0 );
    }
    return( 1 );
#endif
}

void RemoteDisco( void )
{
}


char *RemoteLink( char far *name, char server )
{
    int                 version;

    server = server;

    version = api_init();
    if( version == 0 ){
        return( "DESQview not installed" );
    } else if( version < 0x200 ) {
        return( "this program requires DESQview 2.00 or higher" );
    }
    api_level( 0x200 );
    if( name == NULL || *name == '\0' ){
        name = "WATCOM Server";
    }

#ifdef SERVER
    if( mal_find( name, fstrlen( name ) ) != 0 ) {
        return( "Server name already in use" );
    }
    GetHandle = mal_new();
    mal_open( GetHandle );
    mal_name( GetHandle, name, fstrlen( name ) );
#else
    GetHandle = mal_me();
    PutHandle = mal_find( name, fstrlen(name) );
    if( PutHandle == 0 ) {
        return( "Unable to find server" );
    }
#endif
    return( NULL );
}

void RemoteUnLink( void )
{
#ifdef SERVER
        mal_free( GetHandle );
#endif
}
