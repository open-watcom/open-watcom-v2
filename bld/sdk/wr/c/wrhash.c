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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "watcom.h"
#include "wrglbl.h"
#include "wrmaini.h"
#include "wrstrdup.h"
#include "wrmsg.h"
#include "wredit.h"
#include "wrlist.h"
#include "editsym.h"
#include "addsym.h"
#include "jdlg.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define FIRST_DEFAULT_ID        101

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef struct SymInitStruct {
    char        *symbol;
    int         value;
} SymInitStruct;

typedef struct WREditSymInfo {
    FARPROC             hcb;
    WRHashTable         *table;
    BOOL                modified;
    WRHashEntryFlags    flags;
} WREditSymInfo;

typedef struct WRAddSymInfo {
    FARPROC     hcb;
    WRHashTable *table;
    char        *symbol;
    WRHashValue value;
    BOOL        modify;
} WRAddSymInfo;

/****************************************************************************/
/* external variables                                                       */
/****************************************************************************/

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/
WINEXPORT BOOL CALLBACK WREditSymbolsProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam );
WINEXPORT BOOL CALLBACK WRAddSymProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam );

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static SymInitStruct SymInit[] = {
    { "IDOK",           IDOK          },
    { "IDCANCEL",       IDCANCEL      },
    { "IDABORT",        IDABORT       },
    { "IDRETRY",        IDRETRY       },
    { "IDIGNORE",       IDIGNORE      },
    { "IDYES",          IDYES         },
    { "IDNO",           IDNO          },
    { NULL,             -1            }
};

#define NUM_INIT_ENTRIES (sizeof( SymInit ) / sizeof( SymInitStruct ) - 1)

static unsigned int WRGetNameHash( char *name )
{
    unsigned int        hash;

    hash = 0;

    for( ; *name != '\0'; name++ ) {
        hash = (hash << 4) + toupper( *name );
        hash = (hash ^ _rotl( hash & 0xF000, 4 )) & 0x0FFF;
    }

    return( hash % NAME_SIZE );
}

static unsigned int WRGetValueHash( WRHashValue value )
{
    return( (unsigned int)value % VALUE_SIZE );
}

static WRHashEntry *WRFindHashEntryFromName( WRHashTable *table, char *name )
{
    WRHashEntry         *entry;
    unsigned int        hash;

    if( table == NULL || name == NULL ) {
        return( NULL );
    }

    hash = WRGetNameHash( name );
    for( entry = table->names[hash]; entry != NULL; entry = entry->name_next ) {
        if( stricmp( entry->name, name ) == 0 ) {
            return( entry );
        }
    }

    return( NULL );
}

static int WRCompareHashEntry( void const *_e1, void const *_e2 )
{
    WRHashEntry * const *e1 = _e1;
    WRHashEntry * const *e2 = _e2;

    if( e1 != NULL && e2 != NULL && *e1 != NULL && *e2 != NULL ) {
        if( (*e1)->value < (*e2)->value ) {
            return( -1 );
        } else if( (*e1)->value > (*e2)->value ) {
            return( 1 );
        }
    }

    return( 0 );
}

BOOL WRAPI WRIsDefaultHashTable( WRHashTable *table )
{
    if( table != NULL ) {
        return( table->count == NUM_INIT_ENTRIES );
    }
    return( TRUE );
}

unsigned long WRAPI WRNumInHashTable( WRHashTable *table )
{
    if( table != NULL ) {
        return( table->count );
    }
    return( 0 );
}

BOOL WRAPI WRIsHashTableDirty( WRHashTable *table )
{
    return( table != NULL && (table->flags & WR_HASH_DIRTY) );
}

void WRAPI WRMakeHashTableClean( WRHashTable *table )
{
    if( table != NULL ) {
        table->flags &= ~WR_HASH_DIRTY;
    }
}

BOOL WRAPI WRDeleteDLGInclude( WResDir dir )
{
    WResTypeNode        *tnode;
    WResResNode         *rnode;
    WResLangNode        *lnode;
    WResLangType        lang;
    BOOL                ok;

    ok = (dir != NULL);

    if( ok ) {
        tnode = WRFindTypeNode( dir, (uint_16)RT_RCDATA, NULL );
        ok = (tnode != NULL);
    }

    if( ok ) {
        rnode = WRFindResNode( tnode, 0, "DLGINCLUDE" );
        ok = (rnode != NULL);
    }

    if( ok ) {
        lang.lang = DEF_LANG;
        lang.sublang = DEF_SUBLANG;
        lnode = WRFindLangNodeFromLangType( rnode, &lang );
        ok = (lnode != NULL);
    }

    if( ok ) {
        ok = WRRemoveLangNodeFromDir( dir, &tnode, &rnode, &lnode );
    }

    return( ok );
}

BOOL WRAPI WRCreateDLGInclude( WResDir *dir, char *include )
{
    WResID              *type;
    WResID              *res;
    WResLangType        lang;
    char                *str;
    int                 len;
    BOOL                ok;

    type = NULL;
    res = NULL;
    str = NULL;
    ok = (dir != NULL && include != NULL);

    if( ok ) {
        if( *dir == NULL ) {
            *dir = WResInitDir();
            ok = (*dir != NULL);
        }
    }

    if( ok ) {
        WRDeleteDLGInclude( *dir );
        type = WResIDFromNum( (uint_16)RT_RCDATA );
        ok = (type != NULL);
    }

    if( ok ) {
        res = WResIDFromStr( "DLGINCLUDE" );
        ok = (res != NULL);
    }

    if( ok ) {
        str = WRStrDup( include );
        ok = (str != NULL);
    }

    if( ok ) {
        lang.lang = DEF_LANG;
        lang.sublang = DEF_SUBLANG;
        len = strlen( include ) + 1;
        ok = !WResAddResource( type, res, MEMFLAG_DISCARDABLE, 0, len, *dir, &lang, NULL );
    }

    if( ok ) {
        ok = WRFindAndSetData( *dir, type, res, &lang, str );
    }

    if( type != NULL ) {
        WRMemFree( type );
    }

    if( res != NULL ) {
        WRMemFree( res );
    }

    if( !ok ) {
        if( str != NULL ) {
            WRMemFree( str );
        }
    }

    return( ok );
}

static void WRAddSymInitInfo( WRHashTable *table )
{
    BOOL        dup;
    int         i;
    WRHashEntry *new;

    for( i = 0; SymInit[i].symbol != NULL; i++ ) {
        dup = FALSE;
        new = WRAddHashEntry( table, SymInit[i].symbol, SymInit[i].value,
                              &dup, FALSE, FALSE );
        if( new != NULL ) {
            new->flags |= WR_HASHENTRY_STANDARD;
            // standard symbols are always in use????
            new->flags &= ~WR_HASHENTRY_UNUSED;
        }
    }

    WRMakeHashTableClean( table );
}

WRHashTable * WRAPI WRInitHashTable( void )
{
    WRHashTable         *table;

    table = (WRHashTable *)WRMemAlloc( sizeof( WRHashTable ) );
    if( table == NULL ) {
        return( NULL );
    }

    memset( table, 0, sizeof( WRHashTable ) );

    table->next_default_value = FIRST_DEFAULT_ID;

    WRAddSymInitInfo( table );

    return( table );
}

void WRAPI WRFreeHashTable( WRHashTable *table )
{
    unsigned int        nhash;
    WRHashEntry         *entry;

    for( nhash = 0; nhash < NAME_SIZE; nhash++ ) {
        for( ;; ) {
            entry = table->names[nhash];
            if( entry == NULL ) {
                break;
            }
            table->names[nhash] = entry->name_next;
            WRMemFree( entry );
        }
    }

    WRMemFree( table );
}

BOOL WRAPI WRMergeHashTable( WRHashTable **dest, WRHashTable *src )
{
    WRHashEntry         *entry;
    WRHashEntry         *new;
    unsigned int        nhash;
    BOOL                ok, dup;

    if( dest != NULL ) {
        if( *dest == NULL ) {
            *dest = WRInitHashTable();
        }
    } else {
        return( FALSE );
    }

    if( src == NULL ) {
        return( TRUE );
    }

    ok = TRUE;

    for( nhash = 0; nhash < NAME_SIZE; nhash++ ) {
        for( entry = src->names[nhash]; entry != NULL; entry = entry->name_next ) {
            dup = FALSE;
            new = WRAddHashEntry( *dest, entry->name, entry->value, &dup, FALSE, FALSE );
            ok = (new != NULL);
        }
    }

    (*dest)->flags = src->flags;
    (*dest)->next_default_value = src->next_default_value;

    return( ok );
}

BOOL WRAPI WRCopyHashTable( WRHashTable **dest, WRHashTable *src )
{
    WRHashEntry         *entry;
    WRHashEntry         *new;
    unsigned int        nhash;
    BOOL                ok, dup;

    if( dest != NULL ) {
        if( *dest != NULL ) {
            WRFreeHashTable( *dest );
            *dest = NULL;
        }
    } else {
        return( FALSE );
    }

    if( src == NULL ) {
        return( TRUE );
    }

    *dest = WRInitHashTable();

    if( *dest == NULL ) {
        return( FALSE );
    }

    ok = TRUE;

    for( nhash = 0; nhash < NAME_SIZE; nhash++ ) {
        for( entry = src->names[nhash]; entry != NULL; entry = entry->name_next ) {
            if( !(entry->flags & WR_HASHENTRY_STANDARD) ) {
                dup = FALSE;
                new = WRAddHashEntry( *dest, entry->name, entry->value, &dup, FALSE, FALSE );
                if( new == NULL || dup ) {
                    ok = FALSE;
                }
            }
        }
    }

    (*dest)->flags = src->flags;
    (*dest)->next_default_value = src->next_default_value;

    return( ok );
}

static unsigned WRAddSymbolsToLorCBox( WRHashTable *table, HWND hDlg, int id,
                                       WRHashEntryFlags flags, UINT add_string_msg,
                                       UINT add_data_msg, UINT reset_msg,
                                       LRESULT err, LRESULT errspace )
{
    HWND                box;
    LRESULT             ret;
    unsigned int        nhash;
    WRHashEntry         *entry;
    unsigned            count;

    if( table == NULL ) {
        return( 0 );
    }

    count = 0;
    box = GetDlgItem( hDlg, id );
    SendMessage( box, WM_SETREDRAW, FALSE, 0 );
    SendMessage( box, reset_msg, 0, 0 );

    for( nhash = 0; nhash < NAME_SIZE; nhash++ ) {
        for( entry = table->names[nhash]; entry != NULL; entry = entry->name_next ) {
            if( entry->flags & flags ) {
                ret = SendMessage( box, add_string_msg, 0, (LPARAM)(LPSTR)entry->name );
                if( ret == err || ret == errspace ) {
                    break;
                }
                SendMessage( box, add_data_msg, (WPARAM)ret, (LPARAM)(LPVOID)entry );
                count++;
            }
        }
    }

    SendMessage( box, WM_SETREDRAW, TRUE, 0 );
    InvalidateRect( box, NULL, TRUE );

    return( count );
}

unsigned WRAPI WRAPI WRAddSymbolsToListBox( WRHashTable *table, HWND hDlg,
                                          int id, WRHashEntryFlags flags )
{
    return( WRAddSymbolsToLorCBox( table, hDlg, id, flags, LB_ADDSTRING, LB_SETITEMDATA,
                                   LB_RESETCONTENT, LB_ERR, LB_ERRSPACE ) );
}

unsigned WRAPI WRAddSymbolsToComboBox( WRHashTable *table, HWND hDlg,
                                           int id, WRHashEntryFlags flags )
{
    return( WRAddSymbolsToLorCBox( table, hDlg, id, flags, CB_ADDSTRING, CB_SETITEMDATA,
                                   CB_RESETCONTENT, CB_ERR, CB_ERRSPACE ) );
}

BOOL WRAPI WRWriteSymbolsToFile( WRHashTable *table, char *filename )
{
    WRHashEntry         *entry;
    WRHashEntry         **tbl;
    FILE                *handle;
    unsigned int        nhash;
    unsigned int        i;
    unsigned int        count;
    int                 max_len;
    int                 len;

    if( table == NULL || table->count == 0 || filename == NULL ) {
        return( FALSE );
    }

    handle = fopen( filename, "wt" );

    if( handle == NULL ) {
        WRPrintErrorMsg( WR_FILEOPENFAILED, filename, strerror( errno ) );
        return( FALSE );
    }

    tbl = (WRHashEntry **)WRMemAlloc( sizeof( WRHashEntry * ) * table->count );
    if( tbl == NULL ) {
        return( FALSE );
    }

    count = 0;
    max_len = 0;
    for( nhash = 0; nhash < NAME_SIZE; nhash++ ) {
        for( entry = table->names[nhash]; entry != NULL; entry = entry->name_next ) {
            if( !(entry->flags & WR_HASHENTRY_STANDARD) ) {
                tbl[count] = entry;
                len = strlen( entry->name );
                max_len = max( len, max_len );
                count++;
            }
        }
    }

    qsort( tbl, count, sizeof( WRHashEntry * ), WRCompareHashEntry );

    for( i = 0; i < count; i++ ) {
        fprintf( handle, "#define %-*s %ld\n", max_len + 2, tbl[i]->name, tbl[i]->value );
    }

    if( fclose( handle ) != 0 ) {
        WRPrintErrorMsg( WR_FILECLOSEFAILED, filename, strerror( errno ) );
        return( FALSE );
    }

    return( TRUE );
}

static BOOL WRDiscardChangesQuery( void )
{
    char        *title;
    char        *msg;
    int         ret;

    title = WRAllocRCString( WR_EDITSYMBOLSTITLE );
    msg = WRAllocRCString( WR_DISCARDCHANGESMSG );

    ret = MessageBox( (HWND)NULL, msg, title, MB_ICONEXCLAMATION | MB_YESNO | MB_TASKMODAL );

    if( msg != NULL ) {
        WRFreeRCString( msg );
    }
    if( title != NULL ) {
        WRFreeRCString( title );
    }

    return( ret == IDYES );
}

static BOOL WRAddUniqueQuery( void )
{
    char        *title;
    char        *msg;
    int         ret;

    title = WRAllocRCString( WR_ADDSYMBOLTITLE );
    msg = WRAllocRCString( WR_QUERYUNIQUEMSG );

    ret = MessageBox( (HWND)NULL, msg, title, MB_ICONEXCLAMATION | MB_YESNO | MB_TASKMODAL );

    if( msg != NULL ) {
        WRFreeRCString( msg );
    }
    if( title != NULL ) {
        WRFreeRCString( title );
    }

    return( ret == IDYES );
}

static BOOL WRForceAddQuery( void )
{
    char        *title;
    char        *msg;
    int         ret;

    title = WRAllocRCString( WR_ADDSYMBOLTITLE );
    msg = WRAllocRCString( WR_FORCEADDQUERYMSG );

    ret = MessageBox( (HWND)NULL, msg, title, MB_ICONEXCLAMATION | MB_YESNO | MB_TASKMODAL );

    if( msg != NULL ) {
        WRFreeRCString( msg );
    }
    if( title != NULL ) {
        WRFreeRCString( title );
    }

    return( ret == IDYES );
}

BOOL WRAPI WRFindUnusedHashValue( WRHashTable *table, WRHashValue *value,
                                      WRHashValue start )
{
    WRHashValue val;
    BOOL        found;

    if( table == NULL || value == NULL ) {
        return( FALSE );
    }

    found = TRUE;
    val = start;
    while( WRValueExists( table, val ) ) {
        val++;
        if( val == 0xffff ) {
            if( start < FIRST_DEFAULT_ID ) {
                val = 0;
            } else {
                val = FIRST_DEFAULT_ID;
            }
        }
        if( val == start ) {
            found = FALSE;
            break;
        }
    }

    if( found ) {
        *value = val;
    }

    return( found );
}

WRHashEntry *WRAPI WRAddDefHashEntry( WRHashTable *table, char *name, BOOL *dup )
{
    WRHashEntry *entry;
    WRHashValue value;

    if( table == NULL || name == NULL ) {
        return( NULL );
    }

    entry = WRFindHashEntryFromName( table, name );
    if( entry != NULL ) {
        if( dup != NULL ) {
            *dup = TRUE;
        }
        return( entry );
    }

    if( !WRFindUnusedHashValue( table, &value, table->next_default_value ) ) {
        return( NULL );
    }

    entry = WRAddHashEntry( table, name, value, dup, FALSE, TRUE );
    if( entry != NULL ) {
        table->next_default_value = value++;
    }

    return( entry );
}

WRHashEntry * WRAPI WRAddHashEntry( WRHashTable *table, char *name, WRHashValue value,
                                        BOOL *dup, BOOL check_unique, BOOL query_force )
{
    WRHashEntry         *entry;
    unsigned int        nhash;
    unsigned int        vhash;
    unsigned int        size;
    char                *symbol;

    if( table == NULL || name == NULL ) {
        return( NULL );
    }

    symbol = WRStrDup( name );
    if( symbol == NULL ) {
        return( NULL );
    }

    WRStripSymbol( symbol );
    if( !WRIsValidSymbol( symbol ) ) {
        WRMemFree( symbol );
        return( NULL );
    }

    // we do not allow the modification of standard entries
    entry = WRFindHashEntryFromName( table, symbol );
    if( entry != NULL ) {
        if( entry->flags & WR_HASHENTRY_STANDARD ) {
            return( NULL );
        }
    }

    if( WRValueExists( table, value ) && check_unique ) {
        if( !WRAddUniqueQuery() ) {
            return( NULL );
        }
    }

    if( entry != NULL ) {
        if( !*dup && query_force ) {
            *dup = WRForceAddQuery();
        }
        // If dup is TRUE force the add
        if( *dup ) {
            if( !WRRemoveName( table, symbol ) ) {
                return( NULL );
            }
        }
        *dup = TRUE;
    } else {
        *dup = FALSE;
    }

    size = sizeof( WRHashEntry ) + strlen( symbol );
    entry = (WRHashEntry *)WRMemAlloc( size );
    if( entry != NULL ) {
        nhash = WRGetNameHash( symbol );
        vhash = WRGetValueHash( value );
        entry->name_prev  = NULL;
        entry->value_prev = NULL;
        if( table->names[nhash] ) {
            table->names[nhash]->name_prev = entry;
        }
        if( table->values[vhash] ) {
            table->values[vhash]->value_prev = entry;
        }
        entry->name_next = table->names[nhash];
        entry->value_next = table->values[vhash];
        table->names[nhash] = entry;
        table->values[vhash] = entry;
        strcpy( entry->name, symbol );
        strupr( entry->name );
        entry->value = value;
        entry->ref_count = 0;
        entry->flags = WR_HASHENTRY_UNUSED;
        table->count++;
        table->flags |= WR_HASH_DIRTY;
    }

    WRMemFree( symbol );

    return( entry );
}

BOOL WRAPI WRRemoveName( WRHashTable *table, char *name )
{
    WRHashEntry         *entry;
    unsigned int        nhash;
    unsigned int        vhash;

    entry = WRFindHashEntryFromName( table, name );

    if( entry == NULL || (entry->flags & WR_HASHENTRY_STANDARD) ) {
        return( FALSE );
    }

    nhash = WRGetNameHash( name );
    vhash = WRGetValueHash( entry->value );

    if( table->names[nhash] == entry ) {
        table->names[nhash] = entry->name_next;
    }

    if( table->values[vhash] == entry ) {
        table->values[vhash] = entry->value_next;
    }

    if( entry->name_next != NULL ) {
        entry->name_next->name_prev = entry->name_prev;
    }
    if( entry->name_prev != NULL ) {
        entry->name_prev->name_next = entry->name_next;
    }
    if( entry->value_next != NULL ) {
        entry->value_next->value_prev = entry->value_prev;
    }
    if( entry->value_prev != NULL ) {
        entry->value_prev->value_next = entry->value_next;
    }

    table->count--;
    table->flags |= WR_HASH_DIRTY;
    WRMemFree( entry );

    return( TRUE );
}

void WRAPI WRHashIncRefCount( WRHashEntry *entry )
{
    if( entry != NULL ) {
        entry->ref_count++;
        if( !(entry->flags & WR_HASHENTRY_STANDARD) ) {
            entry->flags &= ~WR_HASHENTRY_UNUSED;
        }
    }
}

void WRAPI WRHashDecRefCount( WRHashEntry *entry )
{
    if( entry != NULL ) {
        if( entry->ref_count != 0 ) {
            entry->ref_count--;
            if( !(entry->flags & WR_HASHENTRY_STANDARD) ) {
                if( entry->ref_count == 0 ) {
                    entry->flags |= WR_HASHENTRY_UNUSED;
                }
            }
        }
    }
}

BOOL WRAPI WRLookupName( WRHashTable *table, char *name, WRHashValue *value )
{
    WRHashEntry *entry;

    entry = WRFindHashEntryFromName( table, name );

    if( entry != NULL ) {
        if( value != NULL ) {
            *value = entry->value;
        }
        return( TRUE );
    }

    return( FALSE );
}

int WRAPI WRModifyName( WRHashTable *table, char *name, WRHashValue value,
                            BOOL check_unique )
{
    WRHashEntry *entry;
    BOOL        dup;

    // Set dup to cause a force if the symbol exists
    dup = TRUE;

    entry = WRAddHashEntry( table, name, value, &dup, check_unique, FALSE );
    if( entry != NULL ) {
        table->flags |= WR_HASH_DIRTY;
        return( TRUE );
    }

    return( FALSE );
}

char * WRAPI WRResolveValue( WRHashTable *table, WRHashValue value )
{
    char                *name;
    WRHashValueList     *vlist;

    name = NULL;
    vlist = WRLookupValue( table, value );
    if( vlist != NULL ) {
        if( vlist->next == NULL ) {
            name = WRStrDup( vlist->entry->name );
        }
        WRValueListFree( vlist );
    }

    return( name );
}

static BOOL WRValueListInsert( WRHashValueList **list, WRHashEntry *entry )
{
    WRHashValueList     *new;

    if( list == NULL || entry == NULL ) {
        return( FALSE );
    }

    new = WRMemAlloc( sizeof( WRHashValueList ) );
    if( new == NULL ) {
        return( FALSE );
    }

    new->entry = entry;
    new->next = *list;
    *list = new;

    return( TRUE );
}

void WRAPI WRValueListFree( WRHashValueList *list )
{
    WRHashValueList     *next;

    while( list != NULL ) {
        next = list->next;
        WRMemFree( list );
        list = next;
    }
}

WRHashValueList * WRAPI WRLookupValue( WRHashTable *table, WRHashValue value )
{
    WRHashValueList     *list;
    WRHashEntry         *entry;
    unsigned int        hash;

    if( table == NULL ) {
        return( NULL );
    }

    list = NULL;
    hash = WRGetValueHash( value );
    for( entry = table->values[hash]; entry != NULL; entry = entry->value_next ) {
        if( entry->value == value ) {
            if( !WRValueListInsert( &list, entry ) ) {
                WRValueListFree( list );
                return( NULL );
            }
        }
    }

    return( list );
}

int WRAPI WRValueExists( WRHashTable *table, WRHashValue value )
{
    WRHashEntry         *entry;
    unsigned int        hash;
    int                 count;

    if( table == NULL ) {
        return( FALSE );
    }

    count = 0;
    hash = WRGetValueHash( value );
    for( entry = table->values[hash]; entry != NULL; entry = entry->value_next ) {
        if( entry->value == value ) {
            count++;
        }
    }

    return( count );
}

void WRAPI WRStripSymbol( char *symbol )
{
    WRStripStr( symbol );
}

BOOL WRAPI WRIsValidSymbol( char *symbol )
{
    if( symbol == NULL ) {
        return( FALSE );
    }


    // the following code was not mbcs'ized as WR does not allow
    // non-ANSI characters in symbol identifiers
    if( !isalpha( *symbol ) && *symbol != '_' ) {
        return( FALSE );
    }

    for( ; *symbol != '\0'; symbol++ ) {
        if( !isalnum( *symbol ) && *symbol != '_' ) {
            return( FALSE );
        }
    }

    return( TRUE );
}

BOOL WRAPI WREditSym( HWND parent, WRHashTable **table,
                          WRHashEntryFlags *flags, FARPROC help_callback )
{
    WREditSymInfo       info;
    WRHashTable         *tmp;
    HINSTANCE           inst;
    FARPROC             proc;
    int                 ret;
    BOOL                ok;

    tmp = NULL;

    ok = (table != NULL && *table != NULL && flags != NULL);

    if( ok ) {
        ok = WRCopyHashTable( &tmp, *table );
    }

    if( ok ) {
        inst = WRGetInstance();
        proc = MakeProcInstance( (FARPROC)WREditSymbolsProc, inst );
        ok = (proc != NULL);
    }

    if( ok ) {
        info.hcb = help_callback;
        info.table = tmp;
        info.modified = FALSE;
        info.flags = *flags;
        ret = JDialogBoxParam( inst, "WRSymbols", parent, (DLGPROC)proc, (LPARAM)(LPVOID)&info );
        FreeProcInstance( proc );
        ok = FALSE;
        if( ret ) {
            UpdateWindow( parent );
            if( info.modified ) {
                ok = WRCopyHashTable( table, tmp );
            }
        }
    }

    if( ok ) {
        *flags = info.flags;
    }

    if( tmp != NULL ) {
        WRFreeHashTable( tmp );
    }

    return( ok );
}

static BOOL WRShowSelectedSymbol( HWND hDlg, WRHashTable *table )
{
    HWND        lbox;
    LRESULT     index;
    WRHashEntry *entry;
    BOOL        standard_entry;
    BOOL        ok;

    _wtouch( table );

    standard_entry = FALSE;
    lbox = GetDlgItem( hDlg, IDB_SYM_LISTBOX );
    index = SendMessage( lbox, LB_GETCOUNT, 0, 0 );
    ok = (index != LB_ERR);

    if( ok ) {
        WRSetEditWithULONG( (unsigned long)index, 10, hDlg, IDB_SYM_NUM );
        ok = (index != 0);
    }

    if( ok ) {
        index = SendMessage( lbox, LB_GETCURSEL, 0, 0 );
        entry = (WRHashEntry *)SendMessage( lbox, LB_GETITEMDATA, (WPARAM)index, 0 );
        ok = (entry != NULL);
    }

    if( ok ) {
        standard_entry = ((entry->flags & WR_HASHENTRY_STANDARD) != 0);
    }

    EnableWindow( GetDlgItem( hDlg, IDB_SYM_MODIFY ), !standard_entry );
    EnableWindow( GetDlgItem( hDlg, IDB_SYM_REMOVE ), !standard_entry );

    return( ok );
}

static BOOL WRAddSymbol( HWND hDlg, WRHashTable *table, BOOL force,
                         char *symbol, WRHashValue value )
{
    LRESULT             index;
    WRHashEntry         *entry;
    BOOL                dup;
    BOOL                ok;

    dup = force;

    ok = (symbol != NULL && !WRIsStrSpace( symbol ));

    if( ok ) {
        WRStripSymbol( symbol );
        ok = WRIsValidSymbol( symbol );
        if( !ok ) {
            WRDisplayErrorMsg( WR_BADSYMBOL );
        }
    }

    if( ok ) {
        strupr( symbol );
        entry = WRAddHashEntry( table, symbol, value, &dup, TRUE, TRUE );
        ok = (entry != NULL);
    }

    if( ok ) {
        if( WRFindUnusedHashValue( table, &value, table->next_default_value ) ) {
            table->next_default_value = value;
        }
    }

    if( ok && !force && dup ) {
        ok = (!force && dup);
    }

    if( ok ) {
        if( dup ) {
            // this is neccessary if the value of the string was moified
            index = SendDlgItemMessage( hDlg, IDB_SYM_LISTBOX, LB_FINDSTRINGEXACT, 0,
                                        (LPARAM)(LPSTR)symbol );
        } else {
            index = SendDlgItemMessage( hDlg, IDB_SYM_LISTBOX, LB_ADDSTRING, 0,
                                        (LPARAM)(LPSTR)symbol );
            SendDlgItemMessage( hDlg, IDB_SYM_LISTBOX, LB_SETITEMDATA,
                                (WPARAM)index, (LPARAM)(LPVOID)entry );
        }
        SendDlgItemMessage( hDlg, IDB_SYM_LISTBOX, LB_SETCURSEL, index, 0 );
        WRShowSelectedSymbol( hDlg, table );
    }

    return( ok );
}

static WRHashEntry *getHashEntry( HWND hDlg )
{
    LRESULT             index;
    LRESULT             count;
    WRHashEntry         *entry;
    HWND                lbox;

    lbox = GetDlgItem( hDlg, IDB_SYM_LISTBOX );
    count = SendMessage( lbox, LB_GETCOUNT, 0, 0 );
    if( count == 0 || count == LB_ERR ) {
        return( NULL );
    }

    index = SendMessage( lbox, LB_GETCURSEL, 0, 0 );
    if( index == LB_ERR ) {
        return( NULL );
    }

    entry = (WRHashEntry *)SendMessage( lbox, LB_GETITEMDATA, (WPARAM)index, 0 );

    return( entry );
}

static BOOL WRAddNewSymbol( HWND hDlg, WRHashTable *table, FARPROC hcb, BOOL modify )
{
    WRAddSymInfo        info;
    WRHashEntry         *entry;
    DLGPROC             proc_inst;
    HINSTANCE           inst;
    BOOL                modified;
    BOOL                ret;

    if( table == NULL ) {
        return( FALSE );
    }

    info.table = table;
    info.hcb = hcb;
    info.value = 0;
    info.modify = modify;

    info.symbol = NULL;
    if( modify ) {
        entry = getHashEntry( hDlg );
        if( entry != NULL ) {
            info.symbol = entry->name;
        }
    }

    ret = FALSE;

    inst = WRGetInstance();
    proc_inst = (DLGPROC)MakeProcInstance( (FARPROC)WRAddSymProc, inst );
    modified = JDialogBoxParam( inst, "WRAddSymbol", hDlg, proc_inst, (LPARAM)(LPVOID)&info );
    FreeProcInstance( (FARPROC)proc_inst );

    if( modified == IDOK ) {
        ret = WRAddSymbol( hDlg, table, modify, info.symbol, info.value );
    }

    if( info.symbol != NULL ) {
        WRMemFree( info.symbol );
    }

    return( ret );
}

static BOOL WRRemoveSymbol( HWND hDlg, WRHashTable *table )
{
    LRESULT             index;
    LRESULT             count;
    WRHashEntry         *entry;
    HWND                lbox;
    BOOL                ret;

    lbox = GetDlgItem( hDlg, IDB_SYM_LISTBOX );
    count = SendMessage( lbox, LB_GETCOUNT, 0, 0 );
    if( count == 0 || count == LB_ERR ) {
        return( TRUE );
    }

    index = SendMessage( lbox, LB_GETCURSEL, 0, 0 );
    entry = (WRHashEntry *)SendMessage( lbox, LB_GETITEMDATA, (WPARAM)index, 0 );
    if( entry == NULL ) {
        return( FALSE );
    }

    ret = WRRemoveName( table, entry->name );

    if( !ret ) {
        return( FALSE );
    }

    count = SendMessage( lbox, LB_DELETESTRING, index, 0 );
    if( count != 0 ) {
        if( index != 0 && index == count ) {
            index--;
        }
        SendDlgItemMessage( hDlg, IDB_SYM_LISTBOX, LB_SETCURSEL, index, 0 );
    }

    WRShowSelectedSymbol( hDlg, table );

    return( TRUE );
}

static int      TextWidth = 0;
static BOOL     TextWidthInit = FALSE;

static void InitTextWidth( HWND hDlg )
{
    RECT        rect;

    GetWindowRect( GetDlgItem( hDlg, IDB_SYM_SYMVALUE ), &rect );
    TextWidth = rect.left;
    GetWindowRect( GetDlgItem( hDlg, IDB_SYM_SYMTEXT ), &rect );
    TextWidth -= rect.left;
}

static void WRDrawHashLBItem( DRAWITEMSTRUCT *dis, WRHashEntry *entry )
{
    int         oldDC;
    COLORREF    bkcolor;
    COLORREF    color;
    HBRUSH      brush;
    RECT        text_rect;
    POINT       pt;
    char        vtext[35];

    oldDC = SaveDC( dis->hDC );

    // paint the item background
    if( (dis->itemState & ODS_SELECTED) && entry != NULL ) {
        bkcolor = GetSysColor( COLOR_HIGHLIGHT );
    } else {
        bkcolor = GetSysColor( COLOR_WINDOW );
    }
    brush = CreateSolidBrush( bkcolor );
    FillRect( dis->hDC, &dis->rcItem, brush );
    DeleteObject( brush );

    if( entry != NULL && entry->name != NULL ) {
        // set the correct text color
        if( (dis->itemState & ODS_DISABLED) || (dis->itemState & ODS_GRAYED) ) {
            color = GetSysColor( COLOR_GRAYTEXT );
        } else {
            if( dis->itemState & ODS_SELECTED ) {
                color = GetSysColor( COLOR_HIGHLIGHTTEXT );
            } else {
                color = GetSysColor( COLOR_WINDOWTEXT );
            }
        }
        SetTextColor( dis->hDC, color );
        SetBkColor( dis->hDC, bkcolor );

        // set the text rectangle of the listbox item
        text_rect.left = dis->rcItem.left + 1;
        text_rect.top = dis->rcItem.top + 1;
        text_rect.right = text_rect.left + TextWidth - 1;
        text_rect.bottom = dis->rcItem.bottom;

        // set the text of the listbox item
        SelectObject( dis->hDC, GetStockObject( ANSI_VAR_FONT ) );
        ExtTextOut( dis->hDC, text_rect.left, text_rect.top, ETO_CLIPPED, &text_rect,
                    entry->name, strlen( entry->name ), NULL );
        //TextOut( dis->hDC, dis->rcItem.left + 1, dis->rcItem.top + 1,
        //         entry->name, strlen( entry->name ) );

        ltoa( (long)entry->value, vtext, 10 );

        // set the value rectangle of the listbox item
        text_rect.left = text_rect.right + 2;
        text_rect.right = dis->rcItem.right - 1;

        // draw the value text
        ExtTextOut( dis->hDC, text_rect.left, text_rect.top,
                    ETO_CLIPPED, &text_rect, vtext, strlen( vtext ), NULL );

        // if the item has been ref'd then draw the checkbox
        if( entry->ref_count != 0 ) {
            SelectObject( dis->hDC, GetStockObject( BLACK_PEN ) );
            pt.x = dis->rcItem.right - 4;
            pt.y = dis->rcItem.top + 2;
            MoveToEx( dis->hDC, pt.x, pt.y, NULL );
            //pt.x -= 5;
            pt.x -= 4;
            pt.y += 10;
            LineTo( dis->hDC, pt.x, pt.y );
            //pt.x -= 5;
            pt.x -= 4;
            pt.y -= 5;
            LineTo( dis->hDC, pt.x, pt.y );
        }
    }

    if( dis->itemState & ODS_FOCUS ) {
        DrawFocusRect( dis->hDC, &dis->rcItem );
    }

    RestoreDC( dis->hDC, oldDC );
}

static void WRDrawHashListBoxItem( HWND hDlg, DRAWITEMSTRUCT *dis )
{
    WRHashEntry *entry;

    if( dis == NULL ) {
        return;
    }

    if( !TextWidthInit ) {
        InitTextWidth( hDlg );
        TextWidthInit = TRUE;
    }

    entry = NULL;

    if( dis->itemID != (UINT)-1 ) {
        entry = (WRHashEntry *)dis->itemData;
    }

    WRDrawHashLBItem( dis, entry );
}

static void WRSetupEditSymDialog( HWND hDlg, WREditSymInfo *info, BOOL first )
{
    WRAddSymbolsToListBox( info->table, hDlg, IDB_SYM_LISTBOX, info->flags );
    SendDlgItemMessage( hDlg, IDB_SYM_LISTBOX, LB_SETCURSEL, 0, 0 );
    WRShowSelectedSymbol( hDlg, info->table );
    if( first ) {
        CheckDlgButton( hDlg, IDB_SYM_SHOW_STANDARD, info->flags & WR_HASHENTRY_STANDARD );
//      CheckDlgButton( hDlg, IDB_SYM_SHOW_UNUSED, info->flags & WR_HASHENTRY_UNUSED );
    }
}

static WRHashEntryFlags WRGetEditSymEntryFlags( HWND hDlg )
{
    WRHashEntryFlags    flags;

    flags = 0;
    if( IsDlgButtonChecked( hDlg, IDB_SYM_SHOW_STANDARD ) ) {
        flags |= WR_HASHENTRY_STANDARD;
    }
//  if( IsDlgButtonChecked( hDlg, IDB_SYM_SHOW_UNUSED ) ) {
        flags |= WR_HASHENTRY_UNUSED;
//  }

    return( flags );
}

static BOOL WRHandleDELKey( HWND hDlg, WREditSymInfo *info )
{
    if( IsWindowEnabled( GetDlgItem( hDlg, IDB_SYM_REMOVE ) ) ) {
        if( WRRemoveSymbol( hDlg, info->table ) ) {
            info->modified = TRUE;
            return( TRUE );
        }
    }
    return( FALSE );
}

WINEXPORT BOOL CALLBACK WREditSymbolsProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
    WREditSymInfo       *info;
    BOOL                ret;
    WORD                wp, cmd;

    ret = FALSE;

    switch( message ) {
    case WM_SYSCOLORCHANGE:
        WRCtl3dColorChange();
        break;

    case WM_INITDIALOG:
        info = (WREditSymInfo *)lParam;
        SET_DLGDATA( hDlg, (LONG_PTR)info );
        if( info == NULL ) {
            EndDialog( hDlg, FALSE );
            break;
        }
        WRSetupEditSymDialog( hDlg, info, TRUE );
        ret = TRUE;
        break;

    case WM_DRAWITEM:
        WRDrawHashListBoxItem( hDlg, (DRAWITEMSTRUCT *)lParam );
        ret = TRUE;
        break;

    case WM_VKEYTOITEM:
        ret = -1;
        info = (WREditSymInfo *)GET_DLGDATA( hDlg );
        if( info != NULL && LOWORD( wParam ) == VK_DELETE ) {
            if( WRHandleDELKey( hDlg, info ) ) {
                ret = -2;
            }
        }
        break;

    case WM_COMMAND:
        info = (WREditSymInfo *)GET_DLGDATA( hDlg );
        wp = LOWORD( wParam );
        cmd = GET_WM_COMMAND_CMD( wParam, lParam );
        switch( wp ) {
        case IDB_SYM_SHOW_STANDARD:
//      case IDB_SYM_SHOW_UNUSED:
            if( cmd == BN_CLICKED ) {
                info->flags = WRGetEditSymEntryFlags( hDlg );
                WRSetupEditSymDialog( hDlg, info, FALSE );
            }
            break;

        case IDB_SYM_HELP:
            if( info != NULL && info->hcb != NULL ) {
                (*info->hcb)();
            }
            break;

        case IDOK:
            if( info != NULL ) {
                info->flags = WRGetEditSymEntryFlags( hDlg );
            }
            EndDialog( hDlg, TRUE );
            ret = TRUE;
            break;

        case IDCANCEL:
            ret = TRUE;
            if( info != NULL && info->modified ) {
                if( !WRDiscardChangesQuery() ) {
                    break;
                }
            }
            EndDialog( hDlg, FALSE );
            break;

        case IDB_SYM_ADD:
        case IDB_SYM_MODIFY:
            if( info == NULL || info->table == NULL ) {
                break;
            }
            if( WRAddNewSymbol( hDlg,info->table, info->hcb, wp == IDB_SYM_MODIFY ) ) {
                info->modified = TRUE;
            }
            break;

        case IDB_SYM_REMOVE:
            if( info == NULL || info->table == NULL ) {
                break;
            }
            if( WRRemoveSymbol( hDlg, info->table ) ) {
                info->modified = TRUE;
            }
            break;

        case IDB_SYM_LISTBOX:
            switch( cmd ) {
            case LBN_SELCHANGE:
                if( info != NULL && info->table != NULL ) {
                    WRShowSelectedSymbol( hDlg, info->table );
                }
                break;
            }
            break;
        }
        break;
    }

    return( ret );
}

static void WRSetAddSymInfo( HWND hDlg, WRAddSymInfo *info )
{
    WRHashValue value;
    char        *str;

    if( info != NULL ) {
        if( info->modify ) {
            str = WRAllocRCString( WR_MODIFYSYMBOLTITLE );
            if( str != NULL ) {
                SendMessage( hDlg, WM_SETTEXT, 0, (LPARAM)(LPSTR)str );
                WRFreeRCString( str );
            }
            if( info->symbol != NULL ) {
                WRSetEditWithStr( info->symbol, hDlg, IDB_ADDSYM_SYM );
            }
        }
        if( !WRFindUnusedHashValue( info->table, &value,
                                    info->table->next_default_value ) ) {
            value = info->table->next_default_value;
        }
        WRSetEditWithSLONG( (signed long)value, 10, hDlg, IDB_ADDSYM_VAL );
    }
}

static BOOL WRGetAddSymInfo( HWND hDlg, WRAddSymInfo *info )
{
    signed long val;

    if( info == NULL ) {
        return( FALSE );
    }

    if( !WRGetSLONGFromEdit( hDlg, IDB_ADDSYM_VAL, NULL, &val ) ) {
        return( FALSE );
    }

    info->value = (WRHashValue)val;
    info->symbol = WRGetStrFromEdit( hDlg, IDB_ADDSYM_SYM, NULL );
    if( info->symbol == NULL ) {
        return( FALSE );
    }

    WRStripSymbol( info->symbol );
    if( !WRIsValidSymbol( info->symbol ) ) {
        WRMemFree( info->symbol );
        info->symbol = NULL;
        return( FALSE );
    }

    return( TRUE );
}

static void WRSetAddSymOK( HWND hDlg )
{
    char        *str;
    signed long val;
    BOOL        enable;

    enable = FALSE;
    str = WRGetStrFromEdit( hDlg, IDB_ADDSYM_SYM, NULL );
    if( str != NULL ) {
        WRStripSymbol( str );
        enable = WRIsValidSymbol( str );
        WRMemFree( str );
    }

    if( enable ) {
        enable = WRGetSLONGFromEdit( hDlg, IDB_ADDSYM_VAL, NULL, &val );
    }

    EnableWindow( GetDlgItem( hDlg, IDOK ), enable );
}

WINEXPORT BOOL CALLBACK WRAddSymProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
    WRAddSymInfo        *info;
    WORD                cmd;
    BOOL                ret;

    ret = FALSE;

    switch( message ) {
    case WM_INITDIALOG:
        info = (WRAddSymInfo *)lParam;
        SET_DLGDATA( hDlg, (LONG_PTR)info );
        WRSetAddSymInfo( hDlg, info );
        WRSetAddSymOK( hDlg );
        ret = TRUE;
        break;

    case WM_SYSCOLORCHANGE:
        WRCtl3dColorChange();
        break;

    case WM_COMMAND:
        info = (WRAddSymInfo *)GET_DLGDATA( hDlg );
        switch( LOWORD( wParam ) ) {
        case IDB_ADDSYM_HELP:
            if( info != NULL && info->hcb != NULL ) {
                (*info->hcb)();
            }
            break;

        case IDB_ADDSYM_SYM:
        case IDB_ADDSYM_VAL:
            cmd = GET_WM_COMMAND_CMD( wParam, lParam );
            switch( cmd ) {
            case EN_CHANGE:
                WRSetAddSymOK( hDlg );
                break;
            }
            break;

        case IDOK:
            if( WRGetAddSymInfo( hDlg, info ) ) {
                EndDialog( hDlg, TRUE );
            }
            ret = TRUE;
            break;

        case IDCANCEL:
            EndDialog( hDlg, FALSE );
            ret = TRUE;
            break;
        }
        break;
    }

    return( ret );
}
