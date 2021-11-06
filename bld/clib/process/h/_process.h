/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Internal prototypes for process management.
*
****************************************************************************/


#include <malloc.h>


#ifdef __WIDECHAR__
#define ARGS_TYPE           const wchar_t *
#define ARGS_TYPE_ARR       const wchar_t * const *
#else
#define ARGS_TYPE           const char *
#define ARGS_TYPE_ARR       const char * const *
#endif

#if defined(__AXP__) || defined(__MIPS__)
#define ARGS_ARRAY_VA(ap)   (ARGS_TYPE_ARR)((ap).__base)
#else
#define ARGS_ARRAY_VA(ap)   (ARGS_TYPE_ARR)(ap)
#endif

#define ARGS_NEXT_VA(ap)    va_arg((ap), ARGS_TYPE)

typedef int (*execveaddr_type)( const char *__path, const char *const __argv[], const char *const __envp[] );

extern execveaddr_type __execaddr( void );
#ifdef __DOS__
extern execveaddr_type  __Exec_addr;
#endif
#ifdef __WIDECHAR__
extern int  __wcenvarg( const wchar_t* const *, const wchar_t* const *, wchar_t**, wchar_t**, unsigned*, size_t*, int );
extern void __wccmdline( wchar_t *, const wchar_t * const *, wchar_t *, int );
extern wchar_t *__wSlash_C( wchar_t *switch_c, unsigned char use_slash );
#else
extern int  __cenvarg( const char* const *, const char* const *, char**, char**, unsigned*, size_t*, int );
extern void __ccmdline( char *, const char * const *, char *, int );
extern char *__Slash_C( char *switch_c, unsigned char use_slash );
#endif
#if defined( __OS2__ ) || defined( __NT__ )
#ifdef __WIDECHAR__
extern int  _wdospawn( int, wchar_t *, wchar_t *, wchar_t *, const wchar_t * const * );
#else
extern int  _dospawn( int, char *, char *, char *, const char * const * );
#endif
#elif defined( __RDOS__ )
extern int  _dospawn( int, char *, char *, char *, const char * const * );
extern int  _doexec( char *, char *, char *, const char * const * );
#endif
