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


#include "wglbl.h"
#include "wstring.h"
#include "winfo.h"
#include "wribbon.h"
#include "wstrdup.h"
#include "wstr.h"
#include "wcopystr.h"
#include "widn2str.h"
#include "wnewitem.h"
#include "sysall.rh"

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
static  void            WMakeDataFromStringBlock( WStringBlock *block, void **data, size_t *size );
static  bool            WInitStringTable( WStringInfo *info, WStringTable *tbl );
static  bool            WMakeStringBlockFromData( void *data, size_t size, WStringBlock *block );
static  WStringBlock    *WFindStringTableBlock( WStringTable *tbl, uint_16 blocknum );
static  WStringBlock    *WAllocStringBlock( void );
static  WStringTable    *WAllocStringTable( bool is32bit );
static  void            WFreeStringTable( WStringTable *tbl );
static  void            WFreeStringTableBlock( WStringBlock *block );
static  WStringNode     *WMakeStringNodeFromStringBlock( WStringBlock * );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/

WStringEditInfo *WAllocStringEInfo( void )
{
    WStringEditInfo *einfo;

    einfo = (WStringEditInfo *)WRMemAlloc( sizeof( WStringEditInfo ) );

    if( einfo != NULL ) {
        memset( einfo, 0, sizeof( WStringEditInfo ) );
        einfo->current_pos = LB_ERR;
    }

    return( einfo );
}

void WFreeStringEInfo( WStringEditInfo *einfo )
{
    if( einfo != NULL ) {
        if( einfo->tbl != NULL ) {
            WFreeStringTable( einfo->tbl );
            einfo->tbl = NULL;
        }
        if( einfo->wsb != NULL ) {
            WDestroyStatusLine( einfo->wsb );
            einfo->wsb = NULL;
        }
        if( einfo->ribbon != NULL ) {
            WDestroyRibbon( einfo );
        }
        if( einfo->edit_dlg != (HWND)NULL && IsWindow( einfo->edit_dlg ) ) {
            DestroyWindow( einfo->edit_dlg );
            einfo->edit_dlg = (HWND)NULL;
        }
        if( einfo->win != (HWND)NULL && IsWindow( einfo->win ) ) {
            SET_WNDINFO( einfo->win, 0 );
            DestroyWindow( einfo->win );
            einfo->win = (HWND)NULL;
        }
        if( einfo->file_name != NULL ) {
            WRMemFree( einfo->file_name );
        }
        WRMemFree( einfo );
    }
}

bool WIsBlockEmpty( WStringBlock *block )
{
    return( WRIsBlockEmpty( &block->block ) );
}

WStringBlock *WFindStringBlock( WStringTable *tbl, uint_16 blocknum )
{
    WStringBlock *block;

    if( tbl != NULL ) {
        for( block = tbl->first_block; block != NULL; block = block->next ) {
            if( (block->blocknum & 0xfff0) == (blocknum & 0xfff0) ) {
                return( block );
            }
        }
    }

    return( NULL );
}

LRESULT WFindStringPos( WStringTable *tbl, uint_16 string_id )
{
    WStringBlock        *block;
    LRESULT             pos;
    int                 i;

    pos = 0;
    if( tbl != NULL ) {
        for( block = tbl->first_block; block != NULL; block = block->next ) {
            for( i = 0; i < STRTABLE_STRS_PER_BLOCK; i++ ) {
                if( block->block.String[i] != NULL ) {
                    if( (block->blocknum & 0xfff0) + i == string_id ) {
                        return( pos );
                    }
                    pos++;
                }
            }
        }
    }

    return( LB_ERR );
}

WStringBlock *WFindStringTableBlock( WStringTable *tbl, uint_16 blocknum )
{
    WStringBlock *block;
    WStringBlock *last;

    last = NULL;
    if( tbl != NULL ) {
        for( block = tbl->first_block; block != NULL; block = block->next ) {
            if( block->blocknum > blocknum ) {
                break;
            } else if( block->blocknum == blocknum ) {
                return( block );
            }
            last = block;
        }
    }

    return( last );
}

void WMakeDataFromStringBlock( WStringBlock *block, void **data, size_t *size )
{
    if( block != NULL ) {
        WRMakeDataFromStringBlock( &block->block, data, size, block->is32bit );
    }
}

bool WMakeStringBlockFromData( void *data, size_t size, WStringBlock *block )
{
    bool ret;

    if( data == NULL || size == 0 || block == NULL ) {
        return( false );
    }

    ret = WRMakeStringBlockFromData( &block->block, data, size, block->is32bit );

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

    if( after != NULL) {
        if( (after->blocknum & 0xfff0) == (blocknum & 0xfff0) ) {
            return( after );
        }
    }

    block = WAllocStringBlock();
    if( block == NULL ) {
        return( NULL );
    }
    block->is32bit = tbl->is32bit;
    block->blocknum = blocknum & 0xfff0;

    if( after != NULL ) {
        if( after->next != NULL ) {
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

bool WRemoveStringBlock( WStringTable *tbl, WStringBlock *block )
{
    bool ok;

    ok = (tbl != NULL && block != NULL);

    if( ok ) {
        if( block->next != NULL ) {
            block->next->prev = block->prev;
        }
        if( block->prev != NULL ) {
            block->prev->next = block->next;
        }
        if( tbl->first_block == block ) {
            tbl->first_block = block->next;
        }
        WFreeStringTableBlock( block );
    }

    return( ok );
}

WStringBlock *WAllocStringBlock( void )
{
    WStringBlock *block;

    block = (WStringBlock *)WRMemAlloc( sizeof( WStringBlock ) );
    if( block != NULL ) {
        memset( block, 0, sizeof( WStringBlock ) );
    }

    return( block );
}

void WFreeStringTable( WStringTable *tbl )
{
    if( tbl != NULL ) {
        WFreeStringTableBlocks( tbl->first_block );
        WRMemFree( tbl );
    }
}

void WFreeStringTableBlocks( WStringBlock *block )
{
    WStringBlock *next;

    for( ; block != NULL; block = next ) {
        next = block->next;
        WFreeStringTableBlock( block );
    }
}

void WFreeStringTableBlock( WStringBlock *block )
{
    int i;

    if( block != NULL ) {
        for( i = 0; i < STRTABLE_STRS_PER_BLOCK; i++ ) {
            if( block->symbol[i] != NULL ) {
                WRMemFree( block->symbol[i] );
            }
        }
        ResFreeStringTableBlock( &block->block );
        WRMemFree( block );
    }
}

static void WFreeStringNode( WStringNode *node )
{
    WStringNode *next;

    for( ; node != NULL; node = next ) {
        next = node->next;
        if( node->block_name != NULL ) {
            WRMemFree( node->block_name );
        }
        if( node->data != NULL ) {
            WRMemFree( node->data );
        }
        WRMemFree( node );
    }
}

WStringNode *WMakeStringNodeFromStringBlock( WStringBlock *block )
{
    WStringNode *node;

    if( block == NULL ) {
        return( NULL );
    }

    node = (WStringNode *)WRMemAlloc( sizeof( WStringNode ) );
    if( node == NULL ) {
        return( NULL );
    }
    memset( node, 0, sizeof( WStringNode ) );
    node->block_name = WResIDFromNum( block->blocknum / 16 + 1 );
    node->MemFlags = block->MemFlags;
    WMakeDataFromStringBlock( block, &node->data, &node->data_size );
    if( node->data == NULL ) {
        WFreeStringNode( node );
        return( NULL );
    }

    return( node );
}

void WFreeStringNodes( WStringInfo *info )
{
    WFreeStringNode( info->tables );
    info->tables = NULL;
}

WStringNode *WMakeStringNodes( WStringTable *tbl )
{
    WStringNode         *node;
    WStringNode         *new;
    WStringBlock        *block;

    node = NULL;
    for( block = tbl->first_block; block != NULL; block = block->next ) {
        new = WMakeStringNodeFromStringBlock( block );
        if( new == NULL ) {
            WFreeStringNode( node );
            return( NULL );
        }
        if( node != NULL ) {
            new->next = node;
            node = new;
        } else {
            node = new;
        }
    }

    return( node );
}

WStringTable *WAllocStringTable( bool is32bit )
{
    WStringTable        *tbl;

    tbl = (WStringTable *)WRMemAlloc( sizeof( WStringTable ) );

    if( tbl == NULL ) {
        return( NULL );
    }

    tbl->is32bit = is32bit;
    tbl->first_block = NULL;

    return( tbl );
}

bool WInitStringTable( WStringInfo *info, WStringTable *tbl )
{
    WStringBlock        *block;
    WStringNode         *node;
    uint_16             blocknum;

    if( info == NULL || tbl == NULL ) {
        return( false );
    }

    for( node = info->tables; node != NULL; node = node->next ) {
        blocknum = (uint_16)WResIDToNum( node->block_name );
        blocknum = (blocknum - 1) * 16;
        if( WFindStringBlock( tbl, blocknum ) ) {
            return( false );
        }
        block = WGetOrMakeStringBlock( tbl, blocknum );
        if( block == NULL ) {
            return( false );
        }
        if( !WMakeStringBlockFromData( node->data, node->data_size, block ) ) {
            return( false );
        }
    }

    return( true );
}

WStringTable *WMakeStringTableFromInfo( WStringInfo *info )
{
    WStringTable        *tbl;
    bool                ok;

    tbl = NULL;

    ok = (info != NULL);

    if( ok ) {
        tbl = WAllocStringTable( info->is32bit );
        ok = (tbl != NULL);
    }

    if( ok ) {
        ok = WInitStringTable( info, tbl );
    }

    if( ok ) {
        //WFreeStringNodes( info );
    } else {
        if( tbl != NULL ) {
            WFreeStringTable( tbl );
            tbl = NULL;
        }
    }

    return( tbl );
}

bool WGetFirstStringInBlock( WStringBlock *block, uint_16 *first )
{
    int         i;

    if( block != NULL && first != NULL ) {
        for( i = 0; i < STRTABLE_STRS_PER_BLOCK; i++ ) {
            if( block->block.String[i] != NULL ) {
                *first = (block->blocknum & 0xfff0) + i;
                return( TRUE );
            }
        }
    }

    return( FALSE );
}

static WStringBlock *WFindLargestBlock( WStringTable *tbl )
{
    WStringBlock *block;
    WStringBlock *largest;

    largest = NULL;
    if( tbl != NULL ) {
        for( block = tbl->first_block; block != NULL; block = block->next ) {
            if( largest == NULL || block->blocknum > largest->blocknum ) {
                largest = block;
            }
        }
    }

    return( largest );
}

uint_16 WFindLargestStringID( WStringTable *tbl )
{
    WStringBlock        *largest;
    int                 i;

    largest = WFindLargestBlock( tbl );
    if( largest != NULL ) {
        for( i = STRTABLE_STRS_PER_BLOCK - 1; i >=0 ; i-- ) {
            if( largest->block.String[i] != NULL ) {
                return( (largest->blocknum & 0xfff0) + i );
            }
        }
        return( largest->blocknum & 0xfff0 );
    }

    return( 0 );
}

bool WResolveStringTable( WStringEditInfo *einfo )
{
    WStringBlock *block;

    if( einfo == NULL || einfo->tbl == NULL ) {
        return( FALSE );
    }

    for( block = einfo->tbl->first_block; block != NULL; block = block->next ) {
        WResolveStringTableBlock( block, einfo->info->symbol_table );
    }

    return( TRUE );
}

bool WResolveStringTableBlock( WStringBlock *block, WRHashTable *symbol_table )
{
    WRHashValueList     *vlist;
    int                 i;

    if( block == NULL || symbol_table == NULL ) {
        return( FALSE );
    }

    for( i = 0; i < STRTABLE_STRS_PER_BLOCK; i++ ) {
        if( block->block.String[i] == NULL ) {
            continue;
        }
        vlist = WRLookupValue( symbol_table, (block->blocknum & 0xfff0) + i );
        if( vlist == NULL ) {
            continue;
        }
        if( vlist->next == NULL ) {
            if( block->symbol[i] != NULL ) {
                WRMemFree( block->symbol[i] );
            }
            block->symbol[i] = WStrDup( vlist->entry->name );
        }
        WRValueListFree( vlist );
    }

    return( TRUE );
}

static bool WResolveStringTableBlockSymIDs( WStringEditInfo *einfo, WStringBlock *block,
                                            WRHashTable *symbol_table )
{
    WRHashValue hv;
    int         i;
    bool        replace;
    char        *text;

    if( symbol_table == NULL ) {
        return( FALSE );
    }

    for( i = 0; i < STRTABLE_STRS_PER_BLOCK; i++ ) {
        if( block->block.String[i] == NULL ) {
            continue;
        }

        text = WResIDNameToStr( block->block.String[i] );
        if( text == NULL ) {
            continue;
        }

        if( WRLookupName( symbol_table, block->symbol[i], &hv ) ) {
            WInsertStringData( einfo, (uint_16)hv, text, block->symbol[i], &replace );
        } else {
            WInsertStringData( einfo, (uint_16)((block->blocknum & 0xfff0) + i),
                               text, block->symbol[i], &replace );
        }

        WRMemFree( text );
    }

    return( TRUE );
}

bool WResolveStringTableSymIDs( WStringEditInfo *einfo )
{
    WStringTable        *new_tbl;
    WStringTable        *old_tbl;
    WStringBlock        *block;
    LRESULT             pos;
    HWND                lbox;

    if( einfo == NULL || einfo->tbl == NULL ) {
        return( FALSE );
    }

    new_tbl = WAllocStringTable( einfo->tbl->is32bit );
    if( new_tbl == NULL ) {
        return( FALSE );
    }

    old_tbl = einfo->tbl;
    einfo->tbl = new_tbl;

    for( block = old_tbl->first_block; block != NULL; block = block->next ) {
        WResolveStringTableBlockSymIDs( einfo, block, einfo->info->symbol_table );
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
    for( block = einfo->tbl->first_block; block != NULL; block = block->next ) {
        WAddEditWinLBoxBlock( einfo, block, LB_ERR );
    }
    SendMessage( lbox, WM_SETREDRAW, TRUE, 0 );

    einfo->current_block = NULL;
    einfo->current_string = 0;
    einfo->current_pos = LB_ERR;

    SendMessage( lbox, LB_SETCURSEL, (WPARAM)pos, 0 );

    return( TRUE );
}
