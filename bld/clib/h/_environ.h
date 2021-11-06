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
* Description:  internal clib environment functions prototypes and macros
*
****************************************************************************/


#ifndef __ENVIRON_H_INCLUDED
#define __ENVIRON_H_INCLUDED

#include <ctype.h>

/* wide environment doesn't use alloc'd mask */
#ifdef __WIDECHAR__
#define ENVARR_SIZE(x)      ((x) * sizeof( wchar_t * ) + sizeof( wchar_t * ))
#define CHECK_WIDE_ENV()    if( _RWD_wenviron == NULL ) __create_wide_environment()
#define ENVP_TYPE_ARR       const wchar_t * const *
#else
#define ENVARR_SIZE(x)      ((x) * (sizeof( char * ) + sizeof( char )) + sizeof( char * ))
#define CHECK_WIDE_ENV()
#define ENVP_TYPE_ARR       const char * const *
#endif

#define ENVP_ARRAY_VA(ap)   (ENVP_TYPE_ARR)va_arg( ap, ENVP_TYPE_ARR )

/*
 * These routines are used internally only, so should not
 * have a _WCRTLINK modifier.
 */
extern int      __findenvadd( const char *name );
extern int      __findenvdel( const char *name );
extern int      __setenv( const char *name, const char *newvalue, int overwrite );
extern int      __putenv( const char *env_string );
#ifdef CLIB_USE_OTHER_ENV
extern void     __create_wide_environment( void );
extern int      __wfindenvadd( const wchar_t *name );
extern int      __wfindenvdel( const wchar_t *name );
extern int      __wsetenv( const wchar_t *name, const wchar_t *newvalue, int overwrite );
extern int      __wputenv( const wchar_t *env_string );
#endif

extern void     __setenvp( void );
#if !defined(__NETWARE__)
extern void     __freeenvp( void );
#endif

#ifdef CLIB_UPDATE_OS_ENV
#if defined( __NT__ )
extern int      __os_env_update_wide( const wchar_t *name, const wchar_t *value );
#endif
extern int      __os_env_update_narrow( const char *name, const char *value );
#endif

#endif
