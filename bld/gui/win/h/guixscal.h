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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#ifndef _GUIXSCAL_H_
#define _GUIXSCAL_H_

extern WPI_TEXTMETRIC   GUItm;

extern void         GUIClientToScaleRect( const guix_rect *client, gui_rect *rect );

extern guix_ord     GUIFromTextX( gui_text_ord text_ord, gui_window *wnd );
extern guix_ord     GUIFromTextY( gui_text_ord text_ord, gui_window *wnd );
extern gui_text_ord GUIToTextX( guix_ord ord, gui_window *wnd );
extern gui_text_ord GUIToTextY( guix_ord ord, gui_window *wnd );

extern void         GUIGetUpdateRows( gui_window *, HWND, gui_text_ord *, gui_text_ord * );

extern void         GUIGetMetrics( gui_window *wnd );
extern bool         GUIGetTheDC( gui_window *wnd );
extern void         GUIReleaseTheDC( gui_window *wnd );

#endif // _GUIXSCAL_H_
