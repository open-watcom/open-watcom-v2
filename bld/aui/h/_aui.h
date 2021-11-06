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
* Description:  Internal interface for AUI library.
*
****************************************************************************/


#ifndef __AUI_H_INCLUDED
#define __AUI_H_INCLUDED

#include "aui.h"
#include "auistr.h"

#define WNDEVENT( w, e, p )     (w)->info->event( w, e, p )

#define ArraySize( x )          (sizeof( x ) / sizeof( *(x) ))

#define UCHAR_VALUE( c )        (unsigned char)(c)

extern void             WndLitInit( void );
extern void             WndLitFini( void );

extern void             WndCoordToGUIPoint( a_window, wnd_coord *, gui_point * );

extern void             WndSelPieceChange( a_window wnd, wnd_coord *piece );
extern bool             WndSelGetEndPiece( a_window wnd, void *parm, wnd_coord *piece );
extern bool             WndSelSetEnd( a_window, void * );
extern bool             WndSelSetStart( a_window, void * );
extern void             WndSelEnds( a_window, wnd_coord **, wnd_coord ** );
extern bool             WndSelected( a_window, wnd_line_piece *, wnd_row, wnd_piece, wnd_colidx *, size_t * );
extern void             WndSelChange( a_window wnd, void *parm );
extern void             WndAdjustDirty( a_window wnd, int by );
extern wnd_row          WndSelRow( a_window );
extern bool             WndPieceHighlights( a_window, int, int );
extern void             WndNullPopItem( a_window wnd );
extern void             WndRowPopUp( a_window, const gui_menu_struct *, wnd_row, wnd_piece );
extern void             WndKeyPopItem( a_window wnd, bool );
extern void             WndSelPopItem( a_window wnd, void *, bool );
extern bool             WndSetPoint( a_window, void *, bool, wnd_coord *, wnd_row, bool );
extern bool             WndHasCurrent( a_window );
extern bool             WndNextCurrent(a_window,bool);
extern bool             WndPrevCurrent(a_window,bool);
extern bool             WndSetCurrent(a_window,void*);
extern void             WndCurrVisible( a_window wnd );
extern void             WndHScrollToCurr( a_window , size_t );
extern void             WndSetHScroll( a_window, gui_ord );
extern void             WndHScrollNotify( a_window );
extern bool             WndAtTop( a_window );
extern void             WndResize( a_window );
extern void             WndDoneChoose( a_window );
extern bool             WndKeyEscape( a_window );
extern bool             WndKeyRubOut( a_window );
extern bool             WndKeyChoose( a_window, int ch );
extern void             WndSayMatchMode( a_window );
extern a_window         WndNoOpen( void );
extern int              WndNextId( void );
extern void             WndFreeId( int i );
extern bool             WndPieceIsHot( a_window wnd, wnd_row row, wnd_piece piece );
extern bool             WndPieceIsTab( a_window wnd, wnd_row row, wnd_piece piece );
extern WNDGETLINE       WndGetLineAbs;
extern void             WndSetWndMax( void );
extern void             WndSetToolBar( gui_event gui_ev );
extern void             WndPaintDirty(a_window);
extern void             WndDirtyCurrChar(a_window);
extern void             WndDirtyScreenChar( a_window wnd, wnd_coord *piece );
extern void             WndDirtyScreenRange( a_window wnd, wnd_coord *piece, wnd_colidx end_colidx );
extern void             WndDirtyScreenPiece( a_window wnd, wnd_coord *piece );
extern void             WndDirtyScreenRow( a_window wnd, wnd_row row );
extern void             WndNextNonIconToFront( a_window wnd );
extern void             WndActive( a_window wnd );
extern void             WndProcPaint( a_window wnd, void *parm );
extern bool             WndIgnoreMouseMove( a_window wnd );
extern void             WndMouseMove( a_window wnd, void *parm );
extern void             WndRButtonUp( a_window wnd, void *parm );
extern void             WndLDblClk( a_window wnd, void *parm );
extern wnd_row          WndGetMouseRow( void );
extern void             WndRButtonDown( a_window wnd, void *parm );
extern void             WndLButtonUp( a_window wnd, void *parm );
extern void             WndLButtonDown( a_window wnd, void *parm );
extern void             WndToSelectMode( a_window wnd );
extern void             WndChooseEvent( a_window wnd, gui_event gui_ev, void *parm );
extern void             WndSelectEvent( a_window wnd, gui_event gui_ev, void *parm );
extern void             WndEndSelectEvent( a_window wnd );
extern void             WndSetCurrCol(a_window wnd);
extern void             WndFiniCacheLines( void );
extern void             WndKillCacheLines( a_window wnd );
extern void             WndKillCacheEntries( a_window wnd, wnd_row row, wnd_piece piece );
extern void             WndCheckCurrentValid( a_window wnd );
extern bool             WndSetCache( a_window wnd, bool on );
extern void             WndAddPopupMenu( a_window wnd );
extern void             WndClick( a_window wnd, gui_ctl_id id );
extern void             WndSetPopup( gui_ctl_id id );
extern bool             WndMouseButtonIsDown( void );
extern bool             WndValid(a_window);
extern void             WndInvokePopUp( a_window wnd, gui_point *point, const gui_menu_struct *menu );
extern const char       *WndPrevChar( const char *buff, const char *curr );
extern wnd_colidx       WndPrevCharColIdx( const char *buff, wnd_colidx colidx );
extern wnd_colidx       WndLastCharColIdx( wnd_line_piece *line );
extern wnd_colidx       WndCharColIdx( const char *buff, wnd_colidx colidx );

extern bool             WndDoingRefresh;
extern gui_menu_items   *WndMainMenuPtr;
extern gui_menu_struct  *WndPopupMenuPtr;

extern bool             WndHaveUserStatusText;
extern bool             WndInternalStatusText( const char * );
extern void             WndResetStatusText( void );
extern void             WndFiniStatusText( void );

extern void             SetWndMenuRow( a_window wnd );
extern wnd_row          WndMenuRow;
extern wnd_piece        WndMenuPiece;

#define WndVirtualRow( wnd, row ) \
        ( ( row ) == WND_NO_ROW ) ? \
        ( row ) : ( ( row ) + ( wnd )->top - ( wnd )->title_rows )

#define WndScreenRow( wnd, row ) \
        ( ( row ) == WND_NO_ROW ) ? \
        ( row ) : ( ( row ) - ( wnd )->top + ( wnd )->title_rows )

#define WndVirtualTop( wnd )    WndVirtualRow( wnd, wnd->title_rows )
#define WndVirtualBottom( wnd ) WndVirtualRow( wnd, wnd->rows )

#endif // __AUI_H_INCLUDED
