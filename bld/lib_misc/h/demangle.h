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
#include "dm_pts.h"

/*
    extra semantics:
        - output == NULL; proper size for a buffer will be returned
        - len == 0; input string is null terminated
        - realloc == NULL; the demangled name will be truncated
        - __demangle_l(); function argument types are incomplete
                        i.e. int foo( int(*)(float) ) -> int foo( int(*)() );
*/

#ifdef __LIB__
_WCRTLINK
#endif
size_t __demangle_t(                            // DEMANGLE A C++ TYPE
    char const *input,                          // - mangled C++ type
    size_t len,                                 // - length of mangled type
    char *output,                               // - for demangled C++ type
    size_t size )                               // - size of output buffer
;

typedef void (*outfunPtr)(void**,dm_pts,int,char const *);

#ifndef __LIB__

size_t __demangle_l(                            // DEMANGLE A C++ NAME
    char const *input,                          // - mangled C++ name
    size_t len,                                 // - length of mangled name
    char *output,                               // - for demangled C++ name
    size_t size )                               // - size of output buffer
;
size_t __demangle_r(                            // DEMANGLE A C++ NAME
    char const *input,                          // - mangled C++ name
    size_t len,                                 // - length of mangled name
    char **output,                              // - for demangled C++ name
    size_t size,                                // - size of output buffer
    char * (*realloc)( char *, size_t ) )       // - size adjuster for output
;
int __is_mangled(                               // IS NAME MANGLED ?
    char const *name,                           // - C++ name
    size_t len )                                // - length of name
;
int __is_mangled_internal(                      // IS NAME MANGLED? INTERNAL?
    char const *name,                           // - C++ name
    size_t len )                                // - length of name
;                                               // RETURNS one of following:
#define __NOT_MANGLED           0       // not mangled name
#define __MANGLED               1       // mangled user name
#define __MANGLED_INTERNAL      2       // mangled compiler generated name
#define __MANGLED_CTOR          3       // mangled ctor name
#define __MANGLED_DTOR          4       // mangled dtor name

int __unmangled_name(                           // FIND UNMANGLED BASE NAME
    char const *name,                           // - mangled C++ name
    size_t len,                                 // - length of mangled name
    char const **base,                          // - location of base name
    size_t *size )                              // - size of base name
;                                               // return TRUE if name mangled

int __scope_name(                               // EXTRACT A C++ SCOPE NAME
    char const *input,                          // - mangled C++ name
    size_t len,                                 // - length of mangled name
    unsigned index,                             // - scope wanted
    char const **scope,                         // - location of name
    size_t *size )                              // - size of output buffer
;                                               // returns TRUE on success

size_t __demangled_basename(                    // CREATE DEMANGLED BASE NAME
    char const *name,                           // - mangled C++ name
    size_t len,                                 // - length of mangled name
    char *output,                               // - for demangled C++ name
    size_t size )                               // - size of output buffer
;                                               // return len of output

#ifdef __DIP__

size_t __mangle_operator(                       // MANGLE OPERATOR NAME
    char const *op,                             // - operator token
    size_t len,                                 // - length of operator token
    char *result )                              // - operator name
;                                               // return len of operator name
                                                // ZERO if not found

void __parse_mangled_name(                      // PARSE MANGLED NAME
    char const *input,                          // - mangled C++ name
    size_t len,                                 // - length of mangled name
    void *cookie,                               // - data to carry around
    outfunPtr ofn )                             // - function to invoke
;                                               // returns TRUE on success

#endif //__DIP__

#endif //!__LIB__

#ifdef __cplusplus
};
#endif
#endif
