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


#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbgwind.h"
#include "dbgitem.h"
#include "mad.h"
#include "madcli.h"
#include "strutil.h"
#include "dbgwtogl.h"


gui_menu_struct *WndAppendToggles( mad_toggle_strings const *toggles, unsigned *pnum_toggles,
                                   const gui_menu_struct *old, unsigned num_old, gui_ctl_id id )
{
    gui_menu_struct     *popup;
    unsigned            num_toggles;
    int                 i;
    gui_menu_struct     *menu;
    int                 menu_items;

    for( num_toggles = 0; toggles[num_toggles].menu != MAD_MSTR_NIL; ++num_toggles ) ;
    menu_items = num_old + num_toggles;
    popup = WndMustAlloc( menu_items * sizeof( gui_menu_struct ) );
    memcpy( popup, old, num_old * sizeof( gui_menu_struct ) );
    for( i = 0; i < num_old; ++i ) {
        popup[i].style &= ~WND_MENU_ALLOCATED;
    }
    menu = popup + num_old;
    for( i = 0; i < num_toggles; ++i ) {
        MADCli( String )( toggles[i].menu, TxtBuff, TXT_LEN );
        menu->id = id++;
        menu->style = GUI_ENABLED;
        menu->hinttext = LIT_ENG( Empty );
        menu->label = DupStr( TxtBuff );
        menu->num_child_menus = 0;
        menu->child = NULL;
        ++menu;
    }
    *pnum_toggles = num_toggles;
    return( popup );
}


void WndDeleteToggles( gui_menu_struct *popup, unsigned num_old, unsigned num_toggles )
{
    int                 i;
    gui_menu_struct     *menu;

    NullPopupMenu( popup );
    menu = popup + num_old;
    for( i = 0; i < num_toggles; ++i ) {
        WndFree( (void *)menu->label );
        ++menu;
    }
    WndFree( popup );
}
