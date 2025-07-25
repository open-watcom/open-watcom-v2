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
* Description:  Prototypes for direct 'calls' to inline functions.
*
****************************************************************************/


#ifndef _XSTRING_H_INCLUDED
#define _XSTRING_H_INCLUDED

#include <string.h>

#define CHAR2INT(c)     ((unsigned char)(c))

#ifdef __cplusplus
extern "C" {
#endif

#ifdef  __INLINE_FUNCTIONS__
extern void     *_inline_memchr( const void *__s, int __c, size_t __n );
extern int      _inline_memcmp( const void *__s1, const void *__s2, size_t __n );
extern void     *_inline_memcpy( void *__s1, const void *__s2, size_t __n );
extern void     *_inline_memset( void *__s, int __c, size_t __n );
extern char     *_inline_strcat( char *__s1, const char *__s2 );
extern char     *_inline_strchr( const char *__s, int __c );
extern int      _inline_strcmp( const char *__s1, const char *__s2 );
extern char     *_inline_strcpy( char *__s1, const char *__s2 );
extern size_t   _inline_strlen( const char *__s );
extern void     _WCFAR *_inline__fmemchr( const void _WCFAR *__s, int __c, size_t __n );
extern void     _WCFAR *_inline__fmemcpy( void _WCFAR *__s1, const void _WCFAR *__s2, size_t __n );
extern void     _WCFAR *_inline__fmemset( void _WCFAR *__s, int __c, size_t __n );
extern int      _inline__fmemcmp( const void _WCFAR *__s1, const void _WCFAR *__s2, size_t __n );
extern char     _WCFAR *_inline__fstrcat( char _WCFAR *__s1, const char _WCFAR *__s2 );
extern char     _WCFAR *_inline__fstrchr( const char _WCFAR *__s, int __c );
extern int      _inline__fstrcmp( const char _WCFAR *__s1, const char _WCFAR *__s2 );
extern char     _WCFAR *_inline__fstrcpy( char _WCFAR *__s1, const char _WCFAR *__s2 );
extern size_t   _inline__fstrlen( const char _WCFAR *__s );
extern void     _inline_movedata( unsigned __srcseg, unsigned __srcoff, unsigned __tgtseg, unsigned __tgtoff, unsigned __len );
#endif  /* __INLINE_FUNCTIONS__ */

#ifdef __cplusplus
};
#endif
#endif
