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


#include "imgedit.h"
#include "settings.h"

/*
 * FlipImage - flip the image along either the x-axis or the y-axis
 */
void FlipImage( WORD whichway )
{
    img_node    *node;
    short       width;
    short       height;
    short       destwidth;
    short       destheight;
    short       new_left;
    short       new_top;
    WPI_RECT    dims;
    HBITMAP     xorflip;
    HBITMAP     oldflip;
    HBITMAP     oldbitmap;
    HBITMAP     andflip;
    WPI_PRES    pres;
    WPI_PRES    mempres;
    HDC         memdc;
    WPI_PRES    flippres;
    HDC         flipdc;
    HCURSOR     prevcursor;
    IMGED_DIM   bottom;
    IMGED_DIM   left;
    IMGED_DIM   right;
    IMGED_DIM   top;

    node = GetCurrentNode();
    if( node == NULL ) {
        return;
    }

    PrintHintTextByID( WIE_FLIPPINGIMAGE, NULL );
    prevcursor = _wpi_setcursor( _wpi_getsyscursor( IDC_WAIT ) );

    if( !DoesRectExist( &dims ) ) {
        _wpi_setwrectvalues( &dims, 0, 0, (IMGED_DIM)node->width, (IMGED_DIM)node->height );
    }
    width = (short)_wpi_getwidthrect( dims );
    height = (short)_wpi_getheightrect( dims );

    pres = _wpi_getpres( HWND_DESKTOP );
    if( node->bitcount == 1 ) {
        xorflip = _wpi_createbitmap( width, height, 1, 1, NULL );
    } else {
#if 1
        xorflip = _wpi_createcompatiblebitmap( pres, width, height );
#else
        xorflip = _wpi_createbitmap( width, height, ColorPlanes, BitsPerPixel, NULL );
#endif
    }
    andflip = _wpi_createbitmap( width, height, 1, 1, NULL );

    mempres = _wpi_createcompatiblepres( pres, Instance, &memdc );
    flippres = _wpi_createcompatiblepres( pres, Instance, &flipdc );
    _wpi_releasepres( HWND_DESKTOP, pres );

    _wpi_getrectvalues( dims, &left, &top, &right, &bottom );
    _wpi_preparemono( flippres, BLACK, WHITE );

    oldbitmap = _wpi_selectobject( mempres, node->hxorbitmap );
    oldflip = _wpi_selectobject( flippres, xorflip );
    _wpi_bitblt( flippres, 0, 0, width, height, mempres, left, top, SRCCOPY );
    _wpi_selectobject( mempres, oldbitmap );
    _wpi_selectobject( flippres, oldflip );

    oldbitmap = _wpi_selectobject( mempres, node->handbitmap );
    oldflip = _wpi_selectobject( flippres, andflip );
    _wpi_bitblt( flippres, 0, 0, width, height, mempres, left, top, SRCCOPY );

    if( whichway == IMGED_FLIPVERT ) {
        destwidth = -1 * width;
        new_left = right - 1;
        new_top = top;
        destheight = height;
    } else {
        destwidth = width;
        new_left = left;
        new_top = bottom - 1;
        destheight = -1 * height;
    }

    _wpi_stretchblt( mempres, new_left, new_top, destwidth, destheight,
                     flippres, 0, 0, width, height, SRCCOPY );
    _wpi_selectobject( mempres, oldbitmap );
    oldbitmap = _wpi_selectobject( mempres, node->hxorbitmap );
    _wpi_selectobject( flippres, oldflip );
    oldflip = _wpi_selectobject( flippres, xorflip );
    _wpi_stretchblt( mempres, new_left, new_top, destwidth, destheight,
                     flippres, 0, 0, width, height, SRCCOPY );
    _wpi_selectobject( mempres, oldbitmap );
    _wpi_selectobject( flippres, oldflip );

    _wpi_deleteobject( xorflip );
    _wpi_deleteobject( andflip );
    _wpi_deletecompatiblepres( mempres, memdc );
    _wpi_deletecompatiblepres( flippres, flipdc );

    InvalidateRect( node->viewhwnd, NULL, TRUE );

    if( !DoKeepRect() ) {
        SetRectExists( FALSE );
    }
    RecordImage( node->hwnd );
    BlowupImage( node->hwnd, NULL );
    if( whichway == IMGED_FLIPHORZ ) {
        PrintHintTextByID( WIE_IMAGEREFLECTEDH, NULL );
    } else {
        PrintHintTextByID( WIE_IMAGEREFLECTEDV, NULL );
    }
    _wpi_setcursor( prevcursor );

} /* FlipImage */

/*
 * clipIntoArea - clip the bitmaps into the area when rotating
 */
static void clipIntoArea( img_node *node, WPI_RECT *rect,
                          HBITMAP rotxorbmp, HBITMAP rotandbmp )
{
    WPI_PRES    pres;
    WPI_PRES    xorpres;
    HDC         xordc;
    WPI_PRES    andpres;
    HDC         anddc;
    WPI_PRES    rotxorpres;
    HDC         rotxordc;
    WPI_PRES    rotandpres;
    HDC         rotanddc;
    HBITMAP     oldxor;
    HBITMAP     oldand;
    HBITMAP     oldxorrot;
    HBITMAP     oldandrot;
    short       width;
    short       height;
    WPI_POINT   centre_pt;
    WPI_POINT   topleft;
    short       start_x;
    short       start_y;
    short       new_width;
    short       new_height;
    IMGED_DIM   left;
    IMGED_DIM   right;
    IMGED_DIM   top;
    IMGED_DIM   bottom;

    width = (short)_wpi_getwidthrect( *rect );
    height = (short)_wpi_getheightrect( *rect );

    /*
     * PM NOTE:  We use getwrectvalues so that 'topleft' can really be
     * replace with 'bottomleft' (which is our origin for blitting in PM).
     * Hence the value of 'top' is really the bottom of our rectangle.
     */
    _wpi_getwrectvalues( *rect, &left, &top, &right, &bottom );
    centre_pt.x = (width / 2) + left;
    centre_pt.y = (height / 2) + top;

    topleft.x = max( left, centre_pt.x - centre_pt.y + top );
    topleft.y = max( top, centre_pt.y - centre_pt.x + left );

    if( topleft.x == left ) {
        start_x = (short)(left - (centre_pt.x - centre_pt.y + top));
        new_width = width;
    } else {
        start_x = 0;
        new_width = height;
    }
    if( topleft.y == top ) {
        start_y = (short)(top - (centre_pt.y - centre_pt.x + left));
        new_height = height;
    } else {
        start_y = 0;
        new_height = width;
    }

    pres = _wpi_getpres( HWND_DESKTOP );
    xorpres = _wpi_createcompatiblepres( pres, Instance, &xordc );
    andpres = _wpi_createcompatiblepres( pres, Instance, &anddc );
    rotxorpres = _wpi_createcompatiblepres( pres, Instance, &rotxordc );
    rotandpres = _wpi_createcompatiblepres( pres, Instance, &rotanddc );
    _wpi_releasepres( HWND_DESKTOP, pres );

    _wpi_torgbmode( xorpres );
    _wpi_torgbmode( andpres );
    _wpi_torgbmode( rotxorpres );
    _wpi_torgbmode( rotandpres );

    oldxor = _wpi_selectobject( xorpres, node->hxorbitmap );
    oldand = _wpi_selectobject( andpres, node->handbitmap );
    oldxorrot = _wpi_selectobject( rotxorpres, rotxorbmp );
    oldandrot = _wpi_selectobject( rotandpres, rotandbmp );

    _wpi_patblt( xorpres, left, top, width, height, WHITENESS );
    _wpi_patblt( andpres, left, top, width, height, BLACKNESS );

    _wpi_bitblt( xorpres, topleft.x, topleft.y, new_width, new_height,
                 rotxorpres, start_x, start_y, SRCCOPY );
    _wpi_bitblt( andpres, topleft.x, topleft.y, new_width, new_height,
                 rotandpres, start_x, start_y, SRCCOPY );

    _wpi_selectobject( xorpres, oldxor );
    _wpi_selectobject( andpres, oldand );
    _wpi_selectobject( rotxorpres, oldxorrot );
    _wpi_selectobject( rotandpres, oldandrot );
    _wpi_deletecompatiblepres( xorpres, xordc );
    _wpi_deletecompatiblepres( andpres, anddc );
    _wpi_deletecompatiblepres( rotxorpres, rotxordc );
    _wpi_deletecompatiblepres( rotandpres, rotanddc );

    InvalidateRect( node->viewhwnd, NULL, TRUE );
    RecordImage( node->hwnd );
    BlowupImage( node->hwnd, NULL );

} /* clipIntoArea */

/*
 * stretchIntoArea - stretch the rotated image into the area specified
 */
static void stretchIntoArea( img_node *node, WPI_RECT *rect,
                             HBITMAP rotxorbmp, HBITMAP rotandbmp )
{
    WPI_PRES    pres;
    WPI_PRES    xorpres;
    HDC         xordc;
    WPI_PRES    andpres;
    HDC         anddc;
    WPI_PRES    rotxorpres;
    HDC         rotxordc;
    WPI_PRES    rotandpres;
    HDC         rotanddc;
    HBITMAP     oldxor;
    HBITMAP     oldand;
    HBITMAP     oldxorrot;
    HBITMAP     oldandrot;
    short       width;
    short       height;
    IMGED_DIM   left;
    IMGED_DIM   right;
    IMGED_DIM   bottom;
    IMGED_DIM   top;

    width = (short)_wpi_getwidthrect( *rect );
    height = (short)_wpi_getheightrect( *rect );

    _wpi_getwrectvalues( *rect, &left, &top, &right, &bottom );

    pres = _wpi_getpres( HWND_DESKTOP );
    xorpres = _wpi_createcompatiblepres( pres, Instance, &xordc );
    andpres = _wpi_createcompatiblepres( pres, Instance, &anddc );
    rotxorpres = _wpi_createcompatiblepres( pres, Instance, &rotxordc );
    rotandpres = _wpi_createcompatiblepres( pres, Instance, &rotanddc );
    _wpi_releasepres( HWND_DESKTOP, pres );

    _wpi_torgbmode( xorpres );
    _wpi_torgbmode( andpres );
    _wpi_torgbmode( rotxorpres );
    _wpi_torgbmode( rotandpres );

    oldxor = _wpi_selectobject( xorpres, node->hxorbitmap );
    oldand = _wpi_selectobject( andpres, node->handbitmap );
    oldxorrot = _wpi_selectobject( rotxorpres, rotxorbmp );
    oldandrot = _wpi_selectobject( rotandpres, rotandbmp );

    _wpi_setstretchbltmode( xorpres, STRETCH_COLOR );
    _wpi_stretchblt( xorpres, left, top, width, height, rotxorpres, 0, 0,
                     height, width, SRCCOPY );

    _wpi_setstretchbltmode( andpres, STRETCH_COLOR );
    _wpi_stretchblt( andpres, left, top, width, height, rotandpres, 0, 0,
                     height, width, SRCCOPY );
    _wpi_selectobject( xorpres, oldxor );
    _wpi_selectobject( andpres, oldand );
    _wpi_selectobject( rotxorpres, oldxorrot );
    _wpi_selectobject( rotandpres, oldandrot );
    _wpi_deletecompatiblepres( xorpres, xordc );
    _wpi_deletecompatiblepres( andpres, anddc );
    _wpi_deletecompatiblepres( rotxorpres, rotxordc );
    _wpi_deletecompatiblepres( rotandpres, rotanddc );

    InvalidateRect( node->viewhwnd, NULL, TRUE );
    RecordImage( node->hwnd );
    BlowupImage( node->hwnd, NULL );

} /* stretchIntoArea */

/*
 * simpleRotate - simply rotate the image around the center of the given rectangle
 */
static void simpleRotate( img_node *node, WPI_RECT *rect, HBITMAP rotxorbmp,
                          HBITMAP rotandbmp, BOOL rectexists )
{
    WPI_POINT   topleft;
    WPI_POINT   centre_pt;
    WPI_PRES    pres;
    HDC         xordc;
    WPI_PRES    xorpres;
    HDC         anddc;
    WPI_PRES    andpres;
    HDC         rotxordc;
    WPI_PRES    rotxorpres;
    HDC         rotanddc;
    WPI_PRES    rotandpres;
    HBITMAP     oldxor;
    HBITMAP     oldand;
    HBITMAP     oldxorrot;
    HBITMAP     oldandrot;
    short       width;
    short       height;
    WPI_RECT    new_rect;
    IMGED_DIM   left;
    IMGED_DIM   top;
    IMGED_DIM   right;
    IMGED_DIM   bottom;

    width = (short)_wpi_getwidthrect( *rect );
    height = (short)_wpi_getheightrect( *rect );

    /*
     * PM NOTE:  The rectangle comes in with bottom = yTop and top = yBottom.
     * To use the same formula to calculate the center point and top left, we
     * use getwrectvalues.
     */
    _wpi_getwrectvalues( *rect, &left, &top, &right, &bottom );
    centre_pt.x = (width / 2) + left;
    centre_pt.y = (height / 2) + top;
    topleft.x = centre_pt.x - centre_pt.y + top;
    topleft.y = centre_pt.y - centre_pt.x + left;

    pres = _wpi_getpres( HWND_DESKTOP );
    xorpres = _wpi_createcompatiblepres( pres, Instance, &xordc );
    andpres = _wpi_createcompatiblepres( pres, Instance, &anddc );
    rotxorpres = _wpi_createcompatiblepres( pres, Instance, &rotxordc );
    rotandpres = _wpi_createcompatiblepres( pres, Instance, &rotanddc );
    _wpi_releasepres( HWND_DESKTOP, pres );

    _wpi_torgbmode( xorpres );
    _wpi_torgbmode( andpres );
    _wpi_torgbmode( rotxorpres );
    _wpi_torgbmode( rotandpres );

    oldxor = _wpi_selectobject( xorpres, node->hxorbitmap );
    oldand = _wpi_selectobject( andpres, node->handbitmap );
    oldxorrot = _wpi_selectobject( rotxorpres, rotxorbmp );
    oldandrot = _wpi_selectobject( rotandpres, rotandbmp );

    _wpi_getwrectvalues( *rect, &left, &top, &right, &bottom );
    _wpi_patblt( xorpres, left, top, width, height, WHITENESS );
    _wpi_patblt( andpres, left, top, width, height, BLACKNESS );

    _wpi_bitblt( xorpres, topleft.x, topleft.y, height, width, rotxorpres, 0, 0, SRCCOPY );
    _wpi_bitblt( andpres, topleft.x, topleft.y, height, width, rotandpres, 0, 0, SRCCOPY );

    _wpi_selectobject( xorpres, oldxor );
    _wpi_selectobject( andpres, oldand );
    _wpi_selectobject( rotxorpres, oldxorrot );
    _wpi_selectobject( rotandpres, oldandrot );
    _wpi_deletecompatiblepres( xorpres, xordc );
    _wpi_deletecompatiblepres( andpres, anddc );
    _wpi_deletecompatiblepres( rotxorpres, rotxordc );
    _wpi_deletecompatiblepres( rotandpres, rotanddc );

    InvalidateRect( node->viewhwnd, NULL, TRUE );
    RecordImage( node->hwnd );
    if( DoKeepRect() ) {
        if( rectexists ) {
            _wpi_setwrectvalues( &new_rect, topleft.x, topleft.y,
                                 topleft.x+height, topleft.y + width );
            SetDeviceClipRect( &new_rect );
        }
    } else {
        rectexists = rectexists;
        new_rect = new_rect;
        SetRectExists( FALSE );
    }

    BlowupImage( node->hwnd, NULL );

} /* simpleRotate */

/*
 * IEPrintRotateAmt
 */
static void IEPrintRotateAmt( int amt )
{
    IEPrintAmtText( WIE_ROTATIONPERCENT, amt );

} /* IEPrintRotateAmt */

/*
 * rotateTheImage - create the rotated bitmaps
 */
static void rotateTheImage( img_node *node, int whichway, WPI_RECT *rect,
                            HBITMAP rotxorbmp, HBITMAP rotandbmp )
{
    WPI_PRES    pres;
    WPI_PRES    rotxorpres;
    HDC         rotxordc;
    WPI_PRES    xorpres;
    HDC         xordc;
    WPI_PRES    rotandpres;
    HDC         rotanddc;
    HBITMAP     oldandrot;
    HBITMAP     oldxorrot;
    HBITMAP     oldxor;
    short       new_height;
    short       new_width;
    long        i;
    long        amt_done;
    long        prev_amt;
    long        total_amt;
    long        temp;
    IMGED_DIM   left;
    IMGED_DIM   top;
    IMGED_DIM   right;
    IMGED_DIM   bottom;
    int         x;
    int         y;
    COLORREF    color;
    bitmap_bits *xorbits;
    bitmap_bits *andbits;
    bitmap_bits *rotxorbits;
    bitmap_bits *rotandbits;

    new_height = (short)_wpi_getwidthrect( *rect );
    new_width = (short)_wpi_getheightrect( *rect );
    oldxor = NULL;

    amt_done = prev_amt = 0L;
    total_amt = (long)new_height * (long)new_width;
    i = 0L;
    _wpi_getwrectvalues( *rect, &left, &top, &right, &bottom );

    if( node->imgtype == BITMAP_IMG ) {
        pres = _wpi_getpres( HWND_DESKTOP );
        xorpres = _wpi_createcompatiblepres( pres, Instance, &xordc );
        rotxorpres = _wpi_createcompatiblepres( pres, Instance, &rotxordc );
        rotandpres = _wpi_createcompatiblepres( pres, Instance, &rotanddc );
        _wpi_releasepres( HWND_DESKTOP, pres );

        _wpi_torgbmode( rotxorpres );
        _wpi_torgbmode( rotandpres );
        oldxorrot = _wpi_selectobject( rotxorpres, rotxorbmp );
        oldandrot = _wpi_selectobject( rotandpres, rotandbmp );

        _wpi_patblt( rotxorpres, 0, 0, new_width, new_height, WHITENESS );
        _wpi_patblt( rotandpres, 0, 0, new_width, new_height, BLACKNESS );
        _wpi_selectobject( rotxorpres, oldxorrot );
        _wpi_selectobject( rotandpres, oldandrot );
        _wpi_deletecompatiblepres( rotandpres, rotanddc );

        _imged_getthebits( xorbits, xorpres, node->hxorbitmap, oldxor );
        _imged_getthebits( rotxorbits, rotxorpres, rotxorbmp, oldxorrot );
        if( whichway == ROTATE_COUNTERCLOCKWISE ) {
            for( y = 0; y < new_height; y++ ) {
                for( x = 0; x < new_width; x++ ) {
                    color = _imged_getpixel( xorbits, xorpres, right - y - 1, top + x );
                    _imged_setpixel( rotxorbits, rotxorpres, x, y, color );

                    i++;
                    temp = i * 100;
                    amt_done = temp / total_amt;
                    if( amt_done - prev_amt >= 2 ) {
                        IEPrintRotateAmt( amt_done );
                        prev_amt = amt_done;
                    }
                }
            }
        } else {
            for( y = 0; y < new_height; y++ ) {
                for( x = 0; x < new_width; x++ ) {
                    color = _imged_getpixel( xorbits, xorpres, left + y, bottom - x - 1 );
                    _imged_setpixel( rotxorbits, rotxorpres, x, y, color );
                    i++;
                    temp = i * 100;
                    amt_done = temp / total_amt;
                    if( amt_done - prev_amt >= 2 ) {
                        IEPrintRotateAmt( amt_done );
                        prev_amt = amt_done;
                    }
                }
            }
        }
        _imged_freethebits( xorbits, xorpres, node->hxorbitmap, FALSE, oldxor );
        _imged_freethebits( rotxorbits, rotxorpres, rotxorbmp, TRUE, oldxorrot );
        _wpi_deletecompatiblepres( xorpres, xordc );
        _wpi_deletecompatiblepres( rotxorpres, rotxordc );

    } else {
        // We can use the bits.c routines for icons and cursors.
        xorbits = GetTheBits( node->hxorbitmap );
        andbits = GetTheBits( node->handbitmap );
        rotxorbits = GetTheBits( rotxorbmp );
        rotandbits = GetTheBits( rotandbmp );

        if( whichway != IMGED_ROTATECC ) {
            for( y = 0; y < new_height; y++ ) {
                for( x = 0; x < new_width; x++ ) {
                    color = MyGetPixel( xorbits, right - y - 1, top + x );
                    MySetPixel( rotxorbits, x, y, color );

                    color = MyGetPixel( andbits, right - y - 1, top + x );
                    MySetPixel( rotandbits, x, y, color );
                    i++;
                    temp = i * 100;
                    amt_done = temp / total_amt;
                    if( amt_done - prev_amt >= 2 ) {
                        IEPrintRotateAmt( amt_done );
                        prev_amt = amt_done;
                    }
                }
            }
        } else {
            for( y = 0; y < new_height; y++ ) {
                for( x = 0; x < new_width; x++ ) {
                    color = MyGetPixel( xorbits, left + y, bottom - x - 1 );
                    MySetPixel( rotxorbits, x, y, color );

                    color = MyGetPixel( andbits, left + y, bottom - x - 1 );
                    MySetPixel( rotandbits, x, y, color );
                    i++;
                    temp = i * 100;
                    amt_done = temp / total_amt;
                    if( amt_done - prev_amt >= 2 ) {
                        IEPrintRotateAmt( amt_done );
                        prev_amt = amt_done;
                    }
                }
            }
        }
        FreeTheBits( xorbits, node->hxorbitmap, FALSE );
        FreeTheBits( andbits, node->handbitmap, FALSE );
        FreeTheBits( rotxorbits, rotxorbmp, TRUE );
        FreeTheBits( rotandbits, rotandbmp, TRUE );
    }

} /* rotateTheImage */

/*
 * RotateImage - rotate the image either clockwise or counterclockwise
 */
void RotateImage( WORD whichway )
{
    img_node    *node;
    HBITMAP     rotxorbmp;
    HBITMAP     rotandbmp;
    HCURSOR     prevcursor;
    int         rotate_type;
    WPI_RECT    rotate_rect;
    BOOL        rectexists;
    short       new_width;
    short       new_height;
    WPI_PRES    pres;

    node = GetCurrentNode();
    if( node == NULL ) {
        return;
    }

    prevcursor = _wpi_setcursor( _wpi_getsyscursor( IDC_WAIT ) );

    PrintHintTextByID( WIE_ROTATINGIMAGE, NULL );

    if( DoesRectExist( &rotate_rect ) ) {
        rectexists = TRUE;
        new_width = (short)_wpi_getheightrect( rotate_rect );
        new_height = (short)_wpi_getwidthrect( rotate_rect );
    } else {
        rectexists = FALSE;
        _wpi_setwrectvalues( &rotate_rect, 0, 0,
                             (IMGED_DIM)node->width, (IMGED_DIM)node->height );
        new_width = node->height;
        new_height = node->width;
    }

    if( node->bitcount == 1 ) {
        rotxorbmp = _wpi_createbitmap( new_width, new_height, 1, 1, NULL );
    } else {
#if 1
        pres = _wpi_getpres( HWND_DESKTOP );
        rotxorbmp = _wpi_createcompatiblebitmap( pres, new_width, new_height );
        _wpi_releasepres( HWND_DESKTOP, pres );
#else
        rotxorbmp = _wpi_createbitmap( new_width, new_height, ColorPlanes,
                                       BitsPerPixel, NULL );
#endif
    }
    rotandbmp = _wpi_createbitmap( new_width, new_height, 1, 1, NULL );

    rotateTheImage( node, whichway, &rotate_rect, rotxorbmp, rotandbmp );

    rotate_type = GetRotateType();
    if( rotate_type == SIMPLE_ROTATE ) {
        simpleRotate( node, &rotate_rect, rotxorbmp, rotandbmp, rectexists );
    } else if( rotate_type == CLIP_ROTATE ) {
        clipIntoArea( node, &rotate_rect, rotxorbmp, rotandbmp );
    } else {
        stretchIntoArea( node, &rotate_rect, rotxorbmp, rotandbmp );
    }

    _wpi_deleteobject( rotxorbmp );
    _wpi_deleteobject( rotandbmp );

    if( whichway == IMGED_ROTATECC ) {
        PrintHintTextByID( WIE_IMAGEROTATEDCCW, NULL );
    } else {
        PrintHintTextByID( WIE_IMAGEROTATEDCW, NULL );
    }
    _wpi_setcursor( prevcursor );

} /* RotateImage */

/*
 * ClearImage - clear the XOR and the AND bitmaps
 */
void ClearImage( void )
{
    WPI_PRES            pres;
    WPI_PRES            xorpres;
    WPI_PRES            andpres;
    HDC                 xormemdc;
    HDC                 andmemdc;
    HBITMAP             oldxor;
    HBITMAP             oldand;
    img_node            *node;
    WPI_RECT            clear_area;
    IMGED_DIM           left;
    IMGED_DIM           top;
    IMGED_DIM           right;
    IMGED_DIM           bottom;
    int                 width;
    int                 height;

    node = GetCurrentNode();
    if( node == NULL ) {
        return;
    }

    if( DoesRectExist( &clear_area ) ) {
        SetRectExists( FALSE );
    } else {
        _wpi_setwrectvalues( &clear_area, 0, 0, node->width, node->height );
    }

    pres = _wpi_getpres( HWND_DESKTOP );
    xorpres = _wpi_createcompatiblepres( pres, Instance, &xormemdc );
    andpres = _wpi_createcompatiblepres( pres, Instance, &andmemdc );
    _wpi_releasepres( HWND_DESKTOP, pres );

    oldxor = _wpi_selectobject( xorpres, node->hxorbitmap );
    oldand = _wpi_selectobject( andpres, node->handbitmap );

    _wpi_getwrectvalues( clear_area, &left, &top, &right, &bottom );
    width = _wpi_getwidthrect( clear_area );
    height = _wpi_getheightrect( clear_area );

    _wpi_patblt( xorpres, left, top, width, height, WHITENESS );
    _wpi_patblt( andpres, left, top, width, height, BLACKNESS );

    _wpi_selectobject( xorpres, oldxor );
    _wpi_selectobject( andpres, oldand );
    _wpi_deletecompatiblepres( xorpres, xormemdc );
    _wpi_deletecompatiblepres( andpres, andmemdc );

    InvalidateRect( node->viewhwnd, NULL, TRUE );
    RecordImage( node->hwnd );
    BlowupImage( node->hwnd, NULL );
    PrintHintTextByID( WIE_AREACLEARED, NULL );

} /* ClearImage */

/*
 * ShiftImage - shift the image in the given direction
 */
void ShiftImage( WORD shiftdirection )
{
    HBITMAP     dup_and;
    HBITMAP     dup_xor;
    HBITMAP     oldbitmap;
    HBITMAP     oldsrcbitmap;
    WPI_PRES    pres;
    HDC         memdc;
    WPI_PRES    mempres;
    HDC         srcdc;
    WPI_PRES    srcpres;
    short       x_src;
    short       y_src;
    short       x_dest;
    short       y_dest;
    short       width;
    short       height;
    short       min_width;
    short       min_height;
    short       rgn_width;
    short       rgn_height;
    img_node    *node;
    WPI_RECT    rect;
    IMGED_DIM   left;
    IMGED_DIM   right;
    IMGED_DIM   top;
    IMGED_DIM   bottom;
    DWORD       message;

    node = GetCurrentNode();
    if( node == NULL ) {
        return;
    }

    dup_and = DuplicateBitmap( node->handbitmap );
    dup_xor = DuplicateBitmap( node->hxorbitmap );

    pres = _wpi_getpres( HWND_DESKTOP );
    mempres = _wpi_createcompatiblepres( pres, Instance, &memdc );
    srcpres = _wpi_createcompatiblepres( pres, Instance, &srcdc );
    _wpi_releasepres( HWND_DESKTOP, pres );

    _wpi_torgbmode( mempres );
    _wpi_torgbmode( srcpres );

    if( DoesRectExist( &rect ) ) {
        width = (short)_wpi_getwidthrect( rect );
        height = (short)_wpi_getheightrect( rect );
    } else {
        _wpi_setwrectvalues( &rect, 0, 0, node->width, node->height );
        width = node->width;
        height = node->height;
    }

    _wpi_getwrectvalues( rect, &left, &top, &right, &bottom );

    oldbitmap = _wpi_selectobject( mempres, node->handbitmap );
    _wpi_patblt( mempres, left, top, width, height, BLACKNESS );
    _wpi_selectobject( mempres, oldbitmap );
    oldbitmap = _wpi_selectobject( mempres, node->hxorbitmap );
    _wpi_patblt( mempres, left, top, width, height, WHITENESS );

    oldsrcbitmap = _wpi_selectobject( srcpres, dup_xor );

    x_src = (short)left;
    y_src = (short)top;
    x_dest =(short)left;
    y_dest = (short)top;
    min_width = (short)min( ImgedConfigInfo.shift, width );
    min_height = (short)min( ImgedConfigInfo.shift, height );
    rgn_width = width;
    rgn_height = height;

    switch( shiftdirection ) {
    case IMGED_LEFT:
        width -= min_width;
        x_src = x_src + min_width;
        message = WIE_IMAGESHIFTEDLEFT;
        break;

    case IMGED_RIGHT:
        width -= min_width;
        x_dest = x_dest + min_width;
        message = WIE_IMAGESHIFTEDRIGHT;
        break;

    case IMGED_UP:
#ifndef __OS2_PM__
        height -= min_height;
        y_src = y_src + min_height;
#else
        height += min_height;
        y_src = y_src - min_height;
#endif
        message = WIE_IMAGESHIFTEDUP;
        break;

    case IMGED_DOWN:
#ifndef __OS2_PM__
        height -= min_height;
        y_dest = y_dest + min_height;
#else
        height += min_height;
        y_dest = y_dest - min_height;
#endif
        message = WIE_IMAGESHIFTEDDOWN;
        break;

    default:
        break;
    }

    _wpi_bitblt( mempres, x_dest, y_dest, width, height, srcpres, x_src, y_src, SRCCOPY );
    _wpi_selectobject( srcpres, oldsrcbitmap );
    oldsrcbitmap = _wpi_selectobject( srcpres, dup_and );
    _wpi_selectobject( mempres, oldbitmap );
    oldbitmap = _wpi_selectobject( mempres, node->handbitmap );
    _wpi_bitblt( mempres, x_dest, y_dest, width, height, srcpres, x_src, y_src, SRCCOPY );

    if( IsShiftWrap() ) {
        switch( shiftdirection ) {
        case IMGED_LEFT:
            width = min_width;
            x_src = (short)left;
            x_dest = (short)(right - width);
            break;

        case IMGED_RIGHT:
            width = min_width;
            x_dest = (short)left;
            x_src = (short)(right - width);
            break;

        case SHIFT_UP:
            height = min_height;
            y_src = (short)top;
            y_dest = (short)(bottom - height);
            break;

        case SHIFT_DOWN:
            height = min_height;
            y_dest = (short)top;
            y_src = (short)(bottom - height);
            break;

        default:
            break;
        }

        _wpi_bitblt( mempres, x_dest, y_dest, width, height, srcpres,
                     x_src, y_src, SRCCOPY );
        _wpi_selectobject( srcpres, oldsrcbitmap );
        _wpi_selectobject( mempres, oldbitmap );
        oldsrcbitmap = _wpi_selectobject( srcpres, dup_xor );
        oldbitmap = _wpi_selectobject( mempres, node->hxorbitmap );
        _wpi_bitblt( mempres, x_dest, y_dest, width, height, srcpres,
                     x_src, y_src, SRCCOPY );
    }
    _wpi_selectobject( srcpres, oldsrcbitmap );
    _wpi_selectobject( mempres, oldbitmap );
    _wpi_deletecompatiblepres( srcpres, srcdc );
    _wpi_deletecompatiblepres( mempres, memdc );

    _wpi_deleteobject( dup_xor );
    _wpi_deleteobject( dup_and );

    RecordImage( node->hwnd );
    BlowupImage( node->hwnd, NULL );
    InvalidateRect( node->viewhwnd, NULL, FALSE );

    IEPrintAmtText( message, ImgedConfigInfo.shift );

} /* ShiftImage */
