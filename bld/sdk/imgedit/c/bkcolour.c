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
#include "ieclrpal.h"

static palette_box      screenColour;
static palette_box      inverseColour;
static palette_box      availColour[16];

/*
 * displayColours - display the colours.
 */
static void displayColours( HWND hwnd )
{
    short       i;
    WPI_PRES    pres;
    HWND        currentwnd;

    inverseColour.colour = GetInverseColour( screenColour.colour );

    currentwnd = _wpi_getdlgitem( hwnd, BK_CURRENT );

    pres = _wpi_getpres( currentwnd );
    _wpi_torgbmode( pres );
    DisplayColourBox( pres, &screenColour );
    DisplayColourBox( pres, &inverseColour );

    for (i=0; i < 16; ++i) {
        DisplayColourBox( pres, &availColour[i] );
    }
    _wpi_releasepres( currentwnd, pres );
} /* displayColours */

/*
 * showColours - displays the colours to choose from
 */
static void showColours( HWND hwnd )
{
    InitFromColourPalette( &screenColour, &inverseColour, &availColour );
    displayColours(hwnd);
} /* showColours */

/*
 * selectColour - select the colour.
 */
static void selectColour( WPI_POINT *pt, HWND hwnd )
{
    int         i;
    WPI_PRES    pres;
    HWND        currentwnd;
    int         top;
    int         bottom;
    WPI_RECT    wrect;

    currentwnd = _wpi_getdlgitem( hwnd, BK_CURRENT );
    pres = _wpi_getpres( currentwnd );
    _wpi_mapwindowpoints( hwnd, currentwnd, pt, 1 );

    _wpi_torgbmode( pres );
    for (i=0; i < 16; ++i) {
        top = availColour[i].box.top;
        bottom = availColour[i].box.bottom;

        top = _wpi_cvth_y( top, 2*SQR_SIZE );
        bottom = _wpi_cvth_y( bottom, 2*SQR_SIZE );
        _wpi_setintwrectvalues(&wrect, availColour[i].box.left, top,
                                            availColour[i].box.right, bottom);
        if ( _wpi_ptinrect(&wrect, *pt) ) {
            screenColour.colour = availColour[i].colour;
            DisplayColourBox( pres, &screenColour );

            inverseColour.colour = GetInverseColour( screenColour.colour );
            DisplayColourBox( pres, &inverseColour );
            break;
        }
    }
    _wpi_releasepres( currentwnd, pres );
} /* selectColour */

/*
 * SelColourProc - Select the colour to represent the background.
 */
WPI_DLGRESULT CALLBACK SelColourProc(HWND hwnd, WPI_MSG msg,
                                        WPI_PARAM1 wparam, WPI_PARAM2 lparam)
{
    PAINTSTRUCT         ps;
    WPI_POINT           pt;
    WPI_PRES            pres;

    if( _wpi_dlg_command( hwnd, &msg, &wparam, &lparam ) ) {
        switch( LOWORD(wparam) ) {
        case IDOK:
            _wpi_enddialog( hwnd, IDOK );
            break;

        case IDCANCEL:
            _wpi_enddialog( hwnd, IDCANCEL );
            break;

        case IDB_HELP:
            IEHelpRoutine();
            return( FALSE );

        default:
            return( FALSE );
        }
    } else {
        switch( msg ) {
        case WM_INITDIALOG:
            showColours(hwnd);
            return( TRUE );

#ifndef __OS2_PM__
        case WM_SYSCOLORCHANGE:
            IECtl3dColorChange();
            break;
#endif

        case WM_LBUTTONDOWN:
            IMGED_MAKEPOINT( wparam, lparam, pt );
            selectColour( &pt, hwnd );
            break;

        case WM_PAINT:
            pres = _wpi_beginpaint( hwnd, NULL, &ps );
#ifdef __OS2_PM__
            WinFillRect( pres, &ps, CLR_PALEGRAY );
#endif
            displayColours( hwnd );
            _wpi_endpaint( hwnd, pres, &ps );
            _wpi_setfocus( hwnd );
            break;

        case WM_CLOSE:
            _wpi_enddialog( hwnd, IDCANCEL );
            break;
        default:
            return( _wpi_defdlgproc(hwnd, msg, wparam, lparam) );
        }
    }
    _wpi_dlgreturn( FALSE );
} /* SelColourProc */

/*
 * ChooseBkColour - Choose the colour to represent the background
 */
void ChooseBkColour( void )
{
    WPI_PROC            fp;
    int                 button_type;

    screenColour.colour = GetBkColour();
    fp = _wpi_makeprocinstance( (WPI_PROC)SelColourProc, Instance );
    button_type = _wpi_dialogbox( HMainWindow, fp, Instance, SELBKCOLOUR, 0L );
    _wpi_freeprocinstance( fp );

    if (button_type == IDCANCEL) {
        return;
    }

    SetBkColour( screenColour.colour );
    SetScreenClr( screenColour.colour );
    PrintHintTextByID( WIE_NEWBKCOLORSELECTED, NULL );
} /* ChooseBkColour */

