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

#include "variety.h"
#include "widechar.h"
#ifdef __LONG_LONG_SUPPORT__
  #include "clibi64.h"
#endif

/* Only support near/far pointers on 16-bit targets, extended DOS
 * and Win386. Ideally we might want to test for non-flat memory model.
 */
#if defined( _M_I86 ) || defined( __DOS__ ) || defined( __WINDOWS__ )
  #define __FAR_SUPPORT__
  typedef CHAR_TYPE     _WCFAR *FAR_STRING;
  typedef char          _WCFAR *FAR_ASCII_STRING;
  typedef wchar_t       _WCFAR *FAR_UNI_STRING;
  typedef int           _WCFAR *FAR_INT;
  typedef signed char   _WCFAR *FAR_CHAR;
  typedef short         _WCFAR *FAR_SHORT;
  typedef long          _WCFAR *FAR_LONG;
  typedef float         _WCFAR *FAR_FLOAT;
  typedef double        _WCFAR *FAR_DOUBLE;
  #ifdef __LONG_LONG_SUPPORT__
    typedef UINT64_TYPE _WCFAR *FAR_INT64;
  #endif
#else
  #undef __FAR_SUPPORT__
  typedef CHAR_TYPE     *FAR_STRING;
  typedef char          *FAR_ASCII_STRING;
  typedef wchar_t       *FAR_UNI_STRING;
  typedef int           *FAR_INT;
  typedef signed char   *FAR_CHAR;
  typedef short         *FAR_SHORT;
  typedef long          *FAR_LONG;
  typedef float         *FAR_FLOAT;
  typedef double        *FAR_DOUBLE;
  #ifdef __LONG_LONG_SUPPORT__
    typedef UINT64_TYPE *FAR_INT64;
  #endif
#endif

#endif
