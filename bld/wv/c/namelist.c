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


#include "dbgdefn.h"
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "dbginfo.h"
//#include "dbgwind.h"
#include "dbgerr.h"
#include "namelist.h"
#include "sortlist.h"
#include "clibext.h"

extern char             *TxtBuff;
extern address          NilAddr;

extern char             *DupStr(char*);
extern int              AddrComp( address a, address b );
extern unsigned         QualifiedSymName( sym_handle *sh, char *name, unsigned max, bool );

typedef struct a_symbol {
    struct a_symbol     *next;
    unsigned long       key;
    char                __hdl[1]; // Variable size
} a_symbol;
#define ASymHdl( s ) ((sym_handle*)((s)->__hdl))

static void SetKey( a_symbol *sym )
{
    unsigned char       b[4];

    b[0] = 0;
    b[1] = 0;
    b[2] = 0;
    b[3] = 0;
    SymName( ASymHdl( sym ), NULL, SN_SOURCE, (char *)b, sizeof( b ) );
    sym->key = ((unsigned long)tolower(b[0])<<24)+
               ((unsigned long)tolower(b[1])<<16)+
                              (tolower(b[2])<< 8)+
                               tolower(b[3]);
}

OVL_EXTERN int SymCompare( void *pa, void *pb )
{
    char        *cmpa,*cmpb;
    a_symbol    *a = *(a_symbol **)pa;
    a_symbol    *b = *(a_symbol **)pb;

    if( a->key == 0 ) SetKey( a );
    if( b->key == 0 ) SetKey( b );
    if( a->key > b->key ) return( +1 );
    if( a->key < b->key ) return( -1 );
    cmpa = TxtBuff;
    cmpb = cmpa + SymName( ASymHdl( a ), NULL, SN_SOURCE, cmpa, TXT_LEN/2 ) + 1;
    SymName( ASymHdl( b ), NULL, SN_SOURCE, cmpb, TXT_LEN/2-1 );
    return( stricmp( cmpa, cmpb ) );
}

static bool CheckType( sym_handle *sym, name_list *name )
{
    sym_info    sinfo;
    mod_handle  mod;

    if( name->d2_only ) {
        mod = SymMod( sym );
        if( mod == NO_MOD ) return( FALSE );
        if( ModHasInfo( mod, HK_TYPE ) != DS_OK ) return( FALSE );
    }
    SymInfo( sym, NULL, &sinfo );
    switch( sinfo.kind ) {
    case SK_CODE:
    case SK_PROCEDURE:
        if( sinfo.compiler ) return( FALSE );
        if( name->type & WF_CODE ) return( TRUE );
        break;
    case SK_DATA:
        if( name->type & WF_DATA ) return( TRUE );
        break;
    case SK_NONE:
        return( TRUE );
    }
    return( FALSE );
}


static SYM_WALKER StickEmIn;
static walk_result StickEmIn( sym_walk_info swi, sym_handle *sym, void *_name )
{
    name_list   *name = _name;
//    char        *p;
    a_symbol    *curr;

    if( swi != SWI_SYMBOL ) return( WR_CONTINUE );
    if( !CheckType( sym, name ) ) return( WR_CONTINUE );
//    p = TxtBuff;
    curr = DbgAlloc( sizeof( a_symbol ) + sym_SIZE - 1 );
    if( curr == NULL ) return( WR_STOP );
    HDLAssign( sym, ASymHdl( curr ), sym );
    curr->key = 0;
    curr->next = name->list;
    name->numrows++;
    name->list = curr;
    return( WR_CONTINUE );
}


static address NameSymAddr( a_symbol *s )
{
    location_list       ll;

    if( SymLocation( ASymHdl( s ), NULL, &ll ) != DS_OK ) {
        return( NilAddr );
    }
    return( ll.e[0].u.addr );
}


static a_symbol *NameGetRow( name_list *name, int i )
{
    int         idx;
    a_symbol    *curr;

    if( name->skip ) {
        idx = i / SKIP_ENTRIES;
        curr = name->skip[ idx ];
        i -= idx * SKIP_ENTRIES;
    } else {
        curr = name->list;
    }
    while( --i >= 0 ) {
        curr = curr->next;
    }
    return( curr );
}

static void UniqList( name_list *name, bool dup_ok )
{
    a_symbol    **owner, *next;
    address     addr, next_addr;

    owner = (a_symbol **)&name->list;
    for( ;; ) {
        next = *owner;
        if( next == NULL ) break;
        if( next->next == NULL ) break;
        addr = NameSymAddr( next );
        next_addr = NameSymAddr( next->next );
        if( ( AddrComp( addr, next_addr ) == 0 &&
              SymCompare( &next, &next->next ) == 0 ) ||
            ( !dup_ok && SymCompare( &next, &next->next ) == 0 ) ) {
            *owner = next->next;
            DbgFree( next );
            name->numrows--;
        } else {
            owner = &next->next;
        }
    }
}


void NameListAddModules( name_list *name, mod_handle mod, bool d2_only, bool dup_ok )
{
    a_symbol    *curr;
    int         i;
    name->numrows = 0;
    name->list = NULL;
    name->d2_only = d2_only;
    WalkSymList( SS_MODULE, &mod, StickEmIn, name );
    name->list = SortLinkedList( name->list, offsetof( a_symbol, next ),
                                 SymCompare, DbgAlloc, DbgFree );
    UniqList( name, dup_ok );
    if( name->numrows > SKIP_ENTRIES ) {
        name->skip = DbgAlloc( (name->numrows/SKIP_ENTRIES + 1)*sizeof(a_symbol*) );
    } else {
        name->skip = NULL;
    }
    if( name->skip ) {
        i = 0;
        for( curr = name->list; curr != NULL; curr = curr->next ) {
            if( i % SKIP_ENTRIES == 0 ) {
                name->skip[ i / SKIP_ENTRIES ] = curr;
            }
            ++i;
        }
    }
}


void    NameListInit( name_list *name, walk_find type )
{
    name->list = NULL;
    name->skip = NULL;
    name->type = type;
}


void    NameListFree( name_list *name )
{
    a_symbol    *curr, *next;

    DbgFree( name->skip );
    for( curr = name->list; curr != NULL; curr = next ) {
        next = curr->next;
        DbgFree( curr );
    }
    name->list = NULL;
    name->numrows = 0;
}

int     NameListNumRows( name_list *name )
{
    if( name->list == NULL ) return( 0 );
    return( name->numrows );
}

unsigned NameListName( name_list *name, int i, char *buff, symbol_name type )
{
    unsigned    rc;
    sym_handle  *sh;

    buff[0] = '\0';
    if( i >= name->numrows || name->list == NULL ) return( 0 );
    sh = ASymHdl( NameGetRow( name, i ) );
    if( type == SN_PARSEABLE ) {
        rc = QualifiedSymName( sh, buff, TXT_LEN, TRUE );
    } else if( type == SN_QUALIFIED ) {
        rc = QualifiedSymName( sh, buff, TXT_LEN, FALSE );
    } else {
        rc = SymName( sh, NULL, type, buff, TXT_LEN );
    }
    return( rc );
}

address NameListAddr( name_list *name, int i )
{
    if( i >= name->numrows || name->list == NULL ) return( NilAddr );
    return( NameSymAddr( NameGetRow( name, i ) ) );
}

sym_handle *NameListHandle( name_list *name, int i )
{
    return( ASymHdl( NameGetRow( name, i ) ) );
}
