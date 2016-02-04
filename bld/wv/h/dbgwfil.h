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
* Description:  Processing of pop-up menu for source file view.
*
****************************************************************************/


extern void     SrcJoinAsm( a_window *wnd, a_window *asw );
extern void     SrcNewAsmNotify( a_window *asw, mod_handle mod, bool track );
extern void     SrcFreeAsm( a_window *wnd );
#ifdef DEADCODE
extern bool     SrcIsTracking( a_window *wnd );
#endif

extern bool     FileOpenGadget( a_window *wnd, wnd_line_piece *line, mod_handle mod );
extern void     FileBreakGadget( a_window *wnd, wnd_line_piece *line, bool curr, brkp *bp );
#ifdef DEADCODE
extern void     FileReset( a_window *wnd );
#endif
extern bool     SrcMoveDot( a_window *wnd, address addr );
extern a_window *SrcWndFind( a_window *wnd, address addr, bool track );
extern bool     SrcHasFileOpen( a_window *wnd );
extern a_window *DoWndFileOpen( const char *name, void *viewhndl, cue_handle *ch, bool track, bool erase, wnd_class_wv wndclass );
extern a_window *DoWndSrcOpen( cue_handle *ch, bool track );
extern a_window *WndSrcOpen( void );
extern void     ProcView( void );
