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
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "win.h"
/*
 * _SetMyDC - set display context for windows
 */
HFONT _SetMyDC( HDC dc, DWORD bkclr, DWORD txtclr )
{
    SetBkColor( dc, bkclr );
    SetTextColor( dc, txtclr );
    return( SelectObject( dc, _FixedFont ) );

} /* _SetMyDC */

/*
 * _MessageLoop
*/
int _MessageLoop( BOOL doexit )
{
    MSG         msg;
    WORD        rc=1;

    while( PeekMessage( &msg, (HWND)NULL, 0, 0, PM_NOYIELD | PM_NOREMOVE ) ) {
        rc = GetMessage( &msg, (HWND)NULL, 0, 0 );
        if( !rc ) {
            if( doexit ) {
                _WindowsExitRtn = NULL;
                exit( msg.wParam );
            }
            break;
        }
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }
    #if defined(__NT__)
        // Yield() is obsolete under Win32
        Sleep(0);
    #else
        Yield();
    #endif
    return( rc );

} /* _MessageLoop */

/*
 * _BlockingMessageLoop
*/
int _BlockingMessageLoop( BOOL doexit )
{
    MSG         msg;
    WORD        rc=1;

    rc = GetMessage( &msg, (HWND)NULL, 0, 0 );
    if( !rc ) {
        if( doexit ) {
            _WindowsExitRtn = NULL;
            exit( msg.wParam );
        }
    } else {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }
    return( _MessageLoop( doexit ) );

} /* _BlockingMessageLoop */

/*
 * _OutOfMemory - display out of memory message
 */
void _OutOfMemory( void )
{
    MessageBox( (HWND)NULL, "Out Of Memory!", "SYSTEM ERROR", MB_OK );

} /* _OutOfMemory */

/*
 * _ExecutionComplete - display execution complete message
 */
void _ExecutionComplete( void )
{
    flushall();
//    MessageBox( NULL, "Execution Complete", "", MB_OK | MB_ICONEXCLAMATION );

} /* _ExecutionComplete */

int     _SetConTitle( LPWDATA w, char *title ) {
//==============================================

    SetWindowText( w->hwnd, title );
    return( 1 );
}

int     _SetAppTitle( char *title ) {
//===================================

    SetWindowText( _MainWindow, title );
    return( 1 );
}

int     _ShutDown( void ) {
//=========================

    flushall();
    DestroyWindow( _MainWindow );
    _MainWindowDestroyed = 1;
    return( 0 );
}


int     _CloseWindow( LPWDATA w ) {
//=================================

    if( w->destroy ) {
        _DestroyAWindow( w );
        DestroyWindow( w->hwnd );
    }
    return( 0 );
}


/*
 * _NewCursor - change the cursor type
 */
void _NewCursor( LPWDATA w, cursors type )
{
    if( w->hascursor ) {
        DestroyCaret();
        w->hascursor = FALSE;
    }
    if( type == KILL_CURSOR ) return;
    w->CaretType = type;
    switch( type ) {
    case SMALL_CURSOR:
        CreateCaret( w->hwnd, (HBITMAP)NULL, 0, w->ychar );
        w->hascursor = TRUE;
        break;
    case FAT_CURSOR:
        CreateCaret( w->hwnd, (HBITMAP)NULL, 4, w->ychar );
        w->hascursor = TRUE;
        break;
    case ORIGINAL_CURSOR:
        break;
    }

} /* _NewCursor */

/*
 * _DisplayCursor - show the current cursor position
 */
void _DisplayCursor( LPWDATA w )
{
    HDC                 dc;
    SIZE                size;

    dc = GetDC( w->hwnd );
    SelectObject( dc, _FixedFont );
    #ifdef _MBCS
        #ifdef __NT__
            GetTextExtentPoint32( dc, w->tmpbuff->data,
                                  FAR_mbsnbcnt(w->tmpbuff->data,w->buffoff+w->curr_pos),
                                  &size );
        #else
            GetTextExtentPoint( dc, w->tmpbuff->data,
                                FAR_mbsnbcnt(w->tmpbuff->data,w->buffoff+w->curr_pos),
                                &size );
        #endif
    #else
        #ifdef __NT__
            GetTextExtentPoint32( dc, w->tmpbuff->data, w->buffoff+w->curr_pos,
                                  &size );
        #else
            GetTextExtentPoint( dc, w->tmpbuff->data, w->buffoff+w->curr_pos,
                                &size );
        #endif
    #endif
    SetCaretPos( size.cx+1, (w->LastLineNumber-w->TopLineNumber)*w->ychar );
    ReleaseDC( w->hwnd, dc );
    ShowCaret( w->hwnd );

} /* _DisplayCursor */

/*
 * _SetInputMode - set whether or not we are in input mode
 */
void _SetInputMode( LPWDATA w, BOOL val  )
{
    WORD cmd;

    w->InputMode = val;
    if( w->InputMode ) cmd = MF_GRAYED;
    else cmd = MF_ENABLED;
    EnableMenuItem( _SubMenuEdit, MSG_FLUSH, cmd );

} /* _SetInputMode */

/*
 * _ShowWindowActive - do windows-specific stuff to make a window active
 */
void _ShowWindowActive( LPWDATA w, LPWDATA last )
{
    if( last != NULL ) {
        CheckMenuItem( _SubMenuWindows, MSG_WINDOWS+last->handles[0],
                 MF_UNCHECKED | MF_BYCOMMAND );
        SendMessage( last->hwnd, WM_NCACTIVATE, FALSE, 0L );
        if( last->CaretType != ORIGINAL_CURSOR ) {
            _NewCursor( last, KILL_CURSOR );
        }
    }
    if( w != NULL ) {
        CheckMenuItem( _SubMenuWindows, MSG_WINDOWS+w->handles[0],
                 MF_CHECKED | MF_BYCOMMAND );
        ShowWindow( w->hwnd, SW_SHOW );
        SendMessage( w->hwnd, WM_NCACTIVATE, TRUE, 0L );
        BringWindowToTop( w->hwnd );
        if( w->CaretType != ORIGINAL_CURSOR ) {
            _NewCursor( w, w->CaretType );
            _DisplayCursor( w );
        }
    }

} /* _ShowWindowActive */
