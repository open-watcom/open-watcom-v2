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
* Description:  Processing of pop-up menu for assembly view.
*
****************************************************************************/


extern void     AsmJoinSrc( a_window *wnd, a_window *src );
extern void     AsmNewSrcNotify( a_window *src, mod_handle mod, bool track );
extern void     AsmMoveDot( a_window *wnd, address addr );
extern a_window *AsmWndFind( a_window *wnd, address addr, bool track );
extern void     AsmFreeSrc( a_window *wnd );
#ifdef DEADCODE
extern bool     AsmIsTracking( a_window *wnd );
#endif
extern bool     AsmOpenGadget( a_window *wnd, wnd_line_piece *line, mod_handle mod );
extern void     AsmChangeOptions( void );
extern a_window *DoWndAsmOpen( address addr, bool track );
extern a_window *WndAsmOpen( void );
