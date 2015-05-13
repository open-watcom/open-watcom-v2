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
* Description:  DOS implementation of close / commit files functions.
*
****************************************************************************/


#include "variety.h"
#include <dos.h>
#include "rtdata.h"
#include "tinyio.h"

#ifdef _M_I86
    #define AUX_INFO    \
        parm caller     [bx] \
        modify exact    [ax];
#else
    #define AUX_INFO    \
        parm caller     [ebx] \
        modify exact    [eax];
#endif

extern unsigned __doserror_( unsigned );
#pragma aux __doserror_ "*"

extern unsigned __dos_close( unsigned handle );
#pragma aux __dos_close = \
        _MOV_AH DOS_CLOSE \
        _INT_21         \
        "call __doserror_" \
        AUX_INFO

extern unsigned __dos_commit( unsigned handle );
#pragma aux __dos_commit = \
        _MOV_AH DOS_COMMIT_FILE \
        "clc"           \
        _INT_21         \
        "call __doserror_" \
        "nop"           \
        AUX_INFO

_WCRTLINK unsigned _dos_close( int handle )
{
    return( __dos_close( handle ) );
}

_WCRTLINK unsigned _dos_commit( int handle )
{
    return( __dos_commit( handle ) );
}
