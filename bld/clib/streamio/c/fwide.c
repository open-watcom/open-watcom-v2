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
* Description:  Platform independent fwide() implementation.
*
****************************************************************************/


#include "variety.h"
#include <stdio.h>
#include <wchar.h>
#include "fileacc.h"
#include "rtdata.h"


_WCRTLINK int fwide( FILE *fp, int mode )
{
    int     new_mode;

    _ValidFile( fp, EOF );
    _AccessFile( fp );

#ifndef __NETWARE__
    /* Set orientation if possible */
    if( mode > 0 && _FP_ORIENTATION(fp) == _NOT_ORIENTED )
        _FP_ORIENTATION(fp) = _WIDE_ORIENTED;
    else if( mode < 0 && _FP_ORIENTATION(fp) == _NOT_ORIENTED )
        _FP_ORIENTATION(fp) = _BYTE_ORIENTED;

    /* Find out what the current orientation is */
    new_mode = _FP_ORIENTATION(fp) == _WIDE_ORIENTED ?
        1 : (_FP_ORIENTATION(fp) == _BYTE_ORIENTED ? -1 : 0);
#else
    new_mode = 0;
#endif

    _ReleaseFile( fp );
    return( new_mode );
}
