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


#ifndef _GUISCALE_H_
#define _GUISCALE_H_

#include "guixscal.h"

extern void GUISetScreen( gui_ord xmin, gui_ord ymin, gui_ord width, gui_ord height );

extern guix_ord GUIScaleToScreenH( gui_ord ord );
extern guix_ord GUIScaleToScreenV( gui_ord ord );
extern guix_ord GUIScaleToScreenX( gui_ord ord );
extern guix_ord GUIScaleToScreenY( gui_ord ord );

extern gui_ord GUIScreenToScaleH( guix_ord screen_ord );
extern gui_ord GUIScreenToScaleV( guix_ord screen_ord );
extern gui_ord GUIScreenToScaleX( guix_ord screen_ord );
extern gui_ord GUIScreenToScaleY( guix_ord screen_ord );

extern bool GUIScreenToScaleRect( const guix_rect *screen_rect, gui_rect *rect );
extern bool GUIScreenToScaleRectR( const guix_rect *screen_rect, gui_rect *rect );

extern bool GUIScaleToScreenRect( const gui_rect *rect, guix_rect *screen_rect );
extern bool GUIScaleToScreenRectR( const gui_rect *rect, guix_rect *screen_rect );

#endif // _GUISCALE_H_
