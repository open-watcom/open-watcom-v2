/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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

#define USE_INT64

#ifdef __INT64__
    #define INT_TYPE            __int64
    #define UINT_TYPE           unsigned __int64
    #define LONG_TYPE           __int64
    #define ULONG_TYPE          unsigned __int64
    #define __I64NAME(_n1,_n2)  _n2

    #define REAL_INT_TYPE           __int64

    #define GET_INT64(__d)          ( __d )
    #define GET_UINT64(__d)         ( __d )
    #define GET_REALINT64(__i)      ( __i )
    #define GET_REALUINT64(__i)     ( __i )

    #define HIGHWORD(__w,__i)       (__w) = (unsigned)( (__i) >> 32 )
    #define LOWWORD(__w,__i)        (__w) = (unsigned)(__i)
    #define MAKE_INT64(__i,__h,__l) (__i) = ( ((INT_TYPE)(__h)) << 32 )  |  (__l)

    #define RETURN_INT64(__i)       return( __i )

#else
    #define INT_TYPE            int
    #define UINT_TYPE           unsigned int
    #define LONG_TYPE           long
    #define ULONG_TYPE          unsigned long
    #define __I64NAME(_n1,_n2)  _n1
#endif

#define INTSIZE                 ( sizeof( INT_TYPE ) )

#define MAKE_SIZE64(__hi,__lo)    ((((__int64)(__hi)) << 32 ) | (unsigned long)(__lo))

#endif
