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
* Description:  Dumper and test program for DIPs.
*
****************************************************************************/


#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include "walloca.h"
#include "bool.h"
#include "dip.h"
#include "digcli.h"

/**
 * Options
 */
static struct {
    unsigned    do_cue_tests : 1;   /* Perform the various Cue tests. */
} Opts = { 1 };


/**
 * Prints an error message.
 *
 * @returns Appropriate exit code for an error.
 *
 * @param   pszFormat   The format string (passed to vfprintf).
 * @param   ...         Format arguments.
 */
static int ErrorMsg( const char *pszFormat, ... )
{
    va_list va;

    fprintf( stderr, "dipdump: error: " );
    va_start( va, pszFormat );
    vfprintf( stderr, pszFormat, va);
    va_end( va );
    return( 1 );
}

/**
 * Prints an informational message.
 *
 * @param   pszFormat   The format string (passed to vfprintf).
 * @param   ...         Format arguments.
 */
static void InfoMsg( const char *pszFormat, ... )
{
    va_list va;

    fprintf( stderr, "dipdump: info: " );
    va_start( va, pszFormat );
    vfprintf( stderr, pszFormat, va);
    va_end( va );
}

/**
 * Get type kind name.
 * @returns type kind name (readonly).
 * @param   kind        The type kind.
 */
static const char *GetTypeKind(type_kind kind)
{
    switch( kind ) {
    case TK_NONE:       return( "TK_NONE" );
    case TK_DATA:       return( "TK_DATA" );
    case TK_CODE:       return( "TK_CODE" );
    case TK_ADDRESS:    return( "TK_ADDRESS" );
    case TK_VOID:       return( "TK_VOID" );
    case TK_BOOL:       return( "TK_BOOL" );
    case TK_ENUM:       return( "TK_ENUM" );
    case TK_CHAR:       return( "TK_CHAR" );
    case TK_INTEGER:    return( "TK_INTEGER" );
    case TK_REAL:       return( "TK_REAL" );
    case TK_COMPLEX:    return( "TK_COMPLEX" );
    case TK_STRING:     return( "TK_STRING" );
    case TK_POINTER:    return( "TK_POINTER" );
    case TK_STRUCT:     return( "TK_STRUCT" );
    case TK_ARRAY:      return( "TK_ARRAY" );
    case TK_FUNCTION:   return( "TK_FUNCTION" );
    case TK_NAMESPACE:  return( "TK_NAMESPACE" );
    default:            return( "!unknown!" );
    }
}

/**
 * Get type modifier name.
 * @returns type modifier name (readonly).
 * @param   modifier    The type modifier.
 * @param   kind        The type kind (needed to understand the modifier).
 */
static const char *GetTypeModifier(type_modifier modifier, type_kind kind)
{
    if( modifier == TM_NONE ) {
        return( "TM_NONE" );
    }
    if( modifier == TM_NONE|TM_FLAG_DEREF ) {
        return( "TM_NONE|TM_FLAG_DEREF" );
    }

    switch( kind ) {
    case TK_NONE:
    case TK_DATA:
    case TK_CODE:
    case TK_VOID:
    case TK_BOOL:
    case TK_ENUM:
    case TK_CHAR:
    case TK_STRUCT:
    case TK_ARRAY:
    case TK_FUNCTION:
    case TK_NAMESPACE:
    default:
        return( "!unknown modifier+kind!" );

    case TK_ADDRESS: //??
    case TK_POINTER:
        switch( modifier ) {
        case TM_NEAR:                       return( "TM_NEAR" );
        case TM_NEAR|TM_FLAG_DEREF:         return( "TM_NEAR|TM_FLAG_DEREF" );
        case TM_FAR:                        return( "TM_FAR" );
        case TM_FAR|TM_FLAG_DEREF:          return( "TM_FAR|TM_FLAG_DEREF" );
        case TM_HUGE:                       return( "TM_HUGE" );
        case TM_HUGE|TM_FLAG_DEREF:         return( "TM_HUGE|TM_FLAG_DEREF" );
        default:
            return( "!unknown pointer modifier!" );
        }

    case TK_INTEGER:
        switch( modifier ) {
        case TM_SIGNED:                     return( "TM_SIGNED" );
        case TM_SIGNED|TM_FLAG_DEREF:       return( "TM_SIGNED|TM_FLAG_DEREF" );
        case TM_UNSIGNED:                   return( "TM_UNSIGNED" );
        case TM_UNSIGNED|TM_FLAG_DEREF:     return( "TM_UNSIGNED|TM_FLAG_DEREF" );
        default:
            return( "!unknown integer modifier!" );
        }

    case TK_REAL:
    case TK_COMPLEX: //??
        switch( modifier ) {
        case TM_IEEE:                       return( "TM_IEEE" );
        case TM_IEEE|TM_FLAG_DEREF:         return( "TM_IEEE|TM_FLAG_DEREF" );
        case TM_VAX1:                       return( "TM_VAX1" );
        case TM_VAX1|TM_FLAG_DEREF:         return( "TM_VAX1|TM_FLAG_DEREF" );
        case TM_VAX2:                       return( "TM_VAX2" );
        case TM_VAX2|TM_FLAG_DEREF:         return( "TM_VAX2|TM_FLAG_DEREF" );
        default:
            return( "!unknown floating point modifier!" );
        }

    case TK_STRING:
        switch( modifier ) {
        case TM_ASCII:                      return( "TM_ASCII" );
        case TM_ASCII|TM_FLAG_DEREF:        return( "TM_ASCII|TM_FLAG_DEREF" );
        case TM_EBCIDIC:                    return( "TM_EBCIDIC" );
        case TM_EBCIDIC|TM_FLAG_DEREF:      return( "TM_EBCIDIC|TM_FLAG_DEREF" );
        case TM_UNICODE:                    return( "TM_UNICODE" );
        case TM_UNICODE|TM_FLAG_DEREF:      return( "TM_UNICODE|TM_FLAG_DEREF" );
        default:
            return( "!unknown string modifier!" );
        }
    }
}

/**
 * Get the tag name.
 * @returns tag name. (readonly)
 * @param   tag     The tag.
 */
static const char *GetTypeTag( symbol_type tag )
{
    switch( tag ) {
    case ST_NONE:       return( "ST_NONE" );
    case ST_OPERATOR:   return( "ST_OPERATOR" );
    case ST_DESTRUCTOR: return( "ST_DESTRUCTOR" );
    case ST_TYPE:       return( "ST_TYPE" );
    case ST_STRUCT_TAG: return( "ST_STRUCT_TAG" );
    case ST_CLASS_TAG:  return( "ST_CLASS_TAG" );
    case ST_UNION_TAG:  return( "ST_UNION_TAG" );
    case ST_ENUM_TAG:   return( "ST_ENUM_TAG" );
    case ST_NAMESPACE:  return( "ST_NAMESPACE" );
    default:
        return( "!unknown tag!" );
    }
}

/**
 * WalkSymList callback, the module pass.
 *
 * @returns WR_CONTINUE;
 * @param   info    Symbol walk info.
 * @param   sym     The Symbol.
 * @param   _idx    Pointer to the symbol index number.
 */
static walk_result Sym2Callback( sym_walk_info info, sym_handle *sym, void *_idx )
{
    int             *idx = (int *)_idx;
    char            buff[2048];
    unsigned        len;
    dip_status      rc;
    location_list   ll = {0};
    sym_info        sinfo;
    int             i;

    /* index */
    printf( "%5d  ", ++*idx );

    /* symbol info */
    rc = SymInfo( sym, NULL, &sinfo );
    if( rc == DS_OK ) {
        switch( sinfo.kind ) {
        case SK_NONE:       printf( "NONE  " ); break;
        case SK_CODE:       printf( "CODE  " ); break;
        case SK_DATA:       printf( "DATA  " ); break;
        case SK_CONST:      printf( "CNST  " ); break;
        case SK_TYPE:       printf( "TYPE  " ); break;
        case SK_PROCEDURE:  printf( "PROC  " ); break;
        case SK_NAMESPACE:  printf( "NSPC  " ); break;
        default:            printf( "kind=%#x!  ", sinfo.kind ); break;
        }
    } else {
        printf( "rc=%#x  ", rc );
        memset( &sinfo, 0, sizeof( sinfo ) );
        sinfo.kind= SK_NONE;
    }

    /* location (i.e. address) */
    ll.num = MAX_LOC_ENTRIES;
    rc = SymLocation( sym, NULL, &ll );
    if( rc == DS_OK ) {
        if( ll.num > 0 ) {
            if( ll.e[0].type == LT_ADDR ) {
                printf( "%04x:%08lx  ", ll.e[0].u.addr.mach.segment, (long)ll.e[0].u.addr.mach.offset );
            } else {
                printf( "%p       ", ll.e[0].u.p ); /// what's this?
            }
        } else {
            printf( "               ");
        }
    } else if( sinfo.kind == SK_CONST ) {
        ll.num = 0;
        memset( buff, 0, sizeof( buff ) );
        rc = SymValue( sym, NULL, &buff[0] );
        if( rc == DS_OK ) {
            switch( sinfo.ret_modifier ) {
            }
            printf( "               " );
        } else {
            printf( "SymValue rc=%#x ", rc );
        }
    } else if( sinfo.kind == SK_NONE || sinfo.kind == SK_TYPE
            || sinfo.kind == SK_NAMESPACE ) {
        printf( "               " );
        ll.num = 0;
    } else {
        printf( "rc=%#x  ", rc );
        ll.num = 0;
    }

    /* info */
    switch( info ) {
        case SWI_SYMBOL:
            printf( "SYMBOL    " );
            break;
        case SWI_INHERIT_START:
            printf( "INH-STRT  " );
            break;
        case SWI_INHERIT_END:
            printf( "INH-END   " );
            break;
        default:
            printf( "%#d  ", info );
            break;
    }

    /* finally, the name. */
    /* try get the name */
    buff[0] = '\0';
    len = SymName( sym, NULL, SN_DEMANGLED, buff, sizeof( buff ) );
    if( len == 0 ) {
        len = SymName( sym, NULL, SN_OBJECT, buff, sizeof( buff ) );
    }
    if( len == 0 ) {
        len = SymName( sym, NULL, SN_SOURCE, buff, sizeof( buff ) );
    }
    if( len > 0 ) {
        printf( "%s\n", buff );
    } else {
        printf( "(len=%u)\n", len );
    }


    /* Get more stuff, mainly to test the APIs. */
    if( 1 ) {
        type_handle *type = alloca( DIPHandleSize( HK_TYPE, 0 ) );

        rc = SymType( sym, type );
        if( rc ) {
        }

#if 0
mod_handle      SymMod( sym_handle * );
unsigned        SymName( sym_handle *, location_context *, symbol_name, char *buff, unsigned buff_size );
dip_status      SymType( sym_handle *, type_handle * );
dip_status      SymValue( sym_handle *, location_context *, void * );
dip_status      SymInfo( sym_handle *, location_context *, sym_info * );
dip_status      SymParmLocation( sym_handle *, location_context *, location_list *, unsigned p );
dip_status      SymObjType( sym_handle *, type_handle *, dip_type_info * );
dip_status      SymObjLocation( sym_handle *, location_context *, location_list * );
search_result   AddrSym( mod_handle, address, sym_handle * );
search_result   LookupSym( symbol_source, void *, lookup_item *, void * );
search_result   LookupSymEx( symbol_source, void *, lookup_item *, location_context *, void * );
search_result   AddrScope( mod_handle, address, scope_block * );
search_result   ScopeOuter( mod_handle, scope_block *, scope_block * );
int             SymCmp( sym_handle *, sym_handle * );
dip_status      SymAddRef( sym_handle * );
dip_status      SymRelease( sym_handle * );
dip_status      SymFreeAll();
#endif

    }


    /* more locations. */
    for( i = 1; i < ll.num; i++ ) {
        if( ll.e[0].type == LT_ADDR ) {
            printf( "       %04x:%08lx\n",
                    ll.e[i].u.addr.mach.segment, (long)ll.e[i].u.addr.mach.offset );
        } else {
            printf( "       %p\n", ll.e[i].u.p); /// what's this?
        }
    }

    /*
     * Perform alternative lookups to the those interfaces.
     */

    return( WR_CONTINUE );
}

/**
 * WalkTypeList callback.
 *
 * @returns WR_CONTINUE
 * @param   th          The type handle.
 * @param   _idx        Pointer to the current module index.
 */
static walk_result Type2Callback( type_handle *th, void *_idx )
{
    int             *idx = (int *)_idx;
    char            buff[2048];
    unsigned        len;
    symbol_type     tag;
    dip_type_info   tinfo;
    dip_status      rc;

    printf( "%5d  ", ++*idx );

/** @todo all this needs some serious work */

    /* type name. */
    len = TypeName( th, 0, &tag, buff, sizeof( buff ) );
    if( len > 0 ) {
        printf( "tag=%d %-13s  name=%s\n"
                "       ",
                tag, GetTypeTag( tag ), buff );
    }

    /* type info */
    rc = TypeInfo( th, NULL, &tinfo );
    if( rc == DS_OK ) {
        printf( "size=%#06lx  kind=%2d %-12s  modifier=%#04x %s\n",
                tinfo.size,
                tinfo.kind, GetTypeKind( tinfo.kind ),
                tinfo.modifier, GetTypeModifier( tinfo.modifier, tinfo.kind ) );

        switch( tinfo.kind ) {
        case TK_ARRAY: {
                array_info ainfo;
                rc = TypeArrayInfo( th, NULL, &ainfo, NULL );
                if( rc == DS_OK ) {
                    printf( "       "
                            "low_bound=%ld num_elts=%lu stride=%lu num_dims=%u column_major=%d\n",
                            ainfo.low_bound, ainfo.num_elts, ainfo.stride,
                            ainfo.num_dims, ainfo.column_major );
                } else {
                    printf( "TypeArrayInfo -> %d\n", rc );
                }
            }
            break;
        case TK_FUNCTION: {
            }
            break;
        }
    } else {
        printf( "TypeInfo -> %d\n", rc );
    }

    return( WR_CONTINUE );
}

/**
 * Compares two cues, the first one being the one which information we use
 * searching for the 2nd.
 *
 * @param   cue             The first cue.
 * @param   cue2            The result cue (depends on search_rc).
 * @param   expected_rc     The expected search result.
 * @param   actual_rc       The actual search result.
 * @param   exp_exact_line  Set if we're to expect a matching line number.
 * @param   exp_le_line     Set if we're to expect a less or equal line number.
 * @param   exp_exact_addr  Set if we're to expect a matching address.
 * @param   exp_le_line     Set if we're to expect a less or equal address.
 * @param   operation       The name of the search operation.
 */
static void CompareCues( cue_handle *cue, cue_handle *cue2,
                         search_result expected_rc, search_result actual_rc,
                         bool exp_exact_line, bool exp_le_line,
                         bool exp_exact_addr, bool exp_le_addr,
                         const char *operation )
{
    if( actual_rc != expected_rc ) {
        printf( "FAILED: %s returned %d instead of %d\n", operation,
                actual_rc, expected_rc );
    }
    if( actual_rc == SR_CLOSEST || actual_rc == SR_EXACT ) {
        address         addr  = CueAddr( cue );
        unsigned long   line  = CueLine( cue );
        address         addr2 = CueAddr( cue2 );
        unsigned long   line2 = CueLine( cue2 );
        int             failed;

        failed  = CueFileId( cue2 ) != CueFileId( cue );
        failed |= exp_exact_line && line2 != line;
        failed |= exp_le_line && line2 <= line;
        failed |= exp_exact_addr
            && (    addr2.mach.segment != addr.mach.segment
                ||  addr2.mach.offset != addr.mach.offset );
        failed |= exp_le_addr
            && (    addr2.mach.segment != addr.mach.segment
                ||  addr2.mach.offset <= addr.mach.offset );
        if( failed ) {
            printf( "FAILED: %s: cue2:{file=%#x line=%lu addr=%04x:%08lx}\n"
                    "       %*s != cue:{file=%#x line=%lu addr=%04x:%08lx}\n",
                    operation,
                    CueFileId( cue2 ), line2, addr2.mach.segment, (long)addr2.mach.offset,
                    strlen( operation ), "",
                    CueFileId( cue ), line, addr.mach.segment, (long)addr.mach.offset );
        }
    }
}

/**
 * WalkFileList callback, the module pass.
 *
 * @returns WR_CONTINUE
 * @param   cue     The file.
 * @param   ignored Unused user argument.
 */
static walk_result File2Callback( cue_handle *cue, void *ignored )
{
    address         prev_addr = {0};
    long            prev_line = -1;
    cue_handle      *next_cue = alloca( DIPHandleSize( HK_CUE, 0 ) );
    cue_handle      *prev_cue = NULL;
    cue_handle      *cue2     = alloca( DIPHandleSize( HK_CUE, 0 ) );
    mod_handle      mod       = CueMod( cue );
    cue_fileid      file_id   = CueFileId( cue );
    search_result   search_rc;
    char            buff[1024];
    size_t          len;
    dip_status      rc;

    /* filename */
    buff[0] = '\0';
    len = CueFile( cue, buff, sizeof( buff ) );
    if( len > 0 ) {
        printf( " %lx %s\n", file_id, buff );
    } else {
        printf( " %lx (len=%u)\n", file_id, len );
    }

    /* check the LineCue function */
    if( Opts.do_cue_tests ) {
        search_rc = LineCue( mod, file_id, 0, 0, cue2 );
        CompareCues( cue, cue2, SR_EXACT, search_rc, true, false, true, false,
                     "LineCue(,,0,)" );
    }

    /* lines */
    do {
        long        line   = CueLine( cue );
        unsigned    column = CueColumn( cue );
        address     addr   = CueAddr( cue );


        printf( "  Line %5ld ", line );
        if( column ) {
            printf( "Col %2d ", column );
        }
        printf( "at %04x:%08lx%s\n", addr.mach.segment, (long)addr.mach.offset,
                prev_line >= 0 && addr.mach.offset < prev_addr.mach.offset
                ? "^"
                : prev_line >= 0 && line < prev_line ? "!" : "" );

        /* do tests */
        if( Opts.do_cue_tests ) {
            if( CueFileId( cue ) !=  file_id ) {
                printf( "ERROR: file id changed! new:%#lx old:%#lx\n",
                        (long)CueFileId( cue ), (long)file_id );
            }
            if( CueMod( cue ) !=  mod ) {
                printf( "ERROR: module changed! new:%#lx old:%#lx\n",
                        (long)CueMod( cue ), (long)file_id );
            }

            /* line searches */
            search_rc = LineCue( mod, file_id, line, 0, cue2 );
            CompareCues( cue, cue2, SR_EXACT, search_rc, true, false, false, false,
                         "LineCue(,,n,)" );
            if( line > prev_line + 1 && prev_line >= 0 ) {
                search_rc = LineCue( mod, file_id, line - 1, 0, cue2 );
                CompareCues( prev_cue, cue2,
                             prev_line == line - 1 ? SR_EXACT : SR_CLOSEST,
                             search_rc, true, false, false, false,
                             "LineCue(,,n-1,)" );
            }

            /* address searches */
            search_rc = AddrCue( mod, addr, cue2 );
            CompareCues( cue, cue2, SR_EXACT, search_rc, false, false, true, false,
                         "AddrCue(,,n,)" );
        }


        /* next */
        rc = CueAdjust( cue, 1, next_cue );
        prev_cue  = cue;
        cue       = next_cue;
        next_cue  = prev_cue;
        prev_addr = addr;
        prev_line = line;
    } while( rc == DS_OK );

    return( WR_CONTINUE );
}

/**
 * WalkModList callback, pass 2.
 *
 * @returns WR_CONTINUE
 * @param   mh          The module.
 * @param   ignored     Pointer to the current module index.
 */
static walk_result Mod2Callback( mod_handle mh, void *_idx )
{
    walk_result     walkres;
    int             i;
    int             need_trailing_newline = 0;
    int             *idx = (int *)_idx;
    ++*idx;

    /*
     * Linenumbers.
     */
    if( 1 && ModHasInfo( mh, HK_CUE ) == DS_OK ) {
        printf( "%03d Line Numbers\n"
                "-----------------\n"
                "\n",
                *idx );
        i = 0;
        walkres = WalkFileList( mh, File2Callback, &i );
        printf( "\n"
                "\n" );
        need_trailing_newline = 1;
    }

    /*
     * Types
     */
    if( 1 && ModHasInfo( mh, HK_TYPE ) == DS_OK ) {
        printf( " %03d Types\n"
                "-----------\n"
                "\n"
                "index   seg:offset    info  lang  name\n"
                "---------------------------------------\n",
                *idx);
        i = 0;
        walkres = WalkTypeList( mh, Type2Callback, &i );
        printf( "\n"
                "\n" );
        need_trailing_newline = 1;
    }


    /*
     * Symbols.
     */
    if( 1 && ModHasInfo( mh, HK_SYM ) == DS_OK ) {
        printf( "%03d Symbols\n"
                "------------\n"
                "\n",
                *idx );
        i = 0;
        walkres = WalkSymList( SS_MODULE, &mh, Sym2Callback, &i );
        printf( "\n"
                "\n" );
        need_trailing_newline = 1;
    }

    if( need_trailing_newline ) {
        printf( "\n" );
    }
    return( WR_CONTINUE );
}

/**
 * WalkSymList callback.
 *
 * @returns WR_CONTINUE;
 * @param   info    Symbol walk info.
 * @param   sym     The Symbol.
 * @param   _idx    Pointer to the symbol index number.
 */
static walk_result SymCallback( sym_walk_info info, sym_handle *sym, void *_idx )
{
#if 1
    return( Sym2Callback( info, sym, _idx ) );
#else
    int             *idx = (int *)_idx;
    char            buff[2048];
    unsigned        len;
    dip_status      rc;
    location_list   ll = {0};
    int             i;

    /* index */
    printf( "%5d  ", ++*idx );

    /* location (i.e. address) */
    ll.num = MAX_LOC_ENTRIES;
    rc = SymLocation( sym, NULL, &ll );
    if( rc == DS_OK ) {
        if( ll.num > 0 ) {
            if( ll.e[0].type == LT_ADDR ) {
                printf( "%04x:%08lx  ", ll.e[0].u.addr.mach.segment, (long)ll.e[0].u.addr.mach.offset );
            } else {
                printf( "%p       ", ll.e[0].u.p); /// what's this?
            }
        } else {
            printf( "               ");
        }
    } else {
        printf( "rc=%#x  ", rc );
        ll.num = 0;
    }

    /* info */
    switch( info ) {
        case SWI_SYMBOL:
            printf("SYMBOL    ");
            break;
        case SWI_INHERIT_START:
            printf("INH-STRT  ");
            break;
        case SWI_INHERIT_END:
            printf("INH-END   ");
            break;
        default:
            printf("%#d  ", info);
            break;
    }

    /* finally, the name. */
    /* try get the name */
    buff[0] = '\0';
    len = SymName( sym, NULL, SN_DEMANGLED, buff, sizeof( buff ) );
    if( len == 0 ) {
        len = SymName( sym, NULL, SN_OBJECT, buff, sizeof( buff ) );
    }
    if( len == 0 ) {
        len = SymName( sym, NULL, SN_SOURCE, buff, sizeof( buff ) );
    }
    if( len > 0 ) {
        printf( "%s\n", buff );
    } else {
        printf( "(len=%u)\n", len );
    }

    /* more locations. */
    for( i = 1; i < ll.num; i++ ) {
        if( ll.e[0].type == LT_ADDR ) {
            printf( "       %04x:%08lx\n",
                    ll.e[i].u.addr.mach.segment, (long)ll.e[i].u.addr.mach.offset );
        } else {
            printf( "       %p\n", ll.e[i].u.p); /// what's this?
        }
    }

    return( WR_CONTINUE );
#endif
}

/**
 * WalkModList callback.
 *
 * @returns WR_CONTINUE
 * @param   mh          The module.
 * @param   _idx        Pointer to the current module index.
 */
static walk_result ModCallback( mod_handle mh, void *_idx )
{
    int         *idx = (int *)_idx;
    char        buff[2048];
    unsigned    len;
    const char  *lang;
    address     addr = {0};

    printf( "%5d  ", ++*idx );

    /* address */
    addr = ModAddr( mh );
    printf( "%04x:%08lx  ", addr.mach.segment, (long)addr.mach.offset );

    /* what info do we have? */
    printf( "%c%c%c%c  ",
            ModHasInfo( mh, HK_IMAGE ) == DS_OK ? 'I' : '-',
            ModHasInfo( mh, HK_TYPE  ) == DS_OK ? 'T' : '-',
            ModHasInfo( mh, HK_CUE   ) == DS_OK ? 'C' : '-',
            ModHasInfo( mh, HK_SYM   ) == DS_OK ? 'S' : '-'
            );

    /* language and name */
    lang = ModSrcLang( mh );
    len = ModName( mh, buff, sizeof(buff) );
    if( len == 0 ) {
        buff[0] = '\0';
    }
    printf( "%-4s  %s\n", lang, buff );

    return( WR_CONTINUE );
}

/**
 * Dumps the loaded debug info.
 *
 * @returns 0 on success, exit code on failure.
 * @param   file    The filename.
 * @param   mh      The DIP/DIG module handle.
 * @param   proc    The process which the module is loaded into.
 */
static int DumpIt( const char *file, mod_handle mh, process_info *proc )
{
    walk_result     walkres;
    struct stat     s;
    char            buff[1024];
    unsigned        len;
    int             i;

    /*
     * Module.
     */
    printf( " Module\n"
            "========\n"
            "\n"
            "name        = %s\n",
            file);
    if( !stat( file, &s ) ) {
        struct tm   *ts;
        char        buff[80];

        ts = gmtime( &s.st_mtime );
        strftime( buff, sizeof( buff ), "%Y-%m-%d %H:%M:%S UCT", ts );
        printf( "timestamp   = %s\n", buff );
    }
    printf( "DIP         = %s\n", ImageDIP( mh ) );

#if 0 /* crashes codeview, nothing on dwarf. */
    buff[0] = '\0';
    len = ModName( mh, buff, sizeof( buff ) );
    if( len ) {
        printf( "module name = %s\n", buff );
    }
#else
    len = len;
    (void)buff;
#endif
    printf( "\n"
            "\n" );

    /*
     * Compiled units?
     */
    if( 1 ) {
        printf( " Compiled Units\n"
                "================\n"
                "\n"
                "index   seg:offset    info  lang  name\n"
                "---------------------------------------\n");
        i = 0;
        walkres = WalkModList( mh, ModCallback, &i );
        printf( "\n"
                "\n" );
    }

#if 0
    /*
     * Types.
     * This crashes DWARF, and with codeview it'll only work on one special module.
     */
    if( 1 ) {
        printf( " Types\n"
                "=======\n"
                "\n"
                "index   seg:offset    info  lang  name\n"
                "---------------------------------------\n");
        i = 0;
        walkres = WalkTypeList( /*mh*/ IMH_GBL, TypeCallback, &i );
        printf( "\n"
                "\n" );
    }
#endif

    /*
     * Global (?) Symbols.
     */
    if( 1 ) {
        printf( " Global Symbols\n"
                "================\n"
                "\n"
                "index  kind   seg:offset    info  lng name\n"
                "------------------------------------------\n");
        i = 0;
        walkres = WalkSymList( SS_MODULE, &mh, SymCallback, &i );
        printf( "\n"
                "\n" );
    }


    /*
     * Iterate compiled modules and dump their line numbers and symbols.
     */
    if( 1 ) {
        i = 0;
        walkres = WalkModList( mh, Mod2Callback, &i );
    }

    printf("\n");
    return( 0 );
}

/**
 * Initializes DIP.
 *
 * @returns success indicator.
 * @param   dips    Pointer to an array of dip names terminated by a NULL entry.
 */
static bool InitDIP( char **dips )
{
    bool    rc = false;

    if( !(DIPInit() & DS_ERR) ) {
        char        *ptr;
        unsigned    dips_loaded = 0;

        for( ptr = *dips++; ptr; ptr = *dips++ ) {
            int     rc = DIPLoad( ptr );

            if( rc & DS_ERR ) {
                rc &= ~DS_ERR;
                switch( rc ) {
                case DS_FOPEN_FAILED:
                    ErrorMsg( "%s - not found\n", ptr );
                    break;
                case DS_INVALID_DIP_VERSION:
                    ErrorMsg( "%s - wrong DIP version\n", ptr );
                    break;
                case DS_INVALID_DIP:
                    ErrorMsg( "%s - invalid DIP\n", ptr );
                    break;
                case DS_TOO_MANY_DIPS:
                    ErrorMsg( "%s - too many DIPs\n", ptr );
                    break;
                default:
                    ErrorMsg( "%s - rc=%#x (%d)\n", ptr, rc, rc );
                    break;
                }
            } else {
                InfoMsg( "Loaded DIP %s\n", ptr);
                dips_loaded++;
            }
        }

        rc = dips_loaded > 0;
        if( !dips_loaded ) {
            ErrorMsg( "Failed to load any DIPs!\n");
            DIPFini();
        }
    }
    return( rc );
}

/**
 * Terminates the DIP manager.
 */
static void TermDIP( void )
{
    /// @todo implement me
}


/**
 * Dumps the debug information in a file.
 *
 * @returns 0 on success, exit code on failure.
 * @param   file    The filename.
 * @param   dips    The dips to load and use.
 */
static int DumpFile( const char *file, char **dips )
{
    int             rc = 1;
    dig_fhandle     fh;

    /*
     * Open the file
     */
    fh = DIGCliOpen( file, DIG_READ );
    if( fh == DIG_NIL_HANDLE ) {
        return( ErrorMsg( "Failed to open '%s'\n", file ) );
    }

    /*
     * Init DIP, create a process and load the file into the process.
     */
    if( InitDIP( dips ) ) {
        process_info    *proc = DIPCreateProcess();

        if( proc != NULL ) {
            int         prty;
            mod_handle  mh = 0;

            for( prty = DIPPriority( 0 ); prty != 0; prty = DIPPriority( prty ) ) {
                DIGCliSeek( fh, 0, DIG_ORG );
                mh = DIPLoadInfo( fh, 0, prty );
                if( mh != NO_MOD ) {
                    break;
                }
            }
            if( mh != NO_MOD ) {
                DIPMapInfo( mh, NULL );
                InfoMsg( "DIP opened '%s' at prty=%d, mh=%lx\n", file, prty, (long)mh );

                /*
                 * Enumerate the debug info.
                 */
                rc = DumpIt( file, mh, proc );

                /*
                 * Cleanup.
                 */
                DIPUnloadInfo( mh );
            } else {
                ErrorMsg( "DIP failed to open '%s'.\n", file);
            }
            DIPDestroyProcess( proc );
        }
        TermDIP();
    }
    DIGCliClose( fh );
    return( rc );
}


static int PrintUsage( void )
/* Prints the dipdump usage instructions and and copyright message. */
{
    printf( "Usage:  dipdump [-h?] [-d<dip>] files...\n"
            "dipdump reads and dumps debugging information\n"
            "Options: -d<dip> attempts to load specified DIP\n"
            "         -h      displays this help"
            "\n" );
    return( 1 );
}


int main( int argc, char **argv )
{
    char    *dips[16] = { 0 };
    int     next_dip = 0;
    int     c;
//    char    *s;

    /* Process command line options */
    if( argc < 2 ) {
        return( PrintUsage() );
    }

    while( (c = getopt( argc, argv, ":hd:" )) != EOF ) {
        switch( c ) {
            case 'd': {
                char    *dip = optarg;

                if( next_dip >= sizeof( dips ) / sizeof( dips[0] ) ) {
                    return( ErrorMsg( "too many DIPs!\n" ) );
                }
                dips[next_dip++] = dip;
                break;
            }
            case 'h':
            default:
                return( PrintUsage() );
        }
    }

#if 0
    /* Add default DIPs at end of list. */
    s = DIPDefaults;
    while( *s ) {
        if( next_dip >= sizeof( dips ) / sizeof( dips[0] ) ) {
            return( ErrorMsg( "too many DIPs!\n" ) );
        }
        dips[next_dip++] = s;
        s += strlen( s ) + 1;
    }
#endif

    /* Try to dump debug info for all remaining arguments */
    while( argv[optind] ) {
        int     rc = DumpFile( argv[optind], dips );

        if( rc ) {
            return( rc );
        }
        ++optind;
    }

    return( 0 );
}
