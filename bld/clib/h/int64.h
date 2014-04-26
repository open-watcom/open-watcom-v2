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


#ifndef _INT64_H_INCLUDED
#define _INT64_H_INCLUDED

#ifdef __WATCOM_INT64__
    #define USE_INT64
#endif
#include "clibi64.h"


#ifdef __INT64__
    #define INT_TYPE            INT64_TYPE
    #define UINT_TYPE           UINT64_TYPE
    #define LONG_TYPE           INT64_TYPE
    #define ULONG_TYPE          UINT64_TYPE
    #define __I64NAME(_n1,_n2)  _n2

    #if !defined(USE_INT64)
        #define __int64                 double
        #define REAL_INT_TYPE           double

        #define GET_INT64(__d)          ( *( (INT_TYPE*)(&(__d)) ) )
        #define GET_UINT64(__d)         ( *( (UINT_TYPE*)(&(__d)) ) )
        #define GET_REALINT64(__i)      ( *( (double*)(&(__i)) ) )
        #define GET_REALUINT64(__i)     ( *( (double*)(&(__i)) ) )

        #define HIGHWORD(__w,__i)       (__w) = (unsigned)(__i).u._32[I64HI32]
        #define LOWWORD(__w,__i)        (__w) = (unsigned)(__i).u._32[I64LO32]
        #define MAKE_INT64(__i,__h,__l) {                               \
                                          (__i).u._32[I64HI32] = (__h); \
                                          (__i).u._32[I64LO32] = (__l); \
                                        }

        #define RETURN_INT64(__i)       return( GET_REALINT64(__i) )
    #else
        #define REAL_INT_TYPE           __int64

        #define GET_INT64(__d)          ( __d )
        #define GET_UINT64(__d)         ( __d )
        #define GET_REALINT64(__i)      ( __i )
        #define GET_REALUINT64(__i)     ( __i )

        #define HIGHWORD(__w,__i)       (__w) = (unsigned)( (__i) >> 32 )
        #define LOWWORD(__w,__i)        (__w) = (unsigned)(__i)
        #define MAKE_INT64(__i,__h,__l) (__i) = ( ((INT_TYPE)(__h)) << 32 )  |  (__l)

        #define RETURN_INT64(__i)       return( __i )
    #endif

#else
    #define INT_TYPE            int
    #define UINT_TYPE           unsigned int
    #define LONG_TYPE           long
    #define ULONG_TYPE          unsigned long
    #define __I64NAME(_n1,_n2)  _n1
#endif

#define INTSIZE                 ( sizeof( INT_TYPE ) )


#endif
