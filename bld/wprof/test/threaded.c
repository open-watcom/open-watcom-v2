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


#include "wsample.h"
#include <stddef.h>
#define INCL_DOSSEMAPHORES
#define INCL_DOSPROCESS
#include <os2.h>

#define NUM_TRDS 3
#define STK_SIZE 4096
char Stacks[NUM_TRDS][STK_SIZE];
int  Trds;
long  TrdsSem = 0;
extern int _STACKLOW;

#define delay( i, x ) for( i = 0; i < x; ++i )
void trd(void) {

    long i;
    long j;
    long k;

    _MARK_( "Thread active" );
    delay( j, 300 ) {
        delay( k, 200 );
    }
    _MARK_( "Thread done" );
#if 0
    delay( j, 300 ) {
        delay( k, 300 );
    }
    delay( j, 300 ) {
        delay( k, 400 );
    }
#endif
    DosSemRequest( &TrdsSem, -1L );
    ++Trds;
    DosSemClear( &TrdsSem );
    DosSleep( 100000 );
    DosExit( EXIT_THREAD, 0 );
}

void main(void)
{
    int         i;
    USHORT      tid;

    _STACKLOW = 0;
    for( i = 0; i < NUM_TRDS; ++i ) {
        DosCreateThread( trd, &tid, &Stacks[ i ][STK_SIZE] );
    }
    for( ;; ) {
        DosSemRequest( &TrdsSem, -1L );
        if( Trds == NUM_TRDS ) break;
        DosSemClear( &TrdsSem );
        DosSleep( 100 );
    }
    DosExit( EXIT_THREAD, 0 );
}
