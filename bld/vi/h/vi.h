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
#include "bool.h"

#include "control.h"

#include "watcom.h"
#include "const.h"
#include "errs.h"
#include "lang.h"
#include "struct.h"
#include "mouse.h"
#include "globals.h"
#include "source.h"
#include "rtns.h"
#include "rtns2.h"
#include "dc.h"

#ifdef __WIN__
  #include "winvi.h"
#endif

#define SKIP_SPACES(s)      while( isspace( *s ) ) s++
#define SKIP_NOSPACES(s)    while( !isspace( *s ) && *s != '\0' ) s++
#define SKIP_DIGITS(s)      while( isdigit( *s ) ) s++
#define SKIP_SYMBOL(s)      while( isalpha( *s ) ) s++
#define SKIP_TOEND(s)       while( *s != '\0' ) s++
#define SKIP_CHAR_SPACES(s) while( isspace( *++s ) )

extern int      FileSysNeedsCR( int handle );

#endif
