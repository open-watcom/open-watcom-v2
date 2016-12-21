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
#include "iedde.h"

static void IECheckIfActiveWindow( void )
{
    HWND        active;

    active = GetActiveWindow();
    if( active != HMainWindow ) {
        if( IsIconic( HMainWindow ) ) {
            ShowWindow( HMainWindow, SW_RESTORE );
        }
#ifdef __NT__
        SetWindowPos( HMainWindow, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
        SetWindowPos( HMainWindow, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
        SetWindowPos( HMainWindow, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
        SetForegroundWindow( HMainWindow );
#else
        SetActiveWindow( HMainWindow );
        SetWindowPos( HMainWindow, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
#endif
    }
}

/*
 * lastChanceSave - called when the user quits and the current image
 *                  is not yet saved
 *                - returns FALSE if CANCEL is selected
 *                - otherwise, returns TRUE
 */
static BOOL lastChanceSave( img_node *node )
{
    int         retcode;
    int         how;
    HMENU       hmenu;
    char        *title;
    char        *text;
    char        *msg_text;
    char        filename[_MAX_PATH];

    if( node == NULL ) {
        return( TRUE );
    }

    IECheckIfActiveWindow();
    if( strnicmp( node->fname, IEImageUntitled, strlen( IEImageUntitled ) ) != 0 ) {
        GetFnameFromPath( node->fname, filename );
        how = SB_SAVE;
    } else {
        strcpy( filename, node->fname );
        how = SB_SAVE_AS;
    }

    retcode = IDCANCEL;
    title = IEAllocRCString( WIE_EXITTITLE );
    if( ImgedIsDDE ) {
        text = IEAllocRCString( WIE_QUERYIMAGEUPDATE );
    } else {
        text = IEAllocRCString( WIE_QUERYIMAGESAVE );
    }
    if( text != NULL ) {
        msg_text = (char *)MemAlloc( strlen( text ) + strlen( filename ) + 1 );
        if( msg_text != NULL ) {
            sprintf( msg_text, text, filename );
            retcode = _wpi_messagebox( HMainWindow, msg_text, title,
                                       MB_YESNOCANCEL | MB_ICONQUESTION );
            MemFree( msg_text );
        }
        IEFreeRCString( text );
    }
    if( title != NULL ) {
        IEFreeRCString( title );
    }

    if( retcode == IDYES ) {
        if( ImgedIsDDE ) {
#ifndef __OS2_PM__
            if( IEUpdateDDEEditSession() ) {
                SetIsSaved( node->hwnd, TRUE );
            } else {
                PrintHintTextByID( WIE_FILENOTSAVED, NULL );
                return( FALSE );
            }
#else
            PrintHintTextByID( WIE_FILENOTSAVED, NULL );
            return( FALSE );
#endif
        } else {
            if( !SaveFile( how ) ) {
                PrintHintTextByID( WIE_FILENOTSAVED, NULL );
                return( FALSE );
            } else {
                hmenu = _wpi_getmenu( HMainWindow );
                _wpi_enablemenuitem( hmenu, IMGED_SAVE, FALSE, FALSE );
                SetIsSaved( node->hwnd, TRUE );
            }
        }
    } else if( retcode == IDCANCEL ) {
        return( FALSE );
    }
    return( TRUE );

} /* lastChanceSave */

/*
 * closeTheImage - get the current image the user is editing, delete
 *                 the node from the linked list, and send a message to
 *                 destroy the MDI child (this will activate another child)
 */
static void closeTheImage( img_node *node )
{
    char        file_name[_MAX_PATH];
    BOOL        ret;
    HWND        hwnd;

    if( node == NULL ) {
        return;
    }

    ret = _wpi_destroywindow( _wpi_getframe( node->viewhwnd ) );
    GetFnameFromPath( node->fname, file_name );
    hwnd = node->hwnd;
    DeleteUndoStack( hwnd );
    if( !DeleteNode( hwnd ) ) {
        WImgEditError( WIE_ERR_BAD_HWND, WIE_INTERNAL_002 );
        return;
    }
#ifdef __OS2_PM__
    ret = DestroyWindow( _wpi_getframe( hwnd ) );
#else
    SendMessage( ClientWindow, WM_MDIDESTROY, (WPARAM)hwnd, 0L );
#endif

} /* closeTheImage */

/*
 * CloseAllImages - used in the close all menu option
 */
void CloseAllImages( void )
{
    img_node    *head;
    img_node    *current;
    WPI_PARAM1  p1;
    HWND        parent;

    head = GetHeadNode();
    if( head == NULL ) {
        PrintHintTextByID( WIE_NOIMAGESTOCLOSE, NULL );
        return;
    }

    current = head;
    while( current != NULL ) {
        if( !current->issaved ) {
            p1 = WPI_MAKEP1( current->hwnd, 0 );
            parent = _wpi_getparent( current->hwnd );
#ifdef __OS2_PM__
            _wpi_sendmessage( parent, WM_ACTIVATE, (WPI_PARAM1)p1, 0L );
#else
            _wpi_sendmessage( parent, WM_MDIRESTORE, (WPI_PARAM1)p1, 0L );
            _wpi_sendmessage( ClientWindow, WM_MDIACTIVATE, (WPI_PARAM1)p1, 0L );
#endif

            if( !lastChanceSave( current ) ) {
                return;
            }
        }
        current = current->next;
    }

    DeleteActiveImage();
    while( head != NULL ) {
        closeTheImage( head );
        head = GetHeadNode();
    }

    ClearImageText();
    GrayEditOptions();
    PrintHintTextByID( WIE_ALLIMAGESCLOSED, NULL );
    _wpi_setwindowtext( _wpi_getframe( HMainWindow ), IEAppTitle );

} /* CloseAllImages */

/*
 * CloseCurrentImage - get the current image the user is editing, delete
 *                     the node from the linked list, and send a message to
 *                     destroy the MDI child (this will activate another child)
 */
void CloseCurrentImage( HWND hwnd )
{
    img_node    *node;
    char        file_name[_MAX_PATH];
    BOOL        ret;

    node = SelectImage( hwnd );
    if( node == NULL ) {
        return;
    }
    ret = DestroyWindow( _wpi_getframe( node->viewhwnd ) );
    GetFnameFromPath( node->fname, file_name );
    DeleteUndoStack( hwnd );
    if( !DeleteNode( hwnd ) ) {
        WImgEditError( WIE_ERR_BAD_HWND, WIE_INTERNAL_002 );
        return;
    }
    DeleteActiveImage();
    ClearImageText();

    GrayEditOptions();
    PrintHintTextByID( WIE_FILEHASBEENCLOSED, file_name );
    SetWindowText( _wpi_getframe( HMainWindow ), IEAppTitle );

#ifdef __OS2_PM__
    ret = DestroyWindow( _wpi_getframe( hwnd ) );
#else
    SendMessage( ClientWindow, WM_MDIDESTROY, (WPARAM)hwnd, 0L );
#endif

} /* CloseCurrentImage */

/*
 * SaveAllImages - save all currently open images
 */
void SaveAllImages( void )
{
    img_node    *head;
    img_node    *current;

    head = GetHeadNode();
    if( head == NULL ) {
        return;
    }

    current = head;
    while( current != NULL ) {
        if( !current->issaved ) {
            SaveFileFromNode( current, SB_SAVE );
        }
        current = current->next;
    }

} /* SaveAllImages */
