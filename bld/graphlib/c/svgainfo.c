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


#include <conio.h>
#include "gdefn.h"
#include "gbios.h"
#include "svgadef.h"
#if defined( __386__ )
  #include "rmalloc.h"
#endif


#if defined( __QNX__ )
extern unsigned         LoadSegLimit( unsigned );
  #if defined( __386__ )
    #if ( __WATCOMC__ >= 1000 )         // V10.0 or better
        #define LSL     "lsl eax,dx"
    #else
        #define LSL     "lsl eax,edx"
    #endif
    #pragma aux         LoadSegLimit = \
                        ".386p" \
                        LSL \
                        parm caller [edx] value [eax];
  #else
    #pragma aux         LoadSegLimit = \
                        ".286p" \
                        "lsl  ax,dx" \
                        parm caller [dx] value [ax];
  #endif
#endif


short _SuperVGAType()
//===================

{
    char                dcc;
    char far            *p;
    char far            *s;
    short               len;
    short               val;
    char                id;
#if defined( __QNX__ )
    unsigned short      seg_len;
#endif

    if (_SVGAType > 0) {      // check for SVGA override
        return _SVGAType;
    }

    dcc = VideoInt( _BIOS_VIDEO_DCC, 0, 0, 0 ) & 0x00ff;
    if( dcc != 0x1a ) {
        return( _SV_NONE );
    }

//  test for VESA standard compatible
    if ((_SVGAType != -1) && TestForVESA()) {
        return _SV_VESA;
    }

//  test for Paradise
    if( _fmemcmp( MK_FP( _RomSeg, _RomOff + 0x007d ), "VGA=", 4 ) == 0 ) {
        return( _SV_PARADISE );
    }
    if( _fmemcmp( MK_FP( _RomSeg, _RomOff + 0x007d ), "(c)", 3 ) == 0 ) {
        return( _SV_PARADISE );
    }

//  test for ATI Wonder
    if( _fmemcmp( MK_FP( _RomSeg, _RomOff + 0x0031 ), "761295520", 9 ) == 0 ) {
        return( _SV_ATI );
    }

//  test for Oak Technologies
    s = MK_FP( _RomSeg, _RomOff + 0 );
    for( len = 512; len > 0; len -= p - s, s = p + 1 ) {
        p = _fmemchr( s, 'O', len );
        if( p == NULL ) break;
        if( _fmemcmp( "OAK", p, 3 ) == 0 ) {
            return( _SV_OAK );
        }
    }

//  test for Trident
    s = MK_FP( _RomSeg, _RomOff + 0 );
    for( len = 512; len > 0; len -= p - s, s = p + 1 ) {
        p = _fmemchr( s, 'T', len );
        if( p == NULL ) break;
        if( _fmemcmp( "TRIDENT", p, 7 ) == 0 ) {
            return( _SV_TRIDENT );
        }
    }

#if 0
//  test for Diamond Viper
    s = MK_FP( _RomSeg, _RomOff + 0 );
    for( len = 512; len > 0; len -= p - s, s = p + 1 ) {
        p = _fmemchr( s, 'D', len );
        if( p == NULL ) break;
        if( _fmemcmp( "Diamond", p, 5 ) == 0 ) {
            // Check to make sure it's a Weitek.
            // first unlock Weitek registers
            outp( 0x3c5, 0x11 );
            val = inp( 0x3c5 );
            outp( 0x3c5, val );
            outp( 0x3c5, val );
            val = inp( 0x3c5 );
            outp( 0x3c5, val & 0xdf );
            // now read User Bits register
            outp( 0x3c4, 0x10 );
            val = inp( 0x3c ) & 0xf0;
            if( val == 0xa0 ) {
                return( _SV_VIPER );
            }
        }
    }
#endif

//  test for Genoa
    p = MK_FP( _RomSeg, _RomOff + 0x0037 );
    val = *(short far *) p;
#if defined( __QNX__ )
    seg_len = LoadSegLimit( _RomSeg );
    if( _RomOff + val <= seg_len - 3 ) {
#endif
    p = MK_FP( _RomSeg, _RomOff + val );
    if( p[ 0 ] == 0x77 && p[ 1 ] == 0x11 && p[ 2 ] == 0x99 && p[ 3 ] == 0x66 ) {
        return( _SV_GENOA );
    }
#if defined( __QNX__ )
    }
#endif

//  test for Video-7
    if( VideoInt_bx( 0x6f00, 0, 0, 0 ) == ( 'V' << 8 ) + '7' ) {
        return( _SV_VIDEO7 );
    }

//  test for Chips and Technologies
    if( ( VideoInt( 0x5f00, 0, 0, 0 ) & 0x00ff ) == 0x5f ) {
        return( _SV_CHIPS );
    }

//  test for S3
    outp( 0x3d4, 0x38 );
    outp( 0x3d4 + 1, 0x48 );    // unlock S3 registers
    outp( 0x3d4, 0x30 );
    val = inp( 0x3d4 + 1 ) >> 4;
    if( val >= 8 && val <= 0x0c ) {
        return( _SV_S3 );
    }

//  test for TSENG (do this one near the end)
    id = 0;
    val = inp( 0x3cd );
    outp( 0x3cd, 0x55 );
    if( inp( 0x3cd ) == 0x55 ) {
        outp( 0x3cd, 0xaa );
        if( inp( 0x3cd ) == 0xaa ) {
            id = 1;
        }
    }
    outp( 0x3cd, val );
    if( id != 0 ) {
        // test for ET-3000 or ET-4000
        outp( 0x3d4, 0x33 );
        val = inp( 0x3d4+1 );
        outp( 0x3d4+1, 0x05 );
        id = inp( 0x3d4+1 );
        outp( 0x3d4+1, val );
        if( id == 0x05 ) {
            return( _SV_TSENG4000 );
        } else {
            return( _SV_TSENG3000 );
        }
    }

//  test for Cirrus Logic (do this after Tseng)
    outpw( 0x3c4, 0x1206 );     // enable Cirrus extended registers
    val = inp( 0x3c5 );
    if( val == 0x12 ) {
        outp( 0x3d4, 0x27 );
        val = inp( 0x3d5 );
        val &= 0xfc;
        if( val == 0xa8 || val == 0xa0 || val == 0x90 || val == 0x98 || val == 0xc8 ) {
            return( _SV_CIRRUS );
        }
    }
//  val = VideoInt( 0x12ff, 0x0080, 0, 0 );
//  if( val >= 2 && val <= 0x31 ) {
//      return( _SV_CIRRUS );
//  }

    return( _SV_NONE );
}

static int TestForVESA()
//======================
{
    short               val;
#if defined( __386__ ) && !defined( __QNX__ )
    char far            *buf;
    RM_ALLOC            mem;
    int                 is_vesa;
#else
    char                buf[ 256 ];
#endif

#if defined( __386__ ) && !defined( __QNX__ )
    if( _RMAlloc( 256, &mem ) ) {
        buf = mem.pm_ptr;
        val = _RMInterrupt( 0x10, 0x4f00, 0, 0, 0, mem.rm_seg, 0 );
        if( val == 0x004f && buf[ 0 ] == 'V' && buf[ 1 ] == 'E' &&
                             buf[ 2 ] == 'S' && buf[ 3 ] == 'A' ) {
            is_vesa = TRUE;
        } else {
            is_vesa = FALSE;
        }
        _RMFree( &mem );
        return( is_vesa );
    }
#else
    val = GetVESAInfo( 0x4f00, 0, &buf );
    if( val == 0x004f && buf[ 0 ] == 'V' && buf[ 1 ] == 'E' &&
                         buf[ 2 ] == 'S' && buf[ 3 ] == 'A' ) {
        return( TRUE );
    }
#endif
    return( FALSE );
}


// Entry-point for FORTRAN to set _SVGAType variable

void _SetSVGAType( short vga_type )
//=================================

{
    _SVGAType = vga_type;
}
