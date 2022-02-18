/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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
#include "rcstr.grh"
#include "wdemain.h"
#include "wdewait.h"
#include "wdehash.h"


/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define HASH_TOUCHED            0x0001
#define HASH_SAVE_REJECT        0x0002

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

bool WdeIsHashTableTouched( WRHashTable *table )
{
    return( table != NULL && (table->user_flags & HASH_TOUCHED) );
}

void WdeUntouchHashTable( WRHashTable *table )
{
    if( table != NULL ) {
        table->user_flags &= ~HASH_TOUCHED;
    }
}

void WdeTouchHashTable( WRHashTable *table )
{
    if( table != NULL ) {
        table->user_flags |= HASH_TOUCHED;
    }
}

void WdeHashClearSaveRejected( WRHashTable *table )
{
    if( table != NULL ) {
        table->user_flags &= ~HASH_SAVE_REJECT;
    }
}

void WdeHashSaveRejected( WRHashTable *table )
{
    if( table != NULL ) {
        table->user_flags |= HASH_SAVE_REJECT;
    }
}

bool WdeIsHashSaveRejectedSet( WRHashTable *table )
{
    return( table != NULL && (table->user_flags & HASH_SAVE_REJECT) );
}

#if 0
bool WdeMergeHashTables( WRHashTable **dest, WRHashTable *src )
{
    WdeTouchHashTable( *dest );
    return( WRMergeHashTable( dest, src ) != 0 );
}
#endif

bool WdeCopyHashTable( WRHashTable **dest, WRHashTable *src )
{
    WdeTouchHashTable( *dest );
    return( WRCopyHashTable( dest, src ) != 0 );
}

WRHashEntry *WdeDefAddHashEntry( WRHashTable *table, const char *name, bool *dup )
{
    WRHashEntry         *entry;
    bool                d;

    d = false;
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

WRHashEntry *WdeAddHashEntry( WRHashTable *table, const char *name,
                               WRHashValue value, bool *dup )
{
    WRHashEntry         *entry;
    bool                d;

    d = false;
    if( dup != NULL ) {
        d = *dup;
    }

    WdeTouchHashTable( table );

    entry = WRAddHashEntry( table, name, value, &d, false, false );

    if( dup != NULL ) {
        d = *dup;
    }

    return( entry );
}

bool WdeRemoveName( WRHashTable *table, const char *name )
{
    WdeTouchHashTable( table );
    return( WRRemoveName( table, name ) != 0 );
}

WRHashValue WdeLookupName( WRHashTable *table, const char *name, bool *found )
{
    WRHashValue         val;

    *found = WRLookupName( table, name, &val ) != 0;

    if( !*found ) {
        return( (WRHashValue)0 );
    }

    return( val );
}

#if 0
bool WdeModifyName( WRHashTable *table, const char *name, WRHashValue value )
{
    WdeTouchHashTable( table );
    return( WRModifyName( table, name, value, false ) != 0 );
}
#endif
