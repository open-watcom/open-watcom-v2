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
* Description:  code generator API calling convention declaration
*
****************************************************************************/


#ifndef _CGAPI_H_INCLUDED
#define _CGAPI_H_INCLUDED

#define _CGCALLBACKAPI
#define _CGAPI

#if defined( _CGDLL )

#if defined( __NT__ )
#define _CGDLLEXPORT    __stdcall __export
#elif defined( __OS2V2__ ) || defined( __OS2__ ) && !defined(__I86__)
#define _CGDLLEXPORT    __syscall __export
#else
#error Unsupported OS
#endif

#else

#define _CGDLLEXPORT

#endif

#endif
