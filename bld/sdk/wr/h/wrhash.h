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
    WR_HASH_DIRTY       = 0x0001
,   WR_HASH_LAST        = 0xffff
} WRHashFlags;

typedef enum {
    WR_HASHENTRY_STANDARD       = 0x0001
,   WR_HASHENTRY_UNUSED         = 0x0002
,   WR_HASHENTRY_ALL            = 0x0003
,   WR_HASHENTRY_LAST           = 0xffff
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
extern WRHashTable      * WR_EXPORT WRInitHashTable     ( void );
extern void             WR_EXPORT WRFreeHashTable       ( WRHashTable *);
extern BOOL             WR_EXPORT WRFindUnusedHashValue ( WRHashTable *table, WRHashValue *value, WRHashValue start );
extern WRHashEntry      * WR_EXPORT WRAddDefHashEntry   ( WRHashTable *table, char *name, BOOL *dup );
extern WRHashEntry      * WR_EXPORT WRAddHashEntry      ( WRHashTable *, char *, WRHashValue, BOOL *, BOOL, BOOL );
extern int              WR_EXPORT WRModifyName          ( WRHashTable *table, char *name, WRHashValue value, BOOL check_unique );
extern BOOL             WR_EXPORT WRLookupName          ( WRHashTable *table, char *name, WRHashValue *value );
extern BOOL             WR_EXPORT WRRemoveName          ( WRHashTable *, char *);
extern unsigned long    WR_EXPORT WRNumInHashTable      ( WRHashTable * );
extern BOOL             WR_EXPORT WRIsDefaultHashTable  ( WRHashTable *table );
extern void             WR_EXPORT WRMakeHashTableClean  ( WRHashTable * );
extern BOOL             WR_EXPORT WRIsHashTableDirty    ( WRHashTable * );
extern unsigned         WR_EXPORT WRAddSymbolsToListBox ( WRHashTable *, HWND, int, WRHashEntryFlags );
extern unsigned         WR_EXPORT WRAddSymbolsToComboBox( WRHashTable *, HWND, int, WRHashEntryFlags );
extern BOOL             WR_EXPORT WRWriteSymbolsToFile  ( WRHashTable *, char * );
extern BOOL             WR_EXPORT WRMergeHashTable      ( WRHashTable **, WRHashTable * );
extern BOOL             WR_EXPORT WRCopyHashTable       ( WRHashTable **, WRHashTable * );
extern void             WR_EXPORT WRHashIncRefCount     ( WRHashEntry * );
extern void             WR_EXPORT WRHashDecRefCount     ( WRHashEntry * );
extern void             WR_EXPORT WRStripSymbol         ( char *symbol );
extern BOOL             WR_EXPORT WRIsValidSymbol       ( char *symbol );
extern void             WR_EXPORT WRValueListFree       ( WRHashValueList *list );
extern WRHashValueList  * WR_EXPORT WRLookupValue       ( WRHashTable *table, WRHashValue value );
extern int              WR_EXPORT WRValueExists         ( WRHashTable *table, WRHashValue value );
extern char             * WR_EXPORT WRResolveValue      ( WRHashTable *, WRHashValue );
extern BOOL             WR_EXPORT WREditSym             ( HWND parent, WRHashTable **table, WRHashEntryFlags *flags, FARPROC help_callback );
extern BOOL             WR_EXPORT WRDeleteDLGInclude    ( WResDir dir );
extern BOOL             WR_EXPORT WRCreateDLGInclude    ( WResDir *dir, char *include );

#endif
