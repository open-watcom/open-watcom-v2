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

WINEXPORT WPI_DLGRESULT CALLBACK ChangeSizeProc( HWND, WPI_MSG, WPI_PARAM1, WPI_PARAM2 );

static int      imgHeight;
static int      imgWidth;
static BOOL     stretchImage;

/*
 * ChangeSizeProc - change the size of the image being edited
 */
WINEXPORT WPI_DLGRESULT CALLBACK ChangeSizeProc( HWND hwnd, WPI_MSG msg,
                                       WPI_PARAM1 wparam, WPI_PARAM2 lparam )
{
    BOOL        trnslate;
    char        *title;
    char        *text;
    char        *msg_text;

    if( _wpi_dlg_command( hwnd, &msg, &wparam, &lparam ) ) {
        switch( LOWORD( wparam ) ) {
        case DLGID_OK:
            imgHeight = _wpi_getdlgitemint( hwnd, SIZE_HEIGHT, &trnslate, TRUE );
            imgWidth = _wpi_getdlgitemint( hwnd, SIZE_WIDTH, &trnslate, TRUE );
            if( !trnslate ) {
                return( FALSE );
            }
            if( imgHeight > MAX_DIM || imgHeight < MIN_DIM ||
                imgWidth > MAX_DIM || imgWidth < MIN_DIM ) {
                title = IEAllocRCString( WIE_NOTE );
                text = IEAllocRCString( WIE_DIMENSIONSBETWEEN );
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
                return( FALSE );
            }

            if( _wpi_isbuttonchecked( hwnd, SIZE_STRETCH ) ) {
                stretchImage = TRUE;
            } else {
                stretchImage = FALSE;
            }
            _wpi_enddialog( hwnd, DLGID_OK );
            break;

        case DLGID_CANCEL:
            _wpi_enddialog( hwnd, DLGID_CANCEL );
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
            _wpi_setdlgitemint( hwnd, SIZE_HEIGHT, imgHeight, FALSE );
            _wpi_setdlgitemint( hwnd, SIZE_WIDTH, imgWidth, FALSE );
            if( stretchImage ) {
                _wpi_checkradiobutton( hwnd, SIZE_STRETCH, SIZE_CLIP, SIZE_STRETCH );
            } else {
                _wpi_checkradiobutton( hwnd, SIZE_STRETCH, SIZE_CLIP, SIZE_CLIP );
            }
            return( TRUE );

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
    _wpi_dlgreturn( FALSE );

} /* ChangeSizeProc */

/*
 * ChangeImageSize - changes the size of the current image being edited
 */
void ChangeImageSize( void )
{
    img_node        *node;
    WPI_PROC        fp;
    WPI_DLGRESULT   button_type;
    img_node        new_node;
    WPI_PRES        pres;
    HDC             srcdc;
    WPI_PRES        srcpres;
    HDC             destdc;
    WPI_PRES        destpres;
    HBITMAP         oldsrc;
    HBITMAP         olddest;
    int             retcode;
    WPI_PARAM2      lparam;
    WPI_RECT        rc;
    short           new_width;
    short           new_height;
    int             y_src;
    int             y_dest;
    char            *title;
    char            *text;

    node = GetCurrentNode();
    if( node == NULL ) {
        return;
    }

    if( node->imgtype != BITMAP_IMG ) {
        return;
    }

    imgHeight = node->height;
    imgWidth = node->width;

    fp = _wpi_makeprocinstance( (WPI_PROC)ChangeSizeProc, Instance );
    button_type = _wpi_dialogbox( HMainWindow, (WPI_DLGPROC)fp, Instance, IMAGESIZE, 0L );
    _wpi_freeprocinstance( fp );

    if( button_type == DLGID_CANCEL ) {
        return;
    }
    if( imgWidth == node->width && imgHeight == node->height ) {
        PrintHintTextByID( WIE_IMAGESIZEUNCHANGED, NULL );
        return;
    }
    title = IEAllocRCString( WIE_INFORMATIONTEXT );
    text = IEAllocRCString( WIE_RESETUNDOSTACKWARNING );
    retcode = _wpi_messagebox( HMainWindow, text, title, MB_YESNO | MB_ICONINFORMATION );
    if( text != NULL ) {
        IEFreeRCString( text );
    }
    if( title != NULL ) {
        IEFreeRCString( title );
    }

    if( retcode == WPI_IDNO ) {
        return;
    }

    new_node.width = (short)imgWidth;
    new_node.height = (short)imgHeight;
    new_node.bitcount = node->bitcount;
    new_node.imgtype = BITMAP_IMG;

    MakeBitmap( &new_node, TRUE );

    pres = _wpi_getpres( HWND_DESKTOP );
    srcpres = _wpi_createcompatiblepres( pres, Instance, &srcdc );
    destpres = _wpi_createcompatiblepres( pres, Instance, &destdc );
    _wpi_releasepres( HWND_DESKTOP, pres );

    oldsrc = _wpi_selectbitmap( srcpres, node->hxorbitmap );
    olddest = _wpi_selectbitmap( destpres, new_node.hxorbitmap );

    if( stretchImage ) {
        _wpi_stretchblt( destpres, 0, 0, imgWidth, imgHeight,
                         srcpres, 0, 0, node->width, node->height, SRCCOPY );
    } else {
#ifdef __OS2_PM__
        y_src = node->height - imgHeight;
        if( y_src < 0 ) {
            y_src = 0;
            y_dest = imgHeight - node->height;
        } else {
            y_dest = 0;
        }
#else
        y_src = 0;
        y_dest = 0;
#endif
        _wpi_bitblt( destpres, 0, y_dest, node->width, node->height,
                     srcpres, 0, y_src, SRCCOPY );
    }
    _wpi_getoldbitmap( srcpres, oldsrc );
    oldsrc = _wpi_selectbitmap( srcpres, node->handbitmap );
    _wpi_getoldbitmap( destpres, olddest );
    olddest = _wpi_selectbitmap( destpres, new_node.handbitmap );

    if( stretchImage ) {
        _wpi_stretchblt( destpres, 0, 0, imgWidth, imgHeight,
                         srcpres, 0, 0, node->width, node->height, SRCCOPY );
    } else {
        _wpi_bitblt( destpres, 0, 0, node->width, node->height, srcpres, 0, 0, SRCCOPY );
    }

    _wpi_getoldbitmap( srcpres, oldsrc );
    _wpi_getoldbitmap( destpres, olddest );
    _wpi_deletebitmap( node->hxorbitmap );
    _wpi_deletebitmap( node->handbitmap );
    _wpi_deletecompatiblepres( srcpres, srcdc );
    _wpi_deletecompatiblepres( destpres, destdc );

    node->hxorbitmap = new_node.hxorbitmap;
    node->handbitmap = new_node.handbitmap;
    node->width = (short)imgWidth;
    node->height = (short)imgHeight;

    SetIsSaved( node->hwnd, FALSE );
    ResetUndoStack( node );

    RePositionViewWnd( node );

    _wpi_getclientrect( node->hwnd, &rc );
    new_width = (short)_wpi_getwidthrect( rc );
    new_height = (short)_wpi_getheightrect( rc );
    lparam = WPI_MAKEP2( new_width, new_height );

    ResizeChild( lparam, node->hwnd, false );
    DisplayImageText( node );
    WriteSetSizeText( WIE_NEWIMAGESIZE, imgWidth, imgHeight );

} /* ChangeImageSize */
