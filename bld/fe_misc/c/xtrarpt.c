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


#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#ifndef NDEBUG
#define XRPT
#endif

#ifdef XRPT
#undef NDEBUG

#include header
#include "carve.h"
#include "initdefs.h"
#include "vstk.h"
#include "ringfns.h"
#include "ringcarv.h"
#include "vbuf.h"

#define printf  @use outputLine@
#define puts    @use outputLine@

typedef union rptreg RPTREG;

typedef struct rptreg_base RPTREG_BASE;
struct rptreg_base              // RPTREG_BASE -- report registration
{   RPTREG* next;               // - next in ring
    void (*processor)(          // - print processor
        FILE *,                 // - - output file
        RPTREG* );              // - - takes self as arg.
};

typedef struct rptreg_ctr RPTREG_CTR;
struct rptreg_ctr               // RPTREG_CTR -- report registration for ctr
{   RPTREG_BASE base;           // - base
    const char* text;           // - report line
    long* a_ctr;                // - addr[ ctr ]
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
    long* table;                // - values
    unsigned dim_row;           // - row dimension
    unsigned dim_col;           // - column dimension
};

typedef struct rptreg_tit RPTREG_TIT;
struct rptreg_tit               // RPTREG_TIT -- report registration for title
{   RPTREG_BASE base;           // - base
    const char* title;          // - title
};

union rptreg                    // RPTREG: one of
{   RPTREG_BASE base;           // - a base
    RPTREG_CTR ctr;             // - counter entry
    RPTREG_AVG avg;             // - average entry
    RPTREG_TAB tab;             // - table
    RPTREG_TIT tit;             // - title
};

static RPTREG* reportList;      // registrations
static carve_t carveRPTREG_CTR; // carving: RPTREG_CTR
static carve_t carveRPTREG_AVG; // carving: RPTREG_AVG
static carve_t carveRPTREG_TAB; // carving: RPTREG_TAB
static carve_t carveRPTREG_TIT; // carving: RPTREG_TIT

static void extraRptInit(       // INITIALIZATION FOR EXTRA REPORTING
    INITFINI* defn )            // - definition
{
    defn = defn;
    reportList = NULL;
    carveRPTREG_CTR = CarveCreate( sizeof( RPTREG_CTR ), 64 );
    carveRPTREG_AVG = CarveCreate( sizeof( RPTREG_AVG ), 64 );
    carveRPTREG_TAB = CarveCreate( sizeof( RPTREG_TAB ), 16 );
    carveRPTREG_TIT = CarveCreate( sizeof( RPTREG_TIT ), 8  );
}


static void extraRptFini(       // COMPLETION FOR EXTRA REPORTING
    INITFINI* defn )            // - definition
{
    defn = defn;
    CarveDestroy( carveRPTREG_CTR );
    CarveDestroy( carveRPTREG_AVG );
    CarveDestroy( carveRPTREG_TAB );
    CarveDestroy( carveRPTREG_TIT );
}

INITDEFN( extra_reports, extraRptInit, extraRptFini );

static void outputLine(         // OUTPUT LINE INTO FILE
    FILE *fp,                   // - file pointer
    const char *msg,            // - message
    ... )
{
    va_list args;

    if( msg[0] != '\0' ) {
        va_start( args, msg );
        vfprintf( fp, msg, args );
        va_end( args );
    }
    fputc( '\n', fp );
}

#if 0
static void outputText(         // OUTPUT TEXT INTO FILE
    FILE *fp,                   // - file pointer
    const char *msg,            // - message
    ... )
{
    va_list args;

    va_start( args, msg );
    vfprintf( fp, msg, args );
    va_end( args );
}
#endif

static void outputLineArgs(     // OUTPUT STRINGS LINE INTO FILE
    FILE *fp,                   // - file pointer
    const char *msg,            // - message
    ... )
{
    va_list args;
    char const *arg;

    va_start( args, msg );
    arg = msg;
    for(;;) {
        if( arg == NULL ) break;
        fputs( arg, fp );
        arg = va_arg( args, char const * );
    }
    va_end( args );
   fputc( '\n', fp );
}

void ExtraRptDumpReport(        // DUMP REPORT INTO FILE
    const char *name )          // - name to use for generating file name
{
    RPTREG *reg;
    FILE *fp;
    char *base;
    auto char make_buff[_MAX_PATH];
    auto char split_buff[_MAX_PATH2];

    _splitpath2( name, split_buff, NULL, NULL, &base, NULL );
    _makepath( make_buff, NULL, NULL, base, ".rpt" );
    fp = fopen( make_buff, "w" );
    if( fp == NULL ) {
        return;
    }
    outputLine( fp, "" );
    outputLine( fp, "Statistics Report" );
    outputLine( fp, "=================" );
    outputLine( fp, "" );
    RingIterBeg( reportList, reg ) {
        (reg->base.processor)( fp, reg );
    } RingIterEnd( reg )
    fclose( fp );
}

static void extraRptPrintCtr(   // PRINT A COUNTER
    FILE *fp,                   // - file pointer
    RPTREG* reg )               // - registration
{
    if( reg->ctr.text != NULL ) {
        char buffer[32];
        memset( buffer, ' ', sizeof(buffer) );
        ltoa( *reg->ctr.a_ctr, buffer + 16, 10 );
        outputLineArgs( fp
                      , buffer + strlen(buffer) - 9
                      , " = "
                      , reg->ctr.text
                      , NULL );
    }
}


void ExtraRptRegisterCtr(       // REGISTER A COUNTER
    long* a_ctr,                // - addr( counter )
    const char* rpt_line )      // - report line
{
    RPTREG* reg;                // - registration entry

    reg = RingCarveAlloc( carveRPTREG_CTR, &reportList );
    reg->base.processor = extraRptPrintCtr;
    *a_ctr = 0;
    reg->ctr.a_ctr = a_ctr;
    reg->ctr.text = rpt_line;
}


void ExtraRptRegisterMax(       // REGISTER A MAXIMUM
    long* a_ctr,                // - addr( counter )
    const char* rpt_line )      // - report line
{
    ExtraRptRegisterCtr( a_ctr, rpt_line );
}


static void extraRptPrintTitle( // PRINT A TITLE
    FILE *fp,                   // - file pointer
    RPTREG* reg )               // - registration
{
    if( reg->tit.title != NULL ) {
        outputLineArgs( fp, "", NULL );
        outputLineArgs( fp, reg->tit.title, NULL );
        outputLineArgs( fp, "", NULL );
    }
}


void ExtraRptRegisterTitle(     // REGISTER A TITLE
    const char* title )         // - title line
{
    RPTREG* reg;                // - registration entry

    reg = RingCarveAlloc( carveRPTREG_TIT, &reportList );
    reg->base.processor = extraRptPrintTitle;
    reg->tit.title = title;
}


static RPTREG* extraRptLookupCtr( // LOOK UP CTR ENTRY
    long* a_ctr )               // - addr[ counter ]
{
    RPTREG* curr;               // - search entry
    RPTREG* entry;              // - entry

    entry = NULL;
    RingIterBeg( reportList, curr ) {
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
    FILE *fp,                   // - file pointer
    RPTREG* reg )               // - registration
{
    long count;                 // - ctr: count
    long total;                 // - ctr: total
    long integ;                 // - computed average: integral part
    long fract;                 // - computed average: fractional part
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
    itoa( fract + 1000, frac_part, 10 );
    frac_part[0] = '.';
    itoa( integ + 100000, int_part, 10 );
    for( p = int_part+1; *p == '0'; ++p ) *p = ' ';
    outputLineArgs( fp
                  , int_part+1
                  , frac_part
                  , " = "
                  , reg->avg.text
                  , NULL );
}


void ExtraRptRegisterAvg(       // REGISTER AVERAGING
    long* a_total,              // - addr[ total ]
    long* a_count,              // - addr[ count ]
    const char* rpt_line )      // - report line
{
    RPTREG* reg;                // - registration entry

    reg = RingCarveAlloc( carveRPTREG_AVG, &reportList );
    reg->base.processor = extraRptPrintAvg;
    reg->avg.text = rpt_line;
    reg->avg.total = extraRptLookupCtr( a_total );
    reg->avg.count = extraRptLookupCtr( a_count );
}


static void extraRptTable(      // PRINT A TABLE
    FILE *fp,                   // - file pointer
    RPTREG* reg )               // - registration
{
    unsigned i;                 // - row index
    unsigned r, c;              // - row, column indices
    long maxval;                // - maximum table value
    unsigned digits;            // - # of digits

    digits = 0;
    maxval = 1;
    for( r = 0; r < reg->tab.dim_row; ++ r ) {
        for( c = 0; c < reg->tab.dim_col; ++ c ) {
            long val = reg->tab.table[ r * reg->tab.dim_col + c ];
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
        outputLine( fp, "" );
        outputLine( fp, (char*)reg->tab.title );
        outputLine( fp, "" );
        sprintf( fmt, "%%%dd", digits + 1 );
        for( r = 0; r < reg->tab.dim_row; ++ r ) {
            long* row = &reg->tab.table[ r * reg->tab.dim_col ];
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
            outputLine( fp, VbufString( &buffer ) );
        }
        outputLine( fp, "" );
        VbufFree( &buffer );
    }
}


void ExtraRptRegisterTab(       // REGISTER TABLE
    char const* title,          // - title
    char const * const *row_labels,//- row labels
    long* table,                // - table
    unsigned rows,              // - # rows
    unsigned cols )             // - # columns
{
    RPTREG* reg;                // - new registration

    reg = RingCarveAlloc( carveRPTREG_TAB, &reportList );
    reg->base.processor = &extraRptTable;
    reg->tab.title = title;
    reg->tab.row_labels = row_labels;
    reg->tab.table = table;
    reg->tab.dim_row = rows;
    reg->tab.dim_col = cols;
}
#else
#error should not be compiled in production mode with XRPT defined
#endif
