/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
#include "doserror.h"


#ifdef _M_I86
    #define AUX_INFO \
        __parm __caller     [__bx] \
        __value             [__ax] \
        __modify __exact    [__ax]
#else
    #define AUX_INFO \
        __parm __caller     [__ebx] \
        __value             [__eax] \
        __modify __exact    [__eax]
#endif

extern unsigned __dos_close( unsigned handle );
#pragma aux __dos_close =   \
        _MOV_AH DOS_CLOSE   \
        _INT_21             \
        "call __doserror_"  \
    AUX_INFO

extern unsigned __dos_commit( unsigned handle );
#pragma aux __dos_commit =  \
        _MOV_AH DOS_COMMIT_FILE \
        "clc"               \
        _INT_21             \
        "call __doserror_"  \
    AUX_INFO

_WCRTLINK unsigned _dos_close( int handle )
{
    return( __dos_close( handle ) );
}

_WCRTLINK unsigned _dos_commit( int handle )
{
    return( __dos_commit( handle ) );
}
