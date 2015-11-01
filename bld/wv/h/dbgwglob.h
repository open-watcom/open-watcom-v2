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


extern void     GlobMenuItem( a_window *wnd, gui_ctl_id id, int row, int piece );
extern int      GlobNumRows( a_window *wnd );
extern  bool    GlobGetLine( a_window *wnd, int row, int piece, wnd_line_piece *line );
extern void     GlobNewMod( a_window *wnd, mod_handle mod );
extern void     GlobRefresh( a_window *wnd );
extern bool     GlobEventProc( a_window * wnd, gui_event gui_ev, void *parm );
extern void     GlobChangeOptions( void );
extern a_window *DoWndGlobOpen( mod_handle mod );
extern a_window *WndGlobOpen( void );
extern bool     ChkFlags( wnd_update_list flags );
