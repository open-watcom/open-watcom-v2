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
* Description:  Simple client for testing remote communication transport.
*
****************************************************************************/


#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <stdlib.h>
#include <conio.h>
#include "testlink.h"
#include "packet.h"

full_block      Data;

#ifdef __WINDOWS__
#include <windows.h>
HANDLE  Instance;
#endif

void RunTime( test_type test, unsigned bytes, unsigned iterations )
{
    clock_t         start;
    unsigned long   diff;
    unsigned long   total;
    unsigned        num;
    unsigned        block_size;
    unsigned long   iter_count;
    unsigned long   temp;
    unsigned        i;

    block_size = ( test & TEST_FULL ) ? sizeof( Data ) : 1;
    num = bytes / block_size;
    iter_count = (long)iterations * num;

    Data[0] = test;
    temp = iter_count;
    for( i = 1; i <= 4; ++i ) {
        Data[i] = temp;
        temp >>= 8;
    }

    RemotePut( Data, 5 );

    start = clock();
    while( --iter_count != 0 ) {
        if( test != TEST_CLIENT_GET )
            RemotePut( Data, block_size );
        if( test != TEST_CLIENT_PUT ) {
            RemoteGet( Data, block_size );
        }
    }

    diff = clock() - start;
    total = (unsigned long) iterations * num * block_size;
    if( diff ) {
        printf( "%ld ticks to transfer %ld bytes in %d byte blocks - BPS = %ld\n",
                diff, total, block_size, (total * CLOCKS_PER_SEC) / diff );
    } else {
	printf( "%ld bytes in %d byte blocks were transferred too quickly\n",
	        total, block_size );
    }
}

int main( unsigned argc, char *argv[] )
{
    const char      *err;
    unsigned        iterations;
    char            *p;
    test_type       test;
    unsigned        len;

#ifdef __WINDOWS__
    {
    extern HANDLE   *_MainWindowData; // KLUDGE!!! (who cares - it's just a test program)
    Instance = *_MainWindowData;
    }
#endif
    err = RemoteLink( ( argc > 1 ) ? argv[1] : "", FALSE );
    if( err != NULL ) {
        printf( "Error: %s\n", err );
        return( 1 );
    }
    if( !RemoteConnect() ) {
        printf( "Error: can not connect to server\n" );
    } else {
        for( ;; ) {
            printf( "enter> " );
            fflush( stdout );
            if( !gets( Data + 1 ) ) break;
            if( Data[1] == 'q' ) break;
            if( Data[1] == '~' ) {
                /* speed test */
                p = Data + 2;
                test = TEST_PUT_GET;
                if( *p == '>' ) {
                    test = TEST_CLIENT_PUT;
                    ++p;
                } else if( *p == '<' ) {
                    test = TEST_CLIENT_GET;
                    ++p;
                }
                while( isspace( *p ) ) ++p;
                iterations = strtoul( p, NULL, 0 );
                if( iterations == 0 ) iterations = 1;
                RunTime( test | TEST_FULL, 1000, iterations );
                RunTime( test, 100, iterations );
            } else {
                /* data transmission test */
                Data[0] = TEST_STRING;
                len = strlen( Data ) + 1;
                RemotePut( Data, len );
                memset( Data, 0, sizeof( Data ) );
                len = RemoteGet( Data, sizeof( Data ) );
                printf( "'%s' - %d bytes\n", Data + 1, len );
            }
        }
        Data[0] = TEST_OVER;
        RemotePut( Data, 1 );
        RemoteDisco();
    }
    RemoteUnLink();
    return( 0 );
}
