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


#ifndef DWUTILS_H_INCLUDED
#define DWUTILS_H_INCLUDED

#include "dwcnf.h"

/*
    These are client independant utility functions.
*/

/* max storage req'd in LEB128 form to store a 32 bit int/uint */
#define MAX_LEB128      5

#define LEB128                  DW_LEB128
#define ULEB128                 DW_ULEB128

extern  char *LEB128( char *__buf, dw_sconst value );
extern  char *ULEB128( char *__buf, dw_uconst value );

#if defined( M_I86 ) || defined( __386__ ) || defined(__AXP__)
#define WriteU16( __p, __v )    ( *(uint_16 *)(__p) = (__v) )
#define WriteU32( __p, __v )    ( *(uint_32 *)(__p) = (__v) )
#define WriteS16( __p, __v )    ( *(uint_16 *)(__p) = (__v) )
#define WriteS32( __p, __v )    ( *(uint_32 *)(__p) = (__v) )
#else
#error "must define WriteU16/32 and WriteS16/32 functions"
#endif

#endif
