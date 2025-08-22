/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  DPMI function wrappers for Win386.
*
****************************************************************************/


#include "bool.h"


#ifdef __WINDOWS__
  #ifdef _M_I86
    #define WINFAR16        __far
    #define WINDPMIFN(x)    __pascal __far __ ## x
  #else
    #define WINFAR16
    #define WINDPMIFN(x)    __pascal x
  #endif

extern DWORD    WINDPMIFN( WDPMIAlloc )( DWORD size );
extern bool     WINDPMIFN( WDPMIFree )( DWORD ptr );
extern bool     WINDPMIFN( WDPMIGetAlias )( DWORD offs32, DWORD WINFAR16 *palias );
extern void     WINDPMIFN( WDPMIFreeAlias )( DWORD alias );
extern bool     WINDPMIFN( WDPMIGetHugeAlias )( DWORD offs32, DWORD WINFAR16 *palias, DWORD size );
extern void     WINDPMIFN( WDPMIFreeHugeAlias )( DWORD alias, DWORD size );
extern DWORD    WINDPMIFN( WDPMIAliasToFlat )( DWORD alias );
// not implemented functions
extern bool     WINDPMIFN( WDPMIResizeDS )( DWORD size );

#endif
