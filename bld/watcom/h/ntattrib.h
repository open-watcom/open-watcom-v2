/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2019-2019 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  DOS<->NT file attributes conversion macros
*
****************************************************************************/


#define _NT_ATTRIBUTES_MASK  (FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY \
                                | FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_ARCHIVE)
#define _DOS_ATTRIBUTES_MASK (_A_SYSTEM | _A_HIDDEN | _A_RDONLY | _A_SUBDIR | _A_ARCH)

#define NT2DOSATTR(a)       ((a) & _NT_ATTRIBUTES_MASK)
#define DOS2NTATTR(a)       (((a) & _DOS_ATTRIBUTES_MASK) ? ((a) & _DOS_ATTRIBUTES_MASK) : FILE_ATTRIBUTE_NORMAL)
