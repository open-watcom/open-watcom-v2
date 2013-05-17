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
* Description:  Editor top level include file.
*
****************************************************************************/


#ifndef __VI_INCLUDED__
#define __VI_INCLUDED__

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#if defined( __NT__ )
  #define _WINSOCKAPI_
  #include <windows.h>
#endif

#include "control.h"

#include "watcom.h"
#include "const.h"
#include "errs.h"
#include "lang.h"
#include "struct.h"
#include "mouse.h"
#include "globals.h"
#include "rtns.h"
#include "rtns2.h"
#include "dc.h"

#ifdef __WIN__
  #include "winvi.h"
#endif

typedef unsigned int U_INT;

#if defined( __WATCOMC__ ) && !defined( __ALPHA__ )
char            *_inline_strchr( const char *__s, int __c );
unsigned int    _inline_strlen( const char *__s );
int             _inline_strcmp( const char *__s1, const char *__s2 );
char            *_inline_strcat( char *__s1, const char *__s2 );
void            *_inline_memcpy( void *__s1, const void *__s2, unsigned int __n );
void            *_inline_memset( void *__s, int __c, unsigned int __n );
#endif

extern int      FileSysNeedsCR( int handle );

#endif
