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


#include <string.h>
#include <stdio.h>
#include "imgedit.h"
#include "ieclrpal.h"

static selected_colour  lButton;
static selected_colour  rButton;
static HWND             hCurrentWnd;
static int              currentHeight;
static int              numberOfColours;
static BOOL             firstTime = TRUE;

/*
 * paintCurrent - paints the current window (processes WM_PAINT)
 */
static void paintCurrent( HWND hwnd )
{
    WPI_PRES            pres;
    WPI_PRES            mempres;
    HDC                 hdc;
    PAINTSTRUCT         ps;
    WPI_RECT            rect;
    HBITMAP             oldbitmap;
    int                 top;
    int                 bottom;

    pres = _wpi_beginpaint( hwnd, NULL, &ps );
#ifdef __OS2_PM__
    WinFillRect( pres, &ps, CLR_PALEGRAY );
#endif

    mempres = _wpi_createcompatiblepres( pres, Instance, &hdc );
    _wpi_torgbmode( pres );
    _wpi_torgbmode( mempres );
    oldbitmap = _wpi_selectobject( mempres, lButton.bitmap );

    _wpi_bitblt( pres, 0, 0, CUR_SQR_SIZE+1, 2*CUR_SQR_SIZE+1, mempres,
                                                            0, 0, SRCCOPY );
    _wpi_selectobject( mempres, oldbitmap );
    oldbitmap = _wpi_selectobject( mempres, rButton.bitmap );

    _wpi_bitblt( pres, CUR_RCOL_X-1, 0, CUR_SQR_SIZE+1, 2*CUR_SQR_SIZE+1,
                                                mempres, 0, 0, SRCCOPY );

    _wpi_selectobject( mempres, oldbitmap );
    _wpi_deletecompatiblepres( mempres, hdc );

    _wpi_setbackmode( pres, TRANSPARENT );
    _wpi_settextcolor( pres, GetInverseColour(lButton.solid) );

    top = _wpi_cvth_y( CUR_COL_Y, currentHeight );
    bottom = _wpi_cvth_y( CUR_COL_Y + CUR_SQR_SIZE, currentHeight );
    _wpi_setintwrectvalues( &rect, CUR_LCOL_X, top, CUR_LCOL_X + CUR_SQR_SIZE,
                                                                   bottom );
    _wpi_drawtext( pres, "L", 1, &rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
    _wpi_settextcolor( pres, GetInverseColour(rButton.solid) );
    _wpi_setintwrectvalues( &rect, CUR_RCOL_X, top, CUR_RCOL_X+CUR_SQR_SIZE,
                                                                    bottom );
    _wpi_drawtext( pres, "R", 1, &rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
    _wpi_endpaint( hwnd, pres, &ps );
} /* paintCurrent */

/*
 * CurrentWndProc - handle messages for the current colour selection window.
 */
MRESULT CALLBACK CurrentWndProc( HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam, WPI_PARAM2 lparam )
{
    switch ( msg ) {
    case WM_CREATE:
        SetCurrentNumColours( 16 );
        break;

    case WM_PAINT:
        paintCurrent( hwnd );
        break;

    case WM_DESTROY:
        if (lButton.bitmap) {
            _wpi_deletebitmap( lButton.bitmap );
        }
        if (rButton.bitmap) {
            _wpi_deletebitmap( rButton.bitmap );
        }
        break;

    default:
        return( DefWindowProc( hwnd, msg, wparam, lparam ) );
    }
    return 0;
} /* CurrentWndProc */

/*
 * CreateCurrentWnd - create the window with the current selection in it.
 */
void CreateCurrentWnd( HWND hparent )
{
    WPI_RECT    rect;

    lButton.colour = BLACK;
    lButton.solid = BLACK;
    lButton.type = NORMAL_CLR;
    rButton.colour = WHITE;
    rButton.solid = WHITE;
    rButton.type = NORMAL_CLR;

#ifdef __OS2_PM__
    hCurrentWnd = PM_CreateCurrentDisp(hparent);
#else
    hCurrentWnd = Win_CreateCurrentDisp(hparent);
#endif

    _wpi_getclientrect( hCurrentWnd, &rect );
    currentHeight = _wpi_getheightrect( rect );

} /* CreateCurrentWnd */

/*
 * SetColour - sets the current colours.
 */
void SetColour( int mousebutton, COLORREF colour, COLORREF solid, wie_clrtype type )
{
    HDC         hdc;
    WPI_PRES    pres;
    WPI_PRES    mempres;
    HBITMAP     oldbitmap;
    HPEN        blackpen;
    HPEN        oldpen;
    HBRUSH      brush;
    HBRUSH      oldbrush;
    int         top;
    int         bottom;

    blackpen = _wpi_createpen( PS_SOLID, 0, BLACK );
    if (mousebutton == LMOUSEBUTTON) {
        lButton.colour = colour;
        lButton.solid = solid;
        lButton.type = type;
        if (lButton.bitmap) {
            _wpi_deletebitmap( lButton.bitmap );
            pres = _wpi_getpres( HWND_DESKTOP );
            if ( (numberOfColours == 2) && (type == NORMAL_CLR) ) {
                lButton.bitmap = _wpi_createbitmap( CUR_SQR_SIZE+1, 2*CUR_SQR_SIZE+1, 1, 1,
                                                NULL );
            } else {
                lButton.bitmap = _wpi_createcompatiblebitmap( pres,
                                        CUR_SQR_SIZE+1, 2*CUR_SQR_SIZE+1 );
            }
            mempres = _wpi_createcompatiblepres( pres, Instance, &hdc );
            _wpi_torgbmode( mempres );
            oldbitmap = _wpi_selectobject( mempres, lButton.bitmap );
            _wpi_releasepres( HWND_DESKTOP, pres );

            oldpen = _wpi_selectobject( mempres, blackpen );
            brush = _wpi_createsolidbrush( solid );
            oldbrush = _wpi_selectobject( mempres, brush );

            top = _wpi_cvth_y( 0, currentHeight );
            bottom = _wpi_cvth_y( CUR_SQR_SIZE+1, currentHeight );

            _wpi_rectangle( mempres, 0, top, CUR_SQR_SIZE+1, bottom );
            _wpi_selectobject( mempres, oldbrush );
            _wpi_deleteobject( brush );

            brush = _wpi_createsolidbrush( colour );
            oldbrush = _wpi_selectobject( mempres, brush );

            top = _wpi_cvth_y( CUR_SQR_SIZE, currentHeight );
            bottom = _wpi_cvth_y( 2*CUR_SQR_SIZE + 1, currentHeight );

            _wpi_rectangle( mempres, 0, top, CUR_SQR_SIZE+1, bottom );
            _wpi_selectobject( mempres, oldbrush );
            _wpi_deleteobject( brush );
            _wpi_selectobject( mempres, oldbitmap );
            _wpi_selectobject( mempres, oldpen );
            _wpi_deletecompatiblepres( mempres, hdc );
        }
    } else {
        rButton.colour = colour;
        rButton.solid = solid;
        rButton.type = type;
        if (rButton.bitmap) {
            _wpi_deletebitmap( rButton.bitmap );
            pres = _wpi_getpres( HWND_DESKTOP );
            if ( (numberOfColours == 2) && (type == NORMAL_CLR) ) {
                rButton.bitmap = _wpi_createbitmap( CUR_SQR_SIZE+1, 2*CUR_SQR_SIZE+1, 1, 1,
                                                NULL );
            } else {
                rButton.bitmap = _wpi_createcompatiblebitmap( pres,
                                CUR_SQR_SIZE+1, 2*CUR_SQR_SIZE+1 );
            }
            mempres = _wpi_createcompatiblepres( pres, Instance, &hdc );
            _wpi_torgbmode( mempres );

            oldbitmap = _wpi_selectobject( mempres, rButton.bitmap );
            _wpi_releasepres( HWND_DESKTOP, pres );
            oldpen = _wpi_selectobject( mempres, blackpen );
            brush = _wpi_createsolidbrush( solid );
            oldbrush = _wpi_selectobject( mempres, brush );

            top = _wpi_cvth_y( 0, currentHeight );
            bottom = _wpi_cvth_y( CUR_SQR_SIZE+1, currentHeight );
            _wpi_rectangle( mempres, 0, top, CUR_SQR_SIZE+1, bottom );
            _wpi_selectobject( mempres, oldbrush );
            _wpi_deleteobject( brush );

            brush = _wpi_createsolidbrush( colour );
            oldbrush = _wpi_selectobject( mempres, brush );

            top = _wpi_cvth_y( CUR_SQR_SIZE, currentHeight );
            bottom = _wpi_cvth_y( 2*CUR_SQR_SIZE + 1, currentHeight );

            _wpi_rectangle( mempres, 0, top, CUR_SQR_SIZE+1, bottom );
            _wpi_selectobject( mempres, oldbrush );
            _wpi_deleteobject( brush );
            _wpi_selectobject( mempres, oldbitmap );
            _wpi_selectobject( mempres, oldpen );
            _wpi_deletecompatiblepres( mempres, hdc );
        }
    }
    _wpi_deleteobject( blackpen );
    InvalidateRect( hCurrentWnd, NULL, TRUE );
} /* SetColour */

/*
 * GetSelectedColour - This function returns the value of the selected
 *                     fill colour (ie dithered).  The parameter 'solid'
 *                     will have the value of the solid colour if it is
 *                     not NULL.
 */
COLORREF GetSelectedColour( int mousebutton, COLORREF *solid, wie_clrtype *type )
{
    if (mousebutton == LMOUSEBUTTON) {
        *type = lButton.type;
        if (solid) {
            *solid = lButton.solid;
        }
        return( lButton.colour );
    } else {
        *type = rButton.type;
        if (solid) {
            *solid = rButton.solid;
        }
        return( rButton.colour );
    }
} /* GetSelectedColour */

/*
 * VerifyCurrentClr - if either of the left or right buttons are screen
 *                    colours, changes them.
 */
void VerifyCurrentClr( COLORREF screen_colour, COLORREF inverse_colour )
{
    if (lButton.type != NORMAL_CLR) {
        if (lButton.type == SCREEN_CLR) {
            lButton.colour = screen_colour;
            lButton.solid = screen_colour;
            if (lButton.bitmap) {
                SetColour( LMOUSEBUTTON, screen_colour, screen_colour,
                                                                SCREEN_CLR );
            }
        } else if (lButton.type == INVERSE_CLR) {
            lButton.colour = inverse_colour;
            lButton.solid = inverse_colour;
            if (lButton.bitmap) {
                SetColour( LMOUSEBUTTON, inverse_colour, inverse_colour,
                                                                INVERSE_CLR );
            }
        }
    }

    if (rButton.type != NORMAL_CLR) {
        if (rButton.type == SCREEN_CLR) {
            rButton.colour = screen_colour;
            rButton.solid = screen_colour;
            if (rButton.bitmap) {
                SetColour( RMOUSEBUTTON, screen_colour, screen_colour,
                                                                SCREEN_CLR );
            }
        } else if (rButton.type == INVERSE_CLR) {
            rButton.colour = inverse_colour;
            rButton.solid = inverse_colour;
            if (rButton.bitmap) {
                SetColour( RMOUSEBUTTON, inverse_colour, inverse_colour,
                                                                INVERSE_CLR );
            }
        }
    }
} /* VerifyCurrentClr */

/*
 * SetCurrentNumColours - sets the number of colours for this module.
 */
void SetCurrentNumColours( int colour_count )
{
    WPI_PRES    pres;
    WPI_PRES    mempres;
    HDC         memdc;
    HBITMAP     oldbitmap;

    numberOfColours = colour_count;
    pres = _wpi_getpres( HWND_DESKTOP );
    _wpi_torgbmode( pres );

    if (lButton.bitmap) {
        _wpi_deletebitmap( lButton.bitmap );
    }

    if (rButton.bitmap) {
        _wpi_deletebitmap( rButton.bitmap );
    }

    if (colour_count == 2) {
        lButton.bitmap = _wpi_createbitmap( CUR_SQR_SIZE+1, 2*CUR_SQR_SIZE+1, 1, 1, NULL );
        rButton.bitmap = _wpi_createbitmap( CUR_SQR_SIZE+1, 2*CUR_SQR_SIZE+1, 1, 1, NULL );
    //} else if (colour_count == 16) {
    } else {
        lButton.bitmap = _wpi_createcompatiblebitmap( pres, CUR_SQR_SIZE+1, 2*CUR_SQR_SIZE+1 );
        rButton.bitmap = _wpi_createcompatiblebitmap( pres, CUR_SQR_SIZE+1, 2*CUR_SQR_SIZE+1 );

        if (firstTime) {
            mempres = _wpi_createcompatiblepres( pres, Instance, &memdc );
            _wpi_torgbmode( mempres );

            oldbitmap = _wpi_selectobject( mempres, lButton.bitmap );
            _wpi_patblt( mempres, 0, 0, CUR_SQR_SIZE+1, 2*CUR_SQR_SIZE+1, BLACKNESS );
            _wpi_selectobject( mempres, oldbitmap );

            oldbitmap = _wpi_selectobject( mempres, rButton.bitmap );
            _wpi_patblt( mempres, 0, 0, CUR_SQR_SIZE+1, 2*CUR_SQR_SIZE+1, WHITENESS );
            _wpi_selectobject( mempres, oldbitmap );
            _wpi_deletecompatiblepres( mempres, memdc );
            firstTime = FALSE;
        }
    }
    _wpi_releasepres( HWND_DESKTOP, pres );
} /* SetCurrentNumColours */

/*
 * ChangeCurrentColour - This routine is called when the colour palette
 *                       displayed is no longer showing the screen colours.
 *                       It verifies that the current colour is not a screen
 *                       colour.
 */
void ChangeCurrentColour( void )
{
    if (lButton.type != NORMAL_CLR) {
        if ( numberOfColours > 2 ) {
            lButton.type = NORMAL_CLR;
        } else {
            SetColour( LMOUSEBUTTON, BLACK, BLACK, NORMAL_CLR );
        }
    }
    if (rButton.type != NORMAL_CLR) {
        if ( numberOfColours > 2 ) {
            rButton.type = NORMAL_CLR;
        } else {
            SetColour( LMOUSEBUTTON, WHITE, WHITE, NORMAL_CLR );
        }
    }
} /* ChangeCurrentColour */

