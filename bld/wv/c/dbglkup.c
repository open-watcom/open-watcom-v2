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
* Description:  Symbol lookup, including some special symbols.
*
****************************************************************************/


#include <ctype.h>
#include <string.h>
#include "dbgdefn.h"
#include "dbglit.h"
#include "dbgtoken.h"
#include "dbginfo.h"
#include "dbgerr.h"
#include "dbgmem.h"
#include "dbgtoggl.h"
#include "dbgio.h"


extern void             Scan( void );
extern unsigned         ScanCmd( char * );
extern bool             ScanItem( bool, char **, size_t * );
extern void             ReqEOC( void );
extern void             ConfigLine( char * );
extern char             *StrCopy( char *, char * );
extern bool             IsInternalMod( mod_handle );
extern bool             IsInternalModName( char *, unsigned );
extern image_entry      *ImageEntry( mod_handle );
extern unsigned         ProgPoke( address, void *, unsigned );
extern char             *GetCmdName( int );


extern char             *TxtBuff;
extern tokens           CurrToken;
extern mod_handle       CodeAddrMod;
extern dip_status       DIPStatus;


typedef struct lookup_list {
    struct lookup_list  *next;
    bool                respect_case;
    char                data[1];
} lookup;

static char CaseTab[] = { "Ignore\0Respect\0" };


static  lookup      *DefLookup;

static char AddTab[] = { "Add\0" };

static void FreeList( lookup *curr )
{
    lookup *next;

    while( curr != NULL ) {
        next = curr->next;
        _Free( curr );
        curr = next;
    }
}


/*
 * LookInit - initialize lookup
 */

static void AddLookSpec( char *start, unsigned len, bool respect, lookup *old )
{
    lookup  *next;

    _Alloc( next, sizeof( struct lookup_list ) + len );
    if( next == NULL ) {
        FreeList( DefLookup );
        DefLookup = old;
        Error( ERR_NONE, LIT( ERR_NO_MEMORY ) );
    }
    next->next = DefLookup;
    next->respect_case = respect;
    memcpy( next->data, start, len );
    next->data[len] = NULLCHAR;
    DefLookup = next;
}


void InitLook( void )
{
    DefLookup = NULL;
    // add these in reverse order since AddLookSpec adds to the front of the list
    AddLookSpec( "*_", 2, FALSE, NULL );
    AddLookSpec( "_*", 2, FALSE, NULL );
    AddLookSpec( "*", 1, FALSE, NULL );
}

void FiniLook( void )
{
    FreeList( DefLookup );
}

/*
 * LookSet - set the default symbol lookup
 */

void LookCaseSet( bool respect )
{
    lookup  *curr;

    for( curr = DefLookup; curr != NULL; curr = curr->next ) {
        curr->respect_case = respect;
    }
    _SwitchSet( SW_CASE_IGNORE, !respect );
}

void LookSet( void )
{
    struct {
        char     *start;
        size_t   len;
        bool     respect;
    }       *curr, new[20];
    lookup  *old, *next;
    bool    respect;
    bool    just_respect;
    bool    need_item;
    bool    add;

    respect = TRUE;
    add = FALSE;
    just_respect = FALSE;
    if( CurrToken == T_DIV ) {
        Scan();
        if( ScanCmd( AddTab ) == 1 ) {
            add = TRUE;
        } else {
            switch( ScanCmd( CaseTab ) ) {
            case 0:
                Error( ERR_LOC, LIT( ERR_BAD_SUBCOMMAND ), GetCmdName( CMD_SET ) );
            case 1:
                respect = FALSE;
                just_respect = TRUE;
                break;
            case 2:
                respect = TRUE;
                just_respect = TRUE;
                break;
            }
        }
    }
    curr = new;
    for( ; ; ) {
        need_item = FALSE;
        if( CurrToken == T_DIV ) {
            Scan();
            switch( ScanCmd( CaseTab ) ) {
            case 0:
                Error( ERR_LOC, LIT( ERR_BAD_SUBCOMMAND ), GetCmdName( CMD_SET ) );
            case 1:
                respect = FALSE;
                need_item = TRUE;
                just_respect = FALSE;
                break;
            case 2:
                respect = TRUE;
                need_item = TRUE;
                just_respect = FALSE;
                break;
            }
        }
        if( !ScanItem( TRUE, &curr->start, &curr->len ) ) break;
        just_respect = FALSE;
        curr->respect = respect;
        ++curr;
    }
    if( need_item ) Error( ERR_LOC, LIT( ERR_WANT_LOOKUP_ITEM ) );
    ReqEOC();
    if( just_respect ) {
        LookCaseSet( respect );
    } else {
        old = DefLookup;
        DefLookup = NULL;
        while( --curr >= new ) {
            AddLookSpec( curr->start, curr->len, curr->respect, old );
        }
        if( add ) {
            if( old != NULL ) {
                for( next = old; next->next != NULL; next = next->next )
                    ;
                next->next = DefLookup;
                DefLookup = old;
            }
        } else {
            FreeList( old );
        }
    }
    for( next = DefLookup; next != NULL; next = next->next ) {
        if( next->respect_case ) {
            _SwitchOff( SW_CASE_IGNORE );
        } else {
            _SwitchOn( SW_CASE_IGNORE );
        }
    }
}

void LookConf( void )
{
    lookup *curr;
    char   *ptr;
    bool   respect;

    respect = TRUE;
    ptr = TxtBuff;
    for( curr = DefLookup; curr != NULL; curr = curr->next ) {
        if( ptr != TxtBuff ) {
            *ptr = NULLCHAR;
            ConfigLine( TxtBuff );
            ptr = StrCopy( "/add ", TxtBuff );
            respect = TRUE;
        }
        if( respect != curr->respect_case ) {
            respect = curr->respect_case;
            if( respect ) {
                ptr = StrCopy( "/respect ", ptr );
            } else {
                ptr = StrCopy( "/ignore ", ptr );
            }
        }
        *ptr++ = '{';
        ptr = StrCopy( curr->data, ptr );
        *ptr++ = '}';
    }
    *ptr = NULLCHAR;
    ConfigLine( TxtBuff );
}

/*
 * LookUp - find a token in a list
 */
int Lookup( char *tokenlist,  char *what, int tokenlen )
{
    int         tokennum = 1;
    int         k,isuppertc;
    char        *t,*w,*ucwhat;
    char        tc,wc;

    _AllocA( ucwhat, tokenlen );
    for( k=0;k<tokenlen;k++) ucwhat[k] = toupper( what[k] );

    t = tokenlist;
    for( ;; ) {
        tc = *t;
        if( tc == 0 ) break;
        w = ucwhat;
        for( ;; ) {
            isuppertc = isupper( tc );
            if( (int) (w - ucwhat) == tokenlen ) {
                if( isuppertc ) break;
                return( tokennum );
            }
            wc = *w++;
            if( isuppertc ) {
                if( tc != wc ) break;
            } else {
                if( toupper( tc ) != wc ) break;
            }
            if( !tc ) return( tokennum );
            t++;
            tc = *t;
        }
        while( *t ) t++;
        t++;
        tokennum++;
    }
    return( 0 );
}

struct mod_lkup {
    char        *start;
    unsigned    len;
    mod_handle  mh;
};

OVL_EXTERN walk_result CheckModName( mod_handle mh, void *d )
{
    struct mod_lkup     *md = d;
    char                name[NAM_LEN];
    unsigned            len;

    len = ModName( mh, name, sizeof( name ) );
    if( len != md->len ) return( WR_CONTINUE );
    if( memicmp( name, md->start, len ) != 0 ) return( WR_CONTINUE );
    md->mh = mh;
    return( WR_STOP );
}

mod_handle LookupModName( mod_handle search, char *start, unsigned len )
{
    struct mod_lkup     d;
    static mod_handle   CacheIntMod = NO_MOD;

    /*
        We're checking for the internal mod name first since that's
        what most mod name specifiers turn out to be.
    */
    if( CacheIntMod != NO_MOD && IsInternalModName( start, len ) ) {
        return( CacheIntMod );
    }

    d.start = start;
    d.len = len;
    d.mh = NO_MOD;
    WalkModList( search, &CheckModName, &d );
    if( CacheIntMod == NO_MOD && IsInternalMod( d.mh ) ) {
        CacheIntMod = d.mh;
    }
    return( d.mh );
}


OVL_EXTERN walk_result CheckImageName( mod_handle mh, void *d )
{
    struct mod_lkup     *md = d;
    image_entry         *image;
    char                *name;
    unsigned            len;

    image = ImageEntry( mh );
    if( image == NULL ) return( WR_CONTINUE );
    name = SkipPathInfo( image->image_name, OP_REMOTE );
    len = ExtPointer( name, OP_REMOTE ) - name;
    if( len != md->len ) return( WR_CONTINUE );
    if( memicmp( name, md->start, len ) != 0 ) return( WR_CONTINUE );
    md->mh = mh;
    return( WR_STOP );
}

mod_handle LookupImageName( char *start, unsigned len )
{
    struct mod_lkup     d;

    d.start = start;
    d.len = len;
    d.mh = NO_MOD;
    WalkImageList( &CheckImageName, &d );
    return( d.mh );
}


static char *strsubst( char *dest, char *pattern, lookup_token *source )
{
    for( ;; ) {
        if( *pattern == NULLCHAR ) break;
        if( *pattern == '*' ) {
            memcpy( dest, source->start, source->len );
            source->start = dest;
            dest += source->len;
        } else {
            *dest = *pattern;
            ++dest;
        }
        ++pattern;
    }
    *dest = NULLCHAR;
    return( dest );
}


static sym_list         *SymListHead;

/*
 * LookupSymList - lookup a symbol name
 */

sym_list *LookupSymList( symbol_source ss, void *d, bool source_only,
                        lookup_item *li )
{
    lookup      *curr;
    bool        save_case;
    char        *start;
    unsigned    len;
    bool        check_codeaddr_mod;
    mod_handle  search_mod;

    check_codeaddr_mod = FALSE;
    if( ss == SS_SCOPED ) {
        if( li->mod == NO_MOD ) {
            DeAliasAddrMod( *(address *)d, &search_mod );
            if( search_mod != CodeAddrMod ) check_codeaddr_mod = TRUE;
        } else {
            ss = SS_MODULE;
            search_mod = li->mod;
            d = &search_mod;
        }
    }
    DIPStatus = DS_OK;
    if( DefLookup == NULL || source_only ) {
        li->source = li->name;
        if( check_codeaddr_mod ) {
            if(LookupSym( SS_MODULE, &CodeAddrMod, li, &SymListHead )!=SR_NONE) {
                return( SymListHead );
            }
        }
        LookupSym( ss, d, li, &SymListHead );
        if( DIPStatus == (DS_ERR|DS_INVALID_OPERATOR) ) {
            Error( ERR_NONE, LIT( ERR_INVALID_OPERATOR ), li->name.start, li->name.len );
        }
        return( SymListHead );
    }
    start = li->name.start;
    len = li->name.len;
    save_case = li->case_sensitive;
    li->name.start = TxtBuff;
    curr = DefLookup;
    for( ;; ) {
        li->case_sensitive = curr->respect_case;
        li->source.start = start;
        li->source.len   = len;
        li->name.len = strsubst( TxtBuff, curr->data, &li->source )
                                - TxtBuff;
        if( check_codeaddr_mod ) {
            if(LookupSym( SS_MODULE, &CodeAddrMod, li, &SymListHead )!=SR_NONE) {
                break;
            }
        }
        if( LookupSym( ss, d, li, &SymListHead ) != SR_NONE ) break;
        if( DIPStatus == (DS_ERR|DS_INVALID_OPERATOR) ) {
            Error( ERR_NONE, LIT( ERR_INVALID_OPERATOR ), li->name.start, li->name.len );
        }
        curr = curr->next;
        if( curr == NULL ) break;
    }
    li->case_sensitive = save_case;
    li->name.start = start;
    li->name.len = len;
    return( SymListHead );
}

void FreeSymHandle( sym_list *sl )
{
    sym_list    **owner;
    sym_list    *curr;

    owner = &SymListHead;
    for( ;; ) {
        curr = *owner;
        if( curr == NULL ) return;
        if( curr == sl ) break;
        owner = &curr->next;
    }
    *owner = curr->next;
    _Free( curr );
}

void PurgeSymHandles( void )
{
    sym_list    *curr;
    sym_list    *next;

    for( curr = SymListHead; curr != NULL; curr = next ) {
        next = curr->next;
        _Free( curr );
    }
    SymListHead = NULL;
}

static bool GetSymAddr( char *name, mod_handle mh, address *addr )
{
    lookup_item         li;
    location_list       ll;
    dip_status          ret;

    if( mh == NO_MOD ) return( FALSE );
    memset( &li, 0, sizeof( li ) );
    li.mod = mh;
    li.name.start = name;
    li.name.len   = strlen( name );
    li.source     = li.name;
    li.file_scope = TRUE;
    li.case_sensitive = TRUE;
    li.type = ST_NONE;
    switch( LookupSym( SS_MODULE, &mh, &li, &SymListHead ) ) {
    case SR_NONE:
    case SR_FAIL:
        return( FALSE );
    }
    ret = SymLocation( SL2SH( SymListHead ), NULL, &ll );
    PurgeSymHandles();
    if( ret != DS_OK ) return( FALSE );
    if( ll.num != 1 || ll.e[0].type != LT_ADDR ) return( FALSE );
    *addr = ll.e[0].u.addr;
    return( TRUE );
}


bool FindNullSym( mod_handle mh, address *addr )
{
    return( GetSymAddr( "__nullarea", mh, addr ) );
}

bool SetWDPresent( mod_handle mh )
{
    address     addr;

    if( !GetSymAddr( "__WD_Present", mh, &addr ) &&
        !GetSymAddr( "___WD_Present", mh, &addr ) ) return( FALSE );
    ProgPoke( addr, "\x1", 1 );
    return( TRUE );
}
