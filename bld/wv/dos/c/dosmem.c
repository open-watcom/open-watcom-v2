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


#include "dbgdefn.h"
#include "farptrs.h"
#include "tinyio.h"
#include <malloc.h>
#include "dosheap.h"

unsigned            OvlAreaSize;

extern void StartupErr(char *);
extern void far _ovl_addarea( unsigned, unsigned );

extern addr_seg     _psp;
extern unsigned     _STACKTOP;

void                *SyMemBeg;
void                *SyMemEnd;

extern unsigned long MemSize;
struct heapstart     *LastSeg;

/*
 * MemInit -- initialize dynamic and symbol memory
 */

void MemInit()
{
    addr_seg        last;

    /* find the address of the first available segment */
    SyMemBeg = (void *)_STACKTOP;
    SyMemBeg = MK_FP( FP_SEG( SyMemBeg ) + ( FP_OFF( SyMemBeg ) >> 4 ),
        FP_OFF( SyMemBeg ) & 0x000f );
    /* find the segment address of the last available segment */
    last = TinyGetPSP();
    last = *(addr_seg*)((char *)MK_FP( last, 0 ) + 2 );

    SyMemEnd = MK_FP( FP_SEG( SyMemBeg ) + ( MemSize >> 4 ),
        FP_OFF( SyMemBeg ) + ( MemSize & 0x000f ) );

    /* check for overflows */
    if( FP_OFF( SyMemEnd ) < FP_OFF( SyMemBeg ) ) {
        SyMemEnd = MK_FP( FP_SEG( SyMemEnd ) + 1, FP_OFF( SyMemEnd ) + 1 );
    }
    SyMemEnd = MK_FP( FP_SEG( SyMemEnd ) + ( FP_OFF( SyMemEnd ) >> 4 ),
        FP_OFF( SyMemEnd ) & 0x000f );
    if( FP_SEG( SyMemEnd ) < FP_SEG( SyMemBeg ) ) {
        StartupErr( "no memory to initialize - reduce dynamic memory" );
    }

    {
    addr_seg        first_free;
    first_free = 1 + FP_SEG( SyMemEnd ) + ( FP_OFF( SyMemEnd ) >> 4 );
    if( first_free + OvlAreaSize >= last
        || first_free + OvlAreaSize < FP_SEG( &SyMemBeg ) ) {
        StartupErr( "no memory to initialize - reduce overlay size" );
    }
    TinySetBlock( first_free - _psp + OvlAreaSize, _psp );
#ifdef _OVERLAYED_
    _ovl_addarea( first_free, OvlAreaSize );
#endif
    }
    LastSeg = 0;
    {
        extern void MemTrackInit(void);

        MemTrackInit();
    }
}

void *ExtraAlloc( unsigned size )
{
    tiny_ret_t ret;
    unsigned   num_para;

    num_para = (size + 0xf) >> 4;
    if( num_para == 0 ) num_para = 0x1000;
    ret = TinyAllocBlock( num_para );
    if( TINY_ERROR( ret ) ) {
        return( malloc( size ) );
    }
    return( MK_FP( TINY_INFO( ret ), 0 ) );
}

void ExtraFree( void * ptr )
{
    unsigned    seg;

    if( ptr == NULL ) return;
    seg = FP_SEG( ptr );
    if( seg <= FP_SEG( SyMemEnd ) && seg >= FP_SEG( SyMemBeg ) ) {
        free( ptr );
    } else {
        TinyFreeBlock( FP_SEG( ptr ) );
    }
}


void *ExtraRealloc( void *p, unsigned size )
{
    unsigned    seg;
    unsigned   num_para;

    seg = FP_SEG( p );
    if( seg <= FP_SEG( SyMemEnd ) && seg >= FP_SEG( SyMemBeg ) ) {
        return( realloc( p, size ) );
    } else {
        num_para = (size + 0xf) >> 4;
        if( num_para == 0 ) num_para = 0x1000;
        TinySetBlock( num_para, seg );
        return( p );
    }
}
