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


#include "uidef.h"
#include "uidialog.h"
#include "uigchar.h"

extern void uiupdatelistbox( a_list *list );

extern void uioncheckbox( a_dialog *ui_dlg_info, VFIELD * v )
{
    ui_dlg_info->dirty = TRUE;
    v->u.check->val = 1;
    uiprintfield( ui_dlg_info, v );
}

extern void uioffcheckbox( a_dialog *ui_dlg_info, VFIELD * v )
{
    ui_dlg_info->dirty = TRUE;
    v->u.check->val = 0;
    uiprintfield( ui_dlg_info, v );
}

extern void uiselectradio( a_dialog *ui_dlg_info, VFIELD * v )
{
    unsigned newval = v->u.radio->value;
    unsigned *oldval = &v->u.radio->group->value;
    VFIELD *oldradio = NULL;
    VFIELD *tmp = ui_dlg_info->fields;
    if( newval == *oldval ) return; // nothing to do

    ui_dlg_info->dirty = TRUE;
    while( tmp->u.radio != NULL ) {
        if( tmp->typ == FLD_RADIO ) {
            if( tmp->u.radio->value == *oldval ) {
                // we have it!
                oldradio = tmp;
                break;
            }
        }
        tmp++;
    }
    if( oldradio == NULL ) return; // error!

    *oldval = newval;
    uiprintfield( ui_dlg_info, v );
    uiprintfield( ui_dlg_info, oldradio );
}

extern void uiselectlist( a_dialog *ui_dlg_info, VFIELD * v, unsigned n )
{
    unsigned *oldchoice = &v->u.list->choice;
//    if( *oldchoice == n ) return;
    // This line is commented out because sometimes the selection will not be
    // highlighted, and we change to the same selection.  Result - selection
    // does not get highlighted.
    *oldchoice = n;
    ui_dlg_info->dirty = TRUE;
    uiprintfield( ui_dlg_info, v );
    uiupdatelistbox( v->u.list );
}

extern void uiselectcombo( a_dialog *ui_dlg_info, VFIELD * v, unsigned n )
{
    ui_dlg_info->dirty = TRUE;
    v->u.combo->list.choice = n;
    uiprintfield( ui_dlg_info, v );
}
