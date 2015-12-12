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


#ifndef _DEMANGLE_H
#define _DEMANGLE_H

#ifdef __cplusplus
extern "C" {
#endif
#include <stddef.h>
#if defined( __WATCOMC__ )
#include <_comdef.h>
#endif
#include "dm_pts.h"

/*
    extra semantics:
        - buff == NULL; proper size for a buffer will be returned
        - len == 0; input string is null terminated
        - realloc == NULL; the demangled name will be truncated
        - __demangle_l(); function argument types are incomplete
                        i.e. int foo( int(*)(float) ) -> int foo( int(*)() );
*/

size_t __demangle_l(                            // DEMANGLE A C++ NAME
    char const *input,                          // - mangled C++ name
    size_t len,                                 // - length of mangled name
    char *buff,                                 // - output buffer for demangled C++ name
    size_t buff_size )                          // - size of output buffer
;

int __is_mangled(                               // IS NAME MANGLED ?
    char const *name,                           // - C++ name
    size_t len )                                // - length of name
;

#if !defined( __WLIB__ ) && !defined( __DISASM__ )

typedef enum {
    __NOT_MANGLED,                              // not mangled name
    __MANGLED,                                  // mangled user name
    __MANGLED_INTERNAL,                         // mangled compiler generated name
    __MANGLED_CTOR,                             // mangled ctor name
    __MANGLED_DTOR,                             // mangled dtor name
} mangled_type;

mangled_type __is_mangled_internal(             // IS NAME MANGLED? INTERNAL?
    char const *name,                           // - C++ name
    size_t len )                                // - length of name
;                                               // RETURNS one of following:

int __unmangled_name(                           // FIND UNMANGLED BASE NAME
    char const *name,                           // - mangled C++ name
    size_t len,                                 // - length of mangled name
    char const **basep,                         // - location of C++ base name
    size_t *base_sizep )                        // - size of C++ base name
;                                               // return TRUE if name mangled

#if !defined( __WLINK__ )

#if !defined( __DIP__ )

#if defined( __WATCOMC__ )
_WCRTLINK
#endif
size_t __demangle_t(                            // DEMANGLE A C++ TYPE
    char const *input,                          // - mangled C++ type
    size_t len,                                 // - length of mangled type
    char *buff,                                 // - output buffer for demangled C++ type
    size_t buff_size )                          // - size of output buffer
;

size_t __demangle_r(                            // DEMANGLE A C++ NAME
    char const *input,                          // - mangled C++ name
    size_t len,                                 // - length of mangled name
    char **buffp,                               // - output buffer for demangled C++ name
    size_t buff_size,                           // - size of output buffer
    void *(*realloc)( void *, size_t ) )        // - size adjuster for output
;

#endif // !__DIP__

int __scope_name(                               // EXTRACT A C++ SCOPE NAME
    char const *input,                          // - mangled C++ name
    size_t len,                                 // - length of mangled name
    unsigned index,                             // - scope wanted
    char const **scopep,                        // - location of C++ scope name
    size_t *scope_sizep )                       // - size of C++ scope name
;                                               // returns TRUE on success

size_t __demangled_basename(                    // CREATE DEMANGLED BASE NAME
    char const *name,                           // - mangled C++ name
    size_t len,                                 // - length of mangled name
    char *buff,                                 // - output buffer for demangled C++ base name
    size_t buff_size )                          // - size of output buffer
;                                               // return len of demangled C++ base name

size_t __mangle_operator(                       // MANGLE OPERATOR NAME
    char const *op,                             // - operator token
    size_t len,                                 // - length of operator token
    char *buff )                                // - output buffer for mangled operator name
;                                               // return len of mangled operator name
                                                // ZERO if not found

#endif // !__WLINK__

#endif // !__WLIB__ && !__DISASM__

#ifdef __cplusplus
};
#endif

#endif
