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


#include "wdeglbl.h"
#include <ctype.h>
#include "wdedebug.h"
#include "wdesdup.h"
#include "wdelist.h"
#include "wdestat.h"
#include "rcstr.gh"
#include "wdemain.h"
#include "wdewait.h"
#include "wdehash.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define HASH_TOUCHED            0x0001
#define HASH_SAVE_REJECT        0x0002

#define TRUE            1
#define FALSE           0

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* external variables                                                       */
/****************************************************************************/

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/

uint_32 WdeNumInHashTable( WdeHashTable *table )
{
    return( WRNumInHashTable( table ) );
}

bool WdeIsHashTableDirty( WdeHashTable *table )
{
    return( WRIsHashTableDirty( table ) );
}

void WdeMakeHashTableClean( WdeHashTable *table )
{
    WRMakeHashTableClean( table );
}

bool WdeIsHashTableTouched( WdeHashTable *table )
{
    return( table != NULL && (table->user_flags & HASH_TOUCHED) );
}

void WdeUntouchHashTable( WdeHashTable *table )
{
    if( table != NULL ) {
        table->user_flags &= ~HASH_TOUCHED;
    }
}

void WdeTouchHashTable( WdeHashTable *table )
{
    if( table != NULL ) {
        table->user_flags |= HASH_TOUCHED;
    }
}

void WdeHashClearSaveRejected( WdeHashTable *table )
{
    if( table != NULL ) {
        table->user_flags &= ~HASH_SAVE_REJECT;
    }
}

void WdeHashSaveRejected( WdeHashTable *table )
{
    if( table != NULL ) {
        table->user_flags |= HASH_SAVE_REJECT;
    }
}

bool WdeIsHashSaveRejectedSet( WdeHashTable *table )
{
    return( table != NULL && (table->user_flags & HASH_SAVE_REJECT) );
}

WdeHashTable *WdeInitHashTable( void )
{
    return( WRInitHashTable() );
}

void WdeFreeHashTable( WdeHashTable *table )
{
    WRFreeHashTable( table );
}

bool WdeMergeHashTables( WdeHashTable **dest, WdeHashTable *src )
{
    WdeTouchHashTable( *dest );
    return( WRMergeHashTable( dest, src ) != 0 );
}

bool WdeCopyHashTable( WdeHashTable **dest, WdeHashTable *src )
{
    WdeTouchHashTable( *dest );
    return( WRCopyHashTable( dest, src ) != 0 );
}

void WdeAddSymbolsToListBox( WdeHashTable *table, HWND hDlg, int id )
{
    WRAddSymbolsToListBox( table, hDlg, id, WR_HASHENTRY_ALL );
}

bool WdeWriteSymbolsToFile( WdeHashTable *table, char *name )
{
    return( WRWriteSymbolsToFile( table, name ) != 0 );
}

void WdeAddSymbolsToComboBox( WdeHashTable *table, HWND hDlg, int id )
{
    WRAddSymbolsToComboBox( table, hDlg, id, WR_HASHENTRY_ALL );
}

WdeHashEntry *WdeDefAddHashEntry( WdeHashTable *table, char *name, bool *dup )
{
    WdeHashEntry        *entry;
    BOOL                d;

    d = FALSE;
    if( dup != NULL ) {
        d = *dup;
    }

    WdeTouchHashTable( table );

    entry = WRAddDefHashEntry( table, name, &d );

    if( dup != NULL ) {
        d = *dup;
    }

    return( entry );
}

WdeHashEntry *WdeAddHashEntry( WdeHashTable *table, char *name,
                               WdeHashValue value, bool *dup )
{
    WdeHashEntry        *entry;
    BOOL                d;

    d = FALSE;
    if( dup != NULL ) {
        d = *dup;
    }

    WdeTouchHashTable( table );

    entry = WRAddHashEntry( table, name, value, &d, FALSE, FALSE );

    if( dup != NULL ) {
        d = *dup;
    }

    return( entry );
}

bool WdeRemoveName( WdeHashTable *table, char *name )
{
    WdeTouchHashTable( table );
    return( WRRemoveName( table, name ) != 0 );
}

WdeHashValue WdeLookupName( WdeHashTable *table, char *name, bool *found )
{
    WdeHashValue        val;

    *found = WRLookupName( table, name, &val ) != 0;

    if( !*found ) {
        return( (WdeHashValue)0 );
    }

    return( val );
}

bool WdeModifyName( WdeHashTable *table, char *name, WdeHashValue value )
{
    WdeTouchHashTable( table );
    return( WRModifyName( table, name, value, FALSE ) != 0 );
}

char *WdeResolveValue( WdeHashTable *table, WdeHashValue value )
{
    return( WRResolveValue( table, value ) );
}

bool WdeIsValidSymbol( char *symbol )
{
    return( WRIsValidSymbol( symbol ) );
}
