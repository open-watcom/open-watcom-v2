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
#include "namelist.h"
#include "strutil.h"
#include "wndsys.h"
#include "symcomp.h"


static name_list *SortedNames;

static const char *SymGetName( const void *data_handle, int item )
{
    item += *(const int *)data_handle;
    if( item >= NameListNumRows( SortedNames ) )
        return( NULL );
    NameListName( SortedNames, item, TxtBuff, SN_QUALIFIED );
    return( TxtBuff );
}

extern char *DlgGetMatchString( gui_window *gui, gui_ctl_id id, size_t *matchoff )
{
    char        *p;
    char        *match;

    memset( TxtBuff, 0, TXT_LEN );
    GUIDlgBuffGetText( gui, id, TxtBuff, TXT_LEN );
    StrTrim( TxtBuff );
    match = TxtBuff - 1;
    for( p = TxtBuff; *p != '\0'; ++p ) {
        if( *p == ' ' ) {
            match = p;
        }
    }
    *matchoff = match + 1 - TxtBuff;
    return( DupStr( match + 1 ) );
}


extern void SymComplete( gui_window *gui, gui_ctl_id id )
{
    char                *match;
    int                 new;
    unsigned            first,last;
    unsigned            num;
    size_t              matchoff;
    char                *savebuff;

    SortedNames = SymCompInit( TRUE, TRUE, FALSE, FALSE, NO_MOD );
    match = DlgGetMatchString( gui, id, &matchoff );
    savebuff = DupStr( TxtBuff );
    if( match != NULL && match[0] != '\0' ) {
        SymCompMatches( SortedNames, match, &first, &last );
        num = last - first;
    } else {
        num = NameListNumRows( SortedNames );
        first = 0;
    }
    switch( num ) {
    case 0:
        WndMsgBox( LIT_DUI( No_Match_Found ) );
        new = -1;
        break;
    case 1:
        new = 0;
        break;
    default:
        new = DlgPickWithRtn( LIT_DUI( Symbol_List ), &first, 0, SymGetName, num );
        break;
    }
    strcpy( TxtBuff, savebuff );
    if( new != -1 ) {
        new += first;
        NameListName( SortedNames, new, TxtBuff + matchoff, SN_QUALIFIED );
        GUISetText( gui, id, TxtBuff );
    }
    GUISetFocus( gui, id );
    WndFree( savebuff );
    WndFree( match );
}
