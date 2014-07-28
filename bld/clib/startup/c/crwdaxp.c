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
* Description:  C runtime read/write data (non-x86 version).
*
****************************************************************************/


#include "variety.h"
#include <wctype.h>

_WCRTDATA unsigned int      _dynend;        // top of dynamic data area
_WCRTDATA unsigned int      _curbrk;        // top of usable memory
_WCRTDATA char              *_LpCmdLine;    // pointer to raw command line
_WCRTDATA char              *_LpPgmName;    // pointer to program name (for argv[0])
_WCRTDATA char              *_LpDllName;    // pointer to dll name (for OS/2, Win32)
_WCRTDATA wchar_t           *_LpwCmdLine;   // pointer to wide raw command line
_WCRTDATA wchar_t           *_LpwPgmName;   // pointer to wide program name (for argv[0])
_WCRTDATA wchar_t           *_LpwDllName;   // pointer to wide dll name (for OS/2, Win32)
_WCRTDATA unsigned int      _STACKLOW;      // lowest address in stack
_WCRTDATA unsigned int      _STACKTOP;      // highest address in stack
_WCRTDATA void              *__ASTACKSIZ;   // alternate stack size
_WCRTDATA void              *__ASTACKPTR;   // alternate stack pointer
_WCRTDATA int               _cbyte;         // used by getch, getche
_WCRTDATA int               _cbyte2;        // used by getch, getche
_WCRTDATA char              *_Envptr;       // offset part of environment pointer
_WCRTDATA unsigned char     _osmajor;       // major OS version number
_WCRTDATA unsigned char     _osminor;       // minor OS version number
_WCRTDATA unsigned short    _osbuild;       // operating system build number
_WCRTDATA unsigned int      _osver;         // operating system build number
_WCRTDATA unsigned int      _winmajor;      // operating system major version number
_WCRTDATA unsigned int      _winminor;      // operating system minor version number
_WCRTDATA unsigned int      _winver;        // operating system version number
_WCRTDATA void (*__FPE_handler)( int );     // f-p exception handler
