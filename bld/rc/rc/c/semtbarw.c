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


#include "global.h"
#include "semantic.h"
#include "semtbarw.h"
#include "restbar.h"
#include "wresdefn.h"
#include "rcrtns.h"
#include "rccore.h"


static void initToolBarItems( ToolBarItems *ret )
{
    ret->next = NULL;
    ret->cnt = 0;
    memset( ret->items, 0, TB_ITEM_CNT * sizeof( uint_16 ) );
}

ToolBar *SemWINCreateToolBar( void )
{
    ToolBar     *ret;

    ret = RESALLOC( sizeof( ToolBar ) );
    ret->last = &ret->first;
    ret->nodecnt = 1;
    initToolBarItems( &ret->first );
    return( ret );
}

void SemWINAddToolBarItem( ToolBar *toolbar, uint_16 item )
{
    ToolBarItems        *node;

    if( toolbar->last->cnt == TB_ITEM_CNT ) {
        toolbar->last->next = RESALLOC( sizeof( ToolBarItems ) );
        toolbar->last = toolbar->last->next;
        initToolBarItems( toolbar->last );
        toolbar->nodecnt++;
    }
    node = toolbar->last;
    node->items[node->cnt] = item;
    node->cnt++;
}

static void semFreeToolBar( ToolBar *toolbar )
{
    ToolBarItems        *cur;
    ToolBarItems        *next;

    for( cur = toolbar->first.next; cur != NULL; cur = next ) {
        next = cur->next;
        RESFREE( cur );
    }
    RESFREE( toolbar );
}

void SemWINWriteToolBar( WResID *name, ToolBar *toolbar,
                      unsigned long item1, unsigned long item2,
                      ResMemFlags flags )
{
    ResLocation         loc;
    unsigned            cnt;
    ToolBarItems        *cur;

    if( !ErrorHasOccured ) {
        loc.start = SemStartResource();
        cnt = ( toolbar->nodecnt - 1 ) * TB_ITEM_CNT;
        cnt += toolbar->last->cnt;
        ResWriteToolBarHeader( CurrResFile.fp, item1, item2, cnt );
        for( cur = &toolbar->first; cur != NULL; cur = cur->next ) {
            ResWriteToolBarItems( CurrResFile.fp, cur->items, cur->cnt );
        }
        loc.len = SemEndResource( loc.start );
        SemAddResourceFree( name, WResIDFromNum( RESOURCE2INT( RT_TOOLBAR ) ), flags, loc );
    }
    semFreeToolBar( toolbar );
}
