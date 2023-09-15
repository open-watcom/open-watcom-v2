/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbglit.h"
#include "dbgerr.h"
#include "dbgmem.h"
#include "dbgio.h"
#include "strutil.h"
#include "dbgscan.h"
#include "dbgmain.h"
#include "dbgshow.h"
#include "dbgprog.h"
#include "remcore.h"
#include "dipimp.h"
#include "dipinter.h"
#include "enterdb.h"
#include "dbglkup.h"
#include "dbgsetfg.h"
#include "dbgmisc.h"

#include "clibext.h"


#define CASE_OPTS \
    pick( "Ignore",  CASE_IGNORE  ) \
    pick( "Respect", CASE_RESPECT )

#define SYM_STR(x)  DEBUG_SYM_STR(x)

enum {
    #define pick(t,e)   e,
    CASE_OPTS
    #undef pick
};

typedef struct lookup_list {
    struct lookup_list  *next;
    bool                respect_case;
    char                data[1];
} lookup;

static  lookup      *DefLookup;

static const char CaseTab[] = {
    #define pick(t,e)   t "\0"
    CASE_OPTS
    #undef pick
};

static void FreeList( lookup *curr )
{
    lookup *next;

    for( ; curr != NULL; curr = next ) {
        next = curr->next;
        _Free( curr );
    }
}


/*
 * LookInit - initialize lookup
 */

static void AddLookSpec( const char *start, unsigned len, bool respect, lookup *old )
{
    lookup  *next;

    _Alloc( next, sizeof( struct lookup_list ) + len );
    if( next == NULL ) {
        FreeList( DefLookup );
        DefLookup = old;
        Error( ERR_NONE, LIT_ENG( ERR_NO_MEMORY ) );
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
    AddLookSpec( "*_", 2, false, NULL );
    AddLookSpec( "_*", 2, false, NULL );
    AddLookSpec( "*", 1, false, NULL );
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
        const char  *start;
        size_t      len;
        bool        respect;
    }       *curr, new[20];
    lookup  *old, *next;
    bool    respect;
    bool    just_respect;
    bool    need_item;
    bool    add;

    respect = true;
    add = false;
    just_respect = false;
    if( CurrToken == T_DIV ) {
        Scan();
        if( ScanCmdAdd() ) {
            add = true;
        } else {
            switch( ScanCmd( CaseTab ) ) {
            case CASE_IGNORE:
                respect = false;
                just_respect = true;
                break;
            case CASE_RESPECT:
                respect = true;
                just_respect = true;
                break;
            default:
                Error( ERR_LOC, LIT_ENG( ERR_BAD_SUBCOMMAND ), GetCmdName( CMD_SET ) );
                break;
            }
        }
    }
    for( curr = new; ; ++curr ) {
        need_item = false;
        if( CurrToken == T_DIV ) {
            Scan();
            switch( ScanCmd( CaseTab ) ) {
            case CASE_IGNORE:
                respect = false;
                need_item = true;
                just_respect = false;
                break;
            case CASE_RESPECT:
                respect = true;
                need_item = true;
                just_respect = false;
                break;
            default:
                Error( ERR_LOC, LIT_ENG( ERR_BAD_SUBCOMMAND ), GetCmdName( CMD_SET ) );
                break;
            }
        }
        if( !ScanItem( true, &curr->start, &curr->len ) )
            break;
        just_respect = false;
        curr->respect = respect;
    }
    if( need_item )
        Error( ERR_LOC, LIT_ENG( ERR_WANT_LOOKUP_ITEM ) );
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

    respect = true;
    ptr = TxtBuff;
    for( curr = DefLookup; curr != NULL; curr = curr->next ) {
        if( ptr != TxtBuff ) {
            *ptr = NULLCHAR;
            ConfigLine( TxtBuff );
            ptr = StrCopyDst( "/add ", TxtBuff );
            respect = true;
        }
        if( respect != curr->respect_case ) {
            respect = curr->respect_case;
            if( respect ) {
                ptr = StrCopyDst( "/respect ", ptr );
            } else {
                ptr = StrCopyDst( "/ignore ", ptr );
            }
        }
        *ptr++ = '{';
        ptr = StrCopyDst( curr->data, ptr );
        *ptr++ = '}';
    }
    *ptr = NULLCHAR;
    ConfigLine( TxtBuff );
}

/*
 * LookUp - find a token in a list
 */
int Lookup( const char *tokenlist,  const char *what, size_t tokenlen )
{
    int         tokennum = 0;
    int         isuppertc;
    size_t      k;
    char        *w,*ucwhat;
    char        tc,wc;
    const char  *t;

    _AllocA( ucwhat, tokenlen );
    for( k = 0; k < tokenlen; k++ )
        ucwhat[k] = toupper( what[k] );

    for( t = tokenlist; (tc = *t) != NULLCHAR; ++t ) {
        w = ucwhat;
        for( ;; ) {
            isuppertc = isupper( tc );
            if( (size_t)( w - ucwhat ) == tokenlen ) {
                if( isuppertc )
                    break;
                return( tokennum );
            }
            wc = *w++;
            if( isuppertc ) {
                if( tc != wc ) {
                    break;
                }
            } else {
                if( toupper( tc ) != wc ) {
                    break;
                }
            }
            if( tc == NULLCHAR )
                return( tokennum );
            t++;
            tc = *t;
        }
        while( *t != NULLCHAR )
            t++;
        tokennum++;
    }
    return( -1 );
}

struct mod_lkup {
    const char  *start;
    unsigned    len;
    mod_handle  mh;
};

static walk_result CheckModName( mod_handle mh, void *d )
{
    struct mod_lkup     *md = d;
    char                name[NAM_LEN];
    unsigned            len;

    len = DIPModName( mh, name, sizeof( name ) );
    if( len != md->len )
        return( WR_CONTINUE );
    if( strnicmp( name, md->start, len ) != 0 )
        return( WR_CONTINUE );
    md->mh = mh;
    return( WR_STOP );
}

mod_handle LookupModName( mod_handle search, const char *start, unsigned len )
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
    DIPWalkModList( search, &CheckModName, &d );
    if( CacheIntMod == NO_MOD && IsInternalMod( d.mh ) ) {
        CacheIntMod = d.mh;
    }
    return( d.mh );
}


static walk_result CheckImageName( mod_handle mh, void *d )
{
    struct mod_lkup     *md = d;
    image_entry         *image;
    const char          *name;
    unsigned            len;

    image = ImageEntry( mh );
    if( image == NULL )
        return( WR_CONTINUE );
    name = SkipPathInfo( image->image_name, OP_REMOTE );
    len = ExtPointer( name, OP_REMOTE ) - name;
    if( len != md->len )
        return( WR_CONTINUE );
    if( strnicmp( name, md->start, len ) != 0 )
        return( WR_CONTINUE );
    md->mh = mh;
    return( WR_STOP );
}

mod_handle LookupImageName( const char *start, unsigned len )
{
    struct mod_lkup     d;

    d.start = start;
    d.len = len;
    d.mh = NO_MOD;
    DIPWalkImageList( &CheckImageName, &d );
    return( d.mh );
}


static char *strsubst( char *dest, char *pattern, lookup_token *source )
{
    for( ; *pattern != NULLCHAR; ++pattern ) {
        if( *pattern == '*' ) {
            memcpy( dest, source->start, source->len );
            source->start = dest;
            dest += source->len;
        } else {
            *dest = *pattern;
            ++dest;
        }
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
    const char  *save_name;
    unsigned    save_len;
    bool        check_codeaddr_mod;
    mod_handle  search_mod;

    check_codeaddr_mod = false;
    if( ss == SS_SCOPED ) {
        if( li->mod == NO_MOD ) {
            DeAliasAddrMod( *(address *)d, &search_mod );
            if( search_mod != CodeAddrMod ) {
                check_codeaddr_mod = true;
            }
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
            if(DIPLookupSym( SS_MODULE, &CodeAddrMod, li, &SymListHead )!=SR_NONE) {
                return( SymListHead );
            }
        }
        DIPLookupSym( ss, d, li, &SymListHead );
        if( DIPStatus == (DS_ERR|DS_INVALID_OPERATOR) ) {
            Error( ERR_NONE, LIT_ENG( ERR_INVALID_OPERATOR ), li->name.start, li->name.len );
        }
        return( SymListHead );
    }
    save_name = li->name.start;
    save_len = li->name.len;
    save_case = li->case_sensitive;
    li->name.start = TxtBuff;
    for( curr = DefLookup; curr != NULL; curr = curr->next ) {
        li->case_sensitive = curr->respect_case;
        li->source.start = save_name;
        li->source.len   = save_len;
        li->name.len = strsubst( TxtBuff, curr->data, &li->source ) - TxtBuff;
        if( check_codeaddr_mod ) {
            if( DIPLookupSym( SS_MODULE, &CodeAddrMod, li, &SymListHead ) != SR_NONE ) {
                break;
            }
        }
        if( DIPLookupSym( ss, d, li, &SymListHead ) != SR_NONE )
            break;
        if( DIPStatus == (DS_ERR|DS_INVALID_OPERATOR) ) {
            Error( ERR_NONE, LIT_ENG( ERR_INVALID_OPERATOR ), li->name.start, li->name.len );
        }
    }
    li->case_sensitive = save_case;
    li->name.start = save_name;
    li->name.len = save_len;
    return( SymListHead );
}

void FreeSymHandle( sym_list *sl )
{
    sym_list    **owner;
    sym_list    *curr;

    for( owner = &SymListHead; (curr = *owner) != NULL; owner = &curr->next ) {
        if( curr == sl ) {
            *owner = curr->next;
            _Free( curr );
            break;
        }
    }
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

static bool GetSymAddr( const char *name, mod_handle mh, address *addr )
{
    lookup_item         li;
    location_list       ll;
    dip_status          ds;

    if( mh == NO_MOD )
        return( false );
    memset( &li, 0, sizeof( li ) );
    li.mod = mh;
    li.name.start = name;
    li.name.len   = strlen( name );
    li.source     = li.name;
    li.file_scope = true;
    li.case_sensitive = true;
    li.type = ST_NONE;
    switch( DIPLookupSym( SS_MODULE, &mh, &li, &SymListHead ) ) {
    case SR_NONE:
    case SR_FAIL:
        return( false );
    }
    ds = DIPSymLocation( SL2SH( SymListHead ), NULL, &ll );
    PurgeSymHandles();
    if( ds != DS_OK )
        return( false );
    if( ll.num != 1 || ll.e[0].type != LT_ADDR )
        return( false );
    *addr = ll.e[0].u.addr;
    return( true );
}


bool FindNullSym( mod_handle mh, address *addr )
{
    return( GetSymAddr( "__nullarea", mh, addr ) );
}

bool SetWDPresent( mod_handle mh )
{
    address     addr;

    if( !GetSymAddr( SYM_STR( DEBUG_PRESENT_NAME ), mh, &addr ) )
        return( false );
    ProgPoke( addr, "\x1", 1 );
    return( true );
}
