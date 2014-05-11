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


#include <string.h>
#include "wresall.h"
#include "semantic.h"
#include "semtbar.h"
#include "global.h"
#include "restbar.h"
#include "wresdefn.h"
#include "rcrtns.h"

static void initToolBarItems( ToolBarItems *ret ) {
    ret->next = NULL;
    ret->cnt = 0;
    memset( ret->items, 0, TB_ITEM_CNT * sizeof( uint_16 ) );
}

ToolBar *SemCreateToolBar( void ) {
    ToolBar     *ret;

    ret = RCALLOC( sizeof( ToolBar ) );
    ret->last = &ret->first;
    ret->nodecnt = 1;
    initToolBarItems( &ret->first );
    return( ret );
}

void SemAddToolBarItem( ToolBar *toolbar, uint_16 item ) {

    ToolBarItems        *node;

    if( toolbar->last->cnt == TB_ITEM_CNT ) {
        toolbar->last->next = RCALLOC( sizeof( ToolBarItems ) );
        toolbar->last = toolbar->last->next;
        initToolBarItems( toolbar->last );
        toolbar->nodecnt++;
    }
    node = toolbar->last;
    node->items[ node->cnt ] = item;
    node->cnt++;
}

void static semFreeToolBar( ToolBar *toolbar ) {

    ToolBarItems        *cur;
    ToolBarItems        *tmp;

    cur = toolbar->first.next;
    while( cur != NULL ) {
        tmp = cur;
        cur = cur->next;
        RCFREE( tmp );
    }
    RCFREE( toolbar );
}

void SemWriteToolBar( WResID *name, ToolBar *toolbar,
                      unsigned long item1, unsigned long item2,
                      ResMemFlags flags ) {

    ResLocation              loc;
    unsigned                 cnt;
    ToolBarItems            *cur;

    if(!ErrorHasOccured) {
        loc.start = SemStartResource();
        cnt = ( toolbar->nodecnt - 1 ) * TB_ITEM_CNT;
        cnt += toolbar->last->cnt;
        ResWriteToolBarHeader( CurrResFile.handle, item1, item2, cnt );
        cur = &toolbar->first;
        while( cur != NULL ) {
            ResWriteToolBarItems( CurrResFile.handle, cur->items, cur->cnt );
            cur = cur->next;
        }
        loc.len = SemEndResource( loc.start );
        SemAddResourceFree( name, WResIDFromNum( RT_TOOLBAR ), flags, loc );
    }
    semFreeToolBar( toolbar );
}
