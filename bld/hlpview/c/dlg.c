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
* Description:  Dialog handling routines.
*
****************************************************************************/


#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "watcom.h"
#include "helpmem.h"
#include "helpchar.h"
#include "uidef.h"
#include "uivedit.h"
#include "uiledit.h"
#include "uidialog.h"
#include "helpio.h"
#include "search.h"
#include "clibext.h"


extern UIPICKGETTEXT    GetListBoxItem;

typedef struct {
    char                str[1];         /* dynamic array */
} ListBoxEntry;

static a_hot_spot       searchButtons[] = {
    {"OK",      EV_ENTER,       1,      -1,     11,     HOT_DEFAULT},
    {"Cancel",  EV_ESCAPE,      3,      -1,     11,     0}
};

static unsigned long    listData;

static a_list listBox = {
    0, &listData, GetListBoxItem, NULL
};

static an_edit_control      editCtl;

static VFIELD helpSearchDialog[] = {
    {{1,    4,      1,      40},    FLD_EDIT,       &editCtl},
    {{3,    4,      8,      40},    FLD_LISTBOX,    &listBox},
    {{1,    -1,     1,      11},    FLD_HOT,        &searchButtons[0]},
    {{3,    -1,     1,      11},    FLD_HOT,        &searchButtons[1]},
    {{12,   0,      0,      0},     FLD_VOID,       NULL}
};

static VFIELD   *editVField = &helpSearchDialog[0];

static a_dialog         *curHelpDialog;
static HelpHdl          searchHdl;

static bool GetLBItemLiteral( void *info, unsigned line, char *buf,
                              unsigned buflen )
{
    char                *name;
    unsigned long       *itemcnt;
    unsigned            height;

    itemcnt = info;
    if( listBox.box == NULL ) {
        height = 0;
    } else {
        height = listBox.box->area.height - 1;
    }
    if( line >= *itemcnt + height ) return( FALSE );
    if( buf != NULL ) {
        if( line >= *itemcnt ) {
            buf[0] = '\0';
        } else {
            name = HelpGetIndexedTopic( searchHdl, line );
            strncpy( buf, name, buflen );
            buf[buflen-1] = '\0';
        }
    } else {
        return( FALSE );
    }
    return( TRUE );
}

bool GetListBoxItem( void *data_handle, unsigned item, char *buf, unsigned buflen )
{
    const char          *name;
    unsigned long       *itemcnt;
    unsigned            height;

    //
    // NYI - this should call GetLBItemLiteral
    //
    itemcnt = (unsigned long *)data_handle;
    if( listBox.box == NULL ) {
        height = 0;
    } else {
        height = listBox.box->area.height - 1;
    }
    if( item >= *itemcnt + height )
        return( FALSE );
    if( buf != NULL ) {
        if( item >= *itemcnt ) {
            name = "";
        } else {
            name = HelpGetIndexedTopic( searchHdl, item );
        }
        if( name != NULL ) {
            while( *name != '\0' ) {
                if( buflen == 1 ) {
                    break;
                }
                if( *name == HELP_ESCAPE ) {
                    name ++;
                    continue;
                }
                *buf = *name;
                buf++;
                name++;
                buflen--;
            }
            *buf = '\0';
        } else {
            return( FALSE );
        }
    }
    return( TRUE );
}

static void fillListBox( char *name )
{
    unsigned            index;

    index = HelpFindFirst( searchHdl, name, NULL );
    listBox.choice = index;
    listBox.box->line = index;
    listBox.box->row = index;
    uiupdatelistbox( &listBox );
}

#define MAX_EDIT_LINE_LEN       100

static void copyLBLinetoEditCtl( unsigned index )
{
    char        *lb_item;
    unsigned    len;

    lb_item = uimalloc( MAX_EDIT_LINE_LEN );
    GetListBoxItem( &listData, index, lb_item, MAX_EDIT_LINE_LEN );
    len = strlen( lb_item );
    uifree( editCtl.buffer );
    editCtl.buffer = lb_item;
    editCtl.length = len;
    uiupdateedit( curHelpDialog, editVField );
    uiprintfield( curHelpDialog, editVField );
}

void HelpDialogCallBack( a_dialog *info )
{
    char                buf[_MAX_PATH];
    unsigned            len;

    if( info != curHelpDialog ) return;
    if( info->curr == editVField ) {
        if( info->edit_data->edit_eline.dirty ) {
            curHelpDialog->edit_data->edit_eline.dirty = FALSE;
            len = editCtl.length;
            if( len > sizeof( buf ) )
                len = sizeof( buf );
            strncpy( buf, editCtl.buffer, len );
            buf[ len ] = '\0';
            len--;
            while( len > 0 && isspace( buf[len] ) ) {
                buf[len] = '\0';
                len--;
            }
            fillListBox( buf );
        }
    }
}

void SearchDlgFini( void )
{
    if( editCtl.buffer != NULL ) {
        uifree( editCtl.buffer );
    }
    editCtl.buffer = NULL;
}

char *HelpSearch( HelpHdl hdl )
{
    EVENT               event;
    char                done;
    char                *ret;

    static EVENT        events[] = {
        EV_NO_EVENT,
        EV_ENTER,
        EV_ESCAPE,
        EV_MOUSE_PRESS,
        EV_LIST_BOX_DCLICK,
        EV_NO_EVENT
    };

    searchHdl = hdl;
    listData = hdl->header.topiccnt;
    curHelpDialog = uibegdialog( "Search", helpSearchDialog, 12, 60, 0, 0 );
    if( editCtl.buffer == NULL ) {
        fillListBox( "" );
    } else {
        fillListBox( editCtl.buffer );
    }
    uipushlist( NULL );     /* modal barrier */
    uipushlist( events );
    done = 0;
    while( !done ) {
        event = uidialog( curHelpDialog );
        switch( event ) {
        case EV_MOUSE_PRESS:
            if( curHelpDialog->curr != NULL ) {
                if( curHelpDialog->curr->u.ptr == &listBox ) {
                    copyLBLinetoEditCtl( listBox.box->row );
                }
            }
            break;
        case EV_KILL_UI:
            uiforceevadd( EV_KILL_UI );
            /* fall through */
        case EV_ESCAPE:
            ret = NULL;
            done = 1;
            break;
        case EV_ENTER:
        case EV_LIST_BOX_DCLICK:
            ret = HelpMemAlloc( MAX_EDIT_LINE_LEN );
            GetLBItemLiteral( &listData, listBox.box->row, ret,
                              MAX_EDIT_LINE_LEN );
            if( ret[0] == '\0' ) {
                HelpMemFree( ret );
                ret = NULL;
            }
            done = 1;
            break;
        }
    }
    uipoplist();
    uipoplist();
    uienddialog( curHelpDialog );
    return( ret );
}
