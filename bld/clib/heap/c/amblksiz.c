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
* Description:  Defines the minimum block size requested from the OS for
*               the heap manager.
*
****************************************************************************/


#include "variety.h"
#include <stdlib.h>
#include <limits.h>

#if defined(__NT__)
_WCRTDATA unsigned _WCDATA _amblksiz = 64*1024;
#elif defined(__WINDOWS_386__)
_WCRTDATA unsigned _WCDATA _amblksiz = 32*1024;
#elif defined(__WINDOWS__)
_WCRTDATA unsigned _WCDATA _amblksiz = 8*1024;
#elif INT_MAX < 65535
_WCRTDATA unsigned _WCDATA _amblksiz = 16;
#else
_WCRTDATA unsigned _WCDATA _amblksiz = 4*1024;
#endif

