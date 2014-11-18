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
* Description:  Main debugger window.
*
****************************************************************************/


#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbgwind.h"
#include "dbgerr.h"
#include "dbgmem.h"
#include <string.h>


extern unsigned int     OptExpr( unsigned int );
extern void             ReqEOC( void );
extern void             Scan( void );
extern unsigned int     ScanCmd( char * );
extern bool             ScanEOC( void );
extern unsigned         NewCurrRadix( unsigned );
extern char             *Format( char *, char *, ... );
extern void             ConfigLine( char * );
extern char             *StrCopy( char *, char * );
extern char             *GetCmdEntry( char *, int, char * );
extern bool             WndDlgTxt( char * );
extern wnd_class        ReqWndName( void );
extern void             WndToolOpen( gui_ord, bool );
extern void             WndToolClose( void );
extern char             *DupStrLen( char *, unsigned );
extern bool             ScanItem( bool, char **, size_t * );
extern char             *GetCmdName( int );
extern void             PushInpStack( void *, bool (*rtn)( void *, inp_rtn_action ), bool );
extern void             TypeInpStack( input_type );
extern bool             HookPendingPush( void );

extern a_window         *WndMain;
extern gui_coord        WndScreen;
extern gui_coord        WndMax;
extern gui_coord        WndScale;
extern tokens           CurrToken;
extern char             *TxtBuff;
extern char             WndNameTab[];
extern gui_colour_set   WndColours[];
extern gui_colour_set   WndStatusColour;

extern WNDOPEN            *WndOpenTab[];

wnd_posn        WndPosition[ WND_NUM_CLASSES ];
static char     *WndFontInfo[ WND_NUM_CLASSES ];
gui_rect        WndMainRect;

static char     DispOptions[] =
{
    "Open\0"
    "Close\0"
    "New\0"
    "MInimize\0"
    "MAximize\0"
    "Restore\0"
    "FLoating\0"
    "Fixed\0"
};

enum {
    OPEN = 1,
    CLOSE,
    NEW,
    MINIMIZE,
    MAXIMIZE,
    RESTORE,
    FLOATING,
    FIXED
};

static char     MiscTab[] = { "TOolbar\0Status\0" };

enum {
    MISC_TOOL = 1,
    MISC_STATUS
};

bool ScanStatus()
{
    return( ScanCmd( MiscTab ) == MISC_STATUS );
}

static int      range( int x, int min_x, int max_x, int default_x )
{
    if( x == -1 ) x = default_x;
    if( x < min_x ) x = min_x;
    if( x > max_x ) x = max_x;
    return( x );
}

static char     GetOption( void )
{
    char        optn;

    if( CurrToken == T_DIV ) {
        Scan();
        optn = ScanCmd( DispOptions );
        if( optn == 0 ) {
            Error( ERR_LOC, LIT( ERR_BAD_OPTION ), GetCmdName( CMD_DISPLAY ) );
        }
    } else {
        optn = OPEN;
    }
    return( optn );
}


extern  char    *GetWndFont( a_window *wnd )
{
    if( WndHasClass( wnd ) ) {
        if( WndFontInfo[ WndClass( wnd ) ] != NULL ) {
            return( WndFontInfo[ WndClass( wnd ) ] );
        }
    }
    return( WndFontInfo[ WND_ALL ] );
}


static void SetFont( wnd_class class, char *font )
{
    GUIMemFree( WndFontInfo[ class ] );
    WndFontInfo[ class ] = font;
}


extern  void    WndFontHook( a_window *wnd )
{
    char        *font;

    if( !WndHasClass( wnd ) ) return;
    font = WndGetFontInfo( wnd );
    if( font != NULL ) {
        SetFont( WndClass( wnd ), font );
    }
}


gui_coord *WndMainClientSize( void )
{
    return( _IsOn( SW_DETACHABLE_WINDOWS ) ? &WndScale : &WndMax );
}

void WndMainResized( void )
{
    a_window    *wnd;
    gui_rect    rect;

    if( _IsOn( SW_DETACHABLE_WINDOWS ) ) return;
    WndGetRect( WndMain, &WndMainRect );
    for( wnd = WndNext( NULL ); wnd != NULL; wnd = WndNext( wnd ) ) {
        if( !WndHasClass( wnd ) ) continue;
        WndPosToRect( &WndPosition[ WndClass( wnd ) ], &rect, WndMainClientSize() );
        WndResizeWindow( wnd, &rect );
    }
}


extern  void    WndResizeHook( a_window *wnd )
{
    gui_rect    rect;

    if( WndIsMaximized( wnd ) || WndIsMinimized( wnd ) ) return;
    if( !WndHasClass( wnd ) ) return;
    WndGetRect( wnd, &rect );
    WndRectToPos( &rect, &WndPosition[ WndClass( wnd ) ], WndMainClientSize() );
}


void InitFont( void )
{
    WndFontInfo[ WND_ALL ] = WndGetFontInfo( WndMain );
}


void FiniFont( void )
{
    int         i;

    for( i = 0; i < WND_NUM_CLASSES; ++i ) {
        SetFont( i, NULL );
    }
}

static char *GUIDupStrLen( char *str, unsigned len )
{
    char        *dup;

    dup = GUIMemAlloc( len + 1 );
    if( dup != NULL ) {
        memcpy( dup, str, len );
        dup[len] = '\0';
    }
    return( dup );
}


void ProcFont( void )
{
    wnd_class class;
    char        *start;
    size_t      len;
    int         i;

    class = ReqWndName();
    if( !ScanItem( FALSE, &start, &len ) ) return;
    ReqEOC();
    if( class == WND_ALL ) {
        for( i = 0; i < WND_NUM_CLASSES; ++i ) {
            SetFont( i, NULL );
        }
    }
    SetFont( class, GUIDupStrLen( start, len ) );
    _SwitchOn( SW_PENDING_REPAINT );
}



static void PrintFont( wnd_class class, char *def )
{
    char        wndname[ 20 ];
    char        *font;

    font = WndFontInfo[ class ];
    if( font == NULL ) return;
    if( def == NULL || strcmp( font, def ) != 0 ) {
        GetCmdEntry( WndNameTab, class+1, wndname );
        Format( TxtBuff, "%s %s {%s}", GetCmdName( CMD_FONT ), wndname, font );
        WndDlgTxt( TxtBuff );
    }
}


void ConfigFont( void )
{
    int         class;

    PrintFont( WND_ALL, NULL );
    for( class = 0; class < WND_NUM_CLASSES; ++class ) {
        if( class == WND_ALL ) continue;
        PrintFont( class, WndFontInfo[ WND_ALL ] );
    }
}


void FontChange( void )
{
    int         i;
    char        *text;

    text = GUIGetFontFromUser( WndFontInfo[ WND_ALL ] );
    if( text == NULL ) return;
    for( i = 0; i < WND_NUM_CLASSES; ++i ) {
        SetFont( i, NULL );
    }
    SetFont( WND_ALL, text );
    _SwitchOn( SW_PENDING_REPAINT );
}


static void ProcSize( int wnd_num )
{
    gui_rect    size;
    char        optn;
    a_window    *wnd;
    a_window    *next;
    gui_coord   min;
    bool        coord_specified;
    gui_rect    def_rect;

    optn = GetOption();
    if( optn == FLOATING || optn == FIXED ) {
        Error( ERR_LOC, LIT( ERR_BAD_OPTION ), GetCmdName( CMD_DISPLAY ) );
    }
    size.x = OptExpr( -1 );
    size.y = -1;
    size.width = -1;
    size.height = -1;
    if( CurrToken == T_COMMA ) {
        Scan();
        size.y = OptExpr( -1 );
        if( CurrToken == T_COMMA ) {
            Scan();
            size.width = OptExpr( -1 );
            if( CurrToken == T_COMMA ) {
                Scan();
                size.height = OptExpr( -1 );
            }
        }
    }
    ReqEOC();

    GUIGetMinSize( &min );
    if( size.x != -1 || size.y != -1 || size.width != -1 || size.height != -1 ) {
        coord_specified = TRUE;
    } else {
        coord_specified = FALSE;
    }
    WndPosToRect( &WndPosition[ wnd_num ], &def_rect, &WndScreen );
    size.x = range( size.x, 0, WndScreen.x, def_rect.x );
    size.y = range( size.y, 0, WndScreen.y, def_rect.y );
    if( size.x + size.width > WndScreen.x ) size.width = WndScreen.x - size.x;
    if( size.y + size.height > WndScreen.y ) size.height = WndScreen.y - size.y;
    size.width = range( size.width, min.x, WndScreen.x, def_rect.width );
    size.height = range( size.height, min.y, WndScreen.y, def_rect.height );
    if( coord_specified ) {
        WndRectToPos( &size, &WndPosition[ wnd_num ], &WndScreen );
    }
    if( wnd_num == WND_CURRENT ) {
        wnd = WndFindActive();
    } else {
        wnd = WndFindClass( NULL, wnd_num );
    }
    switch( optn ) {
    case OPEN:
    case NEW:
        if( optn == NEW || wnd == NULL ) {
            WndOpenTab[ wnd_num ]();
        } else {
            WndRestoreToFront( wnd );
            WndPosToRect( &WndPosition[ wnd_num ], &size, WndMainClientSize() );
            WndResizeWindow( wnd, &size );
        }
        break;
    case CLOSE:
        if( wnd_num == WND_ALL ) {
            for( wnd = WndNext( NULL ); wnd != NULL; wnd = next ) {
                next = WndNext( wnd );
                if( WndHasClass( wnd ) ) WndClose( wnd );
            }
        } else {
            if( wnd != NULL ) {
                WndClose( wnd );
            }
        }
        break;
    case MINIMIZE:
        if( wnd == NULL ) wnd = WndOpenTab[ wnd_num ]();
        WndMinimizeWindow( wnd );
        break;
    case MAXIMIZE:
        if( wnd == NULL ) wnd = WndOpenTab[ wnd_num ]();
        WndMaximizeWindow( wnd );
        break;
    case RESTORE:
        if( wnd == NULL ) wnd = WndOpenTab[ wnd_num ]();
        WndRestoreWindow( wnd );
        break;
    }
}


static void ProcTool( void )
{
    int         height;
    char        optn,type,tmp;

    type = FIXED;
    optn = OPEN;
    while( CurrToken == T_DIV ) {
        tmp = GetOption();
        if( tmp == FLOATING || tmp == FIXED ) {
            type = tmp;
        } else {
            optn = tmp;
        }
    }
    height = OptExpr( 0 );
    if( optn == CLOSE ) {
        WndToolClose();
    } else {
        WndToolOpen( height, type == FIXED );
    }
}


static void ProcStatus( void )
{
    char        optn;

    optn = GetOption();
    if( optn == OPEN ) {
        WndCreateStatusWindow( &WndStatusColour );
    } else if( optn == CLOSE ) {
        WndCloseStatusWindow();
    } else {
        Error( ERR_LOC, LIT( ERR_BAD_OPTION ), GetCmdName( CMD_DISPLAY ) );
    }
}


OVL_EXTERN bool DoneRefresh( void *dummy, inp_rtn_action action )
{
    dummy = dummy;
    switch( action ) {
    case INP_RTN_INIT:
        return( TRUE );
    case INP_RTN_EOL:
        WndFreshAll();
        WndDirty( NULL );
        return( FALSE );
    case INP_RTN_FINI:
        return( TRUE );
    default:
        return( FALSE );
    }
}


static void PushRefresh( void )
{
    PushInpStack( NULL, DoneRefresh, FALSE );
    TypeInpStack( INP_NO_CMD );
}

/*
 * ProcDisplay -- process a display command
 */

void ProcDisplay()
{
    wnd_class   class;
    unsigned    old;

    if( ScanEOC() ) {
        while( HookPendingPush() );
        PushRefresh();
        return;
    }
    old = NewCurrRadix( 10 );
    switch( ScanCmd( MiscTab ) ) {
    case MISC_TOOL:
        ProcTool();
        break;
    case MISC_STATUS:
        ProcStatus();
        break;
    default:
        class = ReqWndName();
        ProcSize( class );
    }
    NewCurrRadix( old );
}


static  void    PrintPosition( int option, int class,
                               gui_rect *rect, char *buff, char *buff2 )
{
    GetCmdEntry( WndNameTab, class+1, buff );
    GetCmdEntry( DispOptions, option, buff2 );
    Format( TxtBuff, "%s %s /%s %d,%d,%d,%d", GetCmdName( CMD_DISPLAY ), buff, buff2,
                     rect->x, rect->y, rect->width, rect->height );
    WndDlgTxt( TxtBuff );
}


void ConfigDisp( void )
{

    a_window    *wnd, *scan;
    char        buff[20];
    char        buff2[20];
    a_window    *head, *next;
    int         h;
    int         class;
    gui_rect    rect;

    ReqEOC();
    GetCmdEntry( WndNameTab, WND_ALL+1, buff );
    GetCmdEntry( DispOptions, CLOSE, buff2 );
    Format( TxtBuff, "%s %s /%s", GetCmdName( CMD_DISPLAY ), buff, buff2 );
    WndDlgTxt( TxtBuff );
    if( WndHaveStatusWindow() ) {
        GetCmdEntry( MiscTab, MISC_STATUS, buff );
        Format( TxtBuff, "%s %s", GetCmdName( CMD_DISPLAY ), buff );
        WndDlgTxt( TxtBuff );
    }
    if( WndHaveToolBar() ) {
        h = WndToolHeight();
        GetCmdEntry( MiscTab, MISC_TOOL, buff );
        GetCmdEntry( DispOptions, WndToolFixed() ? FIXED : FLOATING, buff2 );
        Format( TxtBuff, "%s %s /%s %d", GetCmdName( CMD_DISPLAY ),
                buff, buff2, h );
        WndDlgTxt( TxtBuff );
    }
    for( class = 0; class < WND_NUM_CLASSES; ++class ) {
        if( class == WND_ALL ) continue;
        if( WndFindClass( NULL, class ) != NULL ) continue;
        WndPosToRect( &WndPosition[ class ], &rect, &WndScreen );
        if( rect.width == 0 ) continue;
        if( rect.height == 0 ) continue;
        PrintPosition( CLOSE, class, &rect, buff, buff2 );
    }
    head = WndNext( NULL );
    if( head == NULL ) return;
    // traverse in reverse order so that windows get created in correct order
    wnd = head;
    for( ;; ) {
        next = WndNext( wnd );
        if( next == NULL ) break;
        wnd = next;
    }
    for( ;; ) {
        class = WndClass( wnd );
        switch( class ) {
        case WND_NO_CLASS:
        case WND_ALL:
            break;
        case WND_BINARY:
        case WND_FILE:
        case WND_MEMORY:
            WndResizeHook( wnd );
            WndPosToRect( &WndPosition[ class ], &rect, &WndScreen );
            PrintPosition( CLOSE, class, &rect, buff, buff2 );
            break;
        case WND_VARIABLE:
        case WND_TMPFILE:
            break;
        default:
            WndResizeHook( wnd );
            WndPosToRect( &WndPosition[ class ], &rect, &WndScreen );
            PrintPosition( OPEN, class, &rect, buff, buff2 );
            break;
        }
        if( wnd == head ) break;
        scan = head;
        for( ;; ) {
            next = WndNext( scan );
            if( next == wnd ) break;
            scan = next;
        }
        wnd = scan;
    }
}
