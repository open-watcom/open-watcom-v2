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
* Description:  Standard linker configuration.
*
****************************************************************************/


#include <stddef.h>
#include <stdarg.h>
#include <time.h>
#include "watcom.h"
#include "bool.h"
#include "iopath.h"

#define NLCHAR         '\n'
#define CTRLZ          '\32'

/* round up by a power of 2 */
#define ROUND_UP( x, r )        (((x)+((r)-1))&~((r)-1))
#define ROUND_SHIFT( x, r )     ROUND_UP( x, 1 << (r) )

#define MAKE_EVEN( x ) ROUND_UP( x, 2 )
#define MAKE_PARA( x ) ROUND_UP( x, 16 )

// useful for stringizing tokens
#define __str( m )      #m
#define __xstr( l )     __str( l )

#ifndef NDEBUG
    #define __location " (" __FILE__ "," __xstr(__LINE__) ")"
    #define DbgVerify( cond, msg ) if( ! ( cond ) ) LnkFatal( msg __location );
    #define DbgDefault( msg ) default: LnkFatal( msg __location )
    #define DbgAssert( cond )   if( !(cond) ) LnkFatal( #cond __location );
#else
    #define DbgVerify( cond, msg )
    #define DbgDefault( msg )
    #define DbgAssert( cond )
#endif

#if defined( __UNIX__ )

    #define CASE_SENSITIVE

#else

//    #define CASE_SENSITIVE

#endif

#ifdef CASE_SENSITIVE
    #define FNAMECMPSTR      strcmp      /* for case  sensitive file systems */
#else
    #define FNAMECMPSTR      stricmp     /* for case insensitive file systems */
#endif

typedef unsigned char   byte;
typedef unsigned        f_handle;

#if defined( USE_VIRTMEM )
typedef unsigned_32     virt_mem;
#else
typedef char            *virt_mem;
#endif
typedef unsigned_32     virt_mem_size;

typedef union {
    virt_mem        vm_ptr;
    virt_mem_size   vm_offs;
} virt_mem_ptr;

#if defined( __QNX__ )
    #define _EXE       0
    #define _QNXLOAD   1
    #define _OS2       2
    #define _PHARLAP   3
    #define _NOVELL    4
    #define _DOS16M    5
//    #define _ELF       6
#else
    #define _EXE       0
    #define _QNXLOAD   1
    #define _OS2       2
    #define _PHARLAP   3
    #define _NOVELL    4
    #define _DOS16M    5
    #define _ELF       6
    #define _ZDOS      7
    #define _RAW       8
    #define _RDOS      9
 #endif
