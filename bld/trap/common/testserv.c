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


#include <stdlib.h>
#include <conio.h>
#include <stdio.h>
#include "testlink.h"
#include "packet.h"

extern void     NothingToDo(void);

char            RWBuff[256];

#ifdef __NETWARE__
char MayRelinquishControl = 1;

void TaskFini()
{
}
#endif

#ifdef __WINDOWS__
#include "windows.h"
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

full_block      Data;

void RunTime()
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
        if( test != TEST_CLIENT_GET ) RemoteGet( Data, block_size );
        if( test != TEST_CLIENT_PUT ) RemotePut( Data, block_size );
    }
}

main( unsigned argc, char *argv[] )
{
    char        *err;
    unsigned    len;

#ifdef __WINDOWS__
    {
    extern HANDLE           *_MainWindowData; // KLUDGE!!! (who cares - it's just a test program)
    Instance = *_MainWindowData;
    }
#endif
    err = RemoteLink( argc > 1 ? argv[1] : "", 1 );
    if( err != 0 ) {
        printf( "%s\n", err );
        return;
    }
    printf( "server running\n" );
    for( ;; ) {
        NothingToDo();
        if( kbhit() && getch() == 'q' ) break;
        if( RemoteConnect() ) {
            printf( "\nCONNECT\n" );
            for( ;; ) {
                len = RemoteGet( &Data, sizeof( Data ) );
                if( Data[0] == TEST_OVER ) break;
                if( Data[0] == TEST_STRING ) {
                    printf( "'%s' - %d bytes\n", &Data[1], len );
                    RemotePut( &Data, len );
                } else {
                    RunTime();
                }
            }
            RemoteDisco();
            printf( "\nDISCO\n" );
        }
    }
    RemoteUnLink();
}
