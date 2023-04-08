/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Top level WOMP include file.
*
****************************************************************************/


#ifndef WOMP_H
#define WOMP_H    1
#include "watcom.h"

#define _WOMP_READ              1       /* code to handle reading objs */
#define _WOMP_WRITE             2       /* code to handle writing objs */
#define _WOMP_EXTRAS            4       /* code for exotic record types */
#define _WOMP_WATFOR            8       /* code specific to WATFOR */
#define _WOMP_WASM              16      /* code specific to WASM */
#define _WOMP_WOMP              32      /* code specific to WOMP */
#define _WOMP_NASM              64      /* code specific to (new) WASM */

/*
   use < > to include wompopt.h so that we don't always get womp version of
   wompopt.h
*/
#include <wompopt.h>

#define STATIC static
#define FORWARD

#define STDIN   0
#define STDOUT  1
#define STDERR  2

#if ( defined( _M_I86SM ) || defined( _M_I86MM ) ) && defined( _M_I86 )
  #error This code can only be compiled for a large data model
#endif

#endif

