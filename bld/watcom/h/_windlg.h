/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2017 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  16/32/64- bit Windows Dialog template structures and macros
*
****************************************************************************/


#define _AdjustUp( size, word ) ( ((size)+((word)-1)) & ~((word)-1) )

#if defined( __WINDOWS__ )
  #define ADJUST_DLGLEN( a )
#else
  #define ADJUST_DLGLEN( a )    (a) = _AdjustUp( a, 4 )
#endif

#define TEMPLATE_HANDLE     GLOBALHANDLE

#include "pushpck1.h"
typedef struct {
    DWORD   dtStyle;
#ifndef __WINDOWS__
    DWORD   dtExtendedStyle;
#endif
#ifdef __WINDOWS__
    BYTE    dtItemCount;
#else
    WORD    dtItemCount;
#endif
    short   dtX;
    short   dtY;
    short   dtCX;
    short   dtCY;
//  char    dtMenuName[];
//  char    dtClassName[];
//  char    dtCaptionText[];
} _DLGTEMPLATE;

typedef struct {
#ifndef __WINDOWS__
    DWORD   ditStyle;
    DWORD   ditExtendedStyle;
#endif
    short   ditX;
    short   ditY;
    short   ditCX;
    short   ditCY;
    WORD    ditID;
#ifdef __WINDOWS__
    DWORD   ditStyle;
#endif
//  char    ditClass[];
//  char    ditText[];
//  BYTE    ditInfo;
//  BYTE    ditData;
} _DLGITEMTEMPLATE;
#include "poppck.h"
