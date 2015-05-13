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
* Description:  Platform independent implementation of creat().
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "rtdata.h"


/* file attributes */
#define _A_RDONLY       0x01


_WCRTLINK int __F_NAME(creat,_wcreat)( const CHAR_TYPE *name, mode_t pmode )
{
#ifndef __NETWARE__
    return( __F_NAME(open,_wopen)( name, O_RDWR | O_CREAT | O_TRUNC, pmode ) );
#else
    int             acc;

    acc = O_CREAT | O_TRUNC;
    if( (pmode & S_IWRITE) && (pmode & S_IREAD) ) {
        acc |= O_RDWR;
    } else if( pmode & S_IWRITE ) {
        acc |= O_WRONLY;
    } else if( pmode & S_IREAD ) {
        acc |= O_RDONLY;
    } else if( !pmode ) {
        acc |= O_RDWR;
    }
    return( __F_NAME(open,_wopen)( name, acc, pmode ) );
#endif
}
