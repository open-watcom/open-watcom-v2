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
* Description:  DWARF specific assertion macros.
*
****************************************************************************/


#ifndef DWASSERT_H_INCLUDED
#define DWASSERT_H_INCLUDED

#include <assert.h>

/*
    _Assert is used for assertions that can help to comment the code.

    _Validate is used for assertions that verify arguments to
        exported functions have proper values.
*/
#define _Validate(__e)  _Assert(__e)
#ifdef NDEBUG
#define _Assert(__ignore)       ((void)0)
#else
/* Note: Old code used to call __assert() directly - not such a good idea
         since that is an undocumented function.
*/
#define _Assert assert
#endif

#endif
