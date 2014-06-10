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


#ifndef WDEHASH_INCLUDED
#define WDEHASH_INCLUDED

#include "wrdll.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef WRHashValue WdeHashValue;

typedef WRHashEntry WdeHashEntry;

typedef WRHashTable WdeHashTable;

/****************************************************************************/
/* function prototypes                                                      */
/****************************************************************************/
extern bool WdeIsHashSaveRejectedSet( WdeHashTable *table );
extern void WdeHashClearSaveRejected( WdeHashTable *table );
extern void WdeHashSaveRejected( WdeHashTable *table );
extern bool WdeIsHashTableTouched( WdeHashTable *table );
extern void WdeUntouchHashTable( WdeHashTable *table );
extern void WdeTouchHashTable( WdeHashTable *table );
extern bool WdeIsValidSymbol( char *symbol );

extern uint_32      WdeNumInHashTable( WdeHashTable * );
extern void         WdeMakeHashTableClean( WdeHashTable * );
extern bool         WdeIsHashTableDirty( WdeHashTable * );
extern WdeHashTable *WdeInitHashTable( void );
extern void         WdeFreeHashTable( WdeHashTable * );
extern bool         WdeRemoveName( WdeHashTable *, char * );
extern WdeHashEntry *WdeDefAddHashEntry( WdeHashTable *table, char *name, bool *dup );
extern WdeHashEntry *WdeAddHashEntry( WdeHashTable *, char *, WdeHashValue, bool * );
extern WdeHashValue WdeLookupName( WdeHashTable *, char *, bool * );
extern char         *WdeResolveValue( WdeHashTable *, WdeHashValue );
extern void         WdeAddSymbolsToListBox( WdeHashTable *, HWND, int );
extern void         WdeAddSymbolsToComboBox( WdeHashTable *, HWND, int );
extern bool         WdeWriteSymbolsToFile( WdeHashTable *, char * );
extern bool         WdeMergeHashTable( WdeHashTable **, WdeHashTable * );
extern bool         WdeCopyHashTable( WdeHashTable **, WdeHashTable * );

#endif
