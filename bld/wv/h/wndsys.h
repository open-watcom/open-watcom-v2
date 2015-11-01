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
* Description:  Windowing 'system' routines.
*
****************************************************************************/


extern gui_resource     WndGadgetArray[];

extern bool     DbgWndSearch( a_window * wnd, bool from_top, int direction );
extern void     ProcPUINYI( a_window *wnd );
extern void     ProcWndSearch( a_window *wnd );
extern void     ProcWndTabLeft( a_window *wnd );
extern void     ProcWndTabRight( a_window *wnd );
extern void     ProcSearchAll( void );
extern void     ProcWndPopUp( a_window *wnd );
extern void     ProcWndFindNext( a_window *wnd );
extern void     ProcWndFindPrev( a_window *wnd );
extern void     WndProcWindow( void );
extern void     WndSysStart( void );
extern void     WndSysEnd( bool pause );
extern void     WndStop( void );
extern void     WndUser( void );
extern void     WndDebug( void );
extern void     WndRedraw( wnd_class wndcls );
extern bool     WndProcMacro( a_window *wnd, unsigned key );
extern void     WndSysInit( void );
extern void     SetUnderLine( a_window *wnd, wnd_line_piece *line );
extern void     SetGadgetLine( a_window *wnd, wnd_line_piece *line, wnd_gadget_type type );
extern void     FiniGadget( void );
extern void     InitGadget( void );
extern bool     OpenGadget( a_window *wnd, wnd_line_piece *line, mod_handle mod, bool src );
extern bool     CheckOpenGadget( a_window *wnd, wnd_row row, bool open, mod_handle mod, bool src, int piece );
extern void     WndStartFreshAll( void );
extern void     WndEndFreshAll( void );
extern void     WndFlushKeys( void );
extern void     WndMsgBox( const char *msg );
extern void     WndInfoBox( const char *msg );
extern void     WndSetOpenNoShow( void );
extern a_window *DbgTitleWndCreate( const char *title, wnd_info *wndinfo, wnd_class wndcls, void *extra, gui_resource *icon, int title_size, bool vdrag );
extern a_window *DbgWndCreate( const char *title, wnd_info *info, wnd_class wndcls, void *extra, gui_resource *icon );
extern void     WndRXError( int num );
#if defined(__GUI__)
extern void     SaveMainScreen( char *name );
extern void     RestoreMainScreen( char *name );
extern void     WndSetWndMainSize( wnd_create_struct *info );
#endif
