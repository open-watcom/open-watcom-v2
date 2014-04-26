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
* Description:  Pentium RDTSC-based profiling support.
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "rtinit.h"
#include "p5prof.h"
#include "ljmphdl.h"
#ifdef __NT__
 #include <windows.h>
#endif
#include "initarg.h"

#define info new_P5_timing_info

char                    _data_ProfEnable;
__int64                 _data_P5_overhead;
_WCRTDATA extern pfun   __longjmp_handler;
pfun                    __prof_old_longjmp_handler;

#ifdef __NT__
#if 0 // not currently in use
HANDLE hDevice = INVALID_HANDLE_VALUE;
#endif
CRITICAL_SECTION __ProfCriticalSection;

void __ProfUnwind( void _WCFAR * ); // forward decl

static void profilog_init( void )
{
    InitializeCriticalSection( &__ProfCriticalSection );
    if( __prof_old_longjmp_handler == NULL ) {
        __prof_old_longjmp_handler = __longjmp_handler;
        __longjmp_handler = __ProfUnwind;
    }
}

static void profilog_fini( void )
{
    DeleteCriticalSection( &__ProfCriticalSection );
#if 0 // not currently in use
    if (hDevice != INVALID_HANDLE_VALUE) {
        long *pcounter = NULL;
        DeviceIoControl(hDevice, 2, &pcounter, sizeof(pcounter), NULL,
            0, NULL, NULL);
        CloseHandle(hDevice);
    }
#endif
}
XI( __new_p5_profilog_init, profilog_init, INIT_PRIORITY_LIBRARY-1 )
YI( __new_p5_profilog_fini, profilog_fini, INIT_PRIORITY_LIBRARY-1 )
#endif

#ifdef __NT__
#define ENTER_CRIT_SECTION() ( EnterCriticalSection( &__ProfCriticalSection ) )
#define EXIT_CRIT_SECTION() ( LeaveCriticalSection( &__ProfCriticalSection ) )
#else
#define ENTER_CRIT_SECTION()
#define EXIT_CRIT_SECTION()
#endif

_WCRTLINK void __ProfEnterCriticalSection( void )
{
    ENTER_CRIT_SECTION();
}

_WCRTLINK void __ProfExitCriticalSection( void )
{
    EXIT_CRIT_SECTION();
}

_WCRTLINK void __ProfEnable( void )
{
    __ProfEnterCriticalSection();
    _data_ProfEnable = 1;
    __ProfExitCriticalSection();
}

_WCRTLINK void __ProfDisable( void )
{
    __ProfEnterCriticalSection();
    _data_ProfEnable = 0;
    __ProfExitCriticalSection();
}

_WCRTLINK __int64 __P5_overhead( void )
{
    return _data_P5_overhead;
}

extern __int64 rdtsc( void );
#pragma aux rdtsc = "rdtsc" value [ edx eax ];

#ifndef __SW_OF
    #error "Must be compiled with /of to find return address"
#endif

extern reg_32 findLongJmpReturn( void );
#pragma aux findLongJmpReturn = \
    "mov eax,[ebp]" \
    "mov eax,8[eax]" \
    value [ eax ] modify [ esp ];

extern reg_32 findReturn( void );
#pragma aux findReturn = \
    "mov eax,+4[ebp]" \
    value [ eax ] modify [ eax ];

extern reg_32 findStack( void );
#pragma aux findStack = \
    "lea eax,+12[ebp]" \
    value [ eax ] modify [ eax ];

extern reg_32 findSecondReturn( void );
#pragma aux findSecondReturn = \
    "mov eax,+12[ebp]" \
    value [ eax ] modify [ eax ];

extern void push_eax( void );
#pragma aux push_eax = \
    "push eax" \
    modify [ esp ];

extern void pop_eax( void );
#pragma aux pop_eax = \
    "pop eax" \
    modify [ esp ];

#define DIRECT_CALL_INDICATOR   (unsigned short)0xc4f7  // test esp,offset(info)
#define INDIRECT_CALL_INDICATOR (unsigned short)0xc5f7  // test ebp,offset(info)
#define BLOCK_OFFSET            2       // offset of offset(info) in instruction
#define PON_CALL_SIZE           10      // push L1; call __PON
#define NUM_ALLOC               10

static info *FindOrAllocInfo( info *callerBlock, reg_32 callee )
{
    int                 i;
    info        *newBlock;

    for( ;; ) {
        if( callerBlock->callee == callee ) break;
        if( callerBlock->callee == 0 ) break;
        if( callerBlock->flag[0] != 0 ) {
            if( callerBlock->dynamic == 0 ) {
                callerBlock->flag[0] = PROFILE_FLAG_DYNAMIC;
                newBlock = (info*)__ProfAlloc( NUM_ALLOC*sizeof( info ) );
                callerBlock->dynamic = newBlock;
                callerBlock = newBlock;
                for( i = 0; i < NUM_ALLOC; ++i ) {
                    newBlock->flag[0] = 0;
                    newBlock->dynamic = 0;
                    newBlock->start_time = 0;
                    newBlock->semaphore = 0;
                    newBlock->count = 0;
                    newBlock->stack = callerBlock->stack;
                    newBlock->cycles = 0;
                    newBlock->caller = 0;
                    newBlock->call_ins = 0;
                    newBlock->callee = 0;
                    ++newBlock;
                }
                --newBlock;
                newBlock->flag[0] = PROFILE_FLAG_END_GROUP;
                break;
            } else {
                callerBlock = callerBlock->dynamic;
            }
        } else {
            ++callerBlock;
        }
    }
    return( callerBlock );
}

void __ProfUnwind( void _WCFAR *ss_esp )
{
    reg_32      returnAddress;
    reg_32      esp;
    info        *block,*next;
    __int64     start,startLessOverhead;
    info        *callerBlock;

    push_eax();
    if( _data_ProfEnable ) {
        ENTER_CRIT_SECTION();
        start = rdtsc();
        startLessOverhead = start - _data_P5_overhead;
        _data_P5_overhead -= start;
        returnAddress = findLongJmpReturn();
        if( *(unsigned short*)returnAddress == DIRECT_CALL_INDICATOR ) {
            // get call site block for longjmp call
            block = *(info**)( returnAddress + BLOCK_OFFSET );
            // get function block from call site block
            block = block->stack;
            esp = (reg_32)ss_esp;
            while( block != 0 && esp >= block->esp ) {
                if( --block->semaphore == 0 ) {
                    if( block->start_time != 0 ) {
                        block->cycles -= block->start_time;
                        block->cycles += startLessOverhead;
                        block->start_time = 0;
                    }
                }
                callerBlock = block->stack;
                block->stack = 0;
                if( callerBlock != 0 ) {
                    if( --callerBlock->semaphore == 0 ) {
                        if( callerBlock->start_time != 0 ) {
                            callerBlock->cycles += startLessOverhead;
                            callerBlock->cycles -= callerBlock->start_time;
                            callerBlock->start_time = 0;
                        }
                    }
                    next = callerBlock->stack; // next function block
                } else {
                    next = 0;
                }
                block = next;
            }
        }
        _data_P5_overhead += rdtsc();
        EXIT_CRIT_SECTION();
    }
    pop_eax();
    __prof_old_longjmp_handler( ss_esp );
}

_WCRTLINK void __ProfProlog( info *block )
{
    __int64             start,startLessOverhead;
    reg_32              returnAddress;
    reg_32              secondReturnAddress;
    info                *callerBlock;
    reg_32              callee,caller,esp;

    push_eax();
    if( _data_ProfEnable ) {
        ENTER_CRIT_SECTION();
        start = rdtsc();
        startLessOverhead = start - _data_P5_overhead;
        _data_P5_overhead -= start;
        returnAddress = findReturn();
        esp = findStack();
        callee = returnAddress - PON_CALL_SIZE;
        if( *(unsigned short*)returnAddress != DIRECT_CALL_INDICATOR ) {
            secondReturnAddress = findSecondReturn();
            callerBlock = *(info**)( secondReturnAddress + BLOCK_OFFSET );
            if( *(unsigned short*)secondReturnAddress == DIRECT_CALL_INDICATOR ) {
                // nothing more to do right here
            } else if( *(unsigned short*)secondReturnAddress == INDIRECT_CALL_INDICATOR ) {
                caller = callerBlock->caller;
                callerBlock = FindOrAllocInfo( callerBlock, callee );
                callerBlock->caller = caller;
            } else {
                callerBlock = 0;
            }
            if( callerBlock != 0 ) {
                callerBlock->count++;
                if( callerBlock->semaphore++ == 0 ) {
                    callerBlock->callee = callee;
                    callerBlock->start_time = startLessOverhead;
                    callerBlock->call_ins = secondReturnAddress - 1;
                }
            }
        }
        block->count++;
        if( block->semaphore++ == 0 ) { // no current invocation
            block->callee = callee;
            block->start_time = startLessOverhead;
            block->esp = esp; // for unwind from longjmp
            block->stack = callerBlock; // for unwind from longjmp
        }
        _data_P5_overhead += rdtsc();
        EXIT_CRIT_SECTION();
    }
    pop_eax();
}

_WCRTLINK void __ProfEpilog( info *block )
{
    __int64             start,startLessOverhead;
    reg_32              secondReturnAddress;
    info                *callerBlock;

    push_eax();
    if( _data_ProfEnable ) {
        ENTER_CRIT_SECTION();
        start = rdtsc();
        startLessOverhead = start - _data_P5_overhead;
        _data_P5_overhead -= start;
        if( --block->semaphore == 0 ) {
            if( block->start_time != 0 ) {
                block->cycles -= block->start_time;
                block->cycles += startLessOverhead;
                block->start_time = 0;
            }
        }
        secondReturnAddress = findSecondReturn();
        callerBlock = *(info**)( secondReturnAddress + BLOCK_OFFSET );
        if( *(unsigned short*)secondReturnAddress == DIRECT_CALL_INDICATOR ) {
            // nothing more to do right here
        } else if( *(unsigned short*)secondReturnAddress == INDIRECT_CALL_INDICATOR ) {
            callerBlock = FindOrAllocInfo( callerBlock, block->callee );
        } else {
            callerBlock = 0;
        }
        if( callerBlock != 0 ) {
            if( --callerBlock->semaphore == 0 ) {
                if( callerBlock->start_time != 0 ) {
                    callerBlock->cycles += startLessOverhead;
                    callerBlock->cycles -= callerBlock->start_time;
                    callerBlock->start_time = 0;
                }
            }
        }
        _data_P5_overhead += rdtsc();
        EXIT_CRIT_SECTION();
    }
    pop_eax();
}

#if 0 // not currently in use
#ifdef __NT__
#define VXDNAME "P5PROF"
#define VXDEXT  ".VXD"
static int findVXD( char *buff )
{
    char fname[ _MAX_PATH2 ];
    char *drive,*dir;

    _splitpath2( _LpPgmName, &fname, &drive, &dir, NULL, NULL );
    _makepath( buff, drive, dir, VXDNAME, VXDEXT );
    if( access( buff, R_OK ) == 0 ) return( TRUE );

    strcpy( buff, ".\\" VXDNAME VXDEXT );
    if( access( buff, R_OK ) == 0 ) return( TRUE );

    GetSystemDirectory( buff, _MAX_PATH );
    strcat( buff, "\\" );
    strcat( buff, VXDNAME VXDEXT );
    if( access( buff, R_OK ) == 0 ) return( TRUE );

    GetWindowsDirectory( buff, _MAX_PATH );
    strcat( buff, "\\" );
    strcat( buff, VXDNAME VXDEXT );
    if( access( buff, R_OK ) == 0 ) return( TRUE );

    _searchenv( VXDNAME VXDEXT, "PATH", buff );
    if( buff[0] != '\0' ) return( TRUE );

    return( FALSE );
}
#endif


_WCRTLINK void __ProfInit( void )
{
#ifdef __NT__
    char buff[_MAX_PATH];
    if( __prof_old_longjmp_handler == NULL ) {
        __prof_old_longjmp_handler = __longjmp_handler;
        __longjmp_handler = __ProfUnwind;
        strcpy( buff, "\\\\.\\" );
        if( findVXD( buff + strlen( buff ) ) ) {
            hDevice = CreateFile("\\\\.\\c:\\dev\\prof\\VXD\\P5PROF.VXD", 0, 0, NULL, 0,
                FILE_FLAG_DELETE_ON_CLOSE, NULL);
            if (hDevice != INVALID_HANDLE_VALUE) {
                DWORD version;
                DWORD nret;
                int *pcounter = (int*)&_data_P5_overhead;

                DeviceIoControl(hDevice, 1, NULL, 0, &version, sizeof(version),
                    &nret, NULL);
                printf("P5PROF.VXD is version %d.%02d\n", HIBYTE(version),
                    LOBYTE(version));
                // nyi - virtualize 2 dwords!
                DeviceIoControl(hDevice, 2, &pcounter, sizeof(pcounter), NULL, 0,
                    NULL, NULL);
            }
        }
    }
#endif
}
#endif
