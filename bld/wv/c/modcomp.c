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
#include "guidlg.h"
#include "modlist.h"
#include "strutil.h"
#include "wndsys.h"


extern char     *DlgGetMatchString( gui_window *gui, gui_ctl_id id, size_t *matchoff );

static const char *ModGetName( const void *data_handle, int item )
{
    const module_list *list = data_handle;

    if( item >= ModListNumRows( list ) )
        return( NULL );
    ModListName( list, item, TxtBuff );
    return( TxtBuff );
}

extern void ModComplete( gui_window *gui, gui_ctl_id id )
{
    char        *match;
    module_list list;
    int         new;
    size_t      matchoff;
    char        *savebuff;

    match = DlgGetMatchString( gui, id, &matchoff );
    savebuff = DupStr( TxtBuff );
    ModListInit( &list, *match == '\0' ? NULL : match );
    ModListAddModules( &list, NO_MOD, FALSE );
    switch( ModListNumRows( &list ) ) {
    case 0:
        WndMsgBox( LIT_DUI( No_Match_Found ) );
        new = -1;
        break;
    case 1:
        new = 0;
        break;
    default:
        new = DlgPickWithRtn( LIT_DUI( Modules ), &list, 0, ModGetName, ModListNumRows( &list ) );
        break;
    }
    strcpy( TxtBuff, savebuff );
    if( new != -1 ) {
        ModListName( &list, new, TxtBuff + matchoff );
        GUISetText( gui, id, TxtBuff );
    }
    GUISetFocus( gui, id );
    ModListFree( &list );
    WndFree( match );
    WndFree( savebuff );
}
