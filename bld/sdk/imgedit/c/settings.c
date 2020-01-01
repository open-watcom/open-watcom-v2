/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
#include "ieprofil.h"


/* Local Window callback functions prototypes */
WPI_EXPORT WPI_DLGRESULT CALLBACK CurrentSettingsDlgProc( HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam, WPI_PARAM2 lparam );

static int      stretchClipPaste;
static int      rotateType;
static bool     fCheckSquareGrid = false;
static bool     fKeepSelectedArea;
static bool     fSavePosition;
static bool     fSaveSettings;
static bool     fWrapShift;

/*
 * CurrentSettingsDlgProc - display the current settings and allows for change
 */
WPI_DLGRESULT CALLBACK CurrentSettingsDlgProc( HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam, WPI_PARAM2 lparam )
{
    BOOL                err;
    char                *title;
    char                *text;
    char                *msg_text;
    short               new_shift;
    static bool         keepsquare;
    bool                ret;

    ret = false;

    if( _wpi_dlg_command( hwnd, &msg, &wparam, &lparam ) ) {
        switch( LOWORD( wparam ) ) {
        case DLGID_OK:
            new_shift = _wpi_getdlgitemshort( hwnd, SHIFT_AMOUNT, &err, TRUE );
            if( new_shift > MAX_DIM || new_shift < MIN_DIM ) {
                title = IEAllocRCString( WIE_NOTE );
                text = IEAllocRCString( WIE_SHIFTBETWEEN );
                if( text != NULL ) {
                    msg_text = (char *)MemAlloc( strlen( text ) + 20 + 1 );
                    if( msg_text != NULL ) {
                        sprintf( msg_text, text, MIN_DIM, MAX_DIM );
                        MessageBox( hwnd, msg_text, title, MB_OK | MB_ICONINFORMATION );
                        MemFree( msg_text );
                    }
                    IEFreeRCString( text );
                }
                if( title != NULL ) {
                    IEFreeRCString( title );
                }
                break;
            }
            ImgedConfigInfo.shift = new_shift;

            fKeepSelectedArea = _wpi_isbuttonchecked( hwnd, LEAVE_AREA );
            fSaveSettings = _wpi_isbuttonchecked( hwnd, SAVE_SETTINGS );
            fSavePosition = _wpi_isbuttonchecked( hwnd, SAVE_POSITION );

            if( _wpi_isbuttonchecked( hwnd, KEEP_SQUARE ) && !keepsquare ) {
                fCheckSquareGrid = true;
            } else if( !_wpi_isbuttonchecked( hwnd, KEEP_SQUARE ) && keepsquare ) {
                fCheckSquareGrid = true;
            } else {
                fCheckSquareGrid = false;
            }

            if( _wpi_isbuttonchecked( hwnd, STRETCH_PASTE ) ) {
                stretchClipPaste = STRETCH_PASTE;
            } else {
                stretchClipPaste = CLIP_PASTE;
            }
            if( _wpi_isbuttonchecked( hwnd, STRETCH_ROTATE ) ) {
                rotateType = STRETCH_ROTATE;
            } else if( _wpi_isbuttonchecked( hwnd, SIMPLE_ROTATE ) ) {
                rotateType = SIMPLE_ROTATE;
            } else {
                rotateType = CLIP_ROTATE;
            }

            fWrapShift = _wpi_isbuttonchecked( hwnd, WRAP_SHIFT );
            SetViewWindow( _wpi_isbuttonchecked( hwnd, SHOW_ONE ) );
            _wpi_enddialog( hwnd, DLGID_OK );
            break;

        case DLGID_CANCEL:
            _wpi_enddialog( hwnd, DLGID_CANCEL );
            break;

        case SETTINGS_HELP:
            IEHelpRoutine();
            break;
        }
    } else {
        switch( msg ) {
        case WM_INITDIALOG:
            _wpi_checkradiobutton( hwnd, STRETCH_PASTE, CLIP_PASTE, stretchClipPaste );
            _wpi_checkradiobutton( hwnd, SIMPLE_ROTATE, CLIP_ROTATE, rotateType );

            if( IsOneViewWindow() ) {
                _wpi_checkradiobutton( hwnd, SHOW_ONE, SHOW_ALL, SHOW_ONE );
            } else {
                _wpi_checkradiobutton( hwnd, SHOW_ONE, SHOW_ALL, SHOW_ALL );
            }
            _wpi_checkdlgbutton( hwnd, LEAVE_AREA, ( fKeepSelectedArea ) ? BST_CHECKED : BST_UNCHECKED );
            _wpi_checkdlgbutton( hwnd, SAVE_SETTINGS, ( fSaveSettings ) ? BST_CHECKED : BST_UNCHECKED );
            _wpi_checkdlgbutton( hwnd, SAVE_POSITION, ( fSavePosition ) ? BST_CHECKED : BST_UNCHECKED );

            keepsquare = ImgedConfigInfo.square_grid;

            _wpi_checkdlgbutton( hwnd, KEEP_SQUARE, ( keepsquare ) ? BST_CHECKED : BST_UNCHECKED );
            _wpi_setdlgitemshort( hwnd, SHIFT_AMOUNT, ImgedConfigInfo.shift, FALSE );

            if( fWrapShift ) {
                _wpi_checkradiobutton( hwnd, WRAP_SHIFT, CLIP_SHIFT, WRAP_SHIFT );
            } else {
                _wpi_checkradiobutton( hwnd, WRAP_SHIFT, CLIP_SHIFT, CLIP_SHIFT );
            }
            ret = true;
            break;

#ifndef __OS2_PM__
        case WM_SYSCOLORCHANGE:
            IECtl3dColorChange();
            break;
#endif

        case WM_CLOSE:
            _wpi_enddialog( hwnd, IDCANCEL );
            break;
        default:
            return( _wpi_defdlgproc( hwnd, msg, wparam, lparam ) );
        }
    }
    _wpi_dlgreturn( ret );

} /* CurrentSettingsDlgProc */

/*
 * SelectOptions - bring up the current settings dialog box
 */
void SelectOptions( void )
{
    WPI_DLGPROC     dlgproc;
    WPI_DLGRESULT   button_type;
    HMENU           hmenu;

    dlgproc = _wpi_makedlgprocinstance( CurrentSettingsDlgProc, Instance );
    button_type = _wpi_dialogbox( HMainWindow, dlgproc, Instance, CURRENT_SETTINGS, 0L );
    _wpi_freedlgprocinstance( dlgproc );

    if( button_type == DLGID_CANCEL ) {
        return;
    }

    hmenu = GetMenu( _wpi_getframe( HMainWindow ) );
    if( fCheckSquareGrid ) {
        CheckSquareGrid( hmenu );
    }

} /* SelectOptions */

/*
 * StretchPastedImage - return whether we should stretch the pasted image or
 *                      not (not => clip the image)
 */
int StretchPastedImage( void )
{
    if( stretchClipPaste == STRETCH_PASTE ) {
        return( 1 );
    } else {
        return( 0 );
    }

} /* StretchPastedImage */

/*
 * GetRotateType - returns the rotate type:
 *                      - rotate the image and clip ONLY if necessary
 *                      - rotate image and clip inside current selection rect
 *                      - rotate image and stretch inside current selection rect
 */
int GetRotateType( void )
{
    return( rotateType );

} /* GetRotateType */

/*
 * DoKeepRect - return whether or not we want to keep the rectangle after rotating
 */
bool DoKeepRect( void )
{
    return( fKeepSelectedArea );

} /* DoKeepRect */

/*
 * SetSettingsDlg - set the values in the settings dialog
 */
void SetSettingsDlg( settings_info *info )
{
    int         rotate;

    if( info->paste == SET_PASTE_STR ) {
        stretchClipPaste = STRETCH_PASTE;
    } else {
        stretchClipPaste = CLIP_PASTE;
    }

    if( info->rotate > 10 ) {
        fKeepSelectedArea = true;
        rotate = info->rotate - 10;
    } else {
        fKeepSelectedArea = false;
        rotate = info->rotate;
    }
    if( rotate == SET_ROT_SIMPLE ) {
        rotateType = SIMPLE_ROTATE;
    } else if( rotate == SET_ROT_STR ) {
        rotateType = STRETCH_ROTATE;
    } else {
        rotateType = CLIP_ROTATE;
    }

    if( info->viewwnd == SET_VIEW_1 ) {
        SetViewWindow( true );
    } else {
        SetViewWindow( false );
    }

    fSaveSettings = ( (info->settings & SET_SAVE_SET) != 0 );
    fSavePosition = ( (info->settings & SET_SAVE_POS) != 0 );

    fWrapShift = info->wrapshift;

} /* SetSettingsDlg */

/*
 * GetSettings - get the values from the current settings dialog
 */
void GetSettings( settings_info *info )
{
    int         keeparea;

    if( stretchClipPaste == STRETCH_PASTE ) {
        info->paste = SET_PASTE_STR;
    } else {
        info->paste = SET_PASTE_CLIP;
    }

    if( fKeepSelectedArea ) {
        keeparea = 10;
    } else {
        keeparea = 0;
    }
    if( rotateType == SIMPLE_ROTATE ) {
        info->rotate = keeparea + SET_ROT_SIMPLE;
    } else if( rotateType == STRETCH_ROTATE ) {
        info->rotate = keeparea + SET_ROT_STR;
    } else {
        info->rotate = keeparea + SET_ROT_CLIP;
    }

    if( IsOneViewWindow() ) {
        info->viewwnd = SET_VIEW_1;
    } else {
        info->viewwnd = SET_VIEW_MORE;
    }

    info->settings = 0;
    if( fSaveSettings ) {
        info->settings |= SET_SAVE_SET;
    }
    if( fSavePosition ) {
        info->settings |= SET_SAVE_POS;
    }
    info->wrapshift = fWrapShift;

} /* GetSettings */

/*
 * IsShiftWrap - return whether or not we wrap the shift
 */
bool IsShiftWrap( void )
{
    return( fWrapShift );

} /* IsShiftWrap */
