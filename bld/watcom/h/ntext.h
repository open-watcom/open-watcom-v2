/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  CLIB internal NT extension function prototypes.
*
****************************************************************************/


#ifndef __NTEXT_INCLUDED__
#define __NTEXT_INCLUDED__

#include "timetwnt.h"
#include "dosftwnt.h"
#include "ntattrib.h"

extern void     __GetNTCreateAttr( unsigned dos_attrib, LPDWORD desired_access, LPDWORD nt_attrib );
extern void     __GetNTAccessAttr( unsigned rwmode, LPDWORD desired_access, LPDWORD nt_attrib );
extern void     __GetNTShareAttr( unsigned share, LPDWORD share_mode );

extern BOOL     __NTFindNextFileWithAttrA( HANDLE h, unsigned nt_attribs, LPWIN32_FIND_DATAA ffd );
#ifdef __WATCOMC__
extern BOOL     __NTFindNextFileWithAttrW( HANDLE h, unsigned nt_attribs, LPWIN32_FIND_DATAW ffd );
#endif

#ifdef __WIDECHAR__
#define __NTFindNextFileWithAttr    __NTFindNextFileWithAttrW
#else
#define __NTFindNextFileWithAttr    __NTFindNextFileWithAttrA
#endif

#endif
