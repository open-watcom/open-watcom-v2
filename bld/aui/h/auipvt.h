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
* Description:  WHEN YOU FIGURE OUT WHAT THIS MODULE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#ifndef _AUIPVT_H_INCLUDED
#define _AUIPVT_H_INCLUDED

#include "aui.h"
#include "auistr.h"

extern void             WndLitInit();
extern void             WndLitFini();

extern  void            WndCoordToGUIPoint(a_window*,wnd_coord*,gui_point*);

extern void             WndSelPieceChange( a_window *wnd, wnd_coord *piece );
extern bool             WndSelGetEndPiece( a_window *wnd, void *parm, wnd_coord *piece );
extern bool             WndSelSetEnd(a_window *,void*);
extern bool             WndSelSetStart(a_window *,void*);
extern void             WndSelEnds(a_window *,wnd_coord **,wnd_coord **);
extern bool             WndSelected(a_window *,wnd_line_piece*,wnd_row ,int ,int *,int *);
extern void             WndSelChange( a_window *wnd, void *parm );
extern void             WndAdjustDirt( a_window *wnd, int by );
extern wnd_row          WndSelRow(a_window *);
extern bool             WndPieceHighlights(a_window *,int ,int );
extern void             WndNullPopItem( a_window *wnd );
extern void             WndRowPopUp(a_window *,gui_menu_struct *,int,int);
extern void             WndKeyPopItem( a_window *wnd,bool);
extern void             WndSelPopItem( a_window *wnd,void*,bool);
extern bool             WndSetPoint(a_window *,void *,bool,wnd_coord *,wnd_row,bool);
extern bool             WndHasCurrent(a_window *);
extern bool             WndNextCurrent(a_window *,bool);
extern bool             WndPrevCurrent(a_window *,bool);
extern bool             WndSetCurrent(a_window *,void*);
extern void             WndCurrVisible( a_window *wnd );
extern void             WndHScrollToCurr(a_window *,int);
extern void             WndSetHScroll(a_window *,gui_ord);
extern void             WndHScrollNotify(a_window *);
extern bool             WndAtTop(a_window *);
extern void             WndChoose(int );
extern void             WndResize(a_window *);
extern void             WndDoneChoose(a_window *);
extern bool             WndKeyEscape(a_window *);
extern bool             WndKeyRubOut(a_window *);
extern bool             WndKeyChoose(a_window *,unsigned int );
extern void             WndSayMatchMode(a_window *);
extern a_window         *WndNoOpen(void);
extern void             WndReshow(void);
extern  int             WndNextId();
extern  void            WndFreeId( int i );
extern  bool            WndPieceIsHot( a_window *wnd, int row, int piece );
extern  bool            WndPieceIsTab( a_window *wnd, int row, int piece );
extern  WNDGETLINE      WndGetLineAbs;
extern void             WndSetWndMax(void);
extern void             WndSetToolBar( gui_event event );
extern void             WndPaintDirt(a_window*);
extern void             WndDirtyCurrChar(a_window*);
extern void             WndDirtyCurr(a_window*);
extern void             WndDirtyScreenChar( a_window *wnd, wnd_coord *piece );
extern void             WndDirtyScreenRange( a_window *wnd, wnd_coord *piece, int end_col );
extern void             WndDirtyScreenPiece( a_window *wnd, wnd_coord *piece );
extern void             WndDirtyScreenRow( a_window *wnd, wnd_row row );
extern void             WndNextNonIconToFront( a_window *wnd );
extern void             WndActive( a_window *wnd );
extern void             WndProcPaint( a_window *wnd, void *parm );
extern bool             WndIgnoreMouseMove( a_window *wnd );
extern void             WndMouseMove( a_window *wnd, void *parm );
extern void             WndRButtonUp( a_window *wnd, void *parm );
extern void             WndLDblClk( a_window *wnd, void *parm );
extern void             WndRButtonDown( a_window *wnd, void *parm );
extern void             WndLButtonUp( a_window *wnd, void *parm );
extern void             WndLButtonDown( a_window *wnd, void *parm );
extern void             WndToSelectMode( a_window *wnd );
extern void             WndChooseEvent( a_window *wnd, gui_event event, void *parm );
extern void             WndSelectEvent( a_window *wnd, gui_event event, void *parm );
extern void             WndEndSelectEvent( a_window *wnd );
extern void             WndSetCurrCol(a_window *wnd);
extern void             WndFiniCacheLines();
extern void             WndKillCacheLines( a_window *wnd );
extern void             WndKillCacheEntries( a_window *wnd, int row, int piece );
extern void             WndCheckCurrentValid( a_window *wnd );
extern bool             WndSetCache( a_window *wnd, bool on );
extern void             WndAddPopupMenu( a_window *wnd );
extern void             WndClick( a_window *wnd, unsigned id );
extern void             WndSetPopup();
extern bool             WndMouseButtonIsDown();
extern bool             WndValid(a_window*);
extern void             WndInvokePopUp( a_window *wnd, gui_point *point, gui_menu_struct *menu );
extern char             *WndPrevChar( char *buff, char *curr );
extern int              WndPrevCharCol( char *buff, int idx );
extern int              WndLastCharCol( wnd_line_piece *line );
extern int              WndCharCol( char *buff, int col );


extern bool             WndDoingRefresh;
extern a_window         *WndMain;
extern gui_menu_struct  *WndMainMenuPtr;
extern gui_menu_struct  *WndPopupMenuPtr;
extern int              WndNumMenus;

extern bool             WndHaveUserStatusText;
extern bool             WndInternalStatusText( char * );
extern void             WndResetStatusText();
extern void             WndFiniStatusText();

extern void             SetWndMenuRow( a_window *wnd );
extern wnd_row          WndMenuRow;
extern int              WndMenuPiece;

#define _Is( wnd, x )    ( ( (wnd)->switches & (x) ) != 0 )
#define _Isnt( wnd, x )  ( ( (wnd)->switches & (x) ) == 0 )
#define _Set( wnd, x )   (wnd)->switches |= (x)
#define _Clr( wnd, x )   (wnd)->switches &= ~(x)

#define WndVirtualRow( wnd, row ) \
        ( ( row ) == WND_NO_ROW ) ? \
        ( row ) : \
        ( ( row ) + ( wnd )->top - ( wnd )->title_size )

#define WndScreenRow( wnd, row ) \
        ( ( row ) == WND_NO_ROW ) ? \
        ( row ) : \
        ( ( row ) - ( wnd )->top + ( wnd )->title_size )

#define WndVirtualTop( wnd ) WndVirtualRow( wnd, wnd->title_size )
#define WndVirtualBottom( wnd ) WndVirtualRow( wnd, wnd->rows )

#endif // _AUIPVT_H_INCLUDED
