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
* Description:  Simple server for testing remote communication transport.
*
****************************************************************************/


#include <stdlib.h>
#if defined(__WATCOMC__)
    #include <conio.h>
#endif
#include <stdio.h>
#include "testlink.h"
#include "packet.h"
#ifdef __WINDOWS__
#include "windows.h"

extern HANDLE   *_MainWindowData; // KLUDGE!!! (who cares - it's just a test program)
#endif

extern void     NothingToDo(void);

char            RWBuff[256];
full_block      Data;

#ifdef __NETWARE__
char MayRelinquishControl = 1;

void TrapFini( void )
{
}
#endif

#ifdef __WINDOWS__
HANDLE  Instance;

void SetLinkName( char *name )
{
    name = name;
}
#endif

void Output( char *p )
{
    printf( "%s", p );
}

void ServMessage( char *msg )
{
    Output( msg );
    Output( "\n" );
}

void RunTime( void )
{
    unsigned long   iter_count;
    unsigned        block_size;
    int             i;
    test_type       test;

    test = Data[0];
    block_size = ( test & TEST_FULL ) ? sizeof( Data ) : 1;
    iter_count = 0;
    for( i = 4; i >= 1; --i ) {
        iter_count <<= 8;
        iter_count += Data[i];
    }

    test &= ~TEST_FULL;
    while( --iter_count != 0 ) {
        if( test != TEST_CLIENT_GET )
            RemoteGet( (byte *)&Data, block_size );
        if( test != TEST_CLIENT_PUT ) {
            RemotePut( (byte *)&Data, block_size );
        }
    }
}

int main( int argc, char *argv[] )
{
    char        *err;
    unsigned    len;

#ifdef __WINDOWS__
    Instance = *_MainWindowData;
#endif
    err = RemoteLink( ( argc > 1 ) ? argv[1] : "", TRUE );
    if( err != 0 ) {
        printf( "%s\n", err );
        return( 1 );
    }
    printf( "server running\n" );
    for( ;; ) {
        NothingToDo();
#if defined(__WATCOMC__)
        // How to do the equivalent of kbhit()?
        if( kbhit() && getch() == 'q' )
            break;
#endif
        if( RemoteConnect() ) {
            printf( "\nCONNECT\n" );
            for( ;; ) {
                len = RemoteGet( (byte *)&Data, sizeof( Data ) );
                if( len == -1 ) {
                    printf( "\nlink broken\n" );
                    break;
                }
                if( Data[0] == TEST_OVER )
                    break;
                if( Data[0] == TEST_STRING ) {
                    printf( "'%s' - %d bytes\n", &Data[1], len );
                    RemotePut( (byte *)&Data, len );
                } else {
                    RunTime();
                }
            }
            RemoteDisco();
            printf( "\nDISCO\n" );
        }
    }
    RemoteUnLink();
    return( 0 );
}
