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


#ifndef _GUIMKEY_H_
#define _GUIMKEY_H_
extern gui_key GUIMapKey( gui_key key );

#define SHIFT   ( GUI_SHIFT( KeyState ) )
#define ALT     ( GUI_ALT( KeyState ) )
#define CTRL    ( GUI_CTRL( KeyState ) )

#define SET_CTRL        ( KeyState |= GUI_KS_CTRL )
#define SET_SHIFT       ( KeyState |= GUI_KS_SHIFT )
#define SET_ALT         ( KeyState |= GUI_KS_ALT )

#define CLR_CTRL        ( KeyState &= ~GUI_KS_CTRL )
#define CLR_SHIFT       ( KeyState &= ~GUI_KS_SHIFT )
#define CLR_ALT         ( KeyState &= ~GUI_KS_ALT )

#endif // _GUIMKEY_H_
