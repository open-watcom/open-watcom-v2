/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2018 The Open Watcom Contributors. All Rights Reserved.
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
#include "dipdump.h"


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
static const char *GetTypeModifier( dig_type_info *ti )
{
    if( ti->modifier == TM_NONE ) {
        return( ( ti->deref ) ? "TM_NONE|TM_FLAG_DEREF" : "TM_NONE" );
    }
    switch( ti->kind ) {
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
        switch( ti->modifier ) {
        case TM_NEAR:       return( ( ti->deref ) ? "TM_NEAR|TM_FLAG_DEREF" : "TM_NEAR" );
        case TM_FAR:        return( ( ti->deref ) ? "TM_FAR|TM_FLAG_DEREF" : "TM_FAR" );
        case TM_HUGE:       return( ( ti->deref ) ? "TM_HUGE|TM_FLAG_DEREF" : "TM_HUGE" );
        }
        return( "!unknown pointer modifier!" );
    case TK_INTEGER:
        switch( ti->modifier ) {
        case TM_SIGNED:     return( ( ti->deref ) ? "TM_SIGNED|TM_FLAG_DEREF" : "TM_SIGNED" );
        case TM_UNSIGNED:   return( ( ti->deref ) ? "TM_UNSIGNED|TM_FLAG_DEREF" : "TM_UNSIGNED" );
        }
        return( "!unknown integer modifier!" );
    case TK_REAL:
    case TK_COMPLEX: //??
        switch( ti->modifier ) {
        case TM_IEEE:       return( ( ti->deref ) ? "TM_IEEE|TM_FLAG_DEREF" : "TM_IEEE" );
        case TM_VAX1:       return( ( ti->deref ) ? "TM_VAX1|TM_FLAG_DEREF" : "TM_VAX1" );
        case TM_VAX2:       return( ( ti->deref ) ? "TM_VAX2|TM_FLAG_DEREF" : "TM_VAX2" );
        }
        return( "!unknown floating point modifier!" );
    case TK_STRING:
        switch( ti->modifier ) {
        case TM_ASCII:      return( ( ti->deref ) ? "TM_ASCII|TM_FLAG_DEREF" : "TM_ASCII" );
        case TM_EBCIDIC:    return( ( ti->deref ) ? "TM_EBCIDIC|TM_FLAG_DEREF" : "TM_EBCIDIC" );
        case TM_UNICODE:    return( ( ti->deref ) ? "TM_UNICODE|TM_FLAG_DEREF" : "TM_UNICODE" );
        }
        return( "!unknown string modifier!" );
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
 * @param   swi     Symbol walk info.
 * @param   sym     The Symbol.
 * @param   _idx    Pointer to the symbol index number.
 */
static walk_result Sym2Callback( sym_walk_info swi, sym_handle *sym, void *_idx )
{
    int             *idx = (int *)_idx;
    char            buff[2048];
    unsigned        len;
    dip_status      ds;
    location_list   ll = {0};
    sym_info        sinfo;
    int             i;

    /* index */
    printf( "%5d  ", ++*idx );

    /* symbol info */
    ds = DIPSymInfo( sym, NULL, &sinfo );
    if( ds == DS_OK ) {
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
        printf( "status=%#x  ", ds );
        memset( &sinfo, 0, sizeof( sinfo ) );
        sinfo.kind= SK_NONE;
    }

    /* location (i.e. address) */
    ll.num = MAX_LOC_ENTRIES;
    ds = DIPSymLocation( sym, NULL, &ll );
    if( ds == DS_OK ) {
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
        ds = DIPSymValue( sym, NULL, &buff[0] );
        if( ds == DS_OK ) {
            switch( sinfo.ret_modifier ) {
            }
            printf( "               " );
        } else {
            printf( "SymValue status=%#x ", ds );
        }
    } else if( sinfo.kind == SK_NONE || sinfo.kind == SK_TYPE
            || sinfo.kind == SK_NAMESPACE ) {
        printf( "               " );
        ll.num = 0;
    } else {
        printf( "status=%#x  ", ds );
        ll.num = 0;
    }

    /* swi */
    switch( swi ) {
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
        printf( "%#d  ", swi );
        break;
    }

    /* finally, the name. */
    /* try get the name */
    buff[0] = '\0';
    len = DIPSymName( sym, NULL, SNT_DEMANGLED, buff, sizeof( buff ) );
    if( len == 0 ) {
        len = DIPSymName( sym, NULL, SNT_OBJECT, buff, sizeof( buff ) );
    }
    if( len == 0 ) {
        len = DIPSymName( sym, NULL, SNT_SOURCE, buff, sizeof( buff ) );
    }
    if( len > 0 ) {
        printf( "%s\n", buff );
    } else {
        printf( "(len=%u)\n", len );
    }


    /* Get more stuff, mainly to test the APIs. */
    if( 1 ) {
        type_handle *th = alloca( DIPHandleSize( HK_TYPE ) );

        ds = DIPSymType( sym, th );
        if( ds ) {
        }
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
    dig_type_info   ti;
    dip_status      ds;

    printf( "%5d  ", ++*idx );

/** @todo all this needs some serious work */

    /* type name. */
    len = DIPTypeName( th, 0, &tag, buff, sizeof( buff ) );
    if( len > 0 ) {
        printf( "tag=%d %-13s  name=%s\n"
                "       ",
                tag, GetTypeTag( tag ), buff );
    }

    /* type info */
    ds = DIPTypeInfo( th, NULL, &ti );
    if( ds == DS_OK ) {
        printf( "size=%#06lx  kind=%2d %-12s  modifier=%#04x deref=%d %s\n",
                ti.size,
                ti.kind, GetTypeKind( ti.kind ),
                ti.modifier, ti.deref, GetTypeModifier( &ti ) );

        switch( ti.kind ) {
        case TK_ARRAY: {
                array_info ainfo;
                ds = DIPTypeArrayInfo( th, NULL, &ainfo, NULL );
                if( ds == DS_OK ) {
                    printf( "       "
                            "low_bound=%ld num_elts=%lu stride=%lu num_dims=%u column_major=%d\n",
                            ainfo.low_bound, ainfo.num_elts, ainfo.stride,
                            ainfo.num_dims, ainfo.column_major );
                } else {
                    printf( "DIPTypeArrayInfo -> %d\n", ds );
                }
            }
            break;
        case TK_FUNCTION: {
            }
            break;
        }
    } else {
        printf( "DIPTypeInfo -> %d\n", ds );
    }

    return( WR_CONTINUE );
}

/**
 * Compares two cues, the first one being the one which information we use
 * searching for the 2nd.
 *
 * @param   cueh1           The first cue.
 * @param   cueh2           The result cue (depends on search_rc).
 * @param   expected_rc     The expected search result.
 * @param   actual_rc       The actual search result.
 * @param   exp_exact_line  Set if we're to expect a matching line number.
 * @param   exp_le_line     Set if we're to expect a less or equal line number.
 * @param   exp_exact_addr  Set if we're to expect a matching address.
 * @param   exp_le_line     Set if we're to expect a less or equal address.
 * @param   operation       The name of the search operation.
 */
static void CompareCues( cue_handle *cueh1, cue_handle *cueh2,
                         search_result expected_rc, search_result actual_rc,
                         bool exp_exact_line, bool exp_le_line,
                         bool exp_exact_addr, bool exp_le_addr,
                         const char *operation )
{
    if( actual_rc != expected_rc ) {
        printf( "FAILED: %s returned %d instead of %d\n", operation, actual_rc, expected_rc );
    }
    if( actual_rc == SR_CLOSEST || actual_rc == SR_EXACT ) {
        address         addr1 = DIPCueAddr( cueh1 );
        unsigned long   line1 = DIPCueLine( cueh1 );
        address         addr2 = DIPCueAddr( cueh2 );
        unsigned long   line2 = DIPCueLine( cueh2 );
        bool            failed;

        failed  = DIPCueFileId( cueh2 ) != DIPCueFileId( cueh1 );
        failed |= exp_exact_line && line2 != line1;
        failed |= exp_le_line && line2 <= line1;
        failed |= exp_exact_addr && ( addr2.mach.segment != addr1.mach.segment || addr2.mach.offset != addr1.mach.offset );
        failed |= exp_le_addr && ( addr2.mach.segment != addr1.mach.segment || addr2.mach.offset <= addr1.mach.offset );
        if( failed ) {
            printf( "FAILED: %s: cue2:{file=%#x line=%lu addr=%04x:%08lx}\n"
                    "       %*s != cue1:{file=%#x line=%lu addr=%04x:%08lx}\n",
                    operation,
                    DIPCueFileId( cueh2 ), line2, addr2.mach.segment, (long)addr2.mach.offset,
                    strlen( operation ), "",
                    DIPCueFileId( cueh1 ), line1, addr1.mach.segment, (long)addr1.mach.offset );
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
static walk_result File2Callback( cue_handle *cueh1, void *ignored )
{
    address         prev_addr = {0};
    long            prev_line = -1;
    cue_handle      *next_cueh = alloca( DIPHandleSize( HK_CUE ) );
    cue_handle      *prev_cueh = NULL;
    cue_handle      *cueh2     = alloca( DIPHandleSize( HK_CUE ) );
    mod_handle      mod       = DIPCueMod( cueh1 );
    cue_fileid      file_id   = DIPCueFileId( cueh1 );
    search_result   search_rc;
    char            buff[1024];
    size_t          len;
    dip_status      ds;

    /* unused parameters */ (void)ignored;

    /* filename */
    buff[0] = '\0';
    len = DIPCueFile( cueh1, buff, sizeof( buff ) );
    if( len > 0 ) {
        printf( " %lx %s\n", file_id, buff );
    } else {
        printf( " %lx (len=%u)\n", file_id, len );
    }

    /* check the LineCue function */
    if( Opts.do_cue_tests ) {
        search_rc = DIPLineCue( mod, file_id, 0, 0, cueh2 );
        CompareCues( cueh1, cueh2, SR_EXACT, search_rc, true, false, true, false, "DIPLineCue(,,0,)" );
    }

    /* lines */
    do {
        long        line   = DIPCueLine( cueh1 );
        unsigned    column = DIPCueColumn( cueh1 );
        address     addr   = DIPCueAddr( cueh1 );


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
            if( DIPCueFileId( cueh1 ) !=  file_id ) {
                printf( "ERROR: file id changed! new:%#lx old:%#lx\n", (long)DIPCueFileId( cueh1 ), (long)file_id );
            }
            if( DIPCueMod( cueh1 ) !=  mod ) {
                printf( "ERROR: module changed! new:%#lx old:%#lx\n", (long)DIPCueMod( cueh1 ), (long)file_id );
            }

            /* line searches */
            search_rc = DIPLineCue( mod, file_id, line, 0, cueh2 );
            CompareCues( cueh1, cueh2, SR_EXACT, search_rc, true, false, false, false, "DIPLineCue(,,n,)" );
            if( line > prev_line + 1 && prev_line >= 0 ) {
                search_rc = DIPLineCue( mod, file_id, line - 1, 0, cueh2 );
                CompareCues( prev_cueh, cueh2, prev_line == line - 1 ? SR_EXACT : SR_CLOSEST,
                             search_rc, true, false, false, false, "DIPLineCue(,,n-1,)" );
            }

            /* address searches */
            search_rc = DIPAddrCue( mod, addr, cueh2 );
            CompareCues( cueh1, cueh2, SR_EXACT, search_rc, false, false, true, false, "DIPAddrCue(,,n,)" );
        }


        /* next */
        ds = DIPCueAdjust( cueh1, 1, next_cueh );
        prev_cueh  = cueh1;
        cueh1      = next_cueh;
        next_cueh  = prev_cueh;
        prev_addr = addr;
        prev_line = line;
    } while( ds == DS_OK );

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
    if( 1 && DIPModHasInfo( mh, HK_CUE ) == DS_OK ) {
        printf( "%03d Line Numbers\n"
                "-----------------\n"
                "\n",
                *idx );
        i = 0;
        walkres = DIPWalkFileList( mh, File2Callback, &i );
        printf( "\n"
                "\n" );
        need_trailing_newline = 1;
    }

    /*
     * Types
     */
    if( 1 && DIPModHasInfo( mh, HK_TYPE ) == DS_OK ) {
        printf( " %03d Types\n"
                "-----------\n"
                "\n"
                "index   seg:offset    info  lang  name\n"
                "---------------------------------------\n",
                *idx);
        i = 0;
        walkres = DIPWalkTypeList( mh, Type2Callback, &i );
        printf( "\n"
                "\n" );
        need_trailing_newline = 1;
    }


    /*
     * Symbols.
     */
    if( 1 && DIPModHasInfo( mh, HK_SYM ) == DS_OK ) {
        printf( "%03d Symbols\n"
                "------------\n"
                "\n",
                *idx );
        i = 0;
        walkres = DIPWalkSymList( SS_MODULE, &mh, Sym2Callback, &i );
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
 * @param   swi     Symbol walk info.
 * @param   sym     The Symbol.
 * @param   _idx    Pointer to the symbol index number.
 */
static walk_result SymCallback( sym_walk_info swi, sym_handle *sym, void *_idx )
{
#if 1
    return( Sym2Callback( swi, sym, _idx ) );
#else
    int             *idx = (int *)_idx;
    char            buff[2048];
    unsigned        len;
    dip_status      ds;
    location_list   ll = {0};
    int             i;

    /* index */
    printf( "%5d  ", ++*idx );

    /* location (i.e. address) */
    ll.num = MAX_LOC_ENTRIES;
    ds = SymLocation( sym, NULL, &ll );
    if( ds == DS_OK ) {
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
        printf( "status=%#x  ", ds );
        ll.num = 0;
    }

    /* swi */
    switch( swi ) {
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
        printf("%#d  ", swi);
        break;
    }

    /* finally, the name. */
    /* try get the name */
    buff[0] = '\0';
    len = DIPSymName( sym, NULL, SNT_DEMANGLED, buff, sizeof( buff ) );
    if( len == 0 ) {
        len = DIPSymName( sym, NULL, SNT_OBJECT, buff, sizeof( buff ) );
    }
    if( len == 0 ) {
        len = DIPSymName( sym, NULL, SNT_SOURCE, buff, sizeof( buff ) );
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
    addr = DIPModAddr( mh );
    printf( "%04x:%08lx  ", addr.mach.segment, (long)addr.mach.offset );

    /* what info do we have? */
    printf( "%c%c%c%c  ",
            DIPModHasInfo( mh, HK_IMAGE ) == DS_OK ? 'I' : '-',
            DIPModHasInfo( mh, HK_TYPE  ) == DS_OK ? 'T' : '-',
            DIPModHasInfo( mh, HK_CUE   ) == DS_OK ? 'C' : '-',
            DIPModHasInfo( mh, HK_SYM   ) == DS_OK ? 'S' : '-'
            );

    /* language and name */
    lang = DIPModSrcLang( mh );
    len = DIPModName( mh, buff, sizeof(buff) );
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

    /* unused parameters */ (void)proc;

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
    printf( "DIP         = %s\n", DIPImageName( mh ) );

#if 0 /* crashes codeview, nothing on dwarf. */
    buff[0] = '\0';
    len = DIPModName( mh, buff, sizeof( buff ) );
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
        walkres = DIPWalkModList( mh, ModCallback, &i );
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
        walkres = DIPWalkTypeList( /*mh*/ IMH_GBL, TypeCallback, &i );
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
        walkres = DIPWalkSymList( SS_MODULE, &mh, SymCallback, &i );
        printf( "\n"
                "\n" );
    }


    /*
     * Iterate compiled modules and dump their line numbers and symbols.
     */
    if( 1 ) {
        i = 0;
        walkres = DIPWalkModList( mh, Mod2Callback, &i );
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

        for( ptr = *dips++; ptr != NULL; ptr = *dips++ ) {
            dip_status  ds;

            ds = DIPLoad( ptr );
            if( ds & DS_ERR ) {
                ds &= ~DS_ERR;
                switch( ds ) {
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
                    ErrorMsg( "%s - status=%#x (%d)\n", ptr, ds, ds );
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
    FILE            *fp;

    /*
     * Open the file
     */
    fp = DIGCli( Open )( file, DIG_READ );
    if( fp == NULL ) {
        return( ErrorMsg( "Failed to open '%s'\n", file ) );
    }

    /*
     * Init DIP, create a process and load the file into the process.
     */
    if( InitDIP( dips ) ) {
        process_info    *proc = DIPCreateProcess();

        if( proc != NULL ) {
            dip_priority    priority;
            mod_handle      mh = 0;

            for( priority = 0; (priority = DIPPriority( priority )) != 0;  ) {
                DIGCli( Seek )( fp, 0, DIG_ORG );
                mh = DIPLoadInfo( fp, 0, priority );
                if( mh != NO_MOD ) {
                    break;
                }
            }
            if( mh != NO_MOD ) {
                DIPMapInfo( mh, NULL );
                InfoMsg( "DIP opened '%s' at prty=%d, mh=%lx\n", file, priority, (long)mh );

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
    DIGCli( Close )( fp );
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
    char    *dips[16] = { NULL, NULL };
    int     next_dip = 0;
    int     c;
    int     rc;
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
            dips[next_dip] = NULL;
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

#if defined( __UNIX__ ) || defined( __DOS__ )
    PathInit();
#endif
    /* Try to dump debug info for all remaining arguments */
    rc = 0;
    while( argv[optind] ) {
        rc = DumpFile( argv[optind], dips );

        if( rc ) {
            break;
        }
        ++optind;
    }
#if defined( __UNIX__ ) || defined( __DOS__ )
    PathFini();
#endif
    return( rc );
}
