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


#include "variety.h"
#include <string.h>
#include "win.h"

/*
 * _RepaintWindow
*/
#if defined( __OS2__ )
void _RepaintWindow( LPWDATA w, PRECT rcPaint, HPS ph )
#else
void _RepaintWindow( LPWDATA w, PRECT rcPaint, HDC ph )
#endif
{
    int                 i;
    WORD                ptop,pbot,pleft,pright,poff,pdown;
    WORD                width;
#ifdef _MBCS
    mb_char _WCI86FAR * image;
    char                mbc[MB_CUR_MAX+1];
#else
    LPSTR               image;
#endif
#if !defined( __OS2__ )
    HFONT               oldfont;
#endif

    /*
     * get area to redraw
     */
    ptop = rcPaint->top / w->ychar;
    if( ptop != 0 && (rcPaint->top % w->ychar != 0) )  ptop--;
    pbot = rcPaint->bottom / w->ychar ;
    if( pbot == 0 )  return;
    if( pbot >= w->height )  pbot = w->height-1;
    if( rcPaint->bottom % w->ychar == 0 )  pbot--;
    pleft = 0;
    pright = w->width-1;
    width = w->width;
    poff = pleft * w->xchar;
    pdown = ptop * w->ychar;

    /*
     * draw all lines
     */
#if defined( __OS2__ )
#else
    oldfont = SelectObject( (HDC)ph, _FixedFont );
#endif
    for( i=ptop;i<=pbot;i++ ) {
        image = &w->image[i*w->width];
#if defined( __OS2__ )
        {
            POINTL      ptl;

            ptl.x = poff;
            ptl.y = ( w->y2 - w->y1 ) - (pdown + w->ychar) + w->base_offset;
            #ifdef _MBCS
            {
                int         count;
                char _WCI86FAR * buff;

                buff = _MemAlloc( sizeof(mb_char)*(width+1) );
                *buff = '\0';
                for( count=0; count<width; count++ ) {
                    _mbvtop( image[pleft+count], mbc );
                    mbc[_mbclen(mbc)] = '\0';
                    FARstrcat( buff, mbc );
                }
                GpiCharStringAt( (HPS)ph, &ptl, FAR_mbsnbcnt(buff,width), buff );
                _MemFree( buff );
            }
            #else
                GpiCharStringAt( (HPS)ph, &ptl, width, &image[pleft] );
            #endif
        }
#else
        SetBkColor( (HDC)ph, _ColorMap[w->background_color] );
        SetTextColor( (HDC)ph, _ColorMap[w->text_color] );
        #ifdef _MBCS
        {
            int         count, bytes;
            char _WCI86FAR *    buff;

            buff = _MemAlloc( sizeof(mb_char)*(width+1) );
            *buff = '\0';
            for( count=0; count<width; count++ ) {
                _mbvtop( image[pleft+count], mbc );
                mbc[_mbclen(mbc)] = '\0';
                FARstrcat( buff, mbc );
            }
            bytes = FARstrlen( buff );
            TextOut( (HDC)ph, poff, pdown, buff, bytes );
            _MemFree( buff );
        }
        #else
            TextOut( (HDC)ph, poff, pdown, &image[pleft], width );
        #endif
#endif
        pdown += w->ychar;
    }
#if !defined( __OS2__ )
    SelectObject( (HDC)ph, oldfont );
#endif

} /* _RepaintWindow */
