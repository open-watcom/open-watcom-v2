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


#ifndef WRHASH_INCLUDED
#define WRHASH_INCLUDED

#include <stdio.h>

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define NAME_SIZE  211
#define VALUE_SIZE 61

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef enum {
    WR_HASH_DIRTY       = 0x0001,
    WR_HASH_LAST        = 0xffff
} WRHashFlags;

typedef enum {
    WR_HASHENTRY_STANDARD       = 0x0001,
    WR_HASHENTRY_UNUSED         = 0x0002,
    WR_HASHENTRY_ALL            = 0x0003,
    WR_HASHENTRY_LAST           = 0xffff
} WRHashEntryFlags;

typedef signed long WRHashValue;

typedef struct WRHashEntryStruct {
    struct WRHashEntryStruct    *name_next;
    struct WRHashEntryStruct    *name_prev;
    struct WRHashEntryStruct    *value_next;
    struct WRHashEntryStruct    *value_prev;
    unsigned long               ref_count;
    WRHashValue                 value;
    WRHashEntryFlags            flags;
    char                        name[1];
} WRHashEntry;

typedef struct WRHashValueList {
    struct WRHashValueList      *next;
    WRHashEntry                 *entry;
} WRHashValueList;

typedef struct {
    WRHashEntry         *names[NAME_SIZE];
    WRHashEntry         *values[VALUE_SIZE];
    unsigned long       count;
    WRHashValue         next_default_value;
    WRHashFlags         flags;
    unsigned long       user_flags;
} WRHashTable;

/****************************************************************************/
/* function prototypes                                                      */
/****************************************************************************/
WRDLLENTRY extern WRHashTable *        WRAPI WRInitHashTable( void );
WRDLLENTRY extern void                 WRAPI WRFreeHashTable( WRHashTable *);
WRDLLENTRY extern BOOL                 WRAPI WRFindUnusedHashValue( WRHashTable *table, WRHashValue *value, WRHashValue start );
WRDLLENTRY extern WRHashEntry *        WRAPI WRAddDefHashEntry( WRHashTable *table, char *name, BOOL *dup );
WRDLLENTRY extern WRHashEntry *        WRAPI WRAddHashEntry( WRHashTable *, char *, WRHashValue, BOOL *, BOOL, BOOL );
WRDLLENTRY extern int                  WRAPI WRModifyName( WRHashTable *table, char *name, WRHashValue value, BOOL check_unique );
WRDLLENTRY extern BOOL                 WRAPI WRLookupName( WRHashTable *table, char *name, WRHashValue *value );
WRDLLENTRY extern BOOL                 WRAPI WRRemoveName( WRHashTable *, char *);
WRDLLENTRY extern unsigned long        WRAPI WRNumInHashTable( WRHashTable * );
WRDLLENTRY extern BOOL                 WRAPI WRIsDefaultHashTable( WRHashTable *table );
WRDLLENTRY extern void                 WRAPI WRMakeHashTableClean( WRHashTable * );
WRDLLENTRY extern BOOL                 WRAPI WRIsHashTableDirty( WRHashTable * );
WRDLLENTRY extern unsigned             WRAPI WRAddSymbolsToListBox( WRHashTable *, HWND, int, WRHashEntryFlags );
WRDLLENTRY extern unsigned             WRAPI WRAddSymbolsToComboBox( WRHashTable *, HWND, int, WRHashEntryFlags );
WRDLLENTRY extern BOOL                 WRAPI WRWriteSymbolsToFile( WRHashTable *, char * );
WRDLLENTRY extern BOOL                 WRAPI WRMergeHashTable( WRHashTable **, WRHashTable * );
WRDLLENTRY extern BOOL                 WRAPI WRCopyHashTable( WRHashTable **, WRHashTable * );
WRDLLENTRY extern void                 WRAPI WRHashIncRefCount( WRHashEntry * );
WRDLLENTRY extern void                 WRAPI WRHashDecRefCount( WRHashEntry * );
WRDLLENTRY extern void                 WRAPI WRStripSymbol( char *symbol );
WRDLLENTRY extern BOOL                 WRAPI WRIsValidSymbol( char *symbol );
WRDLLENTRY extern void                 WRAPI WRValueListFree( WRHashValueList *list );
WRDLLENTRY extern WRHashValueList *    WRAPI WRLookupValue( WRHashTable *table, WRHashValue value );
WRDLLENTRY extern int                  WRAPI WRValueExists( WRHashTable *table, WRHashValue value );
WRDLLENTRY extern char *               WRAPI WRResolveValue( WRHashTable *, WRHashValue );
WRDLLENTRY extern BOOL                 WRAPI WREditSym( HWND parent, WRHashTable **table, WRHashEntryFlags *flags, FARPROC help_callback );
WRDLLENTRY extern BOOL                 WRAPI WRDeleteDLGInclude( WResDir dir );
WRDLLENTRY extern BOOL                 WRAPI WRCreateDLGInclude( WResDir *dir, char *include );

#endif
