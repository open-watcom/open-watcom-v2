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


#ifndef _GUIGADGT_H_
#define _GUIGADGT_H_
extern void GUISetShowGadget( p_gadget gadget, bool show, bool set, int pos );
extern EVENT GUIGadgetFilter( gui_window *wnd, EVENT ev, int *prev, int *diff );
extern bool GUIUseGadget( gui_window *wnd, p_gadget gadget );
extern bool GUIDrawGadgetLine( p_gadget gadget );
extern void GUIInitGadget( p_gadget gadget, ORD start, ORD length, ORD anchor );
extern bool GUICreateGadget( gui_window *wnd, a_gadget_direction dir,
                      ORD anchor, ORD start, ORD length,
                      p_gadget *gadget_ptr, gui_scroll_styles style );
extern int GUIGetScrollOffset( void );
#endif
