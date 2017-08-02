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
* Description:  Test if NULL is passed as NULL in C++.
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#define _FAIL   fail(__LINE__)
#define _PASS   puts( "PASS " __FILE__ ); fflush( stdout ); return( 0 );

int arg1 = 0xabcd;
int arg2 = 0x9876;

void fail( unsigned line )
{
    printf( "failure on line %u\n", line );
    abort();
}

void vararg_fn( int num, ... )
{
    va_list args;

    if( num != 3 ) fail(__LINE__);
    va_start( args, num );
    if( va_arg( args, int ) != arg1 ) _FAIL;
    if( va_arg( args, void * ) != NULL ) _FAIL;
    if( va_arg( args, int ) != arg2 ) _FAIL;
    va_end( args );
}

int main( )
{
    vararg_fn( 3, arg1, NULL, arg2 );
    _PASS;
}
