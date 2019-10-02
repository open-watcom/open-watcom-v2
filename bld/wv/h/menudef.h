/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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


#define MENU_LIT( x )               ((char *)(pointer_uint)DBG_DUI_MENU_##x )
#define MENU_DO_ITEM( a, b, c, d )  { MENU_LIT( b ), a, GUI_STYLE_MENU_ENABLED, MENU_LIT( HELP_##b ), { c, d } },
#define MENU_ITEM( a, b )           MENU_DO_ITEM( a, b, 0, NULL )
#define MENU_CASCADE( a, b, c )     MENU_DO_ITEM( a, b, ArraySize( c ), c )
#define MENU_CASCADE_DUMMY( a, b )  MENU_ITEM( a, b )
#define MENU_BAR                    { "", 0, GUI_STYLE_MENU_SEPARATOR, NULL, { 0, NULL } },
