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


#ifndef __CPPLIB_H__
#define __CPPLIB_H__

#ifndef __cplusplus
    #error CPPLIB.H only compiles under C++
#endif

// CPPLIB.H -- definitions for C++ library
//
// 91/11/07 -- J.W.Welch        -- defined
// 92/10/07 -- J.W.Welch        -- major re-vamp for exceptions
// 92/10/19 -- Greg Bentz       -- Add RT_FAR in case of HUGE model.
// 92/10/30 -- J.W.Welch        -- add definitions for exception handling
// 93/04/02 -- J.W.Welch        -- adapt to new BLK_TYPE_...
// 93/12/07 -- Greg Bentz       -- convert to 10.0 thread data
// 94/10/28 -- J.W.Welch        -- support for fs registration
// 95/05/24 -- J.W.Welch        -- RTCTL conversion
// 95/06/14 -- Greg Bentz       -- DLL linkage

#include <variety.h>
#ifdef _WPRTLINK_RESIDENT
  #undef _WPRTLINK
  #define _WPRTLINK
#endif

struct  ACTIVE_EXC;
struct  DISPATCH_EXC;
union   RW_DTREG;
union   RO_DTREG;
struct  THREAD_CTL;
struct  _EXC_PR;

#if defined( M_I386 ) || defined( M_I86 )
    #define M_PC_INTEL 1
#endif


#include <stddef.h>

#define PointUsingOffset( type, base, offset ) \
    ( (type*)( (char*)base + offset ) )
#define PointOffset( base, offset ) \
    PointUsingOffset( void, base, offset )

// this is necessary to avoid unnecessary overhead on pointer arithmetic
#ifdef __HUGE__
#define RT_FAR __far
typedef unsigned    RT_STATE_VAR;       // State variable
typedef unsigned    offset_t;           // offset
#else
#define RT_FAR
typedef size_t      RT_STATE_VAR;       // State variable
typedef size_t      offset_t;           // offset
#endif

// TYPEDEFS : c vs c++ language insensitive typedefs

typedef void (*pFUNVOIDVOID)                    // function returns void
                ( void );                       // - takes void
typedef int  (*pFUNINTUNSIGNED)                 // function returns int
                ( unsigned );                   // - takes unsigned
typedef void (*pFUNpVOIDVOID)                   // function returns void
                ( void * );                     // - takes void *
typedef void *(*pFUNdtor)                       // dtor
                ( void *                        // - address
                , unsigned );                   // - dtor vbase flag
typedef void (*pFUNctor)                        // ctor
                ( void * );                     // - address
typedef void (*pFUNctorV)                       // ctor
                ( void *                        // - address
                , unsigned );                   // - ctor vbase flag
typedef void (*pFUNcopy)                        // copy ctor
                ( void *                        // - target
                , void * );                     // - source
typedef void (*pFUNcopyV)                       // copy ctor
                ( void *                        // - target
                , unsigned                      // - ctor vbase flag
                , void * );                     // - source

// twits at Microsoft define "boolean" in their Win32 header files
typedef int rboolean;

// set up indication of -xs

#if defined( __SW_XS ) || defined( __SW_XSS ) || defined( __SW_XST )
#define RT_EXC_ENABLED
#endif


extern "C" {

#if defined(__MT__)
#include <lock.h>
#endif

#include "wcpp.h"

#pragma warning 604 9
#pragma warning 594 9

#include "rtdata.h"

#pragma warning 604 1
#pragma warning 594 1

#if !defined(__PENPOINT__) && defined(__MT__)
    #pragma warning 604 9
    #pragma warning 594 9
    #include "thread.h"
    #pragma warning 604 1
    #pragma warning 594 1
#endif

#if defined( __USE_FS ) || defined( __USE_RW ) || defined( __USE_PD )
    #include "fsreg.h"
#endif


#if defined( M_ALPHA ) || defined( M_I386 )
    // pad to 4-byte boundary
    #define AlignPad1 char padding[3];
    #define AlignPad2 char padding[2];
#elif defined( M_I86 )
    // pad to 2-byte boundary
    #define AlignPad1 char padding[1];
    #define AlignPad2 ;
#else
    #error bad target
#endif


