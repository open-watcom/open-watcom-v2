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
* Description:  Pentium profiling support (RDTSC based).
*
****************************************************************************/


#if defined( __WATCOMC__ ) && !defined( _COMDEF_H_INCLUDED )
 #include <_comdef.h>
#endif

typedef unsigned long reg_32;

#include "pushpck1.h"
typedef struct P5_timing_info {
    reg_32      count;
    reg_32      semaphore;
    reg_32      lo_cycle;
    reg_32      hi_cycle;
    char        name[1];
} P5_timing_info;

typedef struct new_P5_timing_info {
    char        flag[4]; // the flag field MUST be first
    reg_32      semaphore;
    struct new_P5_timing_info *stack;
    reg_32      esp;
    struct new_P5_timing_info *dynamic;
    long long   count;
    long long   cycles;
    long long   start_time;
    reg_32      caller;
    reg_32      call_ins;
    reg_32      callee;
} new_P5_timing_info;

typedef struct block_count_info {
    char        flag[4]; // the flag field MUST be first
    reg_32      lo_count;
    reg_32      hi_count;
    reg_32      address;
    reg_32      function;
} block_count_info;
#include "poppck.h"

#define PROFILE_FLAG_DYNAMIC    '+'
#define PROFILE_FLAG_END_GROUP  '-'
#define PROFILE_FLAG_BLOCK      'b'
#define PROFILE_LONG_FORMAT_LEN 20

extern void *__ProfAlloc( unsigned long size );

#if defined( __WATCOMC__ )

_WCRTLINK extern void __ProfInit( void );
_WCRTLINK extern void __ProfExitCriticalSection( void );
_WCRTLINK extern void __ProfEnterCriticalSection( void );
_WCRTLINK extern void __ProfEnable( void );
_WCRTLINK extern void __ProfDisable( void );
_WCRTLINK extern __int64 __P5_overhead( void );

#if !defined( _M_I86 )
#pragma aux __ProfProlog "__PON" parm routine [] modify []
_WCRTLINK extern void __ProfProlog( new_P5_timing_info *block );
#pragma aux __ProfEpilog "__POFF" parm routine [] modify []
_WCRTLINK extern void __ProfEpilog( new_P5_timing_info *block );
#endif

#endif
