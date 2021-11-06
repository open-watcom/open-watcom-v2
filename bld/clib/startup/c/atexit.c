/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Module for adding functions to the atexit() list, as well
*               invoking the functions on library shutdown.
*
****************************************************************************/


#include "variety.h"
#include <stdio.h>
#include <stdlib.h>
#include "rtdata.h"
#include "extfunc.h"
#include "rtinit.h"


#define EXIT_LIMIT      32

typedef void    __exit_fn( void );
#if defined(_M_IX86)
    #pragma aux (__outside_CLIB) __exit_fn;
#endif

static __exit_fn    * _HUGEDATA _ExitList[EXIT_LIMIT];
static int          _ExitCount;

_WRTLFCONV int atexit( void (_WCCALLBACK * func)( void ) )
{
    if( _ExitCount < EXIT_LIMIT ) {
        _ExitList[_ExitCount++] = (__exit_fn *)func;
        return( 0 );                /* indicate added successfully */
    }
    return( -1 );                   /* indicate no room */
}

static void _Full_at_exit_rtn( void )
{
    int         count;

    count = _ExitCount;
    if( count == ( EXIT_LIMIT + 1 ) ) {
        return;                     /* already done once */
    }
    _ExitCount = EXIT_LIMIT + 1;    /* prevent others being registered */
    /* call functions in reverse order of their registration */
    while( count-- > 0 ) {
        (*_ExitList[count])();      /* invoke user exit routine */
    }
}

AYI( _Full_at_exit_rtn, INIT_PRIORITY_PROGRAM + 32 );
