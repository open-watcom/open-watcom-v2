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
* Description:  Internal prototypes for process management.
*
****************************************************************************/


#include <malloc.h>

typedef int (*execveaddr_type)( const char *__path, const char *const __argv[],
                            const char *const __envp[] );

extern execveaddr_type __execaddr( void );
#ifdef __WIDECHAR__
extern int  __wcenvarg( const wchar_t* const *, const wchar_t* const *, wchar_t**, wchar_t**, unsigned*, size_t*, int );
extern void __wccmdline( wchar_t *, const wchar_t * const *, wchar_t *, int );
extern int  _wdospawn( int, wchar_t *, wchar_t *, wchar_t *, const wchar_t * const * );
#else
extern int  __cenvarg( const char* const *, const char* const *, char**, char**, unsigned*, size_t*, int );
extern void __ccmdline( char *, const char * const *, char *, int );
#if defined( _M_I86 ) && defined( __DOS__ )
extern int  _dospawn(int,char _WCI86NEAR *,char _WCI86NEAR *,unsigned, const char * const *);
#else
extern int  _dospawn(int,char *,char *, char *, const char * const *);
#endif
#if defined( __DOS__ )
#pragma aux _dospawn parm caller [];
#endif
#endif
#ifdef __RDOS__
extern int  _doexec(char *,char *, const char * const *);
#endif
extern void __init_execve( void );
