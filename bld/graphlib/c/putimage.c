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
#include "picdef.h"


_WCRTLINK void _WCI86FAR _CGRAPH _putimage( short x, short y, char _WCI86HUGE * image,
/*==========================================*/ short dispmode )

/* This routine retrieves an picture stored at image and places it on the
   screen starting at ( x, y ), in viewport coordinates. The mode in which
   the picture is placed on the screen is specified by dispmode.    */

{
    if( _GrProlog() ) {
        _L2putimage( _VtoPhysX( x ), _VtoPhysY( y ), image, dispmode );
        _GrEpilog();
    }
}

Entry1( _PUTIMAGE, _putimage ) // alternate entry-point


#if defined( _DEFAULT_WINDOWS )

void _WCI86FAR _L2putimage( short x, short y, char _WCI86HUGE *image, short dispmode )
/**************************************************************************/

/* This routine retrieves an picture stored at image and places it on the
   screen starting at ( x, y ), in physical coordinates. The mode in which
   the picture is placed on the screen is specified by dispmode.    */

{
    long                Rop;
    WPI_PRES            dc;
    HRGN                temprgn;
    WPI_RECT            clip_rect;
    short               x1, y1, x2, y2;
    short               clipy1, clipy2;
    short               desty;
    struct picture _WCI86FAR *picture;

    picture = (struct picture _WCI86FAR *) image;
    x1 = x;
    y1 = y;
    x2 = x1 + picture->picwidth - 1;
    y2 = y1 + picture->picheight - 1;

  #if defined( __OS2__ )
    desty = _wpi_cvth_y( y2, _GetPresHeight() );
  #else
    desty = y;
  #endif

    if( _L0BlockClip( &x1, &y1, &x2, &y2 ) != 0 ) { /* clip rectangle to vp */
        _ErrorStatus = _GRNOOUTPUT;                 /* image is completely  */
        return;                                     /* outside view port    */
    }

    dc = _Mem_dc;

    temprgn = _ClipRgn;
    clipy1 = _wpi_cvth_y( _CurrState->clip_def.ymin, _GetPresHeight() );
    clipy2 = _wpi_cvth_y( _CurrState->clip_def.ymax + 1, _GetPresHeight() );
    _wpi_setintwrectvalues( &clip_rect,
                           _CurrState->clip_def.xmin,
                           clipy1,
                           _CurrState->clip_def.xmax + 1,
                           clipy2 );
    _ClipRgn = _wpi_createrectrgn( dc, &clip_rect );
    _wpi_getclipbox( dc, &clip_rect);
    _wpi_selectcliprgn( dc, _ClipRgn );
    _wpi_deletecliprgn( dc, temprgn );

    // Map our putimage mode to windows.
    switch( dispmode ) {
        case _GPSET     : Rop = SRCCOPY;
                          break;
        case _GPRESET   : Rop = NOTSRCCOPY;
                          break;
        case _GAND      : Rop = SRCAND;
                          break;
        case _GOR       : Rop = SRCPAINT;
                          break;
        case _GXOR      : Rop = SRCINVERT;
                          break;
        default         : return;
    }

// Move the image from the memory DC to the screen
    _wpi_bitblt( dc, x, desty, picture->picwidth,
                 picture->picheight, picture->buffer,
                 0, 0, Rop );

// Cleanup
    temprgn = _ClipRgn;
    _ClipRgn = _wpi_createrectrgn( dc, &clip_rect );
    _wpi_selectcliprgn( dc, _ClipRgn );
    _wpi_deletecliprgn( dc, temprgn );

// Update screen
    x2 += 1;
    y2 += 1;
    y1 = _wpi_cvth_y( y1, _GetPresHeight() );
    y2 = _wpi_cvth_y( y2, _GetPresHeight() );
    _MyInvalidate( x1, y1, x2, y2 );
    _RefreshWindow();
}

#else

static void             NegImage( char _WCI86HUGE *, long );


void _WCI86FAR _L2putimage( short x, short y, char _WCI86HUGE * image, short dispmode )
/*===========================================================================

   This routine retrieves an picture stored at image and places it on the
   screen starting at ( x, y ), in physical coordinates. The mode in which
   the picture is placed on the screen is specified by dispmode.    */

{
    short               prev_mode;       /* previous display mode */
    short               line_len;
    long                size;
    struct picture _WCI86FAR *picture;

    picture = (struct picture _WCI86FAR *) image;
    line_len = _RowLen( picture->picwidth );
    prev_mode = _setplotaction( dispmode );
    if( dispmode == _GPRESET ) {
        size = (long) ( picture->picheight ) * line_len;
        NegImage( &picture->buffer, size );
    }
    _L1PutPic( x, y, line_len, (struct picture _WCI86HUGE *)image );
    if( dispmode == _GPRESET ) {
        NegImage( &picture->buffer, size );
    }
    _setplotaction( prev_mode );
}


static void NegSome( char _WCI86FAR *pic, long size )
//==============================================

{
    while( size != 0 ) {
        *pic = ~(*pic);
        ++pic;
        --size;
    }
}

#define _64_K           0x10000L

static void NegImage( char _WCI86HUGE *pic, long size )
/*================================================

    This routine negates every byte of the image buffer so that
    the image can be displayed in _GPRESET mode.
    Note: Huge pointers are no longer guaranteed to be normalized
          and may not be contiguous. Therefore we must negate only
          up to 64K boundaries at a time. */

{
#if !defined( __386__ )
    long                max;

    max = _64_K - FP_OFF( pic );      // max before pointer overflows
    if( size > max ) {
        NegSome( (char _WCI86FAR *) pic, max );      // bring to 64K boundary
        pic += max;
        size -= max;
        while( size >= _64_K ) {
            NegSome( (char _WCI86FAR *) pic, _64_K );
            pic += _64_K;
            size -= _64_K;
        }
    }
#endif
    NegSome( (char _WCI86FAR *) pic, size );         // do remaining part
}

#endif
