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
* Description:  Profiler test app, Part II.
*
****************************************************************************/


#include <stdio.h>
#include "wsample.h"

#include "pt.h"

extern char flags[SIZE+1];
extern int count, niter;

extern void (*check_if)(int);


static void oops_oops( unsigned num )
{
        while( --num )
        {
                if( (num & 0xff) == 0x00 ) printf( "\r .... \r" );
                if( (num & 0xff) == 0x80 ) printf( "\r **** \r" );
        }
}

static void blink_blink( unsigned num )
{
    unsigned long   dogy;

_MARK_( "start of blinking ..." );
    dogy = num;
    dogy *= 20;
    while( --dogy )
    {
        if( (unsigned int) dogy == 0x0000 ) printf( "\r 0000 \r" );
        if( (unsigned int) dogy == 0x4000 ) printf( "\r 4444 \r" );
        if( (unsigned int) dogy == 0x8000 ) printf( "\r 8888 \r" );
        if( (unsigned int) dogy == 0xc000 ) printf( "\r cccc \r" );
    }
_MARK_( "... end of blinking" );
}

void up_and_up_has_an_extremely_long_and_verbose_function_name( unsigned num )
{
        while( --num )
        {
                check_if( num );
                if( (num & 0xff) == 0x00 ) printf( "\r (((( \r" );
                if( (num & 0xff) == 0x80 ) printf( "\r )))) \r" );
        }
        printf( "\r MMMM \r" );
}

void dont_do_it()
{
    blink_blink( SIZE );
    up_and_up_has_an_extremely_long_and_verbose_function_name( SIZE );
    oops_oops( SIZE );
}
