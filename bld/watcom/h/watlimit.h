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


#ifndef _WATLIMIT_H_INCLUDED_
#define _WATLIMIT_H_INCLUDED_

#include <limits.h>

#define UINT_8_MAX      UCHAR_MAX
#define UINT_16_MAX     USHRT_MAX
#if defined (LONG_IS_64BITS)
#define UINT_32_MAX     UINT_MAX
#else
#define UINT_32_MAX     ULONG_MAX
#endif

#define INT_8_MAX       SCHAR_MAX
#define INT_8_MIN       SCHAR_MIN
#define INT_16_MAX      SHRT_MAX
#define INT_16_MIN      SHRT_MIN
#if defined (LONG_IS_64BITS)
#define INT_32_MAX      INT_MAX
#define INT_32_MIX      INT_MIX
#else
#define INT_32_MAX      LONG_MAX
#define INT_32_MIN      LONG_MIN
#endif

#endif
