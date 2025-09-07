/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2025      The Open Watcom Contributors. All Rights Reserved.
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
* Description:  These are macros that define the 'real' functions that
*               the library memory allocators use.
*
****************************************************************************/


#ifndef _LIBALLOC_H_INCLUDED
#define _LIBALLOC_H_INCLUDED

#ifdef __WATCOMC__
    #include <malloc.h> // Non-standard interfaces - _nfree() etc.
#else
    #include <stdlib.h>
#endif
#ifdef __NETWARE__
    #include "nwlibmem.h"
#endif


#ifdef __NETWARE__
    /*
     * NetWare uses Alloc and Free because the heap will not have
     * been initialised at _Prelude time...
     */
    #define lib_malloc( x )         _NW_malloc( x )
    #define lib_free( x )           _NW_free( x )
  #if defined (_NETWARE_CLIB)
    #define lib_realloc( x, y, z)   _NW_realloc( x, y, z )
  #else
    #define lib_realloc( x, y)      _NW_realloc( x, y)
  #endif
    #define lib_calloc( x, y )      _NW_calloc( x, y )
    #define lib_strdup( x )         __clib_strdup( x )
#elif defined(__RDOSDEV__)
    #define lib_malloc( x )         malloc( x )
    #define lib_free( x )           free( x )
    #define lib_realloc( x, y )     realloc( x, y )

    #define lib_nmalloc( x )        malloc( x )
    #define lib_nfree( x )          free( x )
    #define lib_nrealloc( x, y )    realloc( x, y )

    #define lib_fmalloc( x )        malloc( x )
    #define lib_ffree( x )          free( x )
    #define lib_frealloc( x, y )    realloc( x, y )

    #define lib_calloc( x, y )      calloc( x, y )
    #define lib_strdup( x )         __clib_strdup( x )
#else
    #define lib_malloc( x )         malloc( x )
    #define lib_free( x )           free( x )
    #define lib_realloc( x, y )     realloc( x, y )

    #define lib_nmalloc( x )        _nmalloc( x )
    #define lib_nfree( x )          _nfree( x )
    #define lib_nrealloc( x, y )    _nrealloc( x, y )

    #define lib_fmalloc( x )        _fmalloc( x )
    #define lib_ffree( x )          _ffree( x )
    #define lib_frealloc( x, y )    _frealloc( x, y )

    #define lib_calloc( x, y )      calloc( x, y )
    #define lib_strdup( x )         __clib_strdup( x )
    #define lib_wcsdup( x )         __clib_wcsdup( x )
#endif

#endif
