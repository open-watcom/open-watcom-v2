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


WPI_COLOUR              _RGB_COLOR[ 256 ];
HRGN                    _ClipRgn;
HFONT                   _CurFnt;
static HFONT            _SysMonoFnt;
static long             _Pres_h;
static WPI_INST         _Inst;


int _MapPlotAct()
//===============

/*  This function maps the WATCOM plot action modes to Windows' drawing
    modes.  Returns the drawing mode for Windows */
{
    switch( _PlotAct ) {
    default :
    case 0 :
        return R2_COPYPEN;
    case 1 :
        return R2_XORPEN;
    case 2 :
        return R2_MASKPEN;
    case 3 :
        return R2_MERGEPEN;
    }
}


#if defined( __WINDOWS__ )

HBITMAP _Mask2Bitmap( HDC dc, char *mask )
/****************************************/

/* This function maps the WATCOM fillmask to a Windows' bitmap */
{
    struct bitmap_desc{
        BITMAPINFOHEADER        Head;
        RGBQUAD                 Colors[2];
    } bminfo;
    BYTE                pad_mask[ 4 * MASK_LEN ];
    int                 i;      /* counters */
    long                color, bkcolor;

    /* set up the byte array for the bitmap */
    color = _Col2RGB( _CurrColor );
    bkcolor = _Col2RGB( _CurrBkColor );
    for( i=0; i < MASK_LEN; ++i ) {
        pad_mask[i*4] = mask[i];
    }

    /* set up all the information about the bitmap */
    bminfo.Head.biSize          = sizeof(BITMAPINFOHEADER);
    bminfo.Head.biWidth         = 8L;
    bminfo.Head.biHeight        = MASK_LEN;
    bminfo.Head.biPlanes        = 1;
    bminfo.Head.biBitCount      = 1;
    bminfo.Head.biCompression   = 0L;
    bminfo.Head.biSizeImage     = 0L;
    bminfo.Head.biXPelsPerMeter = 0L;
    bminfo.Head.biYPelsPerMeter = 0L;
    bminfo.Head.biClrUsed       = 0L;
    bminfo.Head.biClrImportant  = 0L;
    bminfo.Colors[0].rgbRed     = GetRValue( bkcolor );
    bminfo.Colors[0].rgbBlue    = GetBValue( bkcolor );
    bminfo.Colors[0].rgbGreen   = GetGValue( bkcolor );
    bminfo.Colors[1].rgbRed     = GetRValue( color );
    bminfo.Colors[1].rgbBlue    = GetBValue( color );
    bminfo.Colors[1].rgbGreen   = GetGValue( color );

    /* return the bitmap handle */
    return ( CreateDIBitmap(  dc,
                              ( LPBITMAPINFOHEADER ) & ( bminfo.Head ),
                              CBM_INIT,
                              pad_mask,
                              ( LPBITMAPINFO )(&bminfo),
                              DIB_RGB_COLORS ) );
}

#else

HBITMAP _Mask2Bitmap( HDC dc, char *mask )
/****************************************/

/* This function maps the WATCOM fillmask to a Windows' bitmap */
{
    unsigned            bmsize;
    BITMAPINFO2*        bminfo;
    BYTE                pad_mask[ 4 * MASK_LEN ];
    HBITMAP             bmp;
    int                 i;      /* counters */

    bmsize = sizeof( BITMAPINFO2 ) + sizeof( WPI_RGBQUAD ) * 2;
    bminfo = _MyAlloc( bmsize );

    _wpi_settextcolor( dc, _Col2RGB( _CurrColor ) );
    _wpi_setbackcolour( dc, _Col2RGB( _CurrBkColor ) );
    /* set up the byte array for the bitmap */
    for( i=0; i < MASK_LEN; ++i ) {
        pad_mask[i * 4] = mask[MASK_LEN - i - 1];
    }

    /* set up all the information about the bitmap */
    _wpi_setbmphdrvalues( ( BITMAPINFOHEADER2 * )bminfo,
                          bmsize,
                          8L,
                          MASK_LEN,
                          1,
                          1,
                          0L,
                          0L,
                          0L,
                          0L,
                          0L,
                          0L );
    _wpi_setrgbquadvalues( &(bminfo->argbColor[0]), 0, 0, 0, 0 );
    _wpi_setrgbquadvalues( &(bminfo->argbColor[1]), 0xff, 0xff, 0xff, 0 );

    /* return the bitmap handle */
    _wpi_bmptorgb( ( BITMAPINFOHEADER2 * )bminfo );
    bmp = _wpi_createdibitmap(  dc,
                              ( BITMAPINFOHEADER2 * )bminfo,
                              CBM_INIT,
                              pad_mask,
                              bminfo,
                              DIB_RGB_COLORS );
    _MyFree( bminfo );
    return( bmp );
}

#endif


int _MapLineStyle( int style )
/****************************/

/* This function maps the WATCOM's line styles to Windows' line styles */
{
    int                 i, dots;

    if( style == SOLID_LINE ) {
        return PS_SOLID;
    } else if( style == 0 ) {
        return PS_NULL;
    } else {
        dots = 0;       // count bits turned on in style
        for( i = 0; i < 16; ++i ) {
            if( style & ( 1 << i ) ) {
                ++dots;
            }
        }
        if( dots <= 4 ) {
            return PS_DASHDOTDOT;
        } else if( dots <= 8 ) {
            return PS_DASHDOT;
        } else if( dots <= 12 ) {
            return PS_DOT;
        } else {
            return PS_DASH;
        }
    }
}


WPI_COLOUR _Col2RGB( short color )
//================================
{
    WPI_COLOUR rgb;

    color %= _CurrState->vc.numcolors;
    rgb = _wpi_palettergb( _Mem_dc,
                           _wpi_getrvalue ( _RGB_COLOR[ color ] ),
                           _wpi_getgvalue ( _RGB_COLOR[ color ] ),
                           _wpi_getbvalue ( _RGB_COLOR[ color ] ) );
    return( rgb );
}


short _RGB2Col( WPI_COLOUR color )
//================================
{
    int         i;
    WPI_COLOUR  rgb;

    for( i = 0; i < _CurrState->vc.numcolors; ++i ){
        rgb = _wpi_palettergb( _Mem_dc,
                               _wpi_getrvalue ( _RGB_COLOR[ i ] ),
                               _wpi_getgvalue ( _RGB_COLOR[ i ] ),
                               _wpi_getbvalue ( _RGB_COLOR[ i ] ) );
        if( ( rgb & 0x00ffffff ) == color ) {
            return i;
        }
    }
    return -1;
}


void _MyInvalidate( WPI_RECTDIM x1, WPI_RECTDIM y1,
                    WPI_RECTDIM x2, WPI_RECTDIM y2 )
//==================================================
/* This function invalidate a rectangular region */
{
    int         t;
    WPI_INST    inst;
    WPI_RECT    rect;
#if defined( __OS2__ )
    WPI_RECT    wrect;
    int         height;
#endif

    if( x1 > x2 ) {
        t = x2;
        x2 = x1;
        x1 = t;
    }
    if( y1 > y2 ) {
        t = y2;
        y2 = y1;
        y1 = t;
    }
    _wpi_setrectvalues( &rect, x1, y1, x2 + 1, y2 + 1 );
  #if defined( __OS2__ )
    GetClientRect( _CurrWin, &wrect );
    height = _wpi_getheightrect( wrect );
    t = _GetPresHeight() - height - _BitBlt_Coord.ycoord;
  #else
    t = _BitBlt_Coord.ycoord;
  #endif
    inst = _GetInst();
    _wpi_offsetrect( inst, &rect,
                     -_BitBlt_Coord.xcoord,
                     -t );
    _wpi_invalidaterect( _CurrWin, &rect, 0 );
}


HPEN _MyCreatePen( WPI_COLOUR color )
/*===================================
  This function creates a pen for windows using the current line style. */
{
    int line_style;

    line_style = _MapLineStyle( _LineStyle );
    return( _wpi_createpen( line_style, 0, color ) );
}


short _CreateSysMonoFnt( WPI_PRES dc )
/*====================================
  This function create the system monospaced font. */
{
#if defined( __OS2__ )
    FATTRS              fat;
    long                rc = 0;

    _SysMonoFnt = 254;
    fat.usRecordLength = sizeof( fat );
    fat.fsSelection = 0;
    fat.lMatch = 0;
    strcpy( fat.szFacename, "Courier\0" );
    fat.idRegistry = 0;
    fat.usCodePage = 0;
    fat.lMaxBaselineExt = 15;
    fat.lAveCharWidth = 15;
    fat.fsType = 0;
    fat.fsFontUse = 0;

    GpiSetCharSet( dc, LCID_DEFAULT );
    rc = GpiCreateLogFont( dc, NULL, _SysMonoFnt, &fat );
    if( rc == FONT_MATCH ) return TRUE;
    else return FALSE;
#else
   dc = dc;
   return TRUE;
#endif
}


HFONT _GetSysMonoFnt()
/*====================
  This function returns the system monospaced font handle. */
{
#if defined( __WINDOWS__ )
    _SysMonoFnt = GetStockObject( SYSTEM_FIXED_FONT );
#endif
    return( _SysMonoFnt );
}


void _SetPresHeight( long h )
/*===========================
  This function set the variable which contains the height of the memory
  presentation space. */
{
    _Pres_h = h;
}


long _GetPresHeight()
/*===================
  This function get the height of the memory presentation space. */
{
    return( _Pres_h );
}


void _SetInst( WPI_INST *inst )
/*=============================
  This function sets the static instant variable. */
{
    _Inst = *inst;
}


WPI_INST _GetInst()
/*=================
  This function return the current instant. */
{
    return _Inst;
}


void _wpi_free( void * ptr )
/*==========================
  What we use to free memory. */
{
    _MyFree( ptr );
}


void * _wpi_malloc( size_t size )
/*===============================
  What we use to allocate memory. */
{
    return _MyAlloc( size );
}

HFONT _MySelectFont( WPI_PRES pres, HFONT f)
/*==========================================
*/
{
    HFONT       retval;
#if defined( __OS2__ )
    retval = GpiQueryCharSet( pres );
    GpiSetCharSet( pres, f );
#else
    retval = _wpi_f_selectfont( pres, f);
#endif
    return retval;
}


void _MyGetOldFont( WPI_PRES pres, HFONT f )
/*===========================================
*/
{
#if defined( __OS2__ )
    GpiSetCharSet( pres, f );
#else
    _wpi_f_getoldfont( pres, f );
#endif
}

void _Set_RGB_COLOR( short i, WPI_COLOUR color )
/*===========================================
*/
{
    _RGB_COLOR[ i ] = color;
}

WPI_COLOUR _Get_RGB_COLOR( short i )
/*===========================================
*/
{
    return _RGB_COLOR[ i ];
}
