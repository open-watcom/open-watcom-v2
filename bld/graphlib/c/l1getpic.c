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
* Description:  Low level bitmap reading routine.
*
****************************************************************************/


#include "gdefn.h"
#include "picdef.h"
#if !defined( _DEFAULT_WINDOWS )
#include <malloc.h>
#include "stkavail.h"
#endif


// Note: Memory in huge segments, under DOS extenders like RUN286,
//       will not necessarily be consecutive. For this reason, when
//       reading a row that will overflow the buffer, we must read
//       into a temporary buffer and copy it a byte at a time.


void _L1GetPic( short x1, short y1, short x2, short y2,
/*===============================*/ struct picture _WCI86HUGE *image )

/*  Copy that portion of the screen inside the viewport to the buffer
    pointed by 'image'. */

{
    short               dx;             /* width of rectangle in pixels     */
    short               dy;             /* height of rectangle in pixels    */
    short               t;
#if defined( _DEFAULT_WINDOWS )
    WPI_PRES            dc;
    short               srcy;
#else
    short               line_len;       /* length of each line in bytes     */
  #if !defined( __386__ )
    unsigned short      new_off;
  #endif
    char                *tmp;
    gr_device _FARD     *dev_ptr;       /* pointer to _CurrState->deviceptr */
    char _WCI86HUGE     *pic;           /* buffer to store image            */
    copy_fn             *copy;          /* pointer to copy routine          */
    setup_fn            *setup;
#endif

    if( x1 > x2 ) {         // ensure x1 < x2
        t = x1;
        x1 = x2;
        x2 = t;
    }
    if( y1 > y2 ) {         // ensure y1 < y2
        t = y1;
        y1 = y2;
        y2 = t;
    }
    if( _L0BlockClip( &x1, &y1, &x2, &y2 ) != 0 ) {     /* clip image to    */
        image->picwidth = 0;                            /* active viewport  */
        image->picheight = 0;
        _ErrorStatus = _GRNOOUTPUT;
        return;
    }
    dx = x2 - x1 + 1;                               /* row width in pixels  */
    dy = y2 - y1 + 1;                               /* height in pixel rows */
    image->picwidth = dx;                   /* save width in image picture  */
    image->picheight = dy;                  /* save height in image picture */
#if defined( _DEFAULT_WINDOWS )
    dc = _Mem_dc;

// Create a memory DC to put the image in
    image->buffer = _wpi_createcompatiblepres( dc, _GetInst(), &( image->pdc ) );
    image->bmp = _wpi_createcompatiblebitmap( dc, dx, dy );
    if( ( image->buffer == NULL ) || ( image->bmp == NULL ) ) {
         _ErrorStatus = _GRINSUFFICIENTMEMORY;
         return;
    }
    _wpi_selectbitmap( image->buffer, image->bmp );

// Transfer the image to a memory DC
  #if defined( __OS2__ )
    srcy = _wpi_cvth_y( y2, _GetPresHeight() );
  #else
    srcy = y1;
  #endif
    _wpi_bitblt( image->buffer, 0, 0, dx, dy, dc, x1, srcy, SRCCOPY );

#else
    image->bpp = _CurrState->vc.bitsperpixel;   /* save bpp - never used ?  */
    line_len = _RowLen( dx );                   /* width of row in bytes    */

    _StartDevice();

    dev_ptr = _CurrState->deviceptr;
    copy = dev_ptr->readrow;
    pic = &image->buffer;
    tmp = NULL;
    setup = dev_ptr->setup;
    for( ; y1 <= y2; ++y1 ) {               /* copy screen image to buffer  */
        ( *setup )( x1, y1, 0 );
  #if !defined( __386__ )
        // check whether the entire row will fit in the buffer
        new_off = FP_OFF( pic ) + line_len - 1;
        if( new_off < FP_OFF( pic ) ) {
            if( tmp == NULL ) {     // may have been already allocated
                if( _stackavail() - line_len > 0x100 ) {
                    tmp = __alloca( _RoundUp( line_len ) );
                }
            }
            if( tmp != NULL ) {
                ( *copy )( tmp, _Screen.mem, dx, _Screen.bit_pos, 0 );
                for( t = 0; t < line_len; ++t ) {
                    *pic = tmp[ t ];
                    ++pic;      // the PIA function will handle this properly
                }
            } else {
                _ErrorStatus = _GRINSUFFICIENTMEMORY;
                pic += line_len;
            }
        } else {
  #endif
            ( *copy )( pic, _Screen.mem, dx, _Screen.bit_pos, 0 );
            pic += line_len;
  #if !defined( __386__ )
        }
  #endif
    }

    _ResetDevice();
#endif
}
