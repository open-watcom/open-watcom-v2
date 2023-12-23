/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2022 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Windows 3.x breakpoint support.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <ctype.h>
#include <dos.h>
#include "stdwin.h"
#include "dbg386.h"
#include "wdebug.h"
#include "di386cli.h"
#include "initfini.h"
#include "descript.h"


/*
 * We keep a breakpoint list to support the "debug debugee only" and
 * to allow the movement of segments (if a segment moves, then we have
 * to replace the breakpoint). The switch is undocumented (';cge')
 * and is only to allow debugging of the debugger.
 *
 * Under Windows 3.1, it seems bloody unlikely that segments move, so
 * maintaining a breakpoint list is unnecessary.
 */

#define GMEM_FLAGS      (GMEM_SHARE + GMEM_MOVEABLE + GMEM_ZEROINIT)
#define BREAK_INCREMENT 64

#define MAX_WATCHES     8

typedef struct {
    uint_64     value;
    dword       linear;
    addr48_ptr  addr;
    word        size;
    word        dregs;
} watch_point;

opcode_type             BreakOpcode;

static break_point      __huge *brkList;
static WORD             numBreaks;
static HGLOBAL          brkHandle;
static watch_point      WatchPoints[MAX_WATCHES];
static int              WatchCount;

opcode_type place_breakpoint( addr48_ptr *addr )
{
    opcode_type old_opcode;

    ReadMemory( addr, &old_opcode, sizeof( old_opcode ) );
    WriteMemory( addr, &BreakOpcode, sizeof( BreakOpcode ) );
    return( old_opcode );
}

int remove_breakpoint( addr48_ptr *addr, opcode_type old_opcode )
{
    return( WriteMemory( addr, &old_opcode, sizeof( old_opcode ) ) != sizeof( old_opcode ) );
}

/*
 * IsOurBreakpoint - check if a segment/offset is a break point we set
 */
bool IsOurBreakpoint( WORD sel, DWORD off )
{
    int i;

    for( i = 0; i < numBreaks; i++ ) {
        if( brkList[i].in_use ) {
            if( brkList[i].addr.segment == sel && brkList[i].addr.offset == off ) {
                return( true );
            }
        }
    }
    return( false );

} /* IsOurBreakpoint */

/*
 * ResetBreakpoints - reset breakpoints in a specified segment
 */
void ResetBreakpoints( WORD sel )
{
    int             i;

    for( i = 0; i < numBreaks; i++ ) {
        if( brkList[i].in_use ) {
            if( brkList[i].addr.segment == sel ) {
                place_breakpoint( &brkList[i].addr );
            }
        }
    }

} /* ResetBreakpoints */

static break_point __far * findBrkEntry( void )
{
    int         i;
    int         old_num;

    for( i = 0; i < numBreaks; i++ ) {
        if( !brkList[i].in_use ) {
            return( &brkList[i] );
        }
    }
    old_num = numBreaks;
    numBreaks += BREAK_INCREMENT;
    if( old_num == 0 ) {
        brkHandle = GlobalAlloc( GMEM_FLAGS, numBreaks * sizeof( brkList[0] ) );
    } else {
        GlobalUnlock( brkHandle );
        brkHandle = GlobalReAlloc( brkHandle, numBreaks * sizeof( brkList[0] ), GMEM_FLAGS );
    }
    brkList = (break_point __huge *)GlobalLock( brkHandle );
    return( &brkList[old_num] );
}

trap_retval TRAP_CORE( Set_break )( void )
{
    set_break_req       *acc;
    set_break_ret       *ret;
    break_point         *brk;

    acc = GetInPtr( 0 );

    Out((OUT_BREAK,"AccSetBreak %4.4x:%8.8x", acc->break_addr.segment, acc->break_addr.offset ));
    Out((OUT_BREAK,"task=%4.4x", DebugeeTask ));

    brk = findBrkEntry();
    brk->old_opcode = place_breakpoint( &acc->break_addr );
    brk->addr = acc->break_addr;
    brk->in_use = true;
    brk->hard_mode = true;

    ret = GetOutPtr( 0 );
    ret->old = brk->old_opcode;
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Clear_break )( void )
{
    int                 i;
    clear_break_req     *acc;

    acc = GetInPtr( 0 );
    Out((OUT_BREAK,"AccRestoreBreak %4.4x:%8.8x", acc->break_addr.segment, acc->break_addr.offset ));
    Out((OUT_BREAK,"task=%4.4x", DebugeeTask ));

    remove_breakpoint( &acc->break_addr, acc->old );

    for( i = 0; i < numBreaks; i++ ) {
        if( brkList[i].addr.segment == acc->break_addr.segment &&
                        brkList[i].addr.offset == acc->break_addr.offset ) {
            brkList[i].in_use = false;
            break;
        }
    }
    return( 0 );
}

/*
 * Code to manipulate debug registers is here.  The Set/GetDebugRegister
 * calls are pragmas that talk to WDEBUG.386.  The pragmas themselves are
 * in bld/watcom/h/wdebug.h
 */
static void setDR6( DWORD tmp )
{
    SetDebugRegister( 6, &tmp );
} /* setDR6 */

static void setDR7( DWORD tmp )
{
    SetDebugRegister( 7, &tmp );
} /* setDR7 */

DWORD GetDR6( void )
{
    DWORD       tmp;

    GetDebugRegister( 6, &tmp );
    return( tmp );
} /* GetDR6 */

#if 0
static DWORD GetDR7( void )
{
    DWORD       tmp;

    GetDebugRegister( 7, &tmp );
    return( tmp );
} /* GetDR7 */
#endif

static dword SetDRn( int i, dword linear, word type )
{
    SetDebugRegister( i, &linear );
    return( ( (dword)type << DR7_RWLSHIFT(i) )
          | ( DR7_GEMASK << DR7_GLSHIFT(i) )
          | ( DR7_LEMASK << DR7_GLSHIFT(i) ) );
} /* SetDRn */

void ClearDebugRegs( void )
{
    int i;

    if( WDebug386 ) {
        for( i = 0; i < 4; i++ )
            SetDRn( i, 0, 0 );
        setDR6( 0 );
        setDR7( 0 );
    }
} /* ClearDebugRegs */

bool IsWatch( void )
{
    return( WatchCount > 0 );
}

static int DRegsCount( void )
{
    int     needed;
    int     i;

    needed = 0;
    for( i = 0; i < WatchCount; i++ ) {
        needed += WatchPoints[i].dregs;
    }
    return( needed );
}

bool SetDebugRegs( void )
{
    int         i;
    int         j;
    int         dr;
    dword       dr7;
    watch_point *wp;
    word        size;
    dword       linear;
    word        type;

    if( !WDebug386 || DRegsCount() > 4 )
        return( false );

    dr  = 0;
    dr7 = DR7_GE | DR7_LE;
    for( wp = WatchPoints, i = WatchCount; i-- > 0; wp++ ) {
        linear = wp->linear;
        size = wp->size;
        if( size == 8 )
            size = 4;
        type = DRLen( size ) | DR7_BWR;
        for( j = 0; j < wp->dregs; j++ ) {
            dr7 |= SetDRn( dr, linear, type );
            dr++;
            linear += size;
        }
    }
    setDR7( dr7 );
    return( true );
} /* SetDebugRegs */

/*
 * CheckWatchPoints - check if a watchpoint was hit
 */
bool CheckWatchPoints( void )
{
    watch_point *wp;
    int         i;
    uint_64     value;

    for( wp = WatchPoints, i = WatchCount; i-- > 0; wp++ ) {
        value = 0;
        ReadMemory( &wp->addr, &value, wp->size );
        if( wp->value != value ) {
            return( true );
        }
    }
    return( false );
} /* CheckWatchPoints */

static dword GetLinear( word segment, dword offset )
{
    descriptor          desc;

    GetDescriptor( segment, &desc );
    return( GET_DESC_BASE( desc ) + offset );
}

trap_retval TRAP_CORE( Set_watch )( void )
{
    set_watch_req       *acc;
    set_watch_ret       *ret;
    watch_point         *wp;
    dword               linear;
    word                size;
    word                dregs;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->multiplier = 20000;
    ret->err = 1;       // failure
    if( WatchCount < MAX_WATCHES ) {
        ret->err = 0;   // OK
        wp = WatchPoints + WatchCount;
        wp->addr.segment = acc->watch_addr.segment;
        wp->addr.offset = acc->watch_addr.offset;
        wp->size = acc->size;
        wp->value = 0;
        ReadMemory( &wp->addr, &wp->value, wp->size );

        linear = GetLinear( wp->addr.segment, wp->addr.offset );
        dregs = 1;
        size = wp->size;
        if( size == 8 ) {
            size = 4;
            dregs++;
        }
        if( linear & ( size - 1 ) )
            dregs++;
        wp->dregs = dregs;
        wp->linear = linear & ~( size - 1 );

        WatchCount++;
        if( WDebug386 && DRegsCount() <= 4 ) {
            ret->multiplier |= USING_DEBUG_REG;
        }
    }
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Clear_watch )( void )
{
    clear_watch_req     *acc;
    watch_point         *dst;
    watch_point         *src;
    int         i;

    acc = GetInPtr( 0 );
    dst = src = WatchPoints;
    for( i = 0; i < WatchCount; i++ ) {
        if( src->addr.segment != acc->watch_addr.segment
         || src->addr.offset != acc->watch_addr.offset ) {
            *dst = *src;
            dst++;
        }
        src++;
    }
    WatchCount--;
    return( 0 );
}
