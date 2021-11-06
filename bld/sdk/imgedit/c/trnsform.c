/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
#include "settings.rh"

/*
 * FlipImage - flip the image along either the x-axis or the y-axis
 */
void FlipImage( WORD whichway )
{
    img_node    *node;
    short       width;
    short       height;
    short       dstwidth;
    short       dstheight;
    short       new_left;
    short       new_top;
    WPI_RECT    dims;
    WPI_HBITMAP xorflip_hbitmap;
    WPI_HBITMAP oldflip_hbitmap;
    WPI_HBITMAP old_hbitmap;
    WPI_HBITMAP andflip_hbitmap;
    WPI_PRES    pres;
    WPI_PRES    mempres;
    HDC         memdc;
    WPI_PRES    flippres;
    HDC         flipdc;
    HCURSOR     prevcursor;
    WPI_RECTDIM bottom;
    WPI_RECTDIM left;
    WPI_RECTDIM right;
    WPI_RECTDIM top;

    node = GetCurrentNode();
    if( node == NULL ) {
        return;
    }

    PrintHintTextByID( WIE_FLIPPINGIMAGE, NULL );
    prevcursor = _wpi_setcursor( _wpi_getsyscursor( IDC_WAIT ) );

    if( !DoesRectExist( &dims ) ) {
        _wpi_setwrectvalues( &dims, 0, 0, node->width, node->height );
    }
    width = (short)_wpi_getwidthrect( dims );
    height = (short)_wpi_getheightrect( dims );

    pres = _wpi_getpres( HWND_DESKTOP );
    if( node->bitcount == 1 ) {
        xorflip_hbitmap = _wpi_createbitmap( width, height, 1, 1, NULL );
    } else {
#if 1
        xorflip_hbitmap = _wpi_createcompatiblebitmap( pres, width, height );
#else
        xorflip_hbitmap = _wpi_createbitmap( width, height, ColorPlanes, BitsPerPixel, NULL );
#endif
    }
    andflip_hbitmap = _wpi_createbitmap( width, height, 1, 1, NULL );

    mempres = _wpi_createcompatiblepres( pres, Instance, &memdc );
    flippres = _wpi_createcompatiblepres( pres, Instance, &flipdc );
    _wpi_releasepres( HWND_DESKTOP, pres );

    _wpi_getrectvalues( dims, &left, &top, &right, &bottom );
    _wpi_preparemono( flippres, BLACK, WHITE );

    old_hbitmap = _wpi_selectbitmap( mempres, node->xor_hbitmap );
    oldflip_hbitmap = _wpi_selectbitmap( flippres, xorflip_hbitmap );
    _wpi_bitblt( flippres, 0, 0, width, height, mempres, left, top, SRCCOPY );
    _wpi_getoldbitmap( mempres, old_hbitmap );
    _wpi_getoldbitmap( flippres, oldflip_hbitmap );

    old_hbitmap = _wpi_selectbitmap( mempres, node->and_hbitmap );
    oldflip_hbitmap = _wpi_selectbitmap( flippres, andflip_hbitmap );
    _wpi_bitblt( flippres, 0, 0, width, height, mempres, left, top, SRCCOPY );

    if( whichway == IMGED_FLIPVERT ) {
        dstwidth = -1 * width;
        new_left = right - 1;
        new_top = top;
        dstheight = height;
    } else {
        dstwidth = width;
        new_left = left;
        new_top = bottom - 1;
        dstheight = -1 * height;
    }

    _wpi_stretchblt( mempres, new_left, new_top, dstwidth, dstheight,
                     flippres, 0, 0, width, height, SRCCOPY );
    _wpi_getoldbitmap( mempres, old_hbitmap );
    old_hbitmap = _wpi_selectbitmap( mempres, node->xor_hbitmap );
    _wpi_getoldbitmap( flippres, oldflip_hbitmap );
    oldflip_hbitmap = _wpi_selectbitmap( flippres, xorflip_hbitmap );
    _wpi_stretchblt( mempres, new_left, new_top, dstwidth, dstheight,
                     flippres, 0, 0, width, height, SRCCOPY );
    _wpi_getoldbitmap( mempres, old_hbitmap );
    _wpi_getoldbitmap( flippres, oldflip_hbitmap );

    _wpi_deletebitmap( xorflip_hbitmap );
    _wpi_deletebitmap( andflip_hbitmap );
    _wpi_deletecompatiblepres( mempres, memdc );
    _wpi_deletecompatiblepres( flippres, flipdc );

    InvalidateRect( node->viewhwnd, NULL, TRUE );

    if( !DoKeepRect() ) {
        SetRectExists( false );
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
                          WPI_HBITMAP rotxor_hbitmap, WPI_HBITMAP rotand_hbitmap )
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
    WPI_HBITMAP oldxor_hbitmap;
    WPI_HBITMAP oldand_hbitmap;
    WPI_HBITMAP oldxorrot_hbitmap;
    WPI_HBITMAP oldandrot_hbitmap;
    short       width;
    short       height;
    WPI_POINT   centre_pt;
    WPI_POINT   topleft;
    short       start_x;
    short       start_y;
    short       new_width;
    short       new_height;
    WPI_RECTDIM left;
    WPI_RECTDIM right;
    WPI_RECTDIM top;
    WPI_RECTDIM bottom;

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

    topleft.x = centre_pt.x - centre_pt.y + top;
    if( topleft.x < left )
        topleft.x = left;
    topleft.y = centre_pt.y - centre_pt.x + left;
    if( topleft.y < top )
        topleft.y = top;

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

    oldxor_hbitmap = _wpi_selectbitmap( xorpres, node->xor_hbitmap );
    oldand_hbitmap = _wpi_selectbitmap( andpres, node->and_hbitmap );
    oldxorrot_hbitmap = _wpi_selectbitmap( rotxorpres, rotxor_hbitmap );
    oldandrot_hbitmap = _wpi_selectbitmap( rotandpres, rotand_hbitmap );

    _wpi_patblt( xorpres, left, top, width, height, WHITENESS );
    _wpi_patblt( andpres, left, top, width, height, BLACKNESS );

    _wpi_bitblt( xorpres, topleft.x, topleft.y, new_width, new_height,
                 rotxorpres, start_x, start_y, SRCCOPY );
    _wpi_bitblt( andpres, topleft.x, topleft.y, new_width, new_height,
                 rotandpres, start_x, start_y, SRCCOPY );

    _wpi_getoldbitmap( xorpres, oldxor_hbitmap );
    _wpi_getoldbitmap( andpres, oldand_hbitmap );
    _wpi_getoldbitmap( rotxorpres, oldxorrot_hbitmap );
    _wpi_getoldbitmap( rotandpres, oldandrot_hbitmap );
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
                             WPI_HBITMAP rotxor_hbitmap, WPI_HBITMAP rotand_hbitmap )
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
    WPI_HBITMAP oldxor_hbitmap;
    WPI_HBITMAP oldand_hbitmap;
    WPI_HBITMAP oldxorrot_hbitmap;
    WPI_HBITMAP oldandrot_hbitmap;
    short       width;
    short       height;
    WPI_RECTDIM left;
    WPI_RECTDIM right;
    WPI_RECTDIM bottom;
    WPI_RECTDIM top;

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

    oldxor_hbitmap = _wpi_selectbitmap( xorpres, node->xor_hbitmap );
    oldand_hbitmap = _wpi_selectbitmap( andpres, node->and_hbitmap );
    oldxorrot_hbitmap = _wpi_selectbitmap( rotxorpres, rotxor_hbitmap );
    oldandrot_hbitmap = _wpi_selectbitmap( rotandpres, rotand_hbitmap );

    _wpi_setstretchbltmode( xorpres, STRETCH_COLOR );
    _wpi_stretchblt( xorpres, left, top, width, height, rotxorpres, 0, 0,
                     height, width, SRCCOPY );

    _wpi_setstretchbltmode( andpres, STRETCH_COLOR );
    _wpi_stretchblt( andpres, left, top, width, height, rotandpres, 0, 0,
                     height, width, SRCCOPY );
    _wpi_getoldbitmap( xorpres, oldxor_hbitmap );
    _wpi_getoldbitmap( andpres, oldand_hbitmap );
    _wpi_getoldbitmap( rotxorpres, oldxorrot_hbitmap );
    _wpi_getoldbitmap( rotandpres, oldandrot_hbitmap );
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
static void simpleRotate( img_node *node, WPI_RECT *rect, WPI_HBITMAP rotxor_hbitmap,
                          WPI_HBITMAP rotand_hbitmap, bool rectexists )
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
    WPI_HBITMAP oldxor_hbitmap;
    WPI_HBITMAP oldand_hbitmap;
    WPI_HBITMAP oldxorrot_hbitmap;
    WPI_HBITMAP oldandrot_hbitmap;
    short       width;
    short       height;
    WPI_RECT    new_rect = { 0 };
    WPI_RECTDIM left;
    WPI_RECTDIM top;
    WPI_RECTDIM right;
    WPI_RECTDIM bottom;

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

    oldxor_hbitmap = _wpi_selectbitmap( xorpres, node->xor_hbitmap );
    oldand_hbitmap = _wpi_selectbitmap( andpres, node->and_hbitmap );
    oldxorrot_hbitmap = _wpi_selectbitmap( rotxorpres, rotxor_hbitmap );
    oldandrot_hbitmap = _wpi_selectbitmap( rotandpres, rotand_hbitmap );

    _wpi_getwrectvalues( *rect, &left, &top, &right, &bottom );
    _wpi_patblt( xorpres, left, top, width, height, WHITENESS );
    _wpi_patblt( andpres, left, top, width, height, BLACKNESS );

    _wpi_bitblt( xorpres, topleft.x, topleft.y, height, width, rotxorpres, 0, 0, SRCCOPY );
    _wpi_bitblt( andpres, topleft.x, topleft.y, height, width, rotandpres, 0, 0, SRCCOPY );

    _wpi_getoldbitmap( xorpres, oldxor_hbitmap );
    _wpi_getoldbitmap( andpres, oldand_hbitmap );
    _wpi_getoldbitmap( rotxorpres, oldxorrot_hbitmap );
    _wpi_getoldbitmap( rotandpres, oldandrot_hbitmap );
    _wpi_deletecompatiblepres( xorpres, xordc );
    _wpi_deletecompatiblepres( andpres, anddc );
    _wpi_deletecompatiblepres( rotxorpres, rotxordc );
    _wpi_deletecompatiblepres( rotandpres, rotanddc );

    InvalidateRect( node->viewhwnd, NULL, TRUE );
    RecordImage( node->hwnd );
    if( DoKeepRect() ) {
        if( rectexists ) {
            _wpi_setwrectvalues( &new_rect, topleft.x, topleft.y, topleft.x + height, topleft.y + width );
            SetDeviceClipRect( &new_rect );
        }
    } else {
        rectexists = rectexists;
        SetRectExists( false );
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
                            WPI_HBITMAP rotxor_hbitmap, WPI_HBITMAP rotand_hbitmap )
{
    WPI_PRES    pres;
    WPI_PRES    rotxorpres;
    HDC         rotxordc;
    WPI_PRES    xorpres;
    HDC         xordc;
    WPI_PRES    rotandpres;
    HDC         rotanddc;
    WPI_HBITMAP oldandrot_hbitmap;
    WPI_HBITMAP oldxorrot_hbitmap;
    WPI_HBITMAP oldxor_hbitmap;
    short       new_height;
    short       new_width;
    long        i;
    long        amt_done;
    long        prev_amt;
    long        total_amt;
    long        temp;
    WPI_RECTDIM left;
    WPI_RECTDIM top;
    WPI_RECTDIM right;
    WPI_RECTDIM bottom;
    int         x;
    int         y;
    COLORREF    color;
    bitmap_bits *xorbits;
    bitmap_bits *andbits;
    bitmap_bits *rotxorbits;
    bitmap_bits *rotandbits;

    new_height = (short)_wpi_getwidthrect( *rect );
    new_width = (short)_wpi_getheightrect( *rect );
    oldxor_hbitmap = NULL;

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
        oldxorrot_hbitmap = _wpi_selectbitmap( rotxorpres, rotxor_hbitmap );
        oldandrot_hbitmap = _wpi_selectbitmap( rotandpres, rotand_hbitmap );

        _wpi_patblt( rotxorpres, 0, 0, new_width, new_height, WHITENESS );
        _wpi_patblt( rotandpres, 0, 0, new_width, new_height, BLACKNESS );
        _wpi_getoldbitmap( rotxorpres, oldxorrot_hbitmap );
        _wpi_getoldbitmap( rotandpres, oldandrot_hbitmap );
        _wpi_deletecompatiblepres( rotandpres, rotanddc );

        _imged_getthebits( xorbits, xorpres, node->xor_hbitmap, oldxor_hbitmap );
        _imged_getthebits( rotxorbits, rotxorpres, rotxor_hbitmap, oldxorrot_hbitmap );
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
        _imged_freethebits( xorbits, xorpres, node->xor_hbitmap, false, oldxor_hbitmap );
        _imged_freethebits( rotxorbits, rotxorpres, rotxor_hbitmap, true, oldxorrot_hbitmap );
        _wpi_deletecompatiblepres( xorpres, xordc );
        _wpi_deletecompatiblepres( rotxorpres, rotxordc );

    } else {
        // We can use the bits.c routines for icons and cursors.
        xorbits = GetTheBits( node->xor_hbitmap );
        andbits = GetTheBits( node->and_hbitmap );
        rotxorbits = GetTheBits( rotxor_hbitmap );
        rotandbits = GetTheBits( rotand_hbitmap );

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
        FreeTheBits( xorbits, node->xor_hbitmap, false );
        FreeTheBits( andbits, node->and_hbitmap, false );
        FreeTheBits( rotxorbits, rotxor_hbitmap, true );
        FreeTheBits( rotandbits, rotand_hbitmap, true );
    }

} /* rotateTheImage */

/*
 * RotateImage - rotate the image either clockwise or counterclockwise
 */
void RotateImage( WORD whichway )
{
    img_node    *node;
    WPI_HBITMAP rotxor_hbitmap;
    WPI_HBITMAP rotand_hbitmap;
    HCURSOR     prevcursor;
    int         rotate_type;
    WPI_RECT    rotate_rect;
    bool        rectexists;
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
        rectexists = true;
        new_width = (short)_wpi_getheightrect( rotate_rect );
        new_height = (short)_wpi_getwidthrect( rotate_rect );
    } else {
        rectexists = false;
        _wpi_setwrectvalues( &rotate_rect, 0, 0, node->width, node->height );
        new_width = node->height;
        new_height = node->width;
    }

    if( node->bitcount == 1 ) {
        rotxor_hbitmap = _wpi_createbitmap( new_width, new_height, 1, 1, NULL );
    } else {
#if 1
        pres = _wpi_getpres( HWND_DESKTOP );
        rotxor_hbitmap = _wpi_createcompatiblebitmap( pres, new_width, new_height );
        _wpi_releasepres( HWND_DESKTOP, pres );
#else
        rotxor_hbitmap = _wpi_createbitmap( new_width, new_height, ColorPlanes,
                                       BitsPerPixel, NULL );
#endif
    }
    rotand_hbitmap = _wpi_createbitmap( new_width, new_height, 1, 1, NULL );

    rotateTheImage( node, whichway, &rotate_rect, rotxor_hbitmap, rotand_hbitmap );

    rotate_type = GetRotateType();
    if( rotate_type == SIMPLE_ROTATE ) {
        simpleRotate( node, &rotate_rect, rotxor_hbitmap, rotand_hbitmap, rectexists );
    } else if( rotate_type == CLIP_ROTATE ) {
        clipIntoArea( node, &rotate_rect, rotxor_hbitmap, rotand_hbitmap );
    } else {
        stretchIntoArea( node, &rotate_rect, rotxor_hbitmap, rotand_hbitmap );
    }

    _wpi_deletebitmap( rotxor_hbitmap );
    _wpi_deletebitmap( rotand_hbitmap );

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
    WPI_HBITMAP         oldxor_hbitmap;
    WPI_HBITMAP         oldand_hbitmap;
    img_node            *node;
    WPI_RECT            clear_area;
    WPI_RECTDIM         left;
    WPI_RECTDIM         top;
    WPI_RECTDIM         right;
    WPI_RECTDIM         bottom;
    int                 width;
    int                 height;

    node = GetCurrentNode();
    if( node == NULL ) {
        return;
    }

    if( DoesRectExist( &clear_area ) ) {
        SetRectExists( false );
    } else {
        _wpi_setwrectvalues( &clear_area, 0, 0, node->width, node->height );
    }

    pres = _wpi_getpres( HWND_DESKTOP );
    xorpres = _wpi_createcompatiblepres( pres, Instance, &xormemdc );
    andpres = _wpi_createcompatiblepres( pres, Instance, &andmemdc );
    _wpi_releasepres( HWND_DESKTOP, pres );

    oldxor_hbitmap = _wpi_selectbitmap( xorpres, node->xor_hbitmap );
    oldand_hbitmap = _wpi_selectbitmap( andpres, node->and_hbitmap );

    _wpi_getwrectvalues( clear_area, &left, &top, &right, &bottom );
    width = _wpi_getwidthrect( clear_area );
    height = _wpi_getheightrect( clear_area );

    _wpi_patblt( xorpres, left, top, width, height, WHITENESS );
    _wpi_patblt( andpres, left, top, width, height, BLACKNESS );

    _wpi_getoldbitmap( xorpres, oldxor_hbitmap );
    _wpi_getoldbitmap( andpres, oldand_hbitmap );
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
    WPI_HBITMAP dupand_hbitmap;
    WPI_HBITMAP dupxor_hbitmap;
    WPI_HBITMAP old_hbitmap;
    WPI_HBITMAP oldsrc_hbitmap;
    WPI_PRES    pres;
    HDC         memdc;
    WPI_PRES    mempres;
    HDC         srcdc;
    WPI_PRES    srcpres;
    short       x_src;
    short       y_src;
    short       x_dst;
    short       y_dst;
    short       width;
    short       height;
    short       min_width;
    short       min_height;
    short       rgn_width;
    short       rgn_height;
    img_node    *node;
    WPI_RECT    rect;
    WPI_RECTDIM left;
    WPI_RECTDIM right;
    WPI_RECTDIM top;
    WPI_RECTDIM bottom;
    msg_id      message;

    node = GetCurrentNode();
    if( node == NULL ) {
        return;
    }

    dupand_hbitmap = DuplicateBitmap( node->and_hbitmap );
    dupxor_hbitmap = DuplicateBitmap( node->xor_hbitmap );

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

    old_hbitmap = _wpi_selectbitmap( mempres, node->and_hbitmap );
    _wpi_patblt( mempres, left, top, width, height, BLACKNESS );
    _wpi_getoldbitmap( mempres, old_hbitmap );
    old_hbitmap = _wpi_selectbitmap( mempres, node->xor_hbitmap );
    _wpi_patblt( mempres, left, top, width, height, WHITENESS );

    oldsrc_hbitmap = _wpi_selectbitmap( srcpres, dupxor_hbitmap );

    x_src = (short)left;
    y_src = (short)top;
    x_dst = (short)left;
    y_dst = (short)top;
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
        x_dst = x_dst + min_width;
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
        y_dst = y_dst + min_height;
#else
        height += min_height;
        y_dst = y_dst - min_height;
#endif
        message = WIE_IMAGESHIFTEDDOWN;
        break;

    default:
        break;
    }

    _wpi_bitblt( mempres, x_dst, y_dst, width, height, srcpres, x_src, y_src, SRCCOPY );
    _wpi_getoldbitmap( srcpres, oldsrc_hbitmap );
    oldsrc_hbitmap = _wpi_selectbitmap( srcpres, dupand_hbitmap );
    _wpi_getoldbitmap( mempres, old_hbitmap );
    old_hbitmap = _wpi_selectbitmap( mempres, node->and_hbitmap );
    _wpi_bitblt( mempres, x_dst, y_dst, width, height, srcpres, x_src, y_src, SRCCOPY );

    if( IsShiftWrap() ) {
        switch( shiftdirection ) {
        case IMGED_LEFT:
            width = min_width;
            x_src = (short)left;
            x_dst = (short)(right - width);
            break;

        case IMGED_RIGHT:
            width = min_width;
            x_dst = (short)left;
            x_src = (short)(right - width);
            break;

        case SHIFT_UP:
            height = min_height;
            y_src = (short)top;
            y_dst = (short)(bottom - height);
            break;

        case SHIFT_DOWN:
            height = min_height;
            y_dst = (short)top;
            y_src = (short)(bottom - height);
            break;

        default:
            break;
        }

        _wpi_bitblt( mempres, x_dst, y_dst, width, height, srcpres,
                     x_src, y_src, SRCCOPY );
        _wpi_getoldbitmap( srcpres, oldsrc_hbitmap );
        _wpi_getoldbitmap( mempres, old_hbitmap );
        oldsrc_hbitmap = _wpi_selectbitmap( srcpres, dupxor_hbitmap );
        old_hbitmap = _wpi_selectbitmap( mempres, node->xor_hbitmap );
        _wpi_bitblt( mempres, x_dst, y_dst, width, height, srcpres,
                     x_src, y_src, SRCCOPY );
    }
    _wpi_getoldbitmap( srcpres, oldsrc_hbitmap );
    _wpi_getoldbitmap( mempres, old_hbitmap );
    _wpi_deletecompatiblepres( srcpres, srcdc );
    _wpi_deletecompatiblepres( mempres, memdc );

    _wpi_deletebitmap( dupxor_hbitmap );
    _wpi_deletebitmap( dupand_hbitmap );

    RecordImage( node->hwnd );
    BlowupImage( node->hwnd, NULL );
    InvalidateRect( node->viewhwnd, NULL, FALSE );

    IEPrintAmtText( message, ImgedConfigInfo.shift );

} /* ShiftImage */
