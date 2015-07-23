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
* Description:  Pentium (ie. RDTSC-based) profiling support routines.
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
#include <stdlib.h>
#ifdef __NT__
    #include <windows.h>
#endif
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/stat.h>
#include "rtinit.h"
#include "p5prof.h"
#include "ljmphdl.h"
#include "initarg.h"

#ifdef __UNIX__
    extern char **_argv;
    #define PGM_NAME _argv[0]
#else
    #define PGM_NAME _LpPgmName
#endif

#ifndef TRUE
    #define TRUE (1==1)
#endif
#ifndef FALSE
    #define FALSE (1!=1)
#endif

extern  void _Bin2String(short int _WCNEAR *, char _WCNEAR *, int);
#if defined(__386__)
 #pragma aux _Bin2String     "_*" parm routine [eax] [edx] [ebx];
#elif defined( _M_I86 )
 #pragma aux _Bin2String     "_*" parm routine [ax] [dx] [bx];
#else
 #error unsupported platform
#endif

union tsc {
        short int       bigint[4];
        __int64         i;
};

extern  void    _RDTSC( union tsc * );
#pragma aux     _RDTSC = \
        ".586" \
        "rdtsc" \
        "mov    [ebx],eax" \
        "mov    4[ebx],edx" \
        parm [ebx] modify [eax edx];

extern  void    *_Start_TI;
extern  void    *_End_TI;
extern  int ___begtext;
#pragma aux ___begtext "*";

#define NEXT_INFO( x ) \
    (void *)( (char *)(x) + ( x->flag[0] == PROFILE_FLAG_BLOCK ? sizeof( block_count_info ) : sizeof( new_P5_timing_info ) ) )

static union tsc                final_tsc;
static union tsc                initial_tsc;

static FILE *OpenPrfFile( int isInit )
{
    char                *drive;
    char                *dir;
    char                *name;
    char                fname[ _MAX_PATH2 ];
    char                pname[ _MAX_PATH2 ];
    FILE                *out;
    int                 already;
    union tsc           start_tsc;
    union tsc           end_tsc;
    struct stat         exe_stat;
    struct stat         prf_stat;

    if( PGM_NAME != NULL ) {
        _splitpath2( PGM_NAME, &fname, &drive, &dir, &name, NULL );
        _makepath( pname, drive, dir, name, ".prf" );
    } else {
        return( NULL );
    }
    already = access( pname, R_OK ) == 0;
    if( already ) {
        stat( pname, &prf_stat );
        stat( PGM_NAME, &exe_stat );
        if( exe_stat.st_mtime > prf_stat.st_mtime ) {
            unlink( pname );
            already = FALSE;
        }
    }
    out = fopen( pname, "at+" );
    if( out == NULL )
        return( NULL );
    if( !already ) {
        fprintf( out, "Format: C/C++ 1\n" );
        _RDTSC( &start_tsc );
        sleep( 1 );
        _RDTSC( &end_tsc );
        fprintf( out, "Ticks Per Second %d\n", (long)( end_tsc.i - start_tsc.i ) );
    }
    return( out );
}

static void new_p5_profile_init( void )
/*
 * Create profiling data file, enable profiling
 */
{
    FILE *out;

    out = OpenPrfFile( TRUE );
    if( out == NULL )
        return;
#ifdef __UNIX__
    fprintf( out, "Start Run\n" );
#else
    if( _LpDllName == NULL ) {
        fprintf( out, "Start Run\n" );
    }
#endif
    fclose( out );
    _RDTSC( &initial_tsc );
    __ProfEnable();
}

enum {
    DUMP_FUNCTIONS,
    DUMP_CALLS,
    DUMP_BLOCKS
};

#define LONG_BUFF_LEN   (PROFILE_LONG_FORMAT_LEN+4)

static char *FormatLong( void *pu, char *buff )
/*************************************************/
{
    int                 i;
    char                *lastspace;

    _Bin2String((short int _WCNEAR *)pu,(char _WCNEAR *)buff, PROFILE_LONG_FORMAT_LEN );
    for( i = 0; buff[i+1] != '\0'; i++ ) {
        if( buff[i] != '0' )
            break;
        buff[i] = ' ';
        lastspace = buff+i;
    }
    return( lastspace );
}

static void dumpOneCallThing( FILE *out, new_P5_timing_info *curr )
/***********************************************/
{
    char                tickBuff[LONG_BUFF_LEN];
    char                countBuff[LONG_BUFF_LEN];
    union tsc           u;
    int                 neg;
    char                *lastspace;

    u.i = curr->cycles;
    if( u.i < 0 ) {
        u.i = -u.i;
        neg = TRUE;
    } else {
        neg = FALSE;
    }
    lastspace = FormatLong( &u, tickBuff );
    if( neg ) *lastspace = '-';
    FormatLong( &curr->count, countBuff );
    if( curr->callee != 0 ) {
        fprintf( out, "%s %s %8.8x %8.8x %8.8x\n", tickBuff,
                countBuff, curr->callee, curr->caller, curr->call_ins );
    }
}

static void dumpOneBlockThing( FILE *out, block_count_info *curr )
/****************************************************************/
{
    char        countBuff[LONG_BUFF_LEN];

    FormatLong( &curr->lo_count, countBuff );
    fprintf( out, "%s %8.8x %8.8x\n", countBuff, curr->address, curr->function );
}

static void dumpOneBlock( FILE *out, new_P5_timing_info *curr, int todump )
/*************************************************************************/
{
    char                *lastspace;
    new_P5_timing_info  *dynamic;

    if( curr->flag[0] == PROFILE_FLAG_DYNAMIC ) {
        dynamic = curr->dynamic;
        for( ;; ) {
            dumpOneBlock( out, dynamic, todump );
            if( dynamic->flag[0] == PROFILE_FLAG_END_GROUP )
                break;
            if( dynamic->flag[0] == PROFILE_FLAG_DYNAMIC )
                break;
            dynamic = NEXT_INFO( dynamic );
        }
    }
    switch( todump ) {
    case DUMP_FUNCTIONS:
        if( curr->flag[0] == PROFILE_FLAG_BLOCK )
            break;
        if( curr->call_ins != 0 )
            break;
        curr->caller = 0;
        dumpOneCallThing( out, curr );
        break;
    case DUMP_CALLS:
        if( curr->flag[0] == PROFILE_FLAG_BLOCK )
            break;
        if( curr->call_ins == 0 )
            break;
        dumpOneCallThing( out, curr );
        break;
    case DUMP_BLOCKS:
        if( curr->flag[0] != PROFILE_FLAG_BLOCK )
            break;
        dumpOneBlockThing( out, (block_count_info*)curr );
        break;
    }
}


static void new_p5_profile_fini( void )
/*
 * Disable profiling, save all data to file
 */
{
    new_P5_timing_info  *curr;
    new_P5_timing_info  *last;
    FILE                *out;
    union tsc           start_tsc;
    union tsc           end_tsc;
    union tsc           total_tsc;
    char                tickBuff[LONG_BUFF_LEN];
    int                 i;

    __ProfEnterCriticalSection();
    __ProfDisable();
    _RDTSC( &final_tsc );
    last = (void *)&_End_TI;
    out = OpenPrfFile( FALSE );
    if( out == NULL )
        return;
#ifdef __UNIX__
    fprintf( out, "Image Name %s\n", PGM_NAME );
#else
    if( _LpDllName != NULL ) {
        fprintf( out, "Image Name %s\n", _LpDllName );
    } else {
        fprintf( out, "Image Name %s\n", PGM_NAME );
    }
#endif
    fprintf( out, "Code Base %8.8x\n", ((char*)&___begtext)-3 );

    start_tsc.i = final_tsc.i - initial_tsc.i;
    total_tsc.i = start_tsc.i - __P5_overhead();

    FormatLong( &total_tsc, tickBuff );
    fprintf( out, "Total Ticks %s\n", tickBuff );
    fprintf( out, "=Number of Ticks==== ==Count============= =callee= =caller= =call instruction=\n" );
    curr = (void *)&_Start_TI;
    while( curr < last ) {
        dumpOneBlock( out, curr, DUMP_FUNCTIONS );
        curr = NEXT_INFO( curr );
    }

    curr = (void *)&_Start_TI;
    while( curr < last ) {
        dumpOneBlock( out, curr, DUMP_CALLS );
        curr = NEXT_INFO( curr );
    }

    fprintf( out, "=Count============== =Block== =Function=\n" );
    curr = (void *)&_Start_TI;
    while( curr < last ) {
        dumpOneBlock( out, curr, DUMP_BLOCKS );
        curr = NEXT_INFO( curr );
    }

    fclose( out );
#if defined(__SW_BR)
    __ProfEnable();
#endif
    __ProfExitCriticalSection();
}

#if defined(_M_IX86)
 #pragma aux __new_p5_profile "*";
#endif
AXI(                  new_p5_profile_init, INIT_PRIORITY_LIBRARY + 1 )
YI( __new_p5_profile, new_p5_profile_fini, INIT_PRIORITY_LIBRARY + 1 )
