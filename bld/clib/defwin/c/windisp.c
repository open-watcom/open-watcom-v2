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
#if defined( __OS2__ )
  #define INCL_GPI
#endif
#include "win.h"


/*
 * _DisplayAllLines - show all visible lines...
 */
void _DisplayAllLines( LPWDATA w, int clearFlag )
{
    LPLDATA     ld;
    int         i,end;
    DWORD       ln;
#ifdef __OS2__
#else
    HDC         dc;
    RECT        rect;
    HBRUSH      oldBrush;
#endif

    /*** If needed, clear the window to avoid residue ***/
    if( clearFlag ) {
        #ifdef __OS2__
            /* Clearing done for OS/2 in _DisplayLineInWindowWithColor */
        #else
            /* Clearing doesn't always work as with OS/2, so it's done here */
            dc = GetDC( w->hwnd );
            #ifndef __NT__
                UnrealizeObject( w->brush );
            #endif
            oldBrush = SelectObject( dc, w->brush );
            #ifdef __NT__
                SetBrushOrgEx( dc, 0, 0, NULL  );
            #endif
            GetClientRect( w->hwnd, &rect );
            FillRect( dc, &rect, w->brush );
            SelectObject( dc, oldBrush );
            ReleaseDC( w->hwnd, dc );
        #endif
    }

    ln = w->TopLineNumber;
    ld = _GetLineDataPointer( w, ln );
    end = w->height;

    for( i=1; i<end; i++ ) {
        if( ld == NULL ) {
            _DisplayLineInWindow( w, i, (LPSTR) " " );
        } else {
            _DisplayLineInWindow( w, i, ld->data );
            ln++;
            ld = _GetLineDataPointer( w, ln );
        }
    }

} /* _DisplayAllLines */


/*
 * _DisplayLineInWindowWithColor - as it sounds!
 */
void _DisplayLineInWindowWithColor( LPWDATA w, int line, LPSTR text, int c1,
                        int c2, int extra, int startcol )
{
    LPSTR       tmp;
    char        buff[256];
    int         start,end,a,spend,cnt1,cnt2;
    WORD        i;
    HWND        hwnd;

    hwnd = w->hwnd;

    /*** Find dimensions of line ***/
    #ifdef _MBCS
        tmp = (LPSTR) FAR_mbsninc( text, startcol );
        a = FAR_mbslen( tmp );
        if( line<1 || line>=w->height )  return;
        start = 0;
        spend = end = w->width - extra;
        if( a < end )
            end = a;
        cnt1 = FAR_mbsnbcnt( tmp, end-start );
        cnt2 = spend - end;
        FAR_mbsnbcpy( buff, tmp, cnt1-start );
        FARmemset( buff+cnt1, ' ', cnt2 );
        tmp = (LPSTR) FAR_mbsninc( buff, cnt1+cnt2 );
        *tmp = '\0';
    #else
        tmp = text;
        tmp += startcol;
        a = FARstrlen( tmp );
        if( line < 1 || line >= w->height ) return;
        start = 0;
        spend = end = w->width-extra;
        if( a < end ) end = a;
        cnt1 = end-start;
        cnt2 = spend-end;
        FARmemcpy( buff, tmp, cnt1 );
        FARmemset( buff+cnt1, ' ', cnt2 );
        buff[cnt1+cnt2] = 0;
    #endif
    line--;

#if defined( __OS2__ )
    {
        RECTL           rcl;
        HPS             ps;
        POINTL          ptl;
        POINTL          points[TXTBOX_COUNT];

        ptl.x = 0;
        ptl.y = (w->y2 - w->y1) - (line+1)*w->ychar + w->base_offset;
        ps = WinGetPS( hwnd );
        _SelectFont( ps );
        GpiQueryTextBox( ps, startcol, w->tmpbuff->data, TXTBOX_COUNT, points );
        rcl.xLeft = points[TXTBOX_BOTTOMRIGHT].x;
        #ifdef _MBCS
            GpiQueryTextBox( ps, __mbslen(buff), buff, TXTBOX_COUNT, points );
        #else
            GpiQueryTextBox( ps, strlen(buff), buff, TXTBOX_COUNT, points );
        #endif
        rcl.xRight = points[TXTBOX_BOTTOMRIGHT].x;
        rcl.yTop = (w->y2 - w->y1) - line*w->ychar;
        rcl.yBottom = rcl.yTop - w->ychar;
        WinFillRect( ps, &rcl, c1 );
        GpiSetColor( ps, c2 );
        #ifdef _MBCS
            GpiCharStringAt( ps, &ptl, _mbsnbcnt(buff,w->width), buff );
        #else
            GpiCharStringAt( ps, &ptl, w->width, buff );
        #endif
        WinReleasePS( ps );
    }
#else
    {
        HDC     dc;
//      SIZE    size;
//      RECT    rect;

        dc = GetDC( hwnd );
        _SetMyDC( dc, _ColorMap[c2], _ColorMap[c1] );
//      #ifdef __NT__
//          GetTextExtentPoint32( dc, buff, strlen(buff), &size );
//      #else
//          GetTextExtentPoint( dc, buff, strlen(buff), &size );
//      #endif
//
        #ifdef _MBCS
            TextOut( dc, 0, line*w->ychar, buff, FAR_mbsnbcnt(buff,w->width) );
        #else
            TextOut( dc, 0, line*w->ychar, buff, w->width );
        #endif

        /*** Clear to end of line to remove any residue ***/
//      GetClientRect( w->hwnd, &rect );
//      rect.top = line * w->ychar;
//      rect.bottom = (line+1) * w->ychar ;
//      rect.left = size.cx;
//      FillRect( dc, &rect, w->brush );
        ReleaseDC( hwnd, dc );
    }
#endif

    /*** Update the w->image array ***/
    #ifdef _MBCS
    {
        mb_char         mbc;
        char *          curMbc;
        int             count;

        i = line*w->width + startcol;
        for( count=0,curMbc=buff; count<w->width-startcol; count++ ) {
            mbc = _mbsnextc( curMbc );          /* get the character */
            curMbc = _mbsinc( curMbc );         /* point to next char */
            w->image[i+count] = mbc;            /* store it in w->image */
        }
    }
    #else
        i = line*w->width + startcol;
        FARmemcpy( &w->image[i], buff, w->width-startcol );
    #endif
} /* _DisplayLineInWindowWithColor */

/*
 * _DisplayLineInWindow - do as it sounds, use default colors
 */
void _DisplayLineInWindow( LPWDATA w, int line, LPSTR text )
{
    int         c1,c2;

    c1 = w->text_color;
    c2 = w->background_color;
    _DisplayLineInWindowWithColor( w, line, text, c1, c2, 0, 0 );

} /* _DisplayLineInWindow */

/*
 * _ClearWindow - erase a window
 */
void _ClearWindow( LPWDATA w )
{
    HWND        hwnd;

    hwnd = w->hwnd;

    /*** Clear the w->image array ***/
    #ifdef _MBCS
    {
        mb_char         mbc;
        int             count;

        mbc = _mbsnextc( " " );
        for( count=0; count<w->width*w->height; count++ )
            w->image[count] = mbc;              /* store space in w->image */
    }
    #else
        FARmemset( w->image, 0x20, w->width*w->height );
    #endif

#if defined( __OS2__ )
    {
        HPS     ps;
        RECTL   rcl;

        ps = WinGetPS( hwnd );
        WinQueryWindowRect( hwnd, &rcl );
        WinFillRect( ps, &rcl, CLR_WHITE );
        WinReleasePS( ps );
    }
#else
    {
        RECT    rect;
        HDC     dc;

        dc = GetDC( hwnd );
        #ifndef __NT__
            UnrealizeObject( w->brush );
        #endif
        SelectObject( dc, w->brush );
        #ifdef __NT__
            SetBrushOrgEx( dc, 0, 0, NULL  );
        #endif
        GetClientRect( hwnd, &rect );
        FillRect( dc, &rect, w->brush );
        ReleaseDC( hwnd, dc );
    }
#endif

} /* _ClearWindow */

/*
 * _ShiftWindow - move current window up/down
*/
void _ShiftWindow( LPWDATA w, int diff )
{
    HWND        hwnd;
    int         amt;
    LPSTR       txt_s,txt_d;
    int         i,sline,eline,add;

    hwnd = w->hwnd;
    amt = -diff*w->ychar;

#if defined( __OS2__ )
    {
    RECTL       rcl;

    WinQueryWindowRect( hwnd, &rcl );
    WinScrollWindow( hwnd, 0, -amt, NULL, &rcl, NULL, NULL,
                     SW_INVALIDATERGN );
    }
#else
    {
    RECT        rect;

    GetClientRect( hwnd, &rect );
    i = (rect.bottom+1) / w->ychar;
    rect.bottom = i * w->ychar;
    ScrollWindow( hwnd, 0, amt, NULL, &rect );
    }
#endif

    if( diff < 0 ) {
        sline = w->height-3+diff;
        if( sline < 0 ) sline = 0;
        eline = 0;
        add = -1;
    } else {
        sline = diff;
        eline = w->height-1;
        if( eline < sline ) eline = sline;
        add = 1;
    }
    i = sline;
    while( i != eline ) {
        txt_s = (LPSTR) &w->image[i*w->width];
        txt_d = (LPSTR) &w->image[(i-diff)*w->width];
        #ifdef _MBCS
            FARmemcpy( txt_d, txt_s, sizeof(mb_char)*w->width );
        #else
            FARmemcpy( txt_d, txt_s, w->width );
        #endif
        i += add;
    }

} /* _ShiftWindow */

/*
 * _ResizeWin - give a window a new size/location
 */
void _ResizeWin( LPWDATA w, int x1, int y1, int x2, int y2 )
{
    int height;

    height = (y2-y1+1)/w->ychar+1;
    if( w->CurrentLineNumber - w->TopLineNumber >= height ) {
        w->CurrentLineNumber = w->TopLineNumber + height-1;
    }

    w->x1 = x1;
    w->x2 = x2;
    w->y1 = y1;
    w->y2 = y2;
    w->width = (x2-x1+1)/w->xchar+1;
    w->height = height;

    /*** Initialize a new w->image array ***/
    _MemFree( w->image );
    #ifdef _MBCS
    {
        mb_char         mbc;
        int             count;

        w->image = _MemAlloc( sizeof(mb_char)*w->width*w->height );
        mbc = _mbsnextc( " " );
        for( count=0; count<w->width*w->height; count++ )
            w->image[count] = mbc;              /* store space in w->image */
    }
    #else
        w->image = _MemAlloc( w->width*w->height );
        FARmemset( w->image, 0x20, w->width*w->height );
    #endif

    if( w->width > w->maxwidth ) w->maxwidth = w->width;

} /* _ResizeWin */
