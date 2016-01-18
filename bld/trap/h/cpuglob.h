/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2016 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Global CPU segment and breakpoint related definition.
*
****************************************************************************/


#if defined( _M_IX86 )

#define TRACE_BIT       0x100
#define BRKPOINT        0xCC

extern unsigned GetDS( void );
extern unsigned GetCS( void );
extern unsigned GetSS( void );
#if defined( _M_I86 )
#pragma aux GetDS = "mov ax,ds" value [ax];
#pragma aux GetCS = "mov ax,cs" value [ax];
#pragma aux GetSS = "mov ax,ss" value [ax];
#else
#pragma aux GetDS = "mov eax,ds" value [eax];
#pragma aux GetCS = "mov eax,cs" value [eax];
#pragma aux GetSS = "mov eax,ss" value [eax];
#endif

extern void BreakPoint( void );
#pragma aux BreakPoint = BRKPOINT;

extern void BreakPointParm( unsigned long );
#if defined( _M_I86 )
#pragma aux BreakPointParm = BRKPOINT parm [dx ax] aborts;
#else
#pragma aux BreakPointParm = BRKPOINT parm [eax] aborts;
#endif

typedef unsigned char   opcode_type;

#elif defined( _M_X64 )

#define TRACE_BIT       0x100
#define BRKPOINT        0xCC

#define GetDS()         0
#define GetCS()         0
#define GetSS()         0

extern void BreakPoint( void );

typedef unsigned char   opcode_type;

#elif defined( __PPC__ )

#define TRACE_BIT       (1U << MSR_L_se)
#define BRKPOINT        0x7FE00008

#define GetDS()         0
#define GetCS()         0

typedef unsigned        opcode_type;

#elif defined( __MIPS__ )

//#define TRACE_BIT      0
#define BRKPOINT        0x0000000D

#define GetDS()         0
#define GetCS()         0

typedef unsigned        opcode_type;

#elif defined( __AXP__ )

//#define TRACE_BIT      0
#define BRKPOINT        0x00000080

#define GetDS()         0
#define GetCS()         0

typedef unsigned        opcode_type;

#else

    #error globcpu.h not configured for machine

#endif
