/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2018 The Open Watcom Contributors. All Rights Reserved.
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
#include <malloc.h>
#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbgerr.h"
#include "tinyio.h"
#include "heap.h"
#include "stdui.h"


#define ROUND_DOWN_SIZE_TO_PARA( __x )  ((__x)>>4)
#define ROUND_UP_SIZE_TO_PARA( __x )    (((__x)+15)>>4)

#define HUGE_PTR_CVT(x)     MK_FP( FP_SEG( x ) + ( FP_OFF( x ) >> 4 ), FP_OFF( x ) & 0x000f );
#define HUGE_PTR_ADD(x,o)   MK_FP( FP_SEG( x ) + ( o >> 4 ), FP_OFF( x ) + ( o & 0x000f ) );

extern void         __far _ovl_addarea( unsigned, unsigned );
extern void         MemTrackInit( void );

extern unsigned     _STACKTOP;

unsigned            OvlAreaSize;

__segment           LastSeg;
void                *SyMemBeg;
void                *SyMemEnd;

/*
 * MemInit -- initialize dynamic and symbol memory
 */

void MemInit( void )
{
    addr_seg        last;

    /* find the address of the first available segment */
    SyMemBeg = (void *)_STACKTOP;
    SyMemBeg = HUGE_PTR_CVT( SyMemBeg );
    /* find the segment address of the last available segment */
    last = TinyGetPSP();
    last = *(addr_seg*)((char *)MK_FP( last, 0 ) + 2 );

    SyMemEnd = HUGE_PTR_ADD( SyMemBeg, MemSize );
    /* check for overflows */
    if( FP_OFF( SyMemEnd ) < FP_OFF( SyMemBeg ) ) {
        SyMemEnd = HUGE_PTR_ADD( SyMemEnd, 0x11 );
    }
    SyMemEnd = HUGE_PTR_CVT( SyMemEnd );

    if( FP_SEG( SyMemEnd ) < FP_SEG( SyMemBeg ) ) {
        StartupErr( "no memory to initialize - reduce dynamic memory" );
    }

    {
    addr_seg        first_free;
    first_free = 1 + FP_SEG( SyMemEnd ) + ROUND_DOWN_SIZE_TO_PARA( FP_OFF( SyMemEnd ) );
    if( first_free + OvlAreaSize >= last
        || first_free + OvlAreaSize < FP_SEG( &SyMemBeg ) ) {
        StartupErr( "no memory to initialize - reduce overlay size" );
    }
    TinySetBlock( first_free - _psp + OvlAreaSize, _psp );
#ifdef _OVERLAYED_
    _ovl_addarea( first_free, OvlAreaSize );
#endif
    }
    LastSeg = _NULLSEG;
    {
        MemTrackInit();
    }
}

LP_VOID ExtraAlloc( size_t size )
{
    tiny_ret_t ret;
    unsigned   num_para;

    num_para = ROUND_UP_SIZE_TO_PARA( size );
    if( num_para == 0 )
        num_para = 0x1000;
    ret = TinyAllocBlock( num_para );
    if( TINY_ERROR( ret ) ) {
        return( malloc( size ) );
    }
    return( MK_FP( TINY_INFO( ret ), 0 ) );
}

void ExtraFree( LP_VOID ptr )
{
    unsigned    seg;

    if( ptr == NULL )
        return;
    seg = FP_SEG( ptr );
    if( seg <= FP_SEG( SyMemEnd ) && seg >= FP_SEG( SyMemBeg ) ) {
        free( ptr );
    } else {
        TinyFreeBlock( FP_SEG( ptr ) );
    }
}
