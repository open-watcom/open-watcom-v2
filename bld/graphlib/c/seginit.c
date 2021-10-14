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
* Description:  Screen access segment initialization.
*
****************************************************************************/


#include "gdefn.h"
#include "gbios.h"
#if !defined( _M_I86 )
#include "extender.h"
#include "rmalloc.h"
#endif
#if defined( __DOS__ )
#include "getltdos.h"
#endif


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//  This module initializes (if necessary) the segments of
//  screen memory and the BIOS and ROM BIOS data areas.
//  To support other DOS-Extender systems, this module must
//  be modified or replaced.
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#if defined( __QNX__ )        // 16 and 32 bit QNX

#include <stdio.h>
#include <i86.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/seginfo.h>
#include <sys/types.h>
#if defined( _M_I86 )
  #include <sys/slib16.h>
#endif


static char __far *qnx_mmap_physical( long addr, unsigned long len, unsigned perms )
//==================================================================================

{
    ushort_t            sel = 0;

    sel = qnx_segment_overlay_flags( addr, len, perms | _PMF_INUSE );
    if( (short)sel == -1 ) {
        _ErrorStatus = _GRERROR;
        return( _MK_FP( -1, -1 ) );
    } else {
        return( _MK_FP( sel, 0 ) );
    }
}


void _InitSegments( void )
//========================

{
    char __far          *adr;

    _StackSeg = _FP_SEG( &adr );        // point to stack segment
    _BiosSeg = 0x40;
    _BiosOff = 0;
    _MonoSeg = 0x28;
    _MonoOff = 0;
    _CgaSeg  = 0x30;
    _CgaOff  = 0;

    adr = qnx_mmap_physical( 0x0a0000L, 0x10000L, _PMF_DATA_RW );
    _EgaSeg = _FP_SEG( adr );
    _EgaOff = _FP_OFF( adr );

    adr = qnx_mmap_physical( 0x0c0000L, 0x10000L, _PMF_DATA_RW );
    _RomSeg = _FP_SEG( adr );
    _RomOff = _FP_OFF( adr );

    _CompileBuf = qnx_segment_alloc( 2048 );
    _CompileSeg = qnx_segment_put( 0, _CompileBuf, _PMF_CODE_RX | _PMF_INUSE );
#if 0
    if( _RomSeg == -1 ) {
        fprintf( stderr, "_InitSegments: unable to allocate RomSeg.\n" );
    }
    if( _EgaSeg == -1 ) {
        fprintf( stderr, "_InitSegments: unable to allocate EgaSeg.\n" );
    }
    if( _CompileSeg == -1 ) {
        fprintf( stderr, "_InitSegments: unable to allocate CompileSeg.\n ");
    }
#else
    if( (short)_RomSeg == -1 || (short)_EgaSeg == -1
                || (short)_CompileSeg == -1 || (int)_CompileBuf == -1 ) {
        _ErrorStatus = _GRERROR;
    }
#endif
}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#elif defined( DOSX286 )        // 286 DOS-Extender


#if 0
#include "phapi.h"
#else
typedef unsigned short USHORT;
typedef unsigned long ULONG;
typedef unsigned short __far * PSEL;
extern void __far __pascal DosMapRealSeg( USHORT, ULONG, PSEL );
extern void __far __pascal DosGetBIOSSeg( PSEL );
extern void __far __pascal DosCreateCSAlias( USHORT, PSEL );
#endif

#define NUM_SELECTORS   4

struct seg_table {
    USHORT              seg;
    ULONG               length;
    PSEL                desc;
};

static struct seg_table _SegTable[NUM_SELECTORS] = {
    0xB000,     0x8000,     &_MonoSeg,
    0xB800,     0x8000,     &_CgaSeg,
    0xA000,     0x10000,    &_EgaSeg,
    0xC000,     0x10000,    &_RomSeg
};


void _InitSegments( void )
//========================

{
    int                 i;
    struct seg_table    *p;

    p = _SegTable;
    for( i = 0; i < NUM_SELECTORS; ++i, ++p ) {
        DosMapRealSeg( p->seg, p->length, p->desc );
    }
    DosGetBIOSSeg( &_BiosSeg );
    _BiosOff = 0;
    _RomOff = 0;
    DosCreateCSAlias( _FP_SEG( &i ), &_StackSeg );
}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#else           // normal 16 and 32 bit DOS


#if defined( _M_I86 )
extern short os_version( void );
#pragma aux os_version = \
        "push bx"       \
        "push cx"       \
        "push dx"       \
        "mov ah,30h"    \
        "int 21h"       \
        "pop dx"        \
        "pop cx"        \
        "pop bx"        \
    __parm      [] \
    __value     [__ax] \
    __modify    []
#endif


void _InitSegments( void )
//========================

{
    unsigned short      seg;
    unsigned char       os_major;
    dbcs_pair           *p;
    dbcs_pair __far     *s;

    _StackSeg = _FP_SEG( &seg );        // point to stack segment
#if !defined( _M_I86 )
    if( _IsRational() || _IsCodeBuilder() ) {
        seg = _FP_SEG( &_BiosSeg );
    } else if( _IsFlashTek() ) {        // FlashTek
        seg = __x386_zero_base_selector;
    } else {
        // variables already initialized to PharLap defaults
        seg = 0;
    }
    if( seg != 0 ) {
        _BiosSeg = seg;
        _MonoSeg = seg;
        _CgaSeg  = seg;
        _EgaSeg  = seg;
        _RomSeg  = seg;
        _BiosOff = 0x00000400;
        _MonoOff = 0x000B0000;
        _CgaOff  = 0x000B8000;
        _EgaOff  = 0x000A0000;
        _RomOff  = 0x000C0000;
    }
#endif
    // check for DBCS
    _IsDBCS = FALSE;
#if defined( _M_I86 )
    os_major = (unsigned char) os_version();
#else
    os_major = _RMInterrupt( 0x21, 0x3000, 0x0, 0x0, 0x0, 0x0, 0x0 );
#endif

    if( os_major >= 5 ) {
        // The DBCS vector table call is not supported in earlier versions
        p = _DBCSPairs;
        s = (dbcs_pair __far *)dos_get_dbcs_lead_table();
        if( s != NULL ) {
            while( s->start_range != 0 ) {
                p->start_range = s->start_range;
                p->end_range = s->end_range;
                ++p;
                ++s;
            }
        }
        p->start_range = 0;
        p->end_range = 0;
        if( _DBCSPairs[0].start_range != 0 ) {
            _IsDBCS = TRUE;
        }
    }
}

#endif
