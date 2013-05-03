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
* Description:  C compiler symbol table management.
*
****************************************************************************/


#include "cvars.h"
#include "cgswitch.h"
#include "pragdefn.h"
#include "caux.h"

extern  void    CSegFree( SEGADDR_T );

unsigned    SymTypedef;

static  void    NewSym( void );

static unsigned Cached_sym_num;
static void     *Cached_sym_addr;
static struct sym_stats {
    unsigned    get, getptr, replace, read, write;
} SymStats;
static unsigned FirstSymInBuf;
static char     *SymBufPtr;
static unsigned NextSymHandle;

void SymInit( void )
{
    int         i;
    unsigned    seg_num;

    NextSymHandle = 0;
    Cached_sym_num = ~0u;
    SymLevel = 0;
    GblSymCount = 0;
    LclSymCount = 0;
    HashTab = (SYM_HASHPTR *)SymHashAlloc( SYM_HASH_SIZE * sizeof( SYM_HASHPTR ) );
    for( i = 0; i < SYM_HASH_SIZE; i++ ) {
        HashTab[i] = NULL;
    }
    TagHead = NULL;
    DeadTags = NULL;
    LabelHead = NULL;
    HashFreeList = NULL;
    NextSymHandle = 0;
    ErrSym = (FIELDPTR)CPermAlloc( sizeof( FIELD_ENTRY ) );
    ErrSym->field_type = TypeDefault();
    SymBufNum = 0;
    FirstSymInBuf = 0;
    LastSymBuf = 0;
    SymBufDirty = 0;
    CurFuncHandle = 0;
    for( seg_num = 0; seg_num < MAX_SYM_SEGS; ++seg_num ) {
        SymSegs[ seg_num ].allocated = 0;
    }
    SymSegment = AllocSegment( &SymSegs[0] );
    SymBufPtr = (char *)SymSegment;
    EnumInit();
}


void SymFini( void )
{
    unsigned    seg_num;
    seg_info    *si;

    for( seg_num = 0; seg_num < MAX_SYM_SEGS; ++seg_num ) {
        si = &SymSegs[ seg_num ];
        if( si->allocated ) {
            CSegFree( si->index );
        }
    }
    if( CompFlags.extra_stats_wanted ) {
        printf( "SymStats: get = %u, rep = %u, read = %u, write = %u"
            ", typedef = %u\n",
        SymStats.get, SymStats.replace, SymStats.read, SymStats.write,
        SymTypedef );
    }
}

unsigned SymGetNumSyms( void )
{
    return( NextSymHandle + 1 );
}

unsigned SymGetNumSpecialSyms( void )
{
    return( (unsigned)SpecialSyms );
}

SYM_HANDLE SymGetFirst( void )
{
    return( 0 );
}

SYM_HANDLE SymGetNext( SYM_HANDLE sym_handle )
{
    unsigned    handle = (unsigned)sym_handle;

    if( handle < NextSymHandle )
        return( (SYM_HANDLE)(handle + 1) );
    else
        return( SYM_INVALID );
}

// This is a temporary function, to be deleted later
void SetNextSymHandle( unsigned val )
{
    NextSymHandle = val;
}

SYM_HANDLE SegSymbol( char *name, int segment )             /* 15-mar-92 */
{
    SYM_ENTRY       sym;
    SYM_HANDLE      handle;

    NewSym();
    handle = (SYM_HANDLE)NextSymHandle;
    memset( &sym, 0, sizeof( SYM_ENTRY ) );
    sym.sym_type = GetType( TYPE_USHORT );
    sym.name = name;
    sym.stg_class = SC_STATIC;
    sym.level = 1;  // make invisible
    SymReplace( &sym, handle );
    if( segment ) {
        SetSegSymHandle( handle, segment );
    }
    return( handle );
}

SYM_HANDLE SpcSymbol( char *name, int stg_class )
{
    SYM_ENTRY       sym;

    NewSym();
    memset( &sym, 0, sizeof( SYM_ENTRY ) );
    sym.sym_type = GetType( TYPE_USHORT );
    sym.name = name;
    sym.stg_class = stg_class;
    SymReplace( &sym, (SYM_HANDLE)NextSymHandle );
    return( (SYM_HANDLE)NextSymHandle );
}

void SpcSymInit( void )
{
    TYPEPTR     ptr2char;
    TYPEPTR     typ;
    SYM_ENTRY   sym;

    /* first entry into SpecialSyms */
    memset( &sym, 0, sizeof( SYM_ENTRY ) );
    sym.handle = 0;
    sym.sym_type = GetType( TYPE_VOID );
    SymReplace( &sym, 0 );
#ifdef __SEH__
    NewSym();                           /* 05-dec-92 */
    TrySymHandle = (SYM_HANDLE)NextSymHandle;               /* create special _try sym */
    sym.stg_class = SC_AUTO;
    sym.name = ".try";
    sym.sym_type = GetType( TYPE_VOID );
    SymReplace( &sym, (SYM_HANDLE)NextSymHandle );
#endif
    /* create special symbol for "extern unsigned int __near __chipbug" */
    NewSym();
    SymChipBug = (SYM_HANDLE)NextSymHandle;
    sym.stg_class = SC_EXTERN;
    sym.name = "__chipbug";
    sym.sym_type = GetType( TYPE_UINT );
    SymReplace( &sym, (SYM_HANDLE)NextSymHandle );

    /* create special symbol table entry for __segname("_CODE") */
    Sym_CS = SegSymbol( ".CS", SEG_CODE );              /* 18-jan-92 */
    /* create special symbol table entry for __segname("_STACK") */
    Sym_SS = SegSymbol( ".SS", SEG_STACK );             /* 13-dec-92 */
    /* create special symbol table entry for __segname("_CONST") */
    SegSymbol( ".CONST", SEG_CONST );
    /* create special symbol table entry for __segname("_DATA") */
    SegSymbol( ".DS", SEG_DATA );

    SpecialSyms = (SYM_HANDLE)NextSymHandle;

    /* Create special symbol table entries for use by stosw, stosb pragmas
     * This should be a TYPE_FUNCTION returning pointer to char
     */
    ptr2char = PtrNode( GetType( TYPE_CHAR ), FLAG_NONE, SEG_DATA );
    typ = TypeNode( TYPE_FUNCTION, ptr2char );

    /* The ".stosw" functions are done through internal AUX entries */

    SymSTOW  = MakeFunction( "_inline_.stosw",  GetType( TYPE_VOID ) );
    SymSTOWB = MakeFunction( "_inline_.stoswb", GetType( TYPE_VOID ) );
    SymMIN       = MakeFunction( "_inline_.min", GetType( TYPE_UINT ) );
    SymMAX       = MakeFunction( "_inline_.max", GetType( TYPE_UINT ) );
    SymMEMCMP= MakeFunction( "_inline_memcmp", GetType( TYPE_INT ) );
    typ = TypeNode( TYPE_FUNCTION, GetType(TYPE_INT) );
#if _CPU == 386
    SymSTOD  = MakeFunction( "_inline_.stosd",  GetType( TYPE_VOID ) );
    SymSTOSB = MakeFunction( "__STOSB", GetType( TYPE_VOID ) );
    SymSTOSD = MakeFunction( "__STOSD", GetType( TYPE_VOID ) );
#endif
#ifdef __SEH__
    SymTryInit = MakeFunction( "__TryInit", typ );      /* 05-dec-92 */
    SymTryFini = MakeFunction( "__TryFini", typ );      /* 05-dec-92 */
    SymExcept  = MakeFunction( "__Except", typ );       /* 05-dec-92 */
    SymFinally = MakeFunction( "__Finally", typ );      /* 23-mar-94 */
    SymTryUnwind = MakeFunction( "__TryUnwind", typ );  /* 16-apr-94 */
#endif
    SymCover = MakeFunction( "__COVERAGE", typ );       /* 04-apr-92 */
    MakeFunction( "__C", typ );                 /* 04-apr-92 */
    SymGet( &sym, (SYM_HANDLE)NextSymHandle );
    sym.stg_class = SC_STATIC;
    SymReplace( &sym, (SYM_HANDLE)NextSymHandle );
}


SYM_HANDLE MakeFunction( char *id, TYPEPTR typ )
{
    SYM_ENTRY   sym;

    memset( &sym, 0, sizeof( SYM_ENTRY ) );
    sym.name = id;
    sym.stg_class = SC_EXTERN;
    sym.flags = SYM_FUNCTION;
    sym.handle = SpecialSyms;
    sym.sym_type = typ;
    NewSym();
    SpecialSyms = (SYM_HANDLE)NextSymHandle;
    SymReplace( &sym, (SYM_HANDLE)NextSymHandle );
    return( (SYM_HANDLE)NextSymHandle );
}


static void NewSym( void )
{
    ++NextSymHandle;
    if( NextSymHandle >= LARGEST_SYM_INDEX ) {
        CErr1( ERR_INTERNAL_LIMIT_EXCEEDED );
        CSuicide();
    }
}


void SymCreate( SYMPTR sym, char *id )
{
    memset( sym, 0, sizeof( SYM_ENTRY ) );
    sym->name = CMemAlloc( strlen( id ) + 1 );
    strcpy( sym->name, id );
    sym->src_loc = TokenLoc;
}


void SymAccess( unsigned sym_num )
{
    unsigned    buf_num;
    unsigned    seg_num;
    seg_info    *si;

    Cached_sym_num = sym_num;
    if( sym_num < FirstSymInBuf || sym_num >= FirstSymInBuf + SYMS_PER_BUF ) {
        buf_num = sym_num / SYMS_PER_BUF;
        if( SymBufDirty ) {
            ++SymStats.write;
            si = &SymSegs[ SymSegNum ];
            if( !si->allocated ) {
                SymSegment = AllocSegment( si );
            }
            SymBufDirty = 0;
        }
        seg_num = buf_num / SYMBUFS_PER_SEG;
        si = &SymSegs[ seg_num ];
        SymSegment = AccessSegment( si );
        SymBufPtr = (char *)SymSegment + (buf_num % SYMBUFS_PER_SEG) * SYM_BUF_SIZE;
        SymBufNum = buf_num;
        SymSegNum = seg_num;
        FirstSymInBuf = buf_num * SYMS_PER_BUF;
    }
    Cached_sym_addr = SymBufPtr + (sym_num - FirstSymInBuf) * sizeof( SYM_ENTRY );
}

SYMPTR SymGetPtr( SYM_HANDLE sym_handle )
{
    unsigned    handle = (unsigned)sym_handle;
    SYMPTR      symptr;

    ++SymStats.getptr;
    if( sym_handle == CurFuncHandle ) {
        symptr = &CurFuncSym;
    } else if( handle < PCH_MaxSymHandle ) {        /* 08-mar-94 */
        symptr = PCH_SymArray[ handle ];
    } else {
        if( handle != Cached_sym_num ) {
            SymAccess( handle );
        }
        symptr = Cached_sym_addr;
    }
    return( symptr );
}

void SymGet( SYMPTR sym, SYM_HANDLE sym_handle )
{
    unsigned    handle = (unsigned)sym_handle;
    SYMPTR      symptr;

    ++SymStats.get;
    if( sym_handle == CurFuncHandle ) {
        symptr = &CurFuncSym;
    } else if( handle < PCH_MaxSymHandle ) {        /* 08-mar-94 */
        symptr = PCH_SymArray[ handle ];
    } else {
        if( handle != Cached_sym_num ) {
            SymAccess( handle );
        }
        symptr = Cached_sym_addr;
    }
    memcpy( sym, symptr, sizeof( SYM_ENTRY ) );
}


void SymReplace( SYMPTR sym, SYM_HANDLE sym_handle )
{
    unsigned    handle = (unsigned)sym_handle;

    ++SymStats.replace;
    if( sym_handle == CurFuncHandle ) {
        memcpy( &CurFuncSym, sym, sizeof( SYM_ENTRY ) );
    }
    if( handle < PCH_MaxSymHandle ) {       /* 08-mar-94 */
        memcpy( PCH_SymArray[ handle ], sym, sizeof( SYM_ENTRY ) );
    } else {
        if( handle != Cached_sym_num ) {
            SymAccess( handle );
        }
        memcpy( Cached_sym_addr, sym, sizeof( SYM_ENTRY ) );
        SymBufDirty = 1;
    }
}


SYM_HASHPTR SymHash( SYMPTR sym, SYM_HANDLE sym_handle )
{
    SYM_HASHPTR     hsym;
    TYPEPTR         typ;
    int             sym_len;

    sym_len = strlen( sym->name );
    hsym = SymHashAlloc( offsetof( sym_hash_entry, name ) + sym_len + 1 );
    hsym->sym_type = NULL;
    if( sym->stg_class == SC_TYPEDEF ) {        /* 28-feb-92 */
        typ = sym->sym_type; {
            typ = TypeNode( TYPE_TYPEDEF, typ );
            typ->u.typedefn = sym_handle;
            sym->sym_type = typ;
        }
        hsym->sym_type = typ;
    }
    hsym->level = sym->level;
    far_memcpy( hsym->name, sym->name, sym_len + 1 );
    if( sym->name[0] != '.' ) {                 /* 19-mar-93 */
        CMemFree( sym->name );
    }
    sym->name = NULL;
    hsym->handle = sym_handle;
    return( hsym );
}


SYM_HANDLE SymAdd( int h, SYMPTR sym )
{
    SYM_HASHPTR     hsym;
    SYM_HASHPTR     *head;

    if( sym == NULL ) return( 0 );
    if( SymLevel == 0 ) {
        ++GblSymCount;
    } else {
        ++LclSymCount;
    }
    NewSym();
    sym->level = SymLevel;
    hsym = SymHash( sym, (SYM_HANDLE)NextSymHandle );
    sym->info.hash_value = h;
    head = &HashTab[ h ];               /* add name to head of list */
    for( ;; ) {
        if( *head == NULL ) break;
        if( (*head)->level <= SymLevel ) break;
        head = &(*head)->next_sym;
    }
    hsym->next_sym = *head;     /* add name to head of list */
    *head = hsym;
    return( hsym->handle );
}


SYM_HANDLE SymAddL0( int h, SYMPTR new_sym )    /* add symbol to level 0 */
{
    SYM_HASHPTR     hsym;
    SYM_HASHPTR     new_hsym;

    if( new_sym == NULL ) return( 0 );
    ++GblSymCount;
    NewSym();
    new_sym->level = 0;
    new_hsym = SymHash( new_sym, (SYM_HANDLE)NextSymHandle );
    new_hsym->next_sym = NULL;
    new_sym->info.hash_value = h;
    hsym = HashTab[h];
    if( hsym == NULL ) {
        HashTab[h] = new_hsym;
    } else {
        while( hsym->next_sym != NULL ) {
            hsym = hsym->next_sym;
        }
        hsym->next_sym = new_hsym;      /* add new symbol to end of list */
    }
    return( new_hsym->handle );
}


SYM_HANDLE GetNewSym( SYMPTR sym, char id, TYPEPTR typ, int stg_class )
{
    char            name[3];
    SYM_HANDLE      sym_handle;

    name[0] = '.';
    name[1] = id;
    name[2] = '\0';
    SymCreate( sym, name );
    sym_handle = SymAdd( 0, sym );
    sym->sym_type = typ;
    sym->stg_class = stg_class;
    sym->flags |= SYM_REFERENCED | SYM_TEMP;
    return( sym_handle );
}


SYM_HANDLE MakeNewSym( SYMPTR sym, char id, TYPEPTR typ, int stg_class )
{
    SYM_HANDLE  sym_handle;

    sym_handle = GetNewSym( sym, id, typ, stg_class );
    if( SymLevel != 0 ) {                               /* 07-may-91 */
        sym->handle = CurFunc->u.func.locals;
        CurFunc->u.func.locals = sym_handle;
    }
    ++TmpSymCount;
    return( sym_handle );
}


SYM_HANDLE SymLook( int h, char *id )
{
    int             len;
    SYM_HASHPTR     hsym;

    len = strlen( id ) + 1;
    for( hsym = HashTab[h]; hsym; hsym = hsym->next_sym ) {
        if( far_strcmp( hsym->name, id, len ) == 0 ) {
            return( hsym->handle );
        }
    }
    return( 0 );
}


SYM_HANDLE SymLookTypedef( int h, char *id, SYMPTR sym )  /* 28-feb-92 */
{
    int             len;
    SYM_HASHPTR     hsym;

    len = strlen( id ) + 1;
    for( hsym = HashTab[h]; hsym; hsym = hsym->next_sym ) {
        if( far_strcmp( hsym->name, id, len ) == 0 ) {
            if( hsym->sym_type == NULL ) break;
            sym->sym_type = hsym->sym_type;
            sym->stg_class = SC_TYPEDEF;
            sym->level = hsym->level;
            return( hsym->handle );
        }
    }
    return( 0 );
}


SYM_HANDLE Sym0Look( int h, char *id )  /* look for symbol on level 0 */
{
    int             len;
    SYM_HASHPTR     hsym;

    len = strlen( id ) + 1;
    for( hsym = HashTab[h]; hsym; hsym = hsym->next_sym ) {
        if( far_strcmp( hsym->name, id, len ) == 0 ) {  /* name matches */
            if( hsym->level == 0 ) return( hsym->handle );
        }
    }
    return( 0 );
}


local void ChkReference( SYM_ENTRY *sym, SYM_NAMEPTR name )
{
    if( sym->flags & SYM_DEFINED ) {
        if( sym->stg_class != SC_EXTERN ) {
            if( !(sym->flags & SYM_REFERENCED) ) {
                if( !(sym->flags & SYM_IGNORE_UNREFERENCE) ) {  /*25-apr-91*/
                    if( sym->is_parm ) {
                        CWarn2p( WARN_PARM_NOT_REFERENCED, ERR_PARM_NOT_REFERENCED, name );
                    } else {
                        CWarn2p( WARN_SYM_NOT_REFERENCED, ERR_SYM_NOT_REFERENCED, name );
                    }
                }
            } else if( ! (sym->flags & SYM_ASSIGNED) ) {
                if( sym->sym_type->decl_type != TYPE_ARRAY
                &&  sym->stg_class != SC_STATIC ) {         /* 06-aug-90 */
                    CWarn2p( WARN_SYM_NOT_ASSIGNED, ERR_SYM_NOT_ASSIGNED, name );
                }
            }
        }
    }
}


local void ChkIncomplete( SYM_ENTRY *sym, SYM_NAMEPTR name )
{
    TYPEPTR     typ;

    if( sym->stg_class != SC_TYPEDEF ) {
        if( !(sym->flags & (SYM_FUNCTION | SYM_TEMP)) ) {
            if( (sym->flags & SYM_REFERENCED) == 0 ) {
                /* if it wasn't referenced, don't worry 19-sep-90 AFS */
                if( sym->stg_class == SC_EXTERN ) {     /* 08-nov-91 */
                    return;
                }
            }
            typ = sym->sym_type;
            SKIP_TYPEDEFS( typ );
            if( SizeOfArg( typ ) == 0  &&  typ->decl_type != TYPE_FUNCTION
            &&  typ->decl_type != TYPE_DOT_DOT_DOT ) {
                if( !(sym->stg_class == SC_EXTERN) ) {
                    CErr2p( ERR_INCOMPLETE_TYPE, name );
                }
            }
        }
    }
}


local void ChkDefined( SYM_ENTRY *sym, SYM_NAMEPTR name )
{
    if( sym->flags & SYM_DEFINED ) {
        if( sym->stg_class == SC_STATIC ) {
            if( !(sym->flags & SYM_REFERENCED) ) {
                if( !(sym->flags & SYM_IGNORE_UNREFERENCE) ) {  /*14-may-91*/
                    CWarn2p( WARN_SYM_NOT_REFERENCED, ERR_SYM_NOT_REFERENCED, name );
                }
            }
        }
    } else {    /* not defined */
        if( sym->flags & SYM_REFERENCED ) {     /* 28-apr-88 AFS */
            if( sym->stg_class == SC_STATIC ) {
                if( sym->flags & SYM_FUNCTION ) {
                    /* Check to see if we have a matching aux entry with code attached */
                    aux_entry   *paux = AuxLookup( name );
                    if( !paux || !paux->info || !paux->info->code ) {
                        CErr2p( ERR_FUNCTION_NOT_DEFINED, name );
                    }
                }
            } else if( sym->stg_class == SC_FORWARD ) {
                sym->stg_class = SC_EXTERN;
            }
        }
    }
}

local void ChkFunction( SYMPTR sym, SYM_NAMEPTR name )
{
#if _CPU == 8086 || _CPU == 386                         /* 05-nov-91 */
    if( sym->stg_class == SC_STATIC ) {
        if( sym->flags & SYM_ADDR_TAKEN ) {
            if( CompFlags.using_overlays ) {
                CWarn2p( WARN_ADDR_OF_STATIC_FUNC_TAKEN, ERR_ADDR_OF_STATIC_FUNC_TAKEN, name );
            }
        } else {
            if( (sym->attrib & (FLAG_FAR | FLAG_NEAR)) == 0
            && (TargetSwitches & BIG_CODE)
            && !CompFlags.multiple_code_segments ) {
                sym->attrib |= FLAG_NEAR;
            }
        }
    }
#else
    sym = sym;
    name = name;
#endif
}

#if _CPU == 370
/*** External name control for IBM 370 restrictions ***/
struct xlist {
    struct xlist    *next;
    char            xname[8+1];
};

local  void InitExtName( struct xlist **where  )
/*** Init extern name list***/
{
    *where = NULL;
}

local void ChkExtName( struct xlist **link, SYM_ENTRY *sym, SYM_NAMEPTR name  )
/***Restricted extern names i.e 8 char upper check *****/
{
    struct xlist    *new, *curr;

    new =  CMemAlloc( sizeof ( struct xlist ) );
    Copy8( name, new->xname );
    strupr( new->xname );
    while( (curr = *link) != NULL ) {
        int cmp;
        cmp =  strcmp( new->xname, curr->xname );
        if( cmp == 0 ) {
            SetErrLoc( &sym->src_loc );
            CErr3p( ERR_DUPLICATE_ID, name, new->xname );
            InitErrLoc();
            CMemFree( new );
            return;
        } else if( cmp < 0 ) {
            break;
        }
        link = &curr->next;
    }
    new->next = *link;
    *link = new;
}

local void FiniExtName( struct xlist *head )
/*** Free xname list **********************/
{
    struct xlist    *next;

    while( head != NULL ) {
        next = head->next;
        CMemFree( head );
        head = next;
    }
}

static  void    Copy8( char const *nstr, char *name )
/***************************************************/
{
    char        *curr;

    for( curr = name; curr < &name[8]; curr++,nstr++ ) {
        if( *nstr == '\0' ) break;
        *curr = *nstr;
    }
    *curr = '\0';
}
#endif /* IBM370 names */

/* divide all the global symbols into buckets based on size of the item
   0 - functions
   1 - 1-byte items
   2 - odd-length items
   3 - 2-byte items
   4 - even-length items (that are not a multiple of 4 in size)
   5 - 4-byte items (or multiple of 4, but not a multiple of 8)
   6 - 8-byte items (or multiple of 8)
*/

#define BUCKETS 7

local int SymBucket( SYM_ENTRY *sym )   /* determine bucket # for symbol */
{
    int             bucket;
    unsigned long   size;

    bucket = 0; /* assume its a function */
    if( (sym->flags & SYM_FUNCTION) == 0 ) {    /* if variable */
        size = SizeOfArg( sym->sym_type );
        switch( size % 8 ) {
        case 0:                 /* multiple of 8 */
            bucket = 6;
            break;
        case 4:                 /* multiple of 4 */
            bucket = 5;
            break;
        case 1:
        case 3:
        case 5:
        case 7:
            bucket = 2;             /* odd length objects */
            if( size == 1 )
                bucket = 1;
            break;
        case 2:
        case 6:
            bucket = 4;             /* even length objects */
            if( size == 2 )
                bucket = 3;
            break;
        }
    }
    return( bucket );
}

local SYM_HASHPTR GetSymList( void )                    /* 25-jun-92 */
{
    SYM_HASHPTR     hsym;
    SYM_HASHPTR     next_hsymptr;
    SYM_HASHPTR     sym_list;
    SYM_HASHPTR     sym_tail;
    unsigned        i;
    unsigned        j;
    SYM_HASHPTR     sym_seglist[ MAX_SYM_SEGS ];
    SYM_HASHPTR     sym_buflist[ SYMBUFS_PER_SEG ];
    SYM_HASHPTR     sym_buftail[ SYMBUFS_PER_SEG ];

    sym_list = NULL;
    for( i=0; i < SYM_HASH_SIZE; i++ ) {
        for( hsym = HashTab[i]; hsym; hsym = next_hsymptr ) {
            if( hsym->level != SymLevel ) break;
            next_hsymptr = hsym->next_sym;
            hsym->next_sym = sym_list;
            sym_list = hsym;
        }
        HashTab[i] = hsym;
    }
    // if SymLevel == 0 then should sort the sym_list so that we don't do
    // a lot of page thrashing.
    if( SymLevel == 0 ) {
        for( i = 0; i < MAX_SYM_SEGS; i++ ) {
            sym_seglist[i] = NULL;
        }
        for( hsym = sym_list; hsym; hsym = next_hsymptr ) {
            next_hsymptr = hsym->next_sym;
            i = (unsigned)hsym->handle / (SYMS_PER_BUF * SYMBUFS_PER_SEG);
            hsym->next_sym = sym_seglist[i];
            sym_seglist[i] = hsym;
        }
        sym_list = NULL;
        sym_tail = NULL;
        for( i = 0; i < MAX_SYM_SEGS; i++ ) {
            for( j = 0; j < SYMBUFS_PER_SEG; j++ ) {
                sym_buflist[j] = NULL;
                sym_buftail[j] = NULL;
            }
            for( hsym = sym_seglist[i]; hsym; hsym = next_hsymptr ) {
                next_hsymptr = hsym->next_sym;
                j = ((unsigned)hsym->handle / SYMS_PER_BUF) % SYMBUFS_PER_SEG;
                hsym->next_sym = sym_buflist[j];
                sym_buflist[j] = hsym;
                if( sym_buftail[j] == NULL )
                    sym_buftail[j] = hsym;
            }
            for( j = 0; j < SYMBUFS_PER_SEG; j++ ) {
                hsym = sym_buflist[j];
                if( hsym != NULL ) {
                    if( sym_list == NULL )
                        sym_list = hsym;
                    if( sym_tail != NULL )
                        sym_tail->next_sym = hsym;
                    sym_tail = sym_buftail[j];
                }
            }
        }
    }
    return( sym_list );
}

local SYM_HASHPTR FreeSym( void )
{
    SYM_HASHPTR     hsym;
    SYM_HASHPTR     next_hsymptr;
    SYM_HASHPTR     sym_list;
    int             sym_len;
    int             bucket;
    SYM_HANDLE      prev_tail;
    SYM_ENTRY       sym;
    SYM_HANDLE      head[BUCKETS];
    SYM_HANDLE      tail[BUCKETS];
  #if _CPU == 370  /* MJC  */
    void            *xlist;
    InitExtName( &xlist );
  #endif

    for( bucket = 0; bucket < BUCKETS; ++bucket ) {
        head[ bucket ] = 0;
        tail[ bucket ] = 0;
    }
    sym_list = GetSymList();
    for( hsym = sym_list; hsym; hsym = next_hsymptr ) {
        SymGet( &sym, hsym->handle );
        next_hsymptr = hsym->next_sym;
        if( (sym.stg_class == SC_TYPEDEF ||             /* 10-nov-87 FWC */
            (SymLevel != 0) ||
            (sym.flags & (SYM_REFERENCED | SYM_DEFINED))) ||
            (sym.stg_class == SC_NULL) ) {
            if( SymLevel == 0 ) {
                bucket = SymBucket( &sym );
                sym.handle = head[ bucket ];
                if( ( sym.flags & SYM_FUNCTION ) == 0 ) {  /* if var */
                    if( sym.stg_class == SC_NULL ) {    /* 28-nov-90 */
                        if( sym.sym_type->decl_type == TYPE_ARRAY ) {
                            if( sym.sym_type->u.array->dimension == 0 ) {
                                sym.sym_type->u.array->dimension = 1;
                            }
                        }
                    }
                    AssignSeg( &sym );
                } else { /* FUNCTION */                 /* 28-oct-91 */
                    ChkFunction( &sym, hsym->name );    /* 05-nov-91 */
                }
                if( tail[ bucket ] == 0 ) {
                    tail[ bucket ] = hsym->handle;
                }
                head[ bucket ] = hsym->handle;
            }
/* keep static names so that we can output static pubdefs and get nicer
   -d1 debugging */
            sym_len = far_strlen_plus1( hsym->name );
            sym.name = CPermAlloc( sym_len );
            far_memcpy( sym.name, hsym->name, sym_len );
            sym.info.backinfo = NULL;
            SymReplace( &sym, hsym->handle );
        }
        SetErrLoc( &sym.src_loc );
        ChkIncomplete( &sym, hsym->name );
        if( SymLevel == 0 ) {
            ChkDefined( &sym, hsym->name );
        } else {
            ChkReference( &sym, hsym->name );
            if( sym.stg_class == SC_STATIC              /* 27-nov-91 */
            && !(sym.flags & SYM_FUNCTION) ) {
                CurFuncNode->op.func.flags &= ~FUNC_OK_TO_INLINE;
                SymReplace( CurFunc, CurFuncHandle );
            }
        }
        InitErrLoc();
    }
    if( SymLevel == 0 ) {
        GlobalSym = 0;
        prev_tail = 0;
        for( bucket = BUCKETS - 1; bucket >= 0; --bucket ) {
            if( head[ bucket ] != 0 ) {
                if( GlobalSym == 0 ) {
                    GlobalSym = head[ bucket ];
                }
                if( prev_tail != 0 ) {
                    SymGet( &sym, prev_tail );
                    sym.handle = head[ bucket ];
                    SymReplace( &sym, prev_tail );
                }
                prev_tail = tail[ bucket ];
            }
        }
    }
    return( sym_list );
}


void AsgnSegs( SYM_HANDLE sym_handle )
{
    SYM_ENTRY   sym;

    for( ; sym_handle; ) {
        SymGet( &sym, sym_handle );
        if( ( sym.flags & SYM_FUNCTION ) == 0 ) {  /* if variable */
            AssignSeg( &sym );
            SymReplace( &sym, sym_handle );
        }
        sym_handle = sym.handle;
    }
}


void EndBlock( void )
{
    SYM_HASHPTR sym_list;

    FreeEnums();
    FreeTags();
    sym_list = FreeSym();
    if( SymLevel != 0 ) {
/*      CurFunc->u.func.locals = FreeVars( sym_list );  */
        if( SymLevel == 1 ) {
            AsgnSegs( CurFunc->u.func.locals );
/*          DumpWeights( CurFunc->u.func.locals ); */
        } else {
            AsgnSegs( GetBlockSymList() );
        }
    }
    --SymLevel;
}


#if 0
local void DumpWeights( SYMPTR sym )
{
    if( DebugFlag > 0 ) {
        for( ; sym; sym = sym->thread ) {
            printf( "%4x: %s\n", sym->u.var.offset, sym->name );
        }
    }
}
#endif



LABELPTR LkLabel( char *name )
{
    LABELPTR    label;

    label = LabelHead;
    while( label != NULL ) {
        if( strcmp( name, label->name ) == 0 ) return( label );
        label = label->next_label;
    }
    label = (LABELPTR)CMemAlloc( sizeof( LABELDEFN ) + strlen( name ) );
    if( label != NULL ) {
        label->next_label = LabelHead;
        LabelHead = label;
        strcpy( label->name, name );
        label->defined    = 0;
        label->referenced = 0;
        label->ref_list   = NextLabel();
    }
    return( label );
}


void FreeLabels( void )
{
    LABELPTR    label;

    for( ; (label = LabelHead) != NULL; ) {
        LabelHead = label->next_label;
        if( label->defined == 0 ) {
            CErr2p( ERR_UNDEFINED_LABEL, label->name );
        } else if( label->referenced == 0 ) {           /* 05-apr-91 */
            CWarn2p( WARN_UNREFERENCED_LABEL, ERR_UNREFERENCED_LABEL, label->name );
        }
        CMemFree( label );
    }
}


#if 0
static void DoSymPurge( SYMPTR sym )
{
    SYMPTR  curr;
    SYMPTR  temp;

    if( (sym->flags & SYM_FUNCTION) && (sym != CurFunc) ) {
        curr = sym->u.func.parms;
        while( curr != NULL ) {
            temp = curr->thread;
            CMemFree( curr );
            curr = temp;
        }
        curr = sym->u.func.locals;
        while( curr != NULL ) {
            temp = curr->thread;
            CMemFree( curr );
            curr = temp;
        }
    }
    CMemFree( sym );
}
#endif


void SymsPurge( void )
{
    LABELPTR    label;

    label = LabelHead;
    while( label != NULL ) {
        LabelHead = label->next_label;
        CMemFree( label );
        label = LabelHead;
    }
//      EnumInit();
//      PurgeTags( TagHead );
    TagHead = NULL;
//      PurgeTags( DeadTags );
    DeadTags = NULL;

#if 0
    {
        int     i;
        SYMPTR  tmp_sym, sym;

        for( i = 0; i < SYM_HASH_SIZE; i++ ) {
            sym = HashTab[i];
            HashTab[i] = NULL;
            while( sym != NULL ) {
                tmp_sym = sym->next_sym;
                DoSymPurge( sym );
                sym = tmp_sym;
            }
        }

        sym = GlobalSym;
        while( sym != NULL ) {
            GlobalSym = sym->thread;
            DoSymPurge( sym );
            sym = GlobalSym;
        }

        sym = SpecialSyms;
        while( sym != NULL ) {
            SpecialSyms = sym->thread;
            DoSymPurge( sym );
            sym = SpecialSyms;
        }
    }
#endif
    TypesPurge();
}

#if 0           /* can't free these because they are in CPermArea */

local void PurgeTags( TAGPTR tag_head )
{
    TAGPTR      tag;
    FIELDPTR    field;
    int         tag_type;

    for( ; tag = tag_head; ) {
        tag_type = tag->sym_type->decl_type;
        if( tag_type == TYPE_STRUCT  ||  tag_type == TYPE_UNION ) {
            for( ; field = tag->u.field_list; ) {
                tag->u.field_list = field->next_field;
                CMemFree( field );
            }
        } else {        /* tag_type == TYPE_ENUM */
            PurgeEnums( tag->u.enum_list );
        }
        tag_head = tag->next_tag;
        CMemFree( tag );
    }
}


local void PurgeEnums( ENUM_HANDLE list_head )
{
    ENUMPTR     ep;

    for( ; ep = list_head; ) {
        list_head = ep->thread;
        CMemFree( ep );
    }
}

#endif

XREFPTR NewXref( XREFPTR next_xref )
{
    XREFPTR     xref;

    xref = (XREFPTR)CMemAlloc( sizeof( XREF_ENTRY ) );
    xref->next_xref = next_xref;
    xref->src_loc = TokenLoc;
    return( xref );
}

void FreeXrefs( XREFPTR xref )
{
    XREFPTR     next;

    while( xref != NULL ) {
        next = xref->next_xref;
        CMemFree( xref );
        xref = next;
    }
}
