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
* Description:  Hint window interface.
*
****************************************************************************/


#ifndef _HINT_H_INCLUDED
#define _HINT_H_INCLUDED

#include "statwnd.h"

typedef struct {
    WORD        menuid;
    WORD        msgid;
} MenuItemHint;

void            HintToolBar( statwnd *wnd, UINT menuid, BOOL select );
WORD            SizeHintBar( statwnd *wnd );
void            HintMenuSelect( statwnd *wnd, HWND hwnd, WPARAM wparam, LPARAM lparam );
MenuItemHint    *SetHintText( statwnd *wnd, MenuItemHint *hints, WORD cnt );
statwnd         *HintWndCreate( HWND parent, RECT *size, HINSTANCE hinstance, LPVOID lpvParam );
void            HintWndDestroy( statwnd *wnd );
int             HintWndInit( HINSTANCE hinstance, statushook hook, int extra );
void            HintFini( void );
HWND            GetHintHwnd( statwnd *wnd );

#endif /* _HINT_H_INCLUDED */
