/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2022 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Global CPU segment related definitions.
*
****************************************************************************/


#if defined( _M_IX86 )

extern unsigned GetDS( void );
extern unsigned GetCS( void );
extern unsigned GetSS( void );
#if defined( _M_I86 )
#pragma aux GetDS = "mov ax,ds" __value [__ax]
#pragma aux GetCS = "mov ax,cs" __value [__ax]
#pragma aux GetSS = "mov ax,ss" __value [__ax]
#else
#pragma aux GetDS = "mov eax,ds" __value [__eax]
#pragma aux GetCS = "mov eax,cs" __value [__eax]
#pragma aux GetSS = "mov eax,ss" __value [__eax]
#endif

#elif defined( _M_X64 )

#define GetDS()         0
#define GetCS()         0
#define GetSS()         0

#elif defined( __PPC__ )

#define GetDS()         0
#define GetCS()         0
#define GetSS()         0

#elif defined( __MIPS__ )

#define GetDS()         0
#define GetCS()         0
#define GetSS()         0

#elif defined( __AXP__ )

#define GetDS()         0
#define GetCS()         0
#define GetSS()         0

#else

    #error globcpu.h not configured for machine

#endif
