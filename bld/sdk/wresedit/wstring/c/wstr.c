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


#include <windows.h>
#include <string.h>
#include "wglbl.h"
#include "wmem.h"
#include "wstring.h"
#include "winfo.h"
#include "wribbon.h"
#include "wstrdup.h"
#include "wstr.h"
#include "wcopystr.h"
#include "widn2str.h"
#include "wnewitem.h"
#include "sys_rc.h"

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static  void            WMakeDataFromStringBlock( WStringBlock *block, void **data, int *size );
static  int             WInitDataFromStringBlock( WStringBlock *block, void *data, int size );
static  int             WInitStringTable        ( WStringInfo *info, WStringTable *tbl );
static  int             WMakeStringBlockFromData( void *data, int size, WStringBlock *block );
static  int             WCalcStringBlockSize    ( WStringBlock *block );
static  WStringBlock    *WFindStringTableBlock  ( WStringTable *tbl, uint_16 blocknum );
static  WStringBlock    *WAllocStringBlock      ( void );
static  WStringTable    *WAllocStringTable      ( int is32bit );
static  void            WFreeStringTable        ( WStringTable *tbl );
static  void            WFreeStringTableBlock   ( WStringBlock *block );
static  WStringNode     *WMakeStringNodeFromStringBlock( WStringBlock * );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/

WStringEditInfo *WAllocStringEInfo ( void )
{
    WStringEditInfo *einfo;

    einfo = (WStringEditInfo *) WMemAlloc ( sizeof(WStringEditInfo) );

    if ( einfo ) {
        memset ( einfo, 0, sizeof(WStringEditInfo) );
        einfo->current_pos = -1;
    }

    return ( einfo );
}

void WFreeStringEInfo ( WStringEditInfo *einfo )
{
    if ( einfo ) {
        if ( einfo->tbl ) {
            WFreeStringTable ( einfo->tbl );
            einfo->tbl = NULL;
        }
        if ( einfo->wsb ) {
            WDestroyStatusLine ( einfo->wsb );
            einfo->wsb = NULL;
        }
        if ( einfo->ribbon ) {
            WDestroyRibbon ( einfo );
        }
        if ( ( einfo->edit_dlg != (HWND)NULL ) &&
             IsWindow ( einfo->edit_dlg ) ) {
            DestroyWindow ( einfo->edit_dlg );
            einfo->edit_dlg = (HWND)NULL;
        }
        if ( ( einfo->win != (HWND)NULL ) && IsWindow ( einfo->win ) ) {
            SetWindowLong( einfo->win, 0, (LONG)0 );
            DestroyWindow ( einfo->win );
            einfo->win = (HWND)NULL;
        }
        if ( einfo->file_name ) {
            WMemFree ( einfo->file_name );
        }
        WMemFree ( einfo );
    }
}

Bool WIsBlockEmpty( WStringBlock *block )
{
    return( WRIsBlockEmpty( &block->block ) );
}

WStringBlock *WFindStringBlock( WStringTable *tbl, uint_16 blocknum )
{
    WStringBlock *block;

    if( tbl ) {
        block = tbl->first_block;
        while( block ) {
            if( ( block->blocknum & 0xfff0 ) == ( blocknum & 0xfff0 ) ) {
                return( block );
            }
            block = block->next;
        }
    }

    return( NULL );
}

int WFindStringPos( WStringTable *tbl, uint_16 string_id )
{
    WStringBlock        *block;
    int                 pos;
    int                 i;

    pos = 0;

    if( tbl ) {
        block = tbl->first_block;
        while( block ) {
            for( i=0; i<STRTABLE_STRS_PER_BLOCK; i++ ) {
                if( block->block.String[i] != NULL ) {
                    if( ( ( block->blocknum & 0xfff0 ) + i ) == string_id ) {
                        return( pos );
                    }
                    pos++;
                }
            }
            block = block->next;
        }
    }

    return( -1 );
}

WStringBlock *WFindStringTableBlock( WStringTable *tbl, uint_16 blocknum )
{
    WStringBlock *block;
    WStringBlock *last;

    last = NULL;
    if( tbl ) {
        block = tbl->first_block;
        while( block ) {
            if( block->blocknum > blocknum ) {
                break;
            } else if( block->blocknum == blocknum ) {
                return( block );
            }
            last = block;
            block = block->next;
        }
    }

    return( last );
}

void WMakeDataFromStringBlock ( WStringBlock *block, void **data, int *size )
{
    if( block ) {
        WRMakeDataFromStringBlock(&block->block, data, size, block->is32bit);
    }
}

int WMakeStringBlockFromData( void *data, int size, WStringBlock *block )
{
    int ret;

    if( !data || !size || !block ) {
        return( FALSE );
    }

    ret = WRMakeStringBlockFromData( &block->block, data, size,
                                     block->is32bit );

    return( ret );
}

WStringBlock *WGetOrMakeStringBlock( WStringTable *tbl, uint_16 blocknum )
{
    WStringBlock        *block;
    WStringBlock        *after;

    if( tbl == NULL ) {
        return( NULL );
    }

    after = WFindStringTableBlock( tbl, blocknum );

    if( after ) {
        if( ( after->blocknum & 0xfff0 ) == ( blocknum & 0xfff0 ) ) {
            return( after );
        }
    }

    block = WAllocStringBlock();
    if( block == NULL ) {
        return( NULL );
    }
    block->is32bit  = tbl->is32bit;
    block->blocknum = blocknum & 0xfff0;

    if( after ) {
        if( after->next ) {
            after->next->prev = block;
        }
        block->next = after->next;
        block->prev = after;
        after->next = block;
    } else {
        block->next = tbl->first_block;
        block->prev = NULL;
        tbl->first_block = block;
    }

    return( block );
}

Bool WRemoveStringBlock( WStringTable *tbl, WStringBlock *block )
{
    Bool ok;

    ok = ( tbl && block );

    if( ok ) {
        if( block->next ) {
            block->next->prev = block->prev;
        }
        if( block->prev ) {
            block->prev->next = block->next;
        }
        if( tbl->first_block == block ) {
            tbl->first_block = block->next;
        }
        WFreeStringTableBlock( block );
    }

    return ( ok );
}

WStringBlock *WAllocStringBlock( void )
{
    WStringBlock *block;

    block = (WStringBlock *) WMemAlloc(sizeof(WStringBlock));
    if( block ) {
        memset( block, 0, sizeof(WStringBlock) );
    }

    return( block );
}

void WFreeStringTable ( WStringTable *tbl )
{
    if( tbl ) {
        WFreeStringTableBlocks( tbl->first_block );
        WMemFree( tbl );
    }
}

void WFreeStringTableBlocks( WStringBlock *block )
{
    WStringBlock *b;

    while( block ) {
        b = block;
        block = block->next;
        WFreeStringTableBlock( b );
    }
}

void WFreeStringTableBlock( WStringBlock *block )
{
    int i;

    if( block ) {
        for( i=0; i<STRTABLE_STRS_PER_BLOCK; i++ ) {
            if( block->symbol[i] != NULL ) {
                WMemFree( block->symbol[i] );
            }
        }
        ResFreeStringTableBlock( &block->block );
        WMemFree( block );
    }
}

void WFreeStringNodes( WStringInfo *info )
{
    WFreeStringNode( info->tables );
    info->tables = NULL;
}

void WFreeStringNode( WStringNode *node )
{
    WStringNode *n;

    while( node ) {
        n = node;
        node = node->next;
        if( n->block_name ) {
            WMemFree( n->block_name );
        }
        if( n->data ) {
            WMemFree( n->data );
        }
        WMemFree( n );
    }
}

WStringNode *WMakeStringNodeFromStringBlock( WStringBlock *block )
{
    WStringNode *node;

    if( block == NULL ) {
        return( NULL );
    }

    node = (WStringNode *)WMemAlloc( sizeof(WStringNode) );
    if( node == NULL ) {
        return( NULL );
    }
    memset( node, 0, sizeof(WStringNode) );
    node->block_name = WResIDFromNum( block->blocknum / 16 + 1 );
    node->MemFlags = block->MemFlags;
    WMakeDataFromStringBlock( block, &node->data, &node->data_size );
    if( node->data == NULL ) {
        WFreeStringNode( node );
        return( NULL );
    }

    return( node );
}

WStringNode *WMakeStringNodes( WStringTable *tbl )
{
    WStringNode         *node;
    WStringNode         *new;
    WStringBlock        *block;

    node = NULL;
    block  = tbl->first_block;
    while( block ) {
        new = WMakeStringNodeFromStringBlock( block );
        if( new == NULL ) {
            WFreeStringNode( node );
            return( NULL );
        }
        if( node ) {
            new->next = node;
            node = new;
        } else {
            node = new;
        }
        block = block->next;
    }

    return( node );
}

WStringTable *WAllocStringTable( int is32bit )
{
    WStringTable        *tbl;

    tbl = (WStringTable *) WMemAlloc( sizeof(WStringTable) );

    if( !tbl ) {
        return ( NULL );
    }

    tbl->is32bit = is32bit;
    tbl->first_block = NULL;

    return( tbl );
}

int WInitStringTable ( WStringInfo *info, WStringTable *tbl )
{
    WStringBlock        *block;
    WStringNode         *node;
    uint_16             blocknum;

    if( !info || !tbl ) {
        return( FALSE );
    }

    node = info->tables;

    while( node ) {
        blocknum = (uint_16) WResIDToNum( node->block_name );
        blocknum = ( blocknum - 1 ) * 16;
        if( WFindStringBlock( tbl, blocknum ) ) {
            return( FALSE );
        }
        block = WGetOrMakeStringBlock( tbl, blocknum );
        if( block == NULL ) {
            return( FALSE );
        }
        if( !WMakeStringBlockFromData(node->data, node->data_size, block) ) {
            return( FALSE );
        }
        node = node->next;
    }

    return( TRUE );
}

WStringTable *WMakeStringTableFromInfo( WStringInfo *info )
{
    WStringTable        *tbl;
    int                 ok;

    tbl = NULL;

    ok = ( info != NULL );

    if( ok ) {
        tbl = WAllocStringTable ( info->is32bit );
        ok = ( tbl != NULL );
    }

    if ( ok ) {
        ok = WInitStringTable ( info, tbl );
    }

    if ( ok ) {
        //WFreeStringNodes( info );
    } else {
        if ( tbl ) {
            WFreeStringTable ( tbl );
            tbl = NULL;
        }
    }

    return ( tbl );
}

Bool WGetFirstStringInBlock ( WStringBlock *block, uint_16 *first )
{
    int         i;

    if ( block && first ) {
        for( i=0; i<STRTABLE_STRS_PER_BLOCK; i++ ) {
            if( block->block.String[i] != NULL ) {
                *first = ( ( block->blocknum & 0xfff0 ) + i );
                return ( TRUE );
            }
        }
    }

    return ( FALSE );
}

static WStringBlock *WFindLargestBlock( WStringTable *tbl )
{
    WStringBlock *block;
    WStringBlock *largest;

    largest = NULL;
    if( tbl ) {
        block = tbl->first_block;
        while( block ) {
            if( ( largest == NULL ) ||
                ( block->blocknum > largest->blocknum ) ) {
                largest = block;
            }
            block = block->next;
        }
    }

    return( largest );
}

uint_16 WFindLargestStringID( WStringTable *tbl )
{
    WStringBlock        *largest;
    int                 i;

    largest = WFindLargestBlock( tbl );
    if( largest ) {
        for( i=STRTABLE_STRS_PER_BLOCK-1; i>=0; i-- ) {
            if( largest->block.String[i] != NULL ) {
                return( ( largest->blocknum & 0xfff0 ) + i );
            }
        }
        return( largest->blocknum & 0xfff0 );
    }

    return( 0 );
}

Bool WResolveStringTable( WStringEditInfo *einfo )
{
    WStringBlock *block;

    if( !einfo || !einfo->tbl ) {
        return( FALSE );
    }

    block = einfo->tbl->first_block;
    while( block ) {
        WResolveStringTableBlock( block, einfo->info->symbol_table );
        block = block->next;
    }

    return( TRUE );
}

Bool WResolveStringTableBlock( WStringBlock *block, WRHashTable *symbol_table )
{
    WRHashValueList     *vlist;
    int                 i;

    if( !block || !symbol_table ) {
        return( FALSE );
    }

    for( i=0; i<STRTABLE_STRS_PER_BLOCK; i++ ) {
        if( block->block.String[i] == NULL ) {
            continue;
        }
        vlist = WRLookupValue( symbol_table, (block->blocknum & 0xfff0) + i );
        if( vlist == NULL ) {
            continue;
        }
        if( vlist->next == NULL ) {
            if( block->symbol[i] != NULL ) {
                WMemFree( block->symbol[i] );
            }
            block->symbol[i] = WStrDup( vlist->entry->name );
        }
        WRValueListFree( vlist );
    }

    return( TRUE );
}

static Bool WResolveStringTableBlockSymIDs( WStringEditInfo *einfo,
                                            WStringBlock *block,
                                            WRHashTable *symbol_table )
{
    WRHashValue hv;
    int         i;
    Bool        replace;
    char        *text;

    if( !symbol_table ) {
        return( FALSE );
    }

    for( i=0; i<STRTABLE_STRS_PER_BLOCK; i++ ) {
        if( block->block.String[i] == NULL ) {
            continue;
        }

        text = WResIDNameToStr( block->block.String[ i ] );
        if( text == NULL ) {
            continue;
        }

        if( WRLookupName( symbol_table, block->symbol[i], &hv ) ) {
            WInsertStringData( einfo, (uint_16)hv, text, block->symbol[i],
                               &replace );
        } else {
            WInsertStringData( einfo, (uint_16)((block->blocknum & 0xfff0)+i),
                               text, block->symbol[i], &replace );
        }

        WMemFree( text );
    }

    return( TRUE );
}

Bool WResolveStringTableSymIDs( WStringEditInfo *einfo )
{
    WStringTable        *new_tbl;
    WStringTable        *old_tbl;
    WStringBlock        *block;
    LRESULT             pos;
    HWND                lbox;

    if( !einfo || !einfo->tbl ) {
        return( FALSE );
    }

    new_tbl = WAllocStringTable( einfo->tbl->is32bit );
    if( new_tbl == NULL ) {
        return( FALSE );
    }

    old_tbl = einfo->tbl;
    einfo->tbl = new_tbl;

    block = old_tbl->first_block;
    while( block ) {
        WResolveStringTableBlockSymIDs( einfo, block, einfo->info->symbol_table );
        block = block->next;
    }

    if( new_tbl->first_block == NULL ) {
        WFreeStringTable( new_tbl );
        einfo->tbl = old_tbl;
        return( FALSE );
    }

    WFreeStringTable( old_tbl );

    lbox = GetDlgItem( einfo->edit_dlg, IDM_STREDLIST );

    pos = SendMessage( lbox, LB_GETCURSEL, 0, 0 );
    if( pos == LB_ERR ) {
        pos = 0;
    }

    SendMessage( lbox, LB_RESETCONTENT, 0, 0 );
    SendMessage( lbox, WM_SETREDRAW, FALSE, 0 );
    block = einfo->tbl->first_block;
    while( block ) {
        WAddEditWinLBoxBlock( einfo, block, -1 );
        block = block->next;
    }
    SendMessage( lbox, WM_SETREDRAW, TRUE, 0 );

    einfo->current_block = NULL;
    einfo->current_string = 0;
    einfo->current_pos = -1;

    SendMessage( lbox, LB_SETCURSEL, (WPARAM) pos, 0 );

    return( TRUE );
}

