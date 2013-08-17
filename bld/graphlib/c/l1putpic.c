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
* Description:  Low level bitmap ouput routine.
*
****************************************************************************/


#include <malloc.h>
#include "gdefn.h"
#include "picdef.h"
#include "stkavail.h"


// Note: Memory in huge segments, under DOS extenders like RUN286,
//       will not necessarily be consecutive. For this reason, when
//       copying a row that will overflow the buffer, we must copy
//       into a temporary buffer before copying it to the screen.


void _L1PutPic( short px, short py, short line_len,
/*===============================*/ struct picture _WCI86HUGE * image )

/*  Copy that portion of the image stored in the structure 'image' to
    the screen, clipping the image to the viewport's boundary. (px,py)
    is the top left corner of the restored image.   */

{
    short               x1, y1;         /* coordinates of opposite          */
    short               x2, y2;         /* ... corners of rectangle         */
    short               dx;             /* width of rectangle in pixels     */
    short               skip_left;      /* # of rows above viewport         */
    short               skip_down;      /* # of bytes before viewport       */
    short               bit_offset;     /* starting bit in buffer           */
    short               plane_len;      /* width of each plane in bytes     */
#if !defined( __386__ )
    short               t;
    unsigned short      new_off;
#endif
    char                *tmp;
    gr_device _FARD     *dev_ptr;       /* pointer to _CurrState->deviceptr */
    char _WCI86HUGE     *pic;           /* buffer to store image            */
    copy_fn             *copy;          /* pointer to copy routine          */
    setup_fn            *setup;

    x1 = px;                            /* new rectangle for restoring image*/
    y1 = py;
    x2 = x1 + image->picwidth - 1;
    y2 = y1 + image->picheight - 1;
    if( _L0BlockClip( &x1, &y1, &x2, &y2 ) != 0 ) { /* clip rectangle to vp */
        _ErrorStatus = _GRNOOUTPUT;                 /* image is completely  */
        return;                                     /* outside view port    */
    }
    pic = &image->buffer;
    skip_left = x1 - px;        /* # of pixels to the left of the viewport  */
    skip_down = y1 - py;                /* # of pixels above the viewport   */
    if( skip_down > 0 ) {                               /* point to start   */
        pic += (long)line_len * skip_down;              /* on 1st row       */
    }
    if( skip_left > 0 ) {                   /* left edge has been clipped   */
        if( _CurrState->misc_info & PLANAR ) {
            pic += skip_left >> 3;              /* point to 1st byte in row */
            bit_offset = skip_left & 7;         /* bit position in 1st byte */
        } else {                            /* mode has one bit plane only  */
            skip_left *= _CurrState->vc.bitsperpixel;
            pic += skip_left >> 3;
            bit_offset = skip_left & 7;
        }
    } else {                                    /* left edge not clipped    */
        bit_offset = 0;                         /* start at byte boundary   */
    }

    _StartDevice();

    plane_len = line_len / _CurrState->vc.bitsperpixel; /* width of plane in bytes  */
    dev_ptr = _CurrState->deviceptr;
    copy = dev_ptr->pixcopy;
    dx = x2 - x1 + 1;
    tmp = NULL;
    setup = dev_ptr->setup;
    for( ; y1 <= y2; ++y1 ) {               /* copy image buffer to screen  */
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
                for( t = 0; t < line_len; ++t ) {
                    tmp[ t ] = *pic;
                    ++pic;      // the PIA function will handle this properly
                }
                ( *copy )( _Screen.mem, tmp, dx, ( _Screen.bit_pos << 8 ) + bit_offset, plane_len );
            } else {
                _ErrorStatus = _GRINSUFFICIENTMEMORY;
                pic += line_len;
            }
        } else {
#endif
            ( *copy )( _Screen.mem, pic, dx, ( _Screen.bit_pos << 8 ) + bit_offset, plane_len );
            pic += line_len;
#if !defined( __386__ )
        }
#endif
    }

    _ResetDevice();
}
