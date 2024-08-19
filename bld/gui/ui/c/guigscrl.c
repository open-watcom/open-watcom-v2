/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2024 The Open Watcom Contributors. All Rights Reserved.
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


#include "guiwind.h"
#include "guiscale.h"

gui_text_ord GUIAPI GUIGetHScrollCol( gui_window * wnd )
{
    if( IS_HSCROLL_ON( wnd ) )
        return( wnd->hgadget->pos );
    return( 0 );
}

gui_text_ord GUIAPI GUIGetVScrollRow( gui_window * wnd )
{
    if( IS_VSCROLL_ON( wnd ) )
        return( wnd->vgadget->pos );
    return( 0 );
}

gui_ord GUIAPI GUIGetHScroll( gui_window * wnd )
{
    if( IS_HSCROLL_ON( wnd ) )
        return( GUIScaleFromScreenH( wnd->hgadget->pos ) );
    return( 0 );
}

gui_ord GUIAPI GUIGetVScroll( gui_window * wnd )
{
    if( IS_VSCROLL_ON( wnd ) )
        return( GUIScaleFromScreenV( wnd->vgadget->pos ) );
    return( 0 );
}
