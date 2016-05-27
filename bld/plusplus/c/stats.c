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


#include "plusplus.h"
#include "stats.h"
#include "preproc.h"
#include "initdefs.h"
#include "vbuf.h"
#include "vstk.h"
#include "pcheader.h"


static void statsInit(          // INITIALIZE STATISTICS GATHERING
    INITFINI* defn )            // - definition
{
    defn = defn;
    CompFlags.stats_printed = 0;
    CompFlags.extra_stats_wanted = 0;
    SrcLineCount = 0;
    IncLineCount = 0;
    WngCount = 0;
    ErrCount = 0;
}


// this prints grammatically correct messages.
// i.e. no errors, or 1 error, or 5 errors
static void intPrint            // PRINT INT LINE SEGMENT
    ( VBUF *buf
    , char *thing
    , char *end
    , unsigned int count )
{
    if( count == 0 ) {
        VbufConcStr( buf, "no " );
    } else {
        VbufConcDecimal( buf, count );
        VbufConcChr( buf, ' ' );
    }
    VbufConcStr( buf, thing );
    if( count != 1 ) {
        VbufConcChr( buf, 's' );
    }
    VbufConcStr( buf, end );
}


static void statsPrint(         // PRINT STATISTICS
    INITFINI* defn )            // - definition
{
    defn = defn;
    if( CompFlags.stats_printed ) {
        return;
    }
    if( CompFlags.quiet_mode ) {
        return;
    }
    if( ! CompFlags.srcfile_compiled ) {
        return;
    }
    if( WholeFName != NULL ) {
        VBUF buffer;
        VbufInit( &buffer );
        VbufConcStr( &buffer, WholeFName );
        VbufConcStr( &buffer, ": " );
        intPrint( &buffer, "line", ", ", SrcLineCount );
        if( IncLineCount != 0 ) {
            VbufConcStr( &buffer, "included " );
            VbufConcDecimal( &buffer, IncLineCount );
            VbufConcStr( &buffer, ", " );
        }
        intPrint( &buffer, "warning", ", ", WngCount );
        intPrint( &buffer, "error", "", ErrCount );
        MsgDisplayLine( VbufString( &buffer ) );
        CompFlags.stats_printed = 1;
        VbufFree( &buffer );
    }
}


INITDEFN( statistics, statsInit, statsPrint )


pch_status PCHReadStats( void )
{
    IncLineCount = PCHReadUInt();
    return( PCHCB_OK );
}


pch_status PCHWriteStats( void )
{
    PCHWriteUInt( IncLineCount );
    return( PCHCB_OK );
}


pch_status PCHInitStats( bool writing )
{
    writing = writing;
    return( PCHCB_OK );
}


pch_status PCHFiniStats( bool writing )
{
    writing = writing;
    return( PCHCB_OK );
}


#ifdef XTRA_RPT

#include "carve.h"
#include "ring.h"

#define printf  @use MsgDisplayLine@
#define puts    @use MsgDisplayLine@

// The extra-report support is compiled only when XTRA_RPT is defined.
// At present, that occurs only in debug versions of the compiler.

typedef union rptreg RPTREG;

typedef struct rptreg_base RPTREG_BASE;
struct rptreg_base              // RPTREG_BASE -- report registration
{   RPTREG* next;               // - next in ring
    void (*processor)(          // - print processor
        RPTREG* );              // - - takes self as arg.
};

typedef struct rptreg_ctr RPTREG_CTR;
struct rptreg_ctr               // RPTREG_CTR -- report registration for ctr
{   RPTREG_BASE base;           // - base
    const char* text;           // - report line
    int *a_ctr;                 // - addr[ ctr ]
};

typedef struct rptreg_avg RPTREG_AVG;
struct rptreg_avg               // RPTREG_AVG -- report registration for average
{   RPTREG_BASE base;           // - base
    const char* text;           // - report line
    RPTREG* total;              // - total accumulation
    RPTREG* count;              // - count accumulation
};

typedef struct rptreg_tab RPTREG_TAB;
struct rptreg_tab               // RPTREG_TAB -- report registration for table
{   RPTREG_BASE base;           // - base
    const char* title;          // - title
    const char * const *row_labels;// - row labels
    int *table;                 // - values
    unsigned dim_row;           // - row dimension
    unsigned dim_col;           // - column dimension
};

union rptreg                    // RPTREG: one of
{   RPTREG_BASE base;           // - a base
    RPTREG_CTR ctr;             // - counter entry
    RPTREG_AVG avg;             // - average entry
    RPTREG_TAB tab;             // - table
};

typedef struct                  // REPO_STAT -- repository statistics
{   VSTK_CTL refset;            // - set of references to symbols
    VSTK_CTL typeset;           // - set of references to types
    SRCFILE srcfile;            // - source file
    int defns;                  // - # definitions
} REPO_STAT;

static RPTREG* rpt_registrations;   // registrations
static carve_t carve_ctr;           // carving: RPTREG_CTR
static carve_t carve_avg;           // carving: RPTREG_AVG
static carve_t carve_tab;           // carving: RPTREG_TAB
static carve_t carve_sf;            // carving: REPO_STAT
static VSTK_CTL srcFiles;           // SRCFILE usages


static bool isReposSym          // TEST IF REPOSITORY SYMBOL
    ( SYMBOL sym )              // - the symbol
{
    return CompFlags.extra_stats_wanted
        && ! SymIsTemporary( sym )
        && ( SF2_TOKEN_LOCN & sym->flag2 );
}


static REPO_STAT* reposStat     // GET REPOSITORY STATISTICS FOR SRCFILE
    ( SRCFILE sf )              // - the source file
{
    REPO_STAT** last;           // - addr[ REPO_STAT ]
    REPO_STAT* retn;            // - REPO_STAT for source file

    VstkIterBeg( &srcFiles, last ) {
        retn = *last;
        if( retn->srcfile == sf ) {
            break;
        }
    }
    if( last == NULL ) {
        retn = CarveAlloc( carve_sf );
        *(REPO_STAT **)VstkPush( &srcFiles ) = retn;
        VstkOpen( &retn->refset, sizeof( SYMBOL ), 32 );
        VstkOpen( &retn->typeset, sizeof( TYPE ), 32 );
        retn->srcfile = sf;
        retn->defns = 0;
    }
    return( retn );
}


static void reportOnType        // SET UP TYPE REFERENCE
    ( SRCFILE curr              // - current source file
    , TYPE type                 // - a type
    , SYMBOL sym )              // - symbol for type
{
    if( NULL != sym && ( SF2_TOKEN_LOCN & sym->flag2 ) ) {
        SRCFILE refed = sym->locn->tl.src_file;
        if( curr != refed ) {
            REPO_STAT *repo = reposStat( curr );
            TYPE *last;

            VstkIterBeg( &repo->typeset, last ) {
                if( *last == type ) {
                    break;
                }
            }
            if( last == NULL ) {
                *(TYPE *)VstkPush( &repo->typeset ) = type;
            }
        }
    }
}


static void extraRptTypeUsage   // TYPE USAGE
    ( TYPE type )               // - the type
{
    SRCFILE current = SrcFileCurrent();
    for( ; type != NULL; type = type->of ) {
        switch( type->id ) {
          case TYP_ENUM :
          case TYP_TYPEDEF :
          { SYMBOL sym = type->u.t.sym;
            reportOnType( current, type, sym );
          } break;
          case TYP_CLASS :
            if( type->u.c.scope->id == SCOPE_FUNCTION ) {
                SYMBOL sym = type->u.c.scope->owner.sym;
                reportOnType( current, type, sym );
            }
            break;
          case TYP_MEMBER_POINTER :
            extraRptTypeUsage( type->u.mp.host );
            continue;
          case TYP_FUNCTION :
          { arg_list* args = type->u.f.args;
            int count = args->num_args;
            for( ; count > 0; ) {
                -- count;
                extraRptTypeUsage( args->type_list[count] );
            }
          }
          // drops thru
          default :
            continue;
        }
        break;
    }
}


void ExtraRptSymUsage(          // REPORT SYMBOL USAGE FROM PRIMARY SOURCE
    SYMBOL sym )
{
    extraRptTypeUsage( sym->sym_type );
    if( isReposSym( sym ) ) {
        SRCFILE current = SrcFileCurrent();
        SRCFILE refed = sym->locn->tl.src_file;
        if( current != refed ) {
            REPO_STAT *repo = reposStat( current );
            SYMBOL *last;

            VstkIterBeg( &repo->refset, last ) {
                if( *last == sym ) {
                    break;
                }
            }
            if( last == NULL ) {
                *(SYMBOL *)VstkPush( &repo->refset ) = sym;
            }
        }
    }
}


void ExtraRptSymDefn(           // REPORT SYMBOL DEFINITION IN PRIMARY SOURCE
    SYMBOL sym )
{
    extraRptTypeUsage( sym->sym_type );
    if( isReposSym( sym ) ) {
        SRCFILE current = SrcFileCurrent();
        REPO_STAT* repo = reposStat( current );
        ++ repo->defns;
    }
}


static char sbuff[512];
static char const fmt_repos[] = "%5u %5u %5u %5u %s";

static void rptRepository       // PRINT REPOSITORY REPORT
    ( INITFINI* defn )          // - definition
{
    REPO_STAT** last;           // - addr[ REPO_STAT ]
    REPO_STAT* repo;            // - REPO_STAT for source file
    unsigned ref_syms;          // - # symbol references
    unsigned ref_types;         // - # type references
    unsigned avg_defs;          // - average definitions
    unsigned avg_syms;          // - average symbol references
    unsigned avg_types;         // - average type references
    unsigned file_count;        // - # files

    defn = defn;
    if( ! CompFlags.extra_stats_wanted ) {
        for(;;) {
            last = VstkPop( &srcFiles );
            if( NULL == last )
                break;
            repo = *last;
            VstkClose( &repo->refset );
            VstkClose( &repo->typeset );
        }
        return;
    }
    MsgDisplayLine( "" );
    MsgDisplayLine( "Repository Statistics" );
    MsgDisplayLine( "" );
    MsgDisplayLine( "Defns  Syms Types Total File" );
    MsgDisplayLine( "" );
    avg_defs = 0;
    avg_syms = 0;
    avg_types = 0;
    file_count = 0;
    for(;;) {
        last = VstkPop( &srcFiles );
        if( NULL == last )
            break;
        repo = *last;
        ref_syms = VstkDimension( &repo->refset );
        ref_types = VstkDimension( &repo->typeset );
        ++file_count;
        avg_syms += ref_syms;
        avg_types += ref_types;
        avg_defs += repo->defns;
        sprintf( sbuff
              , fmt_repos
              , repo->defns
              , ref_syms
              , ref_types
              , ref_syms + ref_types
              , SrcFileName( repo->srcfile )
              );
        MsgDisplayLine( sbuff );
        VstkClose( &repo->refset );
        VstkClose( &repo->typeset );
    }
    if( file_count > 0 ) {
        unsigned fuzz = file_count / 2;
        MsgDisplayLine( "" );
        sprintf( sbuff
              , fmt_repos
              , avg_defs
              , avg_syms
              , avg_types
              , avg_syms + avg_types
              , "---- Totals ------"
              );
        MsgDisplayLine( sbuff );
        avg_defs = ( avg_defs + fuzz ) / file_count;
        avg_syms = ( avg_syms + fuzz ) / file_count;
        avg_types = ( avg_types + fuzz ) / file_count;
        sprintf( sbuff
              , fmt_repos
              , avg_defs
              , avg_syms
              , avg_types
              , avg_syms + avg_types
              , "---- Averages ----"
              );
        MsgDisplayLine( sbuff );
        MsgDisplayLine( "" );
        sprintf( sbuff, "%u files processed", file_count );
        MsgDisplayLine( sbuff );
        MsgDisplayLine( "" );
    }
    fflush( stdout );
}

INITDEFN( repos_rpts, InitFiniStub, rptRepository );


static void extraRptInit(       // INITIALIZATION FOR EXTRA REPORTING
    INITFINI* defn )            // - definition
{
    defn = defn;
    rpt_registrations = NULL;
    carve_ctr = CarveCreate( sizeof( RPTREG_CTR ), 32 );
    carve_avg = CarveCreate( sizeof( RPTREG_AVG ), 32 );
    carve_tab = CarveCreate( sizeof( RPTREG_TAB ),  4 );
    carve_sf  = CarveCreate( sizeof( REPO_STAT  ), 32 );
    VstkOpen( &srcFiles, sizeof( SYMBOL ), 32 );
}


static void extraRptFini(       // COMPLETION FOR EXTRA REPORTING
    INITFINI* defn )            // - definition
{
    RPTREG* reg;                // - current registration

    defn = defn;
    if( CompFlags.extra_stats_wanted ) {
        MsgDisplayLine( "" );
        MsgDisplayLine( "-zi Statistics" );
        MsgDisplayLine( "" );
        RingIterBeg( rpt_registrations, reg ) {
            (reg->base.processor)( reg );
        } RingIterEnd( reg )
    }
    CarveDestroy( carve_ctr );
    CarveDestroy( carve_avg );
    CarveDestroy( carve_tab );
    CarveDestroy( carve_sf );
    VstkClose( &srcFiles );
}

INITDEFN( extra_reports, extraRptInit, InitFiniStub );
INITDEFN( dump_reports, InitFiniStub, extraRptFini );


static void extraRptPrintCtr(   // PRINT A COUNTER
    RPTREG* reg )               // - registration
{
    if( reg->ctr.text != NULL ) {
        char buffer[32];
        memset( buffer, ' ', sizeof(buffer) );
        sprintf( buffer + 16, "%u", *reg->ctr.a_ctr );
        MsgDisplayLineArgs( buffer + strlen(buffer) - 9
                          , " = "
                          , reg->ctr.text
                          , NULL );
    }
}


void ExtraRptRegisterCtr(       // REGISTER A COUNTER
    int *a_ctr,                 // - addr( counter )
    const char* rpt_line )      // - report line
{
    RPTREG* reg;                // - registration entry

    reg = RingCarveAlloc( carve_ctr, &rpt_registrations );
    reg->base.processor = extraRptPrintCtr;
    *a_ctr = 0;
    reg->ctr.a_ctr = a_ctr;
    reg->ctr.text = rpt_line;
}


void ExtraRptRegisterMax(       // REGISTER A MAXIMUM
    int *a_ctr,                 // - addr( counter )
    const char* rpt_line )      // - report line
{
    ExtraRptRegisterCtr( a_ctr, rpt_line );
}


static RPTREG* extraRptLookupCtr( // LOOK UP CTR ENTRY
    int *a_ctr )                // - addr[ counter ]
{
    RPTREG* curr;               // - search entry
    RPTREG* entry;              // - entry

    entry = NULL;
    RingIterBeg( rpt_registrations, curr ) {
        if( curr->base.processor == extraRptPrintCtr
         && curr->ctr.a_ctr == a_ctr ) {
            entry = curr;
            break;
        }
    } RingIterEnd( curr )
    DbgVerify( entry != NULL, "extraRptLookup -- missing counter" );
    return entry;
}


static void extraRptPrintAvg(   // PRINT AN AVERAGE
    RPTREG* reg )               // - registration
{
    int count;                  // - ctr: count
    int total;                  // - ctr: total
    int integ;                  // - computed average: integral part
    int fract;                  // - computed average: fractional part
    char* p;                    // - walks thru buffer
    auto char int_part[16];     // - formatting buffers
    auto char frac_part[16];

    count = *reg->avg.count->ctr.a_ctr;
    if( count == 0 ) {
        integ = 0;
        fract = 0;
    } else {
        total = *reg->avg.total->ctr.a_ctr;
        integ = total / count;
        total -= integ * count;
        fract = ( total * 2000 + count ) / count / 2;
    }
    sprintf( frac_part, "%d", fract + 1000 );
    frac_part[0] = '.';
    sprintf( int_part, "%d", integ + 100000 );
    for( p = int_part+1; *p == '0'; ++p ) *p = ' ';
    MsgDisplayLineArgs( int_part+1
                      , frac_part
                      , " = "
                      , reg->avg.text
                      , NULL );
}


void ExtraRptRegisterAvg(       // REGISTER AVERAGING
    int *a_total,               // - addr[ total ]
    int *a_count,               // - addr[ count ]
    const char* rpt_line )      // - report line
{
    RPTREG* reg;                // - registration entry

    reg = RingCarveAlloc( carve_avg, &rpt_registrations );
    reg->base.processor = extraRptPrintAvg;
    reg->avg.text = rpt_line;
    reg->avg.total = extraRptLookupCtr( a_total );
    reg->avg.count = extraRptLookupCtr( a_count );
}


static void extraRptTable(      // PRINT A TABLE
    RPTREG* reg )               // - registration
{
    unsigned i;                 // - row index
    unsigned r, c;              // - row, column indices
    int maxval;                 // - maximum table value
    unsigned digits;            // - # of digits

    digits = 0;
    maxval = 1;
    for( r = 0; r < reg->tab.dim_row; ++ r ) {
        for( c = 0; c < reg->tab.dim_col; ++ c ) {
            int val = reg->tab.table[ r * reg->tab.dim_col + c ];
            for( ; maxval < val; ++ digits, maxval *= 10 );
        }
    }
    if( digits > 0 ) {
        char fmt[16];
        char buf[32];
        VBUF buffer;
        unsigned label_width;
        size_t len;
        char const * const *row_lbl;

        label_width = 0;
        row_lbl = reg->tab.row_labels;
        if( row_lbl != NULL ) {
            for( i = 0; i < reg->tab.dim_row; ++i ) {
                len = strlen( row_lbl[i] );
                if( len > label_width ) {
                    label_width = len;
                }
            }
        }
        VbufInit( &buffer );
        MsgDisplayLine( "" );
        MsgDisplayLine( (char*)reg->tab.title );
        MsgDisplayLine( "" );
        sprintf( fmt, "%%%dd", digits + 1 );
        for( r = 0; r < reg->tab.dim_row; ++ r ) {
            int *row = &reg->tab.table[ r * reg->tab.dim_col ];
            VbufRewind( &buffer );
            if( row_lbl == NULL ) {
                sprintf( buf, "%4d", r );
                buf[4] = ':';
                buf[5] = ' ';
                buf[6] = '\0';
                VbufConcStr( &buffer, buf );
            } else {
                char const *l = row_lbl[r];
                unsigned b;
                for( b = label_width - strlen(l); b != 0; --b ) {
                    VbufConcChr( &buffer, ' ' );
                }
                VbufConcStr( &buffer, l );
                VbufConcStr( &buffer, ": " );
            }
            for( c = 0; c < reg->tab.dim_col; ++ c ) {
                sprintf( buf, fmt, row[ c ] );
                VbufConcStr( &buffer, buf );
            }
            MsgDisplayLine( VbufString( &buffer ) );
        }
        MsgDisplayLine( "" );
        VbufFree( &buffer );
    }
}


void ExtraRptRegisterTab(       // REGISTER TABLE
    char const* title,          // - title
    char const * const *row_labels,//- row labels
    int *table,                 // - table
    unsigned rows,              // - # rows
    unsigned cols )             // - # columns
{
    RPTREG* reg;                // - new registration

    reg = RingCarveAlloc( carve_tab, &rpt_registrations );
    reg->base.processor = &extraRptTable;
    reg->tab.title = title;
    reg->tab.row_labels = row_labels;
    reg->tab.table = table;
    reg->tab.dim_row = rows;
    reg->tab.dim_col = cols;
}

#endif
