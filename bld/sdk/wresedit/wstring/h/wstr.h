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


#ifndef WSTR_INCLUDED
#define WSTR_INCLUDED

#include "wstring.h"
#include "winfo.h"
#include "wstat.h"
#include "wtoolbar.h"
#include "wrdll.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* data types                                                               */
/****************************************************************************/
typedef struct WStringBlock {
    bool                is32bit;
    uint_16             blocknum;
    uint_16             MemFlags;
    StringTableBlock    block;
    char                *symbol[STRTABLE_STRS_PER_BLOCK];
    struct WStringBlock *next;
    struct WStringBlock *prev;
} WStringBlock;

typedef struct WStringTable {
    bool                is32bit;
    WStringBlock        *first_block;
} WStringTable;

typedef struct WStringEditInfo {
    WStringHandle       hndl;
    WStringInfo         *info;
    WStringTable        *tbl;
    WStringBlock        *current_block;
    uint_16             current_string;
    int                 current_pos;
    HWND                win;
    HWND                edit_dlg;
    WStatBar            *wsb;
    WToolBar            *ribbon;
    bool                show_ribbon;
    char                *file_name;
    WRFileType          file_type;
    bool                combo_change;
} WStringEditInfo;

/****************************************************************************/
/* function prototypes                                                      */
/****************************************************************************/
extern WStringEditInfo  *WAllocStringEInfo( void );
extern void             WFreeStringEInfo( WStringEditInfo *einfo );
extern WStringTable     *WMakeStringTableFromInfo( WStringInfo *info );
extern void             WFreeStringNodes( WStringInfo *info );
extern WStringNode      *WMakeStringNodes( WStringTable *tbl );
extern bool             WGetFirstStringInBlock( WStringBlock *block, uint_16 * );
extern WStringBlock     *WGetOrMakeStringBlock( WStringTable *tbl, uint_16 blocknum );
extern WStringBlock     *WFindStringBlock( WStringTable *tbl, uint_16 blocknum );
extern bool             WIsBlockEmpty( WStringBlock *block );
extern bool             WRemoveStringBlock( WStringTable *tbl, WStringBlock *block );
extern int              WFindStringPos( WStringTable *tbl, uint_16 string_id );
extern void             WFreeStringTableBlocks( WStringBlock *block );
extern uint_16          WFindLargestStringID( WStringTable *tbl );

extern bool WResolveStringTable( WStringEditInfo *einfo );
extern bool WResolveStringTableBlock( WStringBlock *block, WRHashTable *symbol_table );

extern bool WResolveStringTableSymIDs( WStringEditInfo *einfo );

#endif
