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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <ctype.h>
#include <dos.h>
#include "stdnt.h"
#if defined(MD_x86)
  #include "dbg386.h"
#endif

#define MAX_WP  32

typedef struct {
    addr48_ptr  loc;
    DWORD       value;
    DWORD       linear;
    unsigned short len;
    unsigned short dregs;
} watch_point;

static watch_point      wpList[ MAX_WP ];

typedef struct break_point {
    struct break_point  *next;
    addr48_ptr  addr;
    BYTE        byte;
} break_point;

break_point *Breaks = NULL;

unsigned ReqSet_break( void )
{
    brkpnt_type         ch;
    set_break_req       *acc;
    set_break_ret       *ret;
    break_point         *new;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );

    ReadMem( acc->break_addr.segment, acc->break_addr.offset, &ch, sizeof(ch) );
    ret->old = ch;
    new = LocalAlloc( LMEM_FIXED, sizeof( *new ) );
    new->byte = ch;
    new->addr = acc->break_addr;
    new->next = Breaks;
    Breaks = new;
    ch = BRK_POINT;
    WriteMem( acc->break_addr.segment, acc->break_addr.offset, &ch, sizeof(ch) );
    return( sizeof( *ret ) );
}

unsigned ReqClear_break( void )
{
    brkpnt_type                 ch;
    clear_break_req             *acc;
    break_point                 *brk, *next;

    // we can assume all breaks are cleared at once

    for( brk = Breaks; brk != NULL; brk = next ) {
        next = brk->next;
        LocalFree( brk );
    }
    Breaks = NULL;
    acc = GetInPtr( 0 );
    ch = acc->old;
    WriteMem( acc->break_addr.segment, acc->break_addr.offset, &ch, sizeof(ch) );
    return( 0 );
}

BOOL FindBreak( WORD segment, DWORD offset, BYTE *ch )
{
    break_point *brk;

    for( brk = Breaks; brk != NULL; brk = brk->next ) {
        if( brk->addr.segment != segment ) continue;
        if( brk->addr.offset != offset ) continue;
        *ch = brk->byte;
        return( TRUE );
    }
    return( FALSE );
}

#if defined(MD_x86)
/*
 * setDR6 - set value of debug register 6
 */
static void setDR6( DWORD tmp )
{
    CONTEXT     con;
    thread_info *ti;

    ti = FindThread( DebugeeTid );
    MyGetThreadContext( ti, &con );
    con.Dr6 = tmp;
    MySetThreadContext( ti, &con );
} /* setDR6 */

/*
 * SetDR7 - set value of debug register 7
 */
void SetDR7( DWORD tmp )
{
    CONTEXT     con;
    thread_info *ti;
//char buff[256];

    ti = FindThread( DebugeeTid );
    if( ti == NULL ) return;
    MyGetThreadContext( ti, &con );
    con.Dr7 = tmp;
//sprintf( buff, "tid=%8.8x, dr7=%8.8x", DebugeeTid, tmp );
//MessageBox( NULL, buff, "Dr7", MB_APPLMODAL+MB_OK );
    MySetThreadContext( ti, &con );
} /* SetDR7 */

/*
 * GetDR6 - get value of debug register 6
 */
DWORD GetDR6( void )
{
    CONTEXT     con;
    thread_info *ti;

    ti = FindThread( DebugeeTid );
    MyGetThreadContext( ti, &con );
    return( con.Dr6 );
} /* GetDR6 */

static DWORD setDRn( int i, DWORD linear, long type )
{
    CONTEXT     con;
    thread_info *ti;

    ti = FindThread( DebugeeTid );
    MyGetThreadContext( ti, &con );
    ((DWORD *) &con.Dr0)[i] = linear;
    MySetThreadContext( ti, &con );

    return( ( type << DR7_RWLSHIFT(i) )
//        | ( DR7_GEMASK << DR7_GLSHIFT(i) ) | DR7_GE
          | ( DR7_LEMASK << DR7_GLSHIFT(i) ) | DR7_LE );
} /* setDRn */
#endif

/*
 * ClearDebugRegs - set for no watch points
 */
void ClearDebugRegs( void )
{
#if defined(MD_x86)
    int i;

    for( i=0;i<4;i++) {
        setDRn( i,0L,0L );
    }
    setDR6( 0 );
    SetDR7( 0 );
#elif defined(MD_axp) || defined(MD_ppc)
    /* nothing to do */
#else
    #error ClearDebugRegs not configured
#endif
} /* ClearDebugRegs */

/*
 * SetDebugRegs - set debug registers for watch points
 */
BOOL SetDebugRegs( void )
{
#if defined(MD_x86)
    int         needed,i,dr;
    DWORD       dr7;
    watch_point *wp;

    needed = 0;
    for( i=0;i<WPCount;i++ ) {
        needed += wpList[i].dregs;
    }
    if( needed > 4 ) {
        return( FALSE );
    }

    dr  = 0;
    dr7 = 0;
    for( i=0, wp = wpList; i<WPCount; i++, wp++ ) {
        dr7 |= setDRn( dr, wp->linear, DRLen( wp->len ) | DR7_BWR );
        dr++;
        if( wp->dregs == 2 ) {
            dr7 |= setDRn( dr, wp->linear+wp->len, DRLen( wp->len ) | DR7_BWR );
            dr++;
        }
    }
    SetDR7( dr7 );
    return( TRUE );
#elif defined(MD_axp) || defined(MD_ppc)
    return( FALSE );
#else
    #error SetDebugRegs not configured
#endif
} /* SetDebugRegs */

/*
 * CheckWatchPoints - check if a watchpoint was hit
 */
BOOL CheckWatchPoints( void )
{
    DWORD       value;
    int         i;

    for( i=0;i<WPCount;i++ ) {
        ReadMem( wpList[i].loc.segment, wpList[i].loc.offset, &value,
                        sizeof( value ) );
        if( value != wpList[i].value ) {
            return( TRUE );
        }
    }
    return( FALSE );
} /* CheckWatchPoints */

#if defined(MD_x86)
DWORD CalcLinear( WORD segment, DWORD offset )
{
    LDT_ENTRY sel;
    thread_info *ti;

    ti = FindThread( DebugeeTid );
    if( ti == NULL ) {
        return( offset );
    }
    if( !GetThreadSelectorEntry( ti->thread_handle, segment, &sel ) ) {
        return( offset );
    }
    return( offset +
            ( ( sel.HighWord.Bits.BaseHi << 24 ) |
              ( sel.HighWord.Bits.BaseMid << 16 ) |
              sel.BaseLow ) );
}
#endif


unsigned ReqSet_watch( void )
{
    set_watch_req       *acc;
    set_watch_ret       *ret;
    DWORD               value;
    watch_point         *curr;
#if defined(MD_x86)
    DWORD               linear;
#endif

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->multiplier = 100000;
    ret->err = 1;
    if( WPCount < MAX_WP ) {
        ret->err = 0;
        curr = wpList + WPCount;
        curr->loc.segment = acc->watch_addr.segment;
        curr->loc.offset = acc->watch_addr.offset;
        ReadMem( acc->watch_addr.segment, acc->watch_addr.offset, &value, sizeof(dword) );
        curr->value = value;
        curr->len = acc->size;
        WPCount++;
#if defined(MD_x86)
        curr->linear = linear = CalcLinear( acc->watch_addr.segment, acc->watch_addr.offset );
        curr->linear &= ~(curr->len-1);
        curr->dregs = (linear & (curr->len-1) ) ? 2 : 1;
        {
            unsigned            i ,needed;

            needed = 0;
            for( i = 0; i < WPCount; ++i ) {
                needed += wpList[ i ].dregs;
            }
            if( needed <= 4 ) ret->multiplier |= USING_DEBUG_REG;
        }
#endif
    }
    return( sizeof( *ret ) );
}

unsigned ReqClear_watch( void )
{
    clear_watch_req     *acc;
    watch_point         *dst;
    watch_point         *src;
    int                 i;

    acc = GetInPtr( 0 );
    dst = src = wpList;
    for( i=0;i<WPCount;i++ ) {
        if( src->loc.segment != acc->watch_addr.segment
         || src->loc.offset != acc->watch_addr.offset ) {
            dst->loc.offset = src->loc.offset;
            dst->loc.segment = src->loc.segment;
            dst->value = src->value;
            dst++;
        }
        src++;
    }
    WPCount--;
    return( 0 );
}
