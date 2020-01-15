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


#ifndef __NTINFO_INCLUDED__
#define __NTINFO_INCLUDED__

extern void     __GetNTDirInfoA( struct dirent *dirp, LPWIN32_FIND_DATAA ffd );
extern void     __GetNTFindInfoA( struct find_t *findt, LPWIN32_FIND_DATAA ffd );
#ifdef __WATCOMC__
extern void     __GetNTDirInfoW( struct _wdirent *dirp, LPWIN32_FIND_DATAW ffd );
extern void     __GetNTFindInfoW( struct _wfind_t *findt, LPWIN32_FIND_DATAW ffd );
#endif

#ifdef __WIDECHAR__
#define __GetNTDirInfo              __GetNTDirInfoW
#define __GetNTFindInfo             __GetNTFindInfoW
#else
#define __GetNTDirInfo              __GetNTDirInfoA
#define __GetNTFindInfo             __GetNTFindInfoA
#endif

#endif
