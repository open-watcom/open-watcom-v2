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


#include "dfdip.h"
#include "dfld.h"
#include "dfmod.h"
#include "dfmodinf.h"
#include "dfscope.h"


extern scope_node *FindScope( scope_node *last, addr_off in ){
/******** find the down scope of start, len ************************/
    scope_node *down;

    down = NULL;
    while( last != NULL ){ /* shimmey down to containing branch */
        if( last->start <= in  && in < last->end  ){
            break;
        }
        last = last->down;
    }
    /* climp up and across tree until you fall out */
    while( last != NULL ){  //look for containing down scope
        if( last->start <= in  && in < last->end  ){
            down = last; // if containing go up the branch
            last = last->up;
        }else{
            last = last->next; /* try next branch */
        }
    }
    return( down );
}

static void FreeScope( scope_node *last ){
/** find the down scope of start, len*****/
    scope_node *old;

    /* climp up and across free across and down */
    while( last != NULL ){  //look for containing down scope
        if( last->up != NULL ){
            old = last;
            last = last->up;
            old->up = NULL; /* don't go up again */
        }else{
            if( last->next != NULL ){
                old = last;
                last = last->next;
            }else{/* done this level */
                old = last;
                last = last->down;
            }
            DCFree( old );
        }
    }
}

extern scope_node *AddScope( scope_node *where, addr_off start,
                                                addr_off end,
                                               drmem_hdl what ){
/**************************************************************/
    scope_node *new;
    scope_node *down;

    new = DCAlloc( sizeof( *new ) );
    new->up = NULL;
    new->down = NULL;
    new->next = NULL;
    new->start = start;
    new->end = end;
    new->what = what;
    if( where != NULL ){
        down = FindScope( where, start );
        if( down != NULL ) {
            new->down = down;
            new->next = down->up;
            down->up = new;
        }
    }
    return( new );
}

extern void InitScope( scope_ctl *ctl ){
/*****************************************/
    ctl->root = NULL;
    ctl->edge = NULL;
    ctl->base = NilAddr;
}

extern void FiniScope( scope_ctl *ctl ){
/**************************************/
    FreeScope( ctl->root );
    ctl->root = NULL;
    ctl->edge = NULL;
    ctl->base = NilAddr;
}
