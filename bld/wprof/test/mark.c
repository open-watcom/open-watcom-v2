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
* Description:  Simple CPU cycle wasting program, useful for testing
*               the profiler. Uses profiler marks to exercise the mark
*               functionality.
*
****************************************************************************/


#include "wsample.h"

#ifndef ITER
    #define ITER    10000
#endif

unsigned long count;

void f10( void )
{
    int i, j;

    _MARK_( "Function f10 active" );

    for( i = 0; i < 10; ++i ) {
        for( j = 0; j < ITER; ++j ) {
            ++count;
        }
    }

    _MARK_( "Function f10 done" );
}

void f20( void )
{
    int i, j;

    _MARK_( "Function f20 active" );

    for( i = 0; i < 20; ++i ) {
        for( j = 0; j < ITER; ++j ) {
            ++count;
        }
    }
    _MARK_( "Function f20 done" );
}

void f30( void )
{
    int i, j;

    _MARK_( "Function f30 active" );

    for( i = 0; i < 30; ++i ) {
        for( j = 0; j < ITER; ++j ) {
            ++count;
        }
    }
    _MARK_( "Function f30 done" );
}

void f40( void )
{
    int i, j;

    _MARK_( "Function f40 active" );

    for( i = 0; i < 40; ++i ) {
        for( j = 0; j < ITER; ++j ) {
            ++count;
        }
    }
    _MARK_( "Function f40 done" );
}

int main( void )
{
    int i;

    _MARK_( "Main function active" );

    for( i = 0; i < 10; ++i ) {
        f10();
        f20();
        f30();
        f40();
    }

    _MARK_( "Main function done" );

    return( 0 );
}
