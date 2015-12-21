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
* Description:  internal clib environment functions prototypes and macros
*
****************************************************************************/


#ifndef __ENVIRON_H_INCLUDED
#define __ENVIRON_H_INCLUDED

#include <ctype.h>

#if defined( __WIDECHAR__ ) || defined( __UNIX__ ) || defined( __RDOS__ ) || defined( __RDOSDEV__ ) || defined( __NETWARE__ )
// single-byte or wide-char
#define _TCSDEC(__p)        (__p - 1)
#define _TCSINC(__p)        (__p + 1)
#define _TCSCMP(__p1,__p2)  ((*(__p1))-(*(__p2)))
#define _TCSICMP(__p1,__p2) (__F_NAME(toupper,towupper)(*(__p1))-__F_NAME(toupper,towupper)(*(__p2)))
#define _TCSTERM(__p)       (*(__p)==NULLCHAR)
#define _TCSNEXTC(__p)      (*(__p))
#define _TCSCHR(__p,__c)    __F_NAME(strchr,wcschr)(__p,__c)
#define _TCSLEN(__p)        __F_NAME(strlen,wcslen)(__p)
#else
// multi-byte
#define _TCSDEC(__p)        _mbsdec(__p)
#define _TCSINC(__p)        (char *)_mbsinc((unsigned char *)__p)
#define _TCSCMP(__p1,__p2)  _mbccmp(__p1,__p2)
#define _TCSICMP(__p1,__p2) (_mbctoupper(_mbsnextc((unsigned char *)__p1))-_mbctoupper(_mbsnextc((unsigned char *)__p2)))
#define _TCSTERM(__p)       _mbterm((unsigned char *)__p)
#define _TCSNEXTC(__p)      _mbsnextc((unsigned char *)__p)
#define _TCSCHR(__p,__c)    (char *)_mbschr((unsigned char *)__p,__c)
#define _TCSLEN(__p)        _mbslen((unsigned char *)__p)
#endif

/*
 * These routines are used internally only, so should not
 * have a _WCRTLINK modifier.
 */
extern void     __create_wide_environment( void );
extern int      __findenv( const char *name, int delete_var );
extern int      __wfindenv( const wchar_t *name, int delete_var );
extern int      __putenv( const char *env_string );
extern int      __wputenv( const wchar_t *env_string );
extern int      __setenv( const char *name, const char *newvalue, int overwrite );
extern int      __wsetenv( const wchar_t *name, const wchar_t *newvalue, int overwrite );

extern void     __setenvp( void );
#if !defined(__NETWARE__)
extern void     __freeenvp( void );
#endif

#endif
