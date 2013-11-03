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
* Description:  Global segment processing
*
****************************************************************************/


#include "ftnstd.h"
#include "global.h"
#include "fcgbls.h"
#include "progsw.h"
#include "cg.h"
#include "wf77segs.h"
#include "cpopt.h"
#include "fmemmgr.h"

global_seg              *GlobalSeg;

static  global_seg      *CurrGSeg;


#if _CPU == 8086
  #define MAX_SEG_SIZE  0x10000
#else
  #define MAX_SEG_SIZE  0xffffffff
#endif


void    InitGlobalSegs( void ) {
//========================

// Initialize global segment processing.

    GlobalSeg  = NULL;
    CurrGSeg   = NULL;
    MaxSegSize = MAX_SEG_SIZE;
}


void    FreeGlobalSegs( void ) {
//========================

// Free global segment list.

    global_seg  *curr_seg;

    while( GlobalSeg != NULL ) {
        curr_seg  = GlobalSeg;
        GlobalSeg = curr_seg->link;
        FMemFree( curr_seg );
    }
}


static  void    NewGlobalSeg( void ) {
//==============================

// Allocate a new global segment.

    global_seg  *new_seg;

    new_seg = FMemAlloc( sizeof( global_seg ) );
    new_seg->segment = SEG_NULL;
    new_seg->link = NULL;
    if( CurrGSeg == NULL ) {
        GlobalSeg = new_seg;
    } else {
        CurrGSeg->link = new_seg;
    }
    CurrGSeg = new_seg;
}

segment_id      AllocGlobal( unsigned_32 g_size, bool init ) {
//============================================================

// Allocate space in global data area and return the global segment.

    segment_id  seg_id;

    if( ProgSw & PS_ERROR ) return( SEG_FREE );
    if( ( CurrGSeg != NULL ) && ( CurrGSeg->size + g_size <= MaxSegSize ) ) {

        // object will fit in current segment
#if _CPU == 8086  || _CPU == 386
        if( ( init == CurrGSeg->initialized ) || !_SmallDataModel( CGOpts ) ) {
            CurrGSeg->size += g_size;
            return( CurrGSeg->segment );
        }
#else
        if( init == CurrGSeg->initialized ) {
            CurrGSeg->size += g_size;
            return( CurrGSeg->segment );
        }
#endif
    }
    NewGlobalSeg();
    seg_id = CurrGSeg->segment;
    CurrGSeg->initialized = init;
    if( g_size < MaxSegSize ) {
        // object smaller than a segment chunk
        CurrGSeg->size = g_size;
    } else {
        CurrGSeg->size = MaxSegSize;
    }
    g_size -= CurrGSeg->size;
    while( g_size > MaxSegSize ) {
        NewGlobalSeg();
        CurrGSeg->size = MaxSegSize;
        CurrGSeg->initialized = init;
        g_size -= MaxSegSize;
    }
    if( g_size != 0 ) {
        NewGlobalSeg();
        CurrGSeg->size = g_size;
        CurrGSeg->initialized = init;
    }
    return( seg_id );
}

