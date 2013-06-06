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
* Description:  Status bar.
*
****************************************************************************/


#include "vi.h"
#include "ssbar.h"
#include "ssbardef.h"
#include "utils.h"
#include "subclass.h"
#include "wstatus.h"
#include "statwnd.h"
#include <assert.h>

#define NARRAY( a )             (sizeof( a ) / sizeof( a[0] ))

#define DEFAULT_STATUSSTRING    "Line:$5L$[Col:$3C$[Mode: $M$[$|$T$[$H"
#define DEFAULT_STATUSSECTIONS  { 60, 105, 192, 244 }

enum buttonType {
    BUTTON_CONTENT,
    BUTTON_ALIGNMENT,
};

HWND            hSSbar;
static bool     haveCapture = FALSE;
static int      curItemID = -1;
static HWND     mod_hwnd;

char *findBlockString( int block )
{
    char    *mod;
    int     i;

    i = 0;
    mod = EditVars.StatusString;
    while( i != block ) {
        while( *mod && *mod != '$' ) {
            mod++;
        }
        assert( *mod != '\0' );
        mod++;
        if( *mod == '[' ) {
            i++;
        }
        mod++;
    }
    return( mod );
}

void totalRedraw( void )
{
    StatusWndSetSeparatorsWithArray( EditVars.StatusSections, EditVars.NumStatusSections );
    UpdateStatusWindow();
    InvalidateRect( StatusWindow, NULL, TRUE );
    UpdateWindow( StatusWindow );
}

void destroyBlock( int i, char *start )
{
    char    new_ss[MAX_STR];

    if( EditVars.NumStatusSections == 1 ) {
        // unfortunately wstatus.c can't handle this right now
        // (it would be nice)
        MyBeep();
        return;
    }

    if( i != EditVars.NumStatusSections ) {
        memmove( EditVars.StatusSections + i, EditVars.StatusSections + i + 1, (EditVars.NumStatusSections - 1 - i) * sizeof( short ) );
    }
    EditVars.NumStatusSections--;
    EditVars.StatusSections = MemReAlloc( EditVars.StatusSections, EditVars.NumStatusSections * sizeof( short ) );

    strncpy( new_ss, EditVars.StatusString, start - EditVars.StatusString );
    new_ss[start - EditVars.StatusString] = '\0';
    while( start[0] && !(start[0] == '$' && start[1] == '[') ) {
        start++;
    }
    if( start[0] == '$' ) {
        start += 2;
        strcat( new_ss, start );
    }
    AddString2( &EditVars.StatusString, new_ss );

    totalRedraw();
}

void splitBlock( int i, char *start )
{
    char    new_ss[MAX_STR];
    int     diff;
    RECT    rect;

    if( EditVars.NumStatusSections == MAX_SECTIONS ) {
        MyBeep();
        return;
    }

    if( i == EditVars.NumStatusSections ) {
        GetWindowRect( StatusWindow, &rect );
        diff = rect.right - EditVars.StatusSections[i - 1];
    } else if( i == 0 ) {
        diff = EditVars.StatusSections[1];
    } else {
        diff = EditVars.StatusSections[i] - EditVars.StatusSections[i - 1];
    }

    if( diff < BOUNDARY_WIDTH * 4 ) {
        MyBeep();
        return;
    }
    EditVars.NumStatusSections++;
    EditVars.StatusSections = MemReAlloc( EditVars.StatusSections, EditVars.NumStatusSections * sizeof( short ) );
    memmove( EditVars.StatusSections + i + 1, EditVars.StatusSections + i, (EditVars.NumStatusSections - 1 - i) * sizeof( short ) );
    if( i > 0 ) {
        EditVars.StatusSections[i] = EditVars.StatusSections[i - 1] + (diff / 2);
    } else {
        EditVars.StatusSections[i] /= 2;
    }

    while( start[0] && !(start[0] == '$' && start[1] == '[') ) {
        start++;
    }
    strncpy( new_ss, EditVars.StatusString, start - EditVars.StatusString );
    new_ss[start - EditVars.StatusString] = '\0';
    strcat( new_ss, "$[ " );
    strcat( new_ss, start );
    AddString2( &EditVars.StatusString, new_ss );

    totalRedraw();
}

void buildNewItem( char *start, int id )
{
    char    new_ss[MAX_STR];
    char    *sz_content[] = { "$T", "$D", "Mode: $M",
                              "Line:$5L", "Col:$3C", "$H" };
    char    *sz_alignment[] = { "$<", "$|", "$>" };
    char    *new_item = "";
    int     type = 0;

    if( id >= SS_FIRST_CONTENT && id <= SS_LAST_CONTENT ) {
        new_item = sz_content[id - SS_FIRST_CONTENT];
        type = BUTTON_CONTENT;
    } else if( id >= SS_FIRST_ALIGNMENT && id <= SS_LAST_ALIGNMENT ) {
        new_item = sz_alignment[id - SS_FIRST_ALIGNMENT];
        type = BUTTON_ALIGNMENT;
    } else {
        assert( 0 );
    }

    strncpy( new_ss, EditVars.StatusString, start - EditVars.StatusString );
    new_ss[start - EditVars.StatusString] = '\0';
    strcat( new_ss, new_item );
    if( type == BUTTON_CONTENT ) {
        // only copy alignments, if any
        while( start[0] && !(start[0] == '$' && start[1] == '[') ) {
            if( start[0] == '$' && (start[1] == '<' || start[1] == '|'
                                                    || start[1] == '>') ) {
                strncat( new_ss, start, 2 );
                start++;
            }
            start++;
        }
    } else {
        // only copy contents, if any
        while( start[0] && !(start[0] == '$' && start[1] == '[') ) {
            if( start[0] == '$' && (start[1] == '<' || start[1] == '|'
                                                    || start[1] == '>') ) {
                start += 2;
                continue;
            }
            // by no means the most efficient way, but hardly matters
            strncat( new_ss, start, 1 );
            start++;
        }
    }
    strcat( new_ss, start );
    AddString2( &EditVars.StatusString, new_ss );

    totalRedraw();
}

void buildDefaults( void )
{
    short   def_sections[] = DEFAULT_STATUSSECTIONS;

    AddString2( &EditVars.StatusString, DEFAULT_STATUSSTRING );

    EditVars.NumStatusSections = NARRAY( def_sections );
    EditVars.StatusSections = MemReAlloc( EditVars.StatusSections, sizeof( def_sections ) );
    memcpy( EditVars.StatusSections, def_sections, sizeof( def_sections ) );

    totalRedraw();
}

void buildNewStatusString( int block, int id )
{
    char    *mod;

    mod = findBlockString( block );

    switch( id ) {
    case SS_SPLIT:
        splitBlock( block, mod );
        break;
    case SS_DESTROY:
        destroyBlock( block, mod );
        break;
    case SS_DEFAULTS:
        buildDefaults();
        break;
    default:
        buildNewItem( mod, id );
    }
}

static void sendNewItem( int x, int id )
{
    int     i;

    if( mod_hwnd == NULL ) {
        return;
    }
    assert( mod_hwnd == StatusWindow );

    i = 0;
    while( i < EditVars.NumStatusSections && EditVars.StatusSections[i] < x ) {
        i++;
    }

    assert( curItemID != -1 );
    buildNewStatusString( i, id );
}

static long processLButtonUp( HWND hwnd, LPARAM lparam )
{
    POINT   m_pt;
    if( haveCapture ) {
        MAKE_POINT( m_pt, lparam );
        ClientToScreen( hwnd, &m_pt );
        ScreenToClient( StatusWindow, &m_pt );
        sendNewItem( m_pt.x, curItemID );
        CursorOp( COP_ARROW );
        DrawRectangleUpDown( hwnd, DRAW_UP );
        ReleaseCapture();
        haveCapture = FALSE;
        curItemID = -1;
    }
    return( 0L );
}

static long processLButtonDown( HWND hwnd )
{
    DrawRectangleUpDown( hwnd, DRAW_DOWN );
    CursorOp( COP_DROPSS );
    SetCapture( hwnd );
    haveCapture = TRUE;
    curItemID = GetDlgCtrlID( hwnd );
    mod_hwnd = (HWND)NULLHANDLE;
    return( 0L );
}

static long processMouseMove( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    RECT    rect;
    POINT   m_pt;

    msg = msg;
    wparam = wparam;

    if( haveCapture == FALSE ) {
        return( 0L );
    }

    // check we aren't on ourselves first
    MAKE_POINT( m_pt, lparam );
    ClientToScreen( hwnd, &m_pt );
    GetWindowRect( GetParent( hwnd ), &rect );
    if( PtInRect( &rect, m_pt ) ) {
        CursorOp( COP_DROPSS );
        mod_hwnd = (HWND)NULLHANDLE;
        return( 0L );
    }

    // otherwise, figure out what we're over & set cursor based on that
    mod_hwnd = GetOwnedWindow( m_pt );
    if( mod_hwnd == StatusWindow ) {
        CursorOp( COP_DROPSS );
    } else {
        mod_hwnd = (HWND)NULLHANDLE;
        CursorOp( COP_NODROP );
    }
    return( 0L );
}

WINEXPORT long CALLBACK StaticSubclassProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    switch( msg ) {
    case WM_NCHITTEST:
        return( HTCLIENT );
    case WM_MOUSEMOVE:
        return( processMouseMove( hwnd, msg, wparam, lparam ) );
    case WM_LBUTTONDOWN:
        return( processLButtonDown( hwnd ) );
    case WM_LBUTTONUP:
        return( processLButtonUp( hwnd, lparam ) );
    }
    return( CallWindowProc( SubclassGenericFindOldProc( hwnd ), hwnd, msg, wparam, lparam ) );
}

void addSubclasses( HWND hwnd )
{
    int     i;
    for( i = SS_FIRST_CONTENT; i <= SS_LAST_CONTENT; i++ ) {
        SubclassGenericAdd( GetDlgItem( hwnd, i ), (WNDPROC)StaticSubclassProc );
    }
    for( i = SS_FIRST_ALIGNMENT; i <= SS_LAST_ALIGNMENT; i++ ) {
        SubclassGenericAdd( GetDlgItem( hwnd, i ), (WNDPROC)StaticSubclassProc );
    }
    for( i = SS_FIRST_COMMAND; i <= SS_LAST_COMMAND; i++ ) {
        SubclassGenericAdd( GetDlgItem( hwnd, i ), (WNDPROC)StaticSubclassProc );
    }
}

void removeSubclasses( HWND hwnd )
{
    int     i;
    for( i = SS_FIRST_CONTENT; i <= SS_LAST_CONTENT; i++ ) {
        SubclassGenericRemove( GetDlgItem( hwnd, i ) );
    }
    for( i = SS_FIRST_ALIGNMENT; i <= SS_LAST_ALIGNMENT; i++ ) {
        SubclassGenericRemove( GetDlgItem( hwnd, i ) );
    }
    for( i = SS_FIRST_COMMAND; i <= SS_LAST_COMMAND; i++ ) {
        SubclassGenericRemove( GetDlgItem( hwnd, i ) );
    }
}

/*
 * SSDlgProc - callback routine for status bar settings drag & drop dialog
 */
WINEXPORT BOOL CALLBACK SSDlgProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    lparam = lparam;
    wparam = wparam;
    hwnd = hwnd;

    switch( msg ) {
    case WM_INITDIALOG:
        hSSbar = hwnd;
        MoveWindowTopRight( hwnd );
        addSubclasses( hwnd );
        return( TRUE );
    case WM_CLOSE:
        removeSubclasses( hwnd );
        hSSbar = (HWND)NULLHANDLE;
        // update editflags (may have closed from system menu)
        EditFlags.SSbar = FALSE;
        DestroyWindow( hwnd );
        break;
    }
    return( FALSE );

} /* SSDlgProc */

/*
 * RefreshSSbar - turn status settings bar on/off
 */
void RefreshSSbar( void )
{
    static DLGPROC      proc;

    if( EditFlags.SSbar ) {
        if( hSSbar != NULL ) {
            return;
        }
        proc = (DLGPROC)MakeProcInstance( (FARPROC)SSDlgProc, InstanceHandle );
        hSSbar = CreateDialog( InstanceHandle, "SSBAR", Root, proc );
    } else {
        if( hSSbar == NULL ) {
            return;
        }
        SendMessage( hSSbar, WM_CLOSE, 0, 0L );
        FreeProcInstance( (FARPROC)proc );
    }
    UpdateStatusWindow();

} /* RefreshSSbar */
