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


#ifndef __VI_INCLUDED__
#define __VI_INCLUDED__
#include "control.h"
#ifndef _FAR
#define _FAR
#endif
typedef unsigned int U_INT;

char *_inline_strchr( const char *__s, int __c );
unsigned int _inline_strlen( const char *__s );
int _inline_strcmp( const char *__s1, const char *__s2 );
char *_inline_strcat( char *__s1, const char *__s2 );
void *_inline_memcpy( void *__s1, const void *__s2, unsigned int __n );
void *_inline_memset( void *__s, int __c, unsigned int __n );

#ifdef __QNX__
    #define FSYS_CASE_SENSITIVE         1
    extern FileSysNeedsCR( int handle );
#else
    #define FSYS_CASE_SENSITIVE         0
    #define FileSysNeedsCR( x )         1
#endif

#if defined( __386__ ) || defined( __AXP__ )
    #define _NEAR
#else
    #define _NEAR near
#endif

#ifndef __AXP__
#define VI_RCS  1
#endif

#include "const.h"
#include "struct.h"
#include "pragmas.h"
#include "errs.h"
#include "globals.h"
#include "rtns.h"
#include "rtns2.h"
#include "mouse.h"
#include "dc.h"
#endif
