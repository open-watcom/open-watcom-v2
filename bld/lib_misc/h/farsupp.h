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
* Description:  Far pointer support typedefs.
*
****************************************************************************/


#ifndef _FARSUPP_H_INCLUDED
#define _FARSUPP_H_INCLUDED

#ifdef __LONG_LONG_SUPPORT__
  #include "clibi64.h"
#endif

/* Only support near/far pointers on 16-bit targets, extended DOS
 * and Win386. Ideally we might want to test for non-flat memory model.
 */
#if defined( _M_I86 ) || defined( __DOS__ ) || defined( __WINDOWS__ )
  #define __FAR_SUPPORT__
  #define _FAR_SUPPORT_     _WCFAR
#else
  #undef  __FAR_SUPPORT__
  #define _FAR_SUPPORT_
#endif

#ifdef __WIDECHAR__
  typedef wchar_t       _FAR_SUPPORT_   *FAR_STRING;
#else
  typedef char          _FAR_SUPPORT_   *FAR_STRING;
#endif
  typedef char          _FAR_SUPPORT_   *FAR_ASCII_STRING;
  typedef wchar_t       _FAR_SUPPORT_   *FAR_WIDE_STRING;
  typedef int           _FAR_SUPPORT_   *FAR_INT;
  typedef signed char   _FAR_SUPPORT_   *FAR_CHAR;
  typedef short         _FAR_SUPPORT_   *FAR_SHORT;
  typedef long          _FAR_SUPPORT_   *FAR_LONG;
  typedef float         _FAR_SUPPORT_   *FAR_FLOAT;
  typedef double        _FAR_SUPPORT_   *FAR_DOUBLE;
#ifdef __LONG_LONG_SUPPORT__
  typedef UINT64_TYPE   _FAR_SUPPORT_   *FAR_INT64;
#endif

#endif
