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
#include "ldstr.h"

static char     *ErrorMsgTitle  = NULL;

/*
 * WImgEditError - display the error message
 *                 errno - determines the string to go with the error
 *                 fname - if a file name goes with the error message
 */
void WImgEditError( DWORD error, LPSTR fname )
{
    switch( error ) {
    case WIE_ERR_BITMAP_TOO_BIG:
        PrintHintTextByID( WIE_ERR_BITMAPNOTOPENED, fname );
        IEPrintErrorMsgByID( -1, error, NULL, MB_ICONSTOP | MB_OK );
        break;

    case WIE_ERR_256CLR_BITMAP:
        PrintHintTextByID( WIE_ERR_BITMAPNOTOPENED, fname );
        IEPrintErrorMsgByID( -1, error, NULL, MB_ICONSTOP | MB_OK );
        break;

    case WIE_ERR_BAD_BITMAP_FILE:
        PrintHintTextByID( WIE_ERR_INVALIDBITMAPFILE, fname );
        IEPrintErrorMsgByID( -1, error, fname, MB_ICONSTOP | MB_OK );
        break;

    case WIE_ERR_TOO_MANY_COLORS:
        PrintHintTextByID( WIE_ERR_INVALIDBITMAPFILE, fname );
        IEPrintErrorMsgByID( -1, error, fname, MB_ICONSTOP | MB_OK );
        break;

    case WIE_ERR_BAD_BITMAP_DATA:
        PrintHintTextByID( WIE_ERR_INVALIDBITMAPDATA, fname );
        IEPrintErrorMsgByID( -1, error, fname, MB_ICONSTOP | MB_OK );
        break;

    case WIE_ERR_BAD_ICON_CLR:
        PrintHintTextByID( WIE_ERR_ICONNOTOPENED, fname );
        IEPrintErrorMsgByID( -1, error, NULL, MB_ICONSTOP | MB_OK );
        break;

    case WIE_ERR_BAD_ICON_FILE:
        PrintHintTextByID( WIE_ERR_INVALIDICONFILE, fname );
        IEPrintErrorMsgByID( -1, error, fname, MB_ICONSTOP | MB_OK );
        break;

    case WIE_ERR_BAD_ICON_DATA:
        PrintHintTextByID( WIE_ERR_INVALIDICONDATA, fname );
        IEPrintErrorMsgByID( -1, error, fname, MB_ICONSTOP | MB_OK );
        break;

    case WIE_ERR_BAD_CURSOR_FILE:
        PrintHintTextByID( WIE_ERR_INVALIDCURSORFILE, fname );
        IEPrintErrorMsgByID( -1, error, fname, MB_ICONSTOP | MB_OK );
        break;

    case WIE_ERR_BAD_CURSOR_DATA:
        PrintHintTextByID( WIE_ERR_INVALIDCURSORDATA, fname );
        IEPrintErrorMsgByID( -1, error, fname, MB_ICONSTOP | MB_OK );
        break;

    case WIE_ERR_BAD_FILENAME:
        PrintHintTextByID( WIE_ERR_INVALIDFILENAME, fname );
        IEPrintErrorMsgByID( -1, error, fname, MB_ICONSTOP | MB_OK );
        break;

    case WIE_ERR_FILE_NOT_OPENED:
        PrintHintTextByID( error, fname );
        break;

    case WIE_ERR_STARTUP_FNO:
        PrintHintTextByID( error, fname );
        break;

    case WIE_ERR_BAD_FILE_EXT:
        PrintHintTextByID( WIE_ERR_FILENOTOPENED, fname );
        IEPrintErrorMsgByID( -1, error, fname, MB_ICONSTOP | MB_OK );
        break;

    case WIE_ERR_SAVE_FAIL:
        PrintHintTextByID( WIE_ERR_FILENOTSAVED, fname );
        IEPrintErrorMsgByID( -1, error, fname, MB_ICONSTOP | MB_OK );
        break;

    case WIE_ERR_PALETTE_NOT16:
        PrintHintTextByID( WIE_ERR_PALETTENOTSAVED, NULL );
        IEPrintErrorMsgByID( -1, error, NULL, MB_ICONINFORMATION | MB_OK );
        break;

    case WIE_ERR_BAD_HWND:
        SetHintText( fname );
        IEPrintErrorMsgByID( -1, error, fname, MB_ICONEXCLAMATION | MB_OK );
        break;

    case WIE_ERR_BAD_IMAGE_TYPE:
        PrintHintTextByID( error, NULL );
        break;

    case WIE_ERR_BAD_SELECTION:
        PrintHintTextByID( error, NULL );
        break;

    case WIE_ERR_BAD_FILLCASE:
        PrintHintTextByID( WIE_ERR_NOFILLREGION, NULL );
        IEPrintErrorMsgByID( -1, error, fname, MB_ICONEXCLAMATION | MB_OK );
        break;

    case WIE_ERR_BAD_ICONINDEX:
        SetHintText( fname );
        IEPrintErrorMsgByID( -1, error, fname, MB_ICONEXCLAMATION | MB_OK );
        break;

    case WIE_ERR_BAD_PALFILE:
        PrintHintTextByID( WIE_ERR_FILENOTOPENED, fname );
        IEPrintErrorMsgByID( -1, error, fname, MB_ICONSTOP | MB_OK );
        break;

    case WIE_DDE_INIT_FAILED:
        PrintHintTextByID( error, NULL );
        IEDisplayErrorMsg( -1, error, MB_ICONSTOP | MB_OK );
        break;

    default:
        break;
    }

} /* WImgEditError */

/*
 * IEAllocRCString
 */
char *IEAllocRCString( DWORD id )
{
    return( AllocRCString( id ) );

} /* IEAllocRCString */

/*
 * IEFreeRCString
 */
void IEFreeRCString( char *str )
{
    FreeRCString( str );

} /* IEFreeRCString */

/*
 * IECopyRCString
 */
DWORD IECopyRCString( DWORD id, char *buf, DWORD bufsize )
{
    return( CopyRCString( id, buf, bufsize ) );

} /* IECopyRCString */

/*
 * IEInitDisplayError
 */
void IEInitDisplayError( HINSTANCE inst )
{
    SetInstance( inst );

} /* IEInitDisplayError */

/*
 * IEPrintErrorMsg
 */
void IEPrintErrorMsg( char *title, char *msg, char *fname, UINT style )
{
    char        *text;

    if( msg != NULL ) {
        if( fname != NULL ) {
            text = (char *)MemAlloc( strlen( msg ) + strlen( fname ) + 1 );
            if( text != NULL ) {
                sprintf( text, msg, fname );
                _wpi_messagebox( HMainWindow, text, title, style );
                MemFree( text );
            }
        } else {
            _wpi_messagebox( HMainWindow, msg, title, style );
        }
    }

} /* IEPrintErrorMsg */

/*
 * IEPrintErrorMsgByID
 */
void IEPrintErrorMsgByID( DWORD title, DWORD msg, char *fname, UINT style )
{
    char        *title_str;
    char        *msg_str;

    if( title == -1 ) {
        title_str = ErrorMsgTitle;
    } else {
        title_str = IEAllocRCString( title );
    }

    msg_str = IEAllocRCString( msg );

    IEPrintErrorMsg( title_str, msg_str, fname, style );

    if( msg_str != NULL ) {
        IEFreeRCString( msg_str );
    }

    if( title != -1 && title_str != NULL ) {
        IEFreeRCString( title_str );
    }

} /* IEPrintErrorMsgByID */

/*
 * IEDisplayErrorMsg
 */
void IEDisplayErrorMsg( DWORD title, DWORD msg, UINT style )
{
    char        *title_str;

    if( title == -1 ) {
        title_str = ErrorMsgTitle;
    } else {
        title_str = IEAllocRCString( title );
    }

    if( !RCMessageBox( HMainWindow, msg, title_str, style ) ) {
        MessageBeep( -1 );
    }

    if( title != -1 && title_str != NULL ) {
        IEFreeRCString( title_str );
    }

} /* IEDisplayErrorMsg */

/*
 * IEFiniErrors
 */
void IEFiniErrors( void )
{
    if( ErrorMsgTitle != NULL ) {
        IEFreeRCString( ErrorMsgTitle );
    }

} /* IEFiniErrors */

/*
 * IEInitErrors
 */
BOOL IEInitErrors( HINSTANCE inst )
{
    IEInitDisplayError( inst );
    ErrorMsgTitle = IEAllocRCString( WIE_ERRORMSGTITLE );
    if( ErrorMsgTitle == NULL ) {
        return( FALSE );
    }
    return( TRUE );

} /* IEInitErrors */
