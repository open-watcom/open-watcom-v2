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


#if defined( _DEFAULT_WINDOWS )
static WPI_COLOUR       GetPalette( short );
static void             PutPalette( short, WPI_COLOUR );
#else
static long             GetPalette( short );
static void             PutPalette( short, long );
#endif


long _WCI86FAR _CGRAPH _remappalette( short pixval, long colour )
/*==========================================================

   This routine sets the colour indexed by pixval to the new colour.  It
   returns the previous colour at pixval or -1 if unsuccessful. */

{
    long                prev;

    if( pixval < 0 || pixval >= _CurrState->vc.numcolors ) {
        _ErrorStatus = _GRINVALIDPARAMETER;
        return( -1 );
    }
#if defined( _NEC_PC )
    if( _CurrState->vc.mode == _98TEXT80 ) {
        _ErrorStatus = _GRERROR;
        return( -1 );
    }
#else
    if( _CurrState->vc.adapter < _MCGA ) {
        _ErrorStatus = _GRERROR;
        return( -1 );
    }
#endif
    prev = GetPalette( pixval );
    PutPalette( pixval, colour );

    return( prev );
}

Entry( _REMAPPALETTE, _remappalette ) // alternate entry-point


void _RemapNum( long _WCI86FAR *colours, short num )
//=============================================

{
    short               i;

#if !( defined( _NEC_PC ) || defined( _DEFAULT_WINDOWS ) )
    if( _CurrState->vc.adapter != _EGA ) {
        if( _FastMap( colours, num ) ) {
            return;
        }
    }
#endif
    for( i = 0; i < num; ++i ) {
        PutPalette( i, colours[ i ] );
    }
}


short _WCI86FAR _CGRAPH _remapallpalette( long _WCI86FAR *colours )
/*=======================================================

   This routine remaps the entire palette to the colours specified by
   the parameter.  It returns a success flag. */

{
    short               num;

#if defined( _NEC_PC )
    if( _CurrState->vc.mode == _98TEXT80 ) {
        _ErrorStatus = _GRERROR;
        return( 0 );
    }
    num = _CurrState->vc.numcolors;
#else
    if( _CurrState->vc.adapter < _MCGA ||
        ( _CurrState->vc.mode == 7 || _CurrState->vc.mode == 15 ) ) {
        _ErrorStatus = _GRERROR;
        return( 0 );
    }
    if( _GrMode ) {
        num = _CurrState->vc.numcolors;
    } else {
        num = 16;       // vc.numcolors is 32
    }
#endif
    _RemapNum( colours, num );
    return( -1 );
}

Entry( _REMAPALLPALETTE, _remapallpalette ) // alternate entry-point


#if defined( _NEC_PC )

static short            NECPalTable[ 8 ] = {
    0, 2, 1, 3, 0, 2, 1, 3
};

static short ConvertColour( long colour )
//=======================================

// Convert a long colour value to an index in the range 0 to 7.

{
    short                 i;

    for( i = 0; i < 16; i++ ) {
        if( colour == _NECDefPalette[ i ] ) {
            return( i & 7 );
        }
    }
    return( colour & 7 );         /* otherwise, just use colour mod 8 */
}


static void PutPalette( short pixval, long colour )
//=================================================

{
    unsigned short      blue;
    unsigned short      green;
    unsigned short      red;
    short               col;
    char                index;
    char                palette;

    switch( _CurrState->vc.mode ) {
    case _98RESS8COLOR :
    case _98RESS16COLOR :
    case _98RES8COLOR :
    case _98RES16COLOR :
    case _98HIRES16COLOR :
    case _98HIRESS16COLOR :
        green = ( colour >> 8 ) & 0x0000000F;
        red = colour & 0x0000000F;
        blue = ( colour >> 16 ) & 0x0000000F;

        outp( 0xA8, pixval );           // colour index value to change
        outp( 0xAA, green );            // amount of green
        outp( 0xAC, red );              //    "   "  red
        outp( 0xAE, blue );             //    "   "  blue
        break;

    case _98RESSCOLOR :
    case _98RESCOLOR :
        col = ConvertColour( colour );
        col = _SwapBits( col );
        index = NECPalTable[ pixval ];
        palette = _NECPalette[ index ];

        if( pixval >= 4 ) {
            palette = ( palette & 0xF0 ) | col;
        } else {
            palette = ( palette & 0x0F ) | ( col << 4 );
        }
        _NECPalette[ index ] = palette;

        outp( 0xAE - 2 * index, palette ); // remap specified colour
        break;
    }
}


static long GetPalette( short pixval )
//====================================

{
    long                prev;
    char                index;
    char                palette;

    switch( _CurrState->vc.mode ) {
    case _98RESS8COLOR :
    case _98RESS16COLOR :
    case _98RES8COLOR :
    case _98RES16COLOR :
        prev = 0;
        break;
    case _98RESSCOLOR :
    case _98RESCOLOR :
        index = NECPalTable[ pixval ];
        palette = _NECPalette[ index ];
        if( pixval >= 4 ) {
            prev = palette & 0x0F;
        } else {
            prev = palette >> 4;
        }
    }
    return( prev );
}


#elif defined( _DEFAULT_WINDOWS )

static void PutPalette( short pixval, WPI_COLOUR colour )
//=================================================

{
    short               red, green, blue;

    red = ( colour & 0x000000ff );
    red = red * 4.0625f;
    green = ( colour & 0x0000ff00 ) >> 8;
    green = green * 4.0625f;
    blue = ( colour & 0x00ff0000 ) >> 16;
    blue = blue * 4.0625f;
    _Set_RGB_COLOR( pixval, _wpi_getrgb( red, green, blue ) );
}


static WPI_COLOUR GetPalette( short pixval )
//==========================================

{
    return _Get_RGB_COLOR( pixval );
}


#else

/* EGA Colour Mapping
   ==================

   In modes 13 and 14, each of the 16 colour indices (or pixel values)
   may be assigned one of the 16 colours values ( 0..7, 0x10 + 8..15 ).

   In mode 16, each of the colour indices may be assigned one of the 64
   possible colours. These colour values are of the form 00rgbRGB, where
   rgb are the secondary intensities and RGB are the high intensities.
   REMAPPALETTE uses the red, green and blue values to derive an EGA
   colour value.

   In mode 16, if there is only 64K of EGA memory (4 colours) the colour
   indices are actually 0, 1, 4, 5.
*/


#define PRIMARY         1
#define SECONDARY       8


static char             EGA_Intensity[] = {
    0,                      /*  0..15 */
    SECONDARY,              /* 16..31 */
    PRIMARY,                /* 32..47 */
    PRIMARY + SECONDARY     /* 48..63 */
};


extern long             GetVGAPalette( short func, short reg );
#if defined ( __386__ )
    #pragma aux GetVGAPalette = 0x55              /* push    ebp  */ \
                                0xcd 0x10         /* int     10H  */ \
                                0x5d              /* pop     ebp  */ \
                                0x86 0xcd         /* xchg    cl,ch    */ \
                                0x0f 0xb7 0xc1    /* movzx   eax,cx   */ \
                                0xc1 0xe0 0x08    /* shl     eax,08H  */ \
                                0x8a 0xc6         /* mov     al,dh    */ \
                                parm caller [eax] [ebx] value [eax] \
                                modify [ecx edx];
#else
    #pragma aux GetVGAPalette = 0x55          /* push bp */ \
                                0xcd 0x10     /* int 10h */ \
                                0x5d          /* pop bp  */ \
                                0x88 0xec     /* mov ah,ch (green) */ \
                                0x88 0xf0     /* mov al,dh (red)   */ \
                                0x88 0xca     /* mov dl,cl (blue)  */ \
                                0xb6 0x00     /* mov dh,0          */ \
                                parm caller [ax] [bx] value [ax dx] modify [cx];
#endif


static void PutPalette( short pixval, long colour )
//=================================================

{
    unsigned short      blue;
    unsigned short      green;
    unsigned short      red;
    short               cnvcol;
    short               mode;

    blue = ( (unsigned long)colour & 0x00ff0000 ) >> 16;
    green = (unsigned short)( colour & 0x0000ff00 ) >> 8;
    red = colour & 0x000000ff;
    switch( _CurrState->vc.adapter ) {
    case _MCGA :
    case _VGA :
    case _SVGA :
        VideoInt( _BIOS_SET_PALETTE + 0x10, pixval, ( green << 8 ) + blue, red << 8 );
        break;
   case _EGA :
        mode = _CurrState->vc.mode;
        if( mode == 13 || mode == 14 ) {
            cnvcol = _CnvColour( colour );
            if( cnvcol > 7 ) {
                cnvcol |= 0x10;         /* set intensity bit */
            }
        } else {
            red >>= 4;      /* map from range 0..63 to range 0..3 */
            green >>= 4;
            blue >>= 4;
            cnvcol = EGA_Intensity[ blue ] + ( EGA_Intensity[ green ] << 1 )
                                           + ( EGA_Intensity[ red ] << 2 );
        }
        VideoInt( _BIOS_SET_PALETTE, ( cnvcol << 8 ) + pixval, 0, 0 );
    }
}


static long GetPalette( short pixval )
//====================================

{
    long                prev;

    switch( _CurrState->vc.adapter ) {
    case _MCGA :
    case _VGA :
    case _SVGA :
        prev = GetVGAPalette( _BIOS_SET_PALETTE + 0x15, pixval );
        break;
    case _EGA :
        prev = 0;
    }
    return( prev );
}

#endif
