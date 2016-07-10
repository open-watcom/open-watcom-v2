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


#include "variety.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "rtinit.h"
#include "p5prof.h"

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
        struct {
            reg_32      lo_cycle;
            reg_32      hi_cycle;
        };
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
extern  char    **_argv;

static void p5_profile_init( void )
/*
 * Reset profiling data
 */
{
    P5_timing_info      *curr;
    P5_timing_info      *last;
    unsigned            len;

    curr = (void *)&_Start_TI;
    last = (void *)&_End_TI;
    // set things to the way they are before profiling begins
    while( (void *)( (char *)curr + offsetof( P5_timing_info, name ) ) < last ) {
        if( curr->semaphore == ~0ul ) {     // if semaphore not open
            curr->count = 0ul;
            curr->lo_cycle = 0;
            curr->hi_cycle = 0;
        }
        len = strlen( curr->name ) + 1;
        len = __ROUND_UP_SIZE( len, 4 );
        curr = (void *)( (char *)curr + ( offsetof( P5_timing_info, name ) + len ) );
    }
}

#if !defined(_MAX_PATH2)
#define _MAX_PATH2 (_MAX_PATH+3) /* maximum size of output buffer for _splitpath2() */
#endif

static void p5_profile_fini( void )
/*
 * Dump profiling data to file
 */
{
    P5_timing_info      *curr;
    P5_timing_info      *last;
    FILE                *out;
    unsigned            len;
    int                 i;
    char                *drive;
    char                *dir;
    char                *name;
#if ! defined(__NETWARE__)
    char                fname[ _MAX_PATH2 ];
#endif
    char                pname[ _MAX_PATH2 ];
    char                stkbuf[24];
    union tsc           final_tsc;
    union tsc           u;

    _RDTSC( &final_tsc );
    curr = (void *)&_Start_TI;
    last = (void *)&_End_TI;
#if defined(__NETWARE__)
    strcpy( pname, *_argv );
    for( i = strlen( pname ); i > 0; i-- ) {
        if( pname[i] == '.' ) break;
    }
    if( i > 0 ) {
        strcpy( &pname[i], ".prf" );
    } else {
        strcpy( pname, "results.prf" );
    }
#else
    strcpy( pname, "results.prf" );
    if( *_argv != NULL ) {
        _splitpath2( *_argv, &fname, &drive, &dir, &name, NULL );
        _makepath( pname, drive, dir, name, ".prf" );
    }
#endif

    out = fopen( pname, "wt+" );
    if( out == NULL ) return;
    while( (void *)( (char *)curr + offsetof( P5_timing_info, name ) ) < last ) {
//      fprintf( out, "%20.20s\t%08x\t%08x%08x\n", curr->name, curr->count, curr->hi_cycle, curr->lo_cycle );
//      fprintf( out, "%08lx%08lx%11lu\t%s\n", curr->hi_cycle, curr->lo_cycle, curr->count, curr->name );
        u.lo_cycle = curr->lo_cycle;
        u.hi_cycle = curr->hi_cycle;
        if( curr->semaphore != ~0ul ) {         // if semaphore still open
            curr->semaphore = ~0ul;             // - reset it
            u.lo_cycle += final_tsc.lo_cycle;   // - adjust cycle time
            if( u.lo_cycle < final_tsc.lo_cycle ) {
                ++u.hi_cycle;
            }
            u.hi_cycle += final_tsc.hi_cycle;
        }
        _Bin2String((short int _WCNEAR *)&u.bigint[0],(char _WCNEAR *)stkbuf,20 );
        for( i = 0; stkbuf[i+1] != '\0'; i++ ) {
            if( stkbuf[i] != '0' ) break;
            stkbuf[i] = ' ';
        }
        fprintf( out, "%s%11lu  %s\n", stkbuf, curr->count, curr->name );
        curr->count = 0ul;
        curr->lo_cycle = 0;
        curr->hi_cycle = 0;
        len = strlen( curr->name ) + 1;
        len = __ROUND_UP_SIZE( len, 4 );
        curr = (void *)( (char *)curr + ( offsetof( P5_timing_info, name ) + len ) );
    }
    fclose( out );

}

#if defined(_M_IX86)
 #pragma aux __p5_profile "*";
#endif
AXI(              p5_profile_init, INIT_PRIORITY_LIBRARY + 1 )
YI( __p5_profile, p5_profile_fini, INIT_PRIORITY_LIBRARY + 1 )
