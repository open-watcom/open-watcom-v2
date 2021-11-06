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


/* Local Window callback functions prototypes */
WPI_EXPORT WPI_DLGRESULT CALLBACK ChangeSizeDlgProc( HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam, WPI_PARAM2 lparam );

static int      imgHeight;
static int      imgWidth;
static bool     stretchImage;

/*
 * ChangeSizeDlgProc - change the size of the image being edited
 */
WPI_DLGRESULT CALLBACK ChangeSizeDlgProc( HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam, WPI_PARAM2 lparam )
{
    BOOL        trnslate;
    char        *title;
    char        *text;
    char        *msg_text;
    bool        ret;

    ret = false;

    if( _wpi_dlg_command( hwnd, &msg, &wparam, &lparam ) ) {
        switch( LOWORD( wparam ) ) {
        case DLGID_OK:
            imgHeight = _wpi_getdlgitemint( hwnd, SIZE_HEIGHT, &trnslate, TRUE );
            imgWidth = _wpi_getdlgitemint( hwnd, SIZE_WIDTH, &trnslate, TRUE );
            if( !trnslate ) {
                break;
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
                break;
            }

            if( _wpi_isbuttonchecked( hwnd, SIZE_STRETCH ) ) {
                stretchImage = true;
            } else {
                stretchImage = false;
            }
            _wpi_enddialog( hwnd, DLGID_OK );
            break;

        case DLGID_CANCEL:
            _wpi_enddialog( hwnd, DLGID_CANCEL );
            break;

        case IDB_HELP:
            IEHelpRoutine();
            break;
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

} /* ChangeSizeDlgProc */

/*
 * ChangeImageSize - changes the size of the current image being edited
 */
void ChangeImageSize( void )
{
    img_node        *node;
    WPI_DLGPROC     dlgproc;
    WPI_DLGRESULT   button_type;
    img_node        new_node;
    WPI_PRES        pres;
    HDC             srcdc;
    WPI_PRES        srcpres;
    HDC             dstdc;
    WPI_PRES        dstpres;
    WPI_HBITMAP     oldsrc_hbitmap;
    WPI_HBITMAP     olddst_hbitmap;
    int             retcode;
    WPI_PARAM2      lparam;
    WPI_RECT        rc;
    short           new_width;
    short           new_height;
    int             y_src;
    int             y_dst;
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

    dlgproc = _wpi_makedlgprocinstance( ChangeSizeDlgProc, Instance );
    button_type = _wpi_dialogbox( HMainWindow, dlgproc, Instance, IMAGESIZE, 0L );
    _wpi_freedlgprocinstance( dlgproc );

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

    MakeBitmap( &new_node, true );

    pres = _wpi_getpres( HWND_DESKTOP );
    srcpres = _wpi_createcompatiblepres( pres, Instance, &srcdc );
    dstpres = _wpi_createcompatiblepres( pres, Instance, &dstdc );
    _wpi_releasepres( HWND_DESKTOP, pres );

    oldsrc_hbitmap = _wpi_selectbitmap( srcpres, node->xor_hbitmap );
    olddst_hbitmap = _wpi_selectbitmap( dstpres, new_node.xor_hbitmap );

    if( stretchImage ) {
        _wpi_stretchblt( dstpres, 0, 0, imgWidth, imgHeight,
                         srcpres, 0, 0, node->width, node->height, SRCCOPY );
    } else {
#ifdef __OS2_PM__
        y_src = node->height - imgHeight;
        if( y_src < 0 ) {
            y_src = 0;
            y_dst = imgHeight - node->height;
        } else {
            y_dst = 0;
        }
#else
        y_src = 0;
        y_dst = 0;
#endif
        _wpi_bitblt( dstpres, 0, y_dst, node->width, node->height,
                     srcpres, 0, y_src, SRCCOPY );
    }
    _wpi_getoldbitmap( srcpres, oldsrc_hbitmap );
    oldsrc_hbitmap = _wpi_selectbitmap( srcpres, node->and_hbitmap );
    _wpi_getoldbitmap( dstpres, olddst_hbitmap );
    olddst_hbitmap = _wpi_selectbitmap( dstpres, new_node.and_hbitmap );

    if( stretchImage ) {
        _wpi_stretchblt( dstpres, 0, 0, imgWidth, imgHeight,
                         srcpres, 0, 0, node->width, node->height, SRCCOPY );
    } else {
        _wpi_bitblt( dstpres, 0, 0, node->width, node->height, srcpres, 0, 0, SRCCOPY );
    }

    _wpi_getoldbitmap( srcpres, oldsrc_hbitmap );
    _wpi_getoldbitmap( dstpres, olddst_hbitmap );
    _wpi_deletebitmap( node->xor_hbitmap );
    _wpi_deletebitmap( node->and_hbitmap );
    _wpi_deletecompatiblepres( srcpres, srcdc );
    _wpi_deletecompatiblepres( dstpres, dstdc );

    node->xor_hbitmap = new_node.xor_hbitmap;
    node->and_hbitmap = new_node.and_hbitmap;
    node->width = (short)imgWidth;
    node->height = (short)imgHeight;

    SetIsSaved( node->hwnd, false );
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
