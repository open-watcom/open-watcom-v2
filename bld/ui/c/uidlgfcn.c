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

extern void uioncheckbox( struct a_dialog * d, struct vfield * v )
{
    d->dirty = TRUE;
    ((a_check*)v->ptr)->val = 1;
    uiprintfield( d, v );
}

extern void uioffcheckbox( struct a_dialog * d, struct vfield * v )
{
    d->dirty = TRUE;
    ((a_check*)v->ptr)->val = 0;
    uiprintfield( d, v );
}

extern void uiselectradio( struct a_dialog * d, struct vfield * v )
{
    unsigned newval = ((a_radio*)v->ptr)->value;
    unsigned* oldval = &((a_radio*)v->ptr)->group->value;
    VFIELD * oldradio = NULL;
    VFIELD * tmp = d->fields;
    if( newval == *oldval ) return; // nothing to do

    d->dirty = TRUE;
    while( tmp->ptr != NULL ) {
        if( tmp->typ == FLD_RADIO ) {
            if( ((a_radio*)tmp->ptr)->value == *oldval ) {
                // we have it!
                oldradio = tmp;
                break;
            }
        }
        tmp++;
    }
    if( oldradio == NULL ) return; // error!

    *oldval = newval;
    uiprintfield( d, v );
    uiprintfield( d, oldradio );
}

extern void uiselectlist( struct a_dialog * d, struct vfield * v, unsigned n )
{
    unsigned* oldchoice = &((a_list*)v->ptr)->choice;
//    if( *oldchoice == n ) return;
    // This line is commented out because sometimes the selection will not be
    // highlighted, and we change to the same selection.  Result - selection
    // does not get highlighted.
    *oldchoice = n;
    d->dirty = TRUE;
    uiprintfield( d, v );
    uiupdatelistbox( ((a_list*)v->ptr) );
}

extern void uiselectcombo( struct a_dialog * d, struct vfield * v, unsigned n )
{
    d->dirty = TRUE;
    ((a_combo_box*)v->ptr)->list.choice = n;
    uiprintfield( d, v );
}
