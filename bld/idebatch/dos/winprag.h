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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


extern void TimeSlice( void );
#pragma aux TimeSlice = \
        "mov  ax,1680h" \
        "int 2fh"       \
    __parm      [] \
    __value     \
    __modify    [__ax]

extern int GetVM( void );
#pragma aux GetVM = \
        "mov  ax,1683h" \
        "int 2fh"       \
    __parm      [] \
    __value     [__bx]
    __modify    [__ax]

//int CS( void );
//#pragma aux CS = "mov ax,cs" value[ax];

//int DS( void );
//#pragma aux DS = "mov ax,ds" value[ax];

extern void BeginCriticalSection( void );
#pragma aux BeginCriticalSection = \
        "mov  ax,1681h" \
        "int 2fh"       \
    __parm      [] \
    __value     \
    __modify    [__ax]

extern void EndCriticalSection( void );
#pragma aux EndCriticalSection = \
        "mov  ax,1682h" \
        "int 2fh"       \
    __parm      [] \
    __value     \
    __modify    [__ax]
