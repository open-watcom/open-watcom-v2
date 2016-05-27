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
* Description:  Break & watchpoint interface
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <direct.h>
#include "rdos.h"
#include "stdrdos.h"
#include "debug.h"

trap_retval ReqSet_break( void )
{
    set_break_req   *acc;
    set_break_ret   *ret;
    struct TDebug   *obj;
    int             sel;
    int             offset;
    int             hw;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    sel = acc->break_addr.segment;
    offset = acc->break_addr.offset;

    hw = ( (sel & 3) == 0 );

    obj = GetCurrentDebug();

    if( obj )
        AddBreak( obj, sel, offset, hw );

    return( sizeof( *ret ) );
}

trap_retval ReqClear_break( void )
{
    clear_break_req *acc;
    struct TDebug   *obj;

    acc = GetInPtr( 0 );

    obj = GetCurrentDebug();

    if( obj )
        ClearBreak( obj, acc->break_addr.segment, acc->break_addr.offset );

    return( 0 );
}

trap_retval ReqSet_watch( void )
{
    set_watch_req   *acc;
    set_watch_ret   *ret;
    struct TDebug   *obj;
    int             sel;
    int             offset;
    int             size;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );

    sel = acc->watch_addr.segment;
    offset = acc->watch_addr.offset;
    size = acc->size;

    obj = GetCurrentDebug();

    if( obj )
        AddWatch( obj, sel, offset, size );

    ret->multiplier = 100000 | USING_DEBUG_REG;
    ret->err = 0;

    return( sizeof( *ret ) );
}

trap_retval ReqClear_watch( void )
{
    clear_watch_req *acc;
    struct TDebug   *obj;

    acc = GetInPtr( 0 );

    obj = GetCurrentDebug();

    if( obj )
        ClearWatch( obj, acc->watch_addr.segment, acc->watch_addr.offset, acc->size );

    return( 0 );
}
