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


#include "gdefn.h"
#include "gbios.h"
#if defined( __386__ )
#include "extender.h"
#include "rmalloc.h"
#endif


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//  This module initializes (if necessary) the segments of
//  screen memory and the BIOS and ROM BIOS data areas.
//  To support other DOS-Extender systems, this module must
//  be modified or replaced.
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#if defined( _NEC_PC )


#if defined( __386__ )
extern int              PharlapAlloc( int, int );
#pragma aux             PharlapAlloc = \
                        "    int     21h      ", \
                        "    jnc     l1       ", \
                        "    sub     eax,eax  ", \
                        "l1:                  ", \
                        parm caller [eax] [ebx] \
                        modify [eax ebx] value [eax];

extern int              DPMIAlloc( int, int );
#pragma aux             DPMIAlloc = \
                        "    int     31h      ", \
                        "    jnc     l1       ", \
                        "    sub     eax,eax  ", \
                        "l1:                  ", \
                        parm caller [eax] [ebx] \
                        modify [eax ebx] value [eax];
#endif


void _InitSegments()
//==================

{
    unsigned short      seg;
    unsigned char far   *hires_mode_flag;

    _StackSeg = FP_SEG( &seg );         // point to stack segment
#if defined( __386__ )
    if( _IsRational() ) {
        seg = FP_SEG( &_BiosSeg );
        _BiosSeg = seg;
        _NecSeg  = seg;
        _TextSeg = seg;
        _AttrSeg = seg;
        _KanjiBuf.seg = DPMIAlloc( 0x0100, 5 );
        _KanjiBuf.buf = MK_FP( seg, _KanjiBuf.seg << 4 );
    } else {    // Pharlap
        _BiosSeg = 0x34;
        _NecSeg  = 0x34;
        _TextSeg = 0x34;
        _AttrSeg = 0x34;
        _KanjiBuf.seg = PharlapAlloc( 0x2537, 5 );
        _KanjiBuf.buf = MK_FP( 0x34, _KanjiBuf.seg << 4 );
    }
    if( _KanjiBuf.seg == 0 ) {
        _ErrorStatus = _GRERROR;
    }

    _BiosOff = 0x00000400;
    hires_mode_flag = (char far *) _BIOS_data( HIRES_FLAG );
    if( *hires_mode_flag & 0x08 ) {
        // Set Offsets for hires mode (1120x750x16 colour)
        _NecOff  = 0x000C0000;
        _TextOff = 0x000E0000;
        _AttrOff = 0x000E2000;
        _GRCGPort = 0xA4;
    } else {
        // Offsets for standard modes
        _NecOff  = 0x000A8000;
        _TextOff = 0x000A0000;
        _AttrOff = 0x000A2000;
        _GRCGPort = 0x7C;
    }

#else // adjust for hires in 286 mode
    hires_mode_flag = (char far *) _BIOS_data( HIRES_FLAG );
    if( *hires_mode_flag & 0x08 ) {
        // Set Segments for hires mode (1120x750x16 colour)
        // Segments default to standard mode values in global.c
        _NecSeg  = 0xC000;
        _TextSeg = 0xE000;
        _AttrSeg = 0xE200;
        _GRCGPort = 0xA4;
    } else {
        _GRCGPort = 0x7C;
    }
#endif
}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#elif defined( __QNX__ )        // 16 and 32 bit QNX

#include <stdio.h>
#include <i86.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/seginfo.h>
#include <sys/types.h>
#if !defined( __386__ )
  #include <sys/slib16.h>
#endif


static char __far *qnx_mmap_physical( long addr, unsigned long len, unsigned perms )
//==================================================================================

{
    ushort_t            sel = 0;

    sel = qnx_segment_overlay_flags( addr, len, perms | _PMF_INUSE );
    if( (short)sel == -1 ) {
        _ErrorStatus = _GRERROR;
        return( MK_FP( -1, -1 ) );
    } else {
        return( MK_FP( sel, 0 ) );
    }
}


void _InitSegments()
//==================

{
    char __far          *adr;

    _StackSeg = FP_SEG( &adr );         // point to stack segment
    _BiosSeg = 0x40;
    _BiosOff = 0;
    _MonoSeg = 0x28;
    _MonoOff = 0;
    _CgaSeg  = 0x30;
    _CgaOff  = 0;

    adr = qnx_mmap_physical( 0x0a0000L, 0x10000L, _PMF_DATA_RW );
    _EgaSeg = FP_SEG( adr );
    _EgaOff = FP_OFF( adr );

    adr = qnx_mmap_physical( 0x0c0000L, 0x10000L, _PMF_DATA_RW );
    _RomSeg = FP_SEG( adr );
    _RomOff = FP_OFF( adr );

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


#include "phapi.h"

#define NUM_SELECTORS   4

struct seg_table {
    USHORT              seg;
    ULONG               length;
    PSEL                desc;
};

static struct seg_table _SegTable[ NUM_SELECTORS ] = {
    0xB000,     0x8000,     &_MonoSeg,
    0xB800,     0x8000,     &_CgaSeg,
    0xA000,     0x10000,    &_EgaSeg,
    0xC000,     0x10000,    &_RomSeg
};


void _InitSegments()
//==================

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
    DosCreateCSAlias( FP_SEG( &i ), &_StackSeg );
}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#else           // normal 16 and 32 bit DOS

#define is_date_char( c )       (( c >= '0' && c <= '9' ) || c == '/' )
#define BIOS_DATE_LEN           (2+1+2+1+2) /* yy-mm-dd */

#define CHECK_IT                                                \
    {                                                           \
        int i;                                                  \
        unsigned num_ok;                                        \
                                                                \
        /* check for IBM BIOS revsion date in ROM */            \
        num_ok = 0;                                             \
        for( i = 0; i < BIOS_DATE_LEN; ++i ) {                  \
            if( is_date_char( p[i] ) ) {                        \
                ++num_ok;                                       \
            }                                                   \
        }                                                       \
        /* wishy-washy test for BIOS dates that */              \
        /* contain some garbage chars. */                       \
        /* Commodore PC60-40 has BIOS date "02/0(/88"). */      \
        return( num_ok < (BIOS_DATE_LEN / 2) );                 \
    }


static int __gfxnonIBM( void )
{
    char _WCFAR *p;

#if defined(__386__)
    if (_ExtenderRealModeSelector == 0)
        return 0;

    p = MK_FP(_ExtenderRealModeSelector, 0xffff5);
#else
    p = MK_FP( 0xf000, 0xfff5 );
#endif
    CHECK_IT
}


#if !defined( __386__ )
extern short  os_version( void );
#pragma aux   os_version = \
        "push bx"       \
        "push cx"       \
        "push dx"       \
        "mov ah,30h"    \
        "int 21h"       \
        "pop dx"        \
        "pop cx"        \
        "pop bx"        \
        value           [ax];   /* al=major ah=minor */

extern dbcs_pair far *  dbcs_vector_table( void );
#pragma aux             dbcs_vector_table = \
        "push ds"       \
        "push si"       \
        "mov ax,6300h"  /* get DBCS vector table */ \
        "int 21h"       \
        "mov dx,ds"     \
        "mov ax,si"     \
        "pop si"        \
        "pop ds"        \
        value           [dx ax];
#endif


void _InitSegments()
//==================

{
    unsigned short      seg;
    unsigned char       os_major;
    dbcs_pair           *p;
    dbcs_pair far       *s;

    _StackSeg = FP_SEG( &seg );         // point to stack segment
#if defined( __386__ )
    if( _IsRational() || _IsCodeBuilder() ) {
        seg = FP_SEG( &_BiosSeg );
    } else if( __X32VM != 0 ) {    // FlashTek
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
#if !defined( __386__ )
    os_major = (unsigned char) os_version();
#else
    os_major = _RMInterrupt( 0x21, 0x3000, 0x0, 0x0, 0x0, 0x0, 0x0 );
#endif

    if( os_major >= 5 ) {
        // The DBCS vector table call is not supported in earlier versions
#if !defined( __386__ )
        s = dbcs_vector_table();
#else
        s = MK_FP( _BiosSeg, _RMInterrupt2( 0x21, 0x6300 ) );
#endif
        p = _DBCSPairs;
        while( s->start_range != 0 ) {
            p->start_range = s->start_range;
            p->end_range = s->end_range;
            ++p;
            ++s;
        }
        p->start_range = 0;
        p->end_range = 0;
        if( _DBCSPairs[ 0 ].start_range != 0 ) {
            _IsDBCS = TRUE;
        }
    } else if (__gfxnonIBM()) {     // when DOS ver < 5
        _IsDBCS = TRUE;             // a NEC PC98 satisfies this condition
    }
}

#endif
