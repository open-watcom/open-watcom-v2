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


#include "variety.h"
#include <stdio.h>
#include <stdlib.h>
#include "rtdata.h"
#include "extfunc.h"
#include "rtinit.h"

#define EXIT_LIMIT      32

static  void    (* _HUGEDATA _ExitList[EXIT_LIMIT])();
static  int     _ExitCount;

int atexit( void (*func)(void) )
    {
        if( _ExitCount < EXIT_LIMIT ) {
            _ExitList[ _ExitCount++ ] = func;
            return( 0 );                /* indicate added successfully */
        }
        return( -1 );                   /* indicate no room */
    }

typedef void exit_fn();
#if defined(_M_IX86)
    #pragma aux (__outside_CLIB) exit_fn;
#endif

static void _Full_at_exit_rtn()
    {
        int count;
        exit_fn *func;

        count = _ExitCount;
        if( count == ( EXIT_LIMIT + 1 ) ) return;/*already done once*/
        _ExitCount = EXIT_LIMIT + 1;/* prevent others being registered */
        /* call functions in reverse order of their registration */
        while( count != 0 ) {
            --count;
            func = _ExitList[ count ];
            (*func)();                 /* invoke user exit routine */
        }
    }

AYI( _Full_at_exit_rtn, INIT_PRIORITY_PROGRAM+32 );
