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


#include "variety.h"
#include "widechar.h"
#include <stdio.h>
#include <errno.h>
#include <io.h>
#include <fcntl.h>
#include <sys\types.h>
#include <sys\stat.h>
#include "seterrno.h"


/* file attributes */

#define _A_RDONLY       0x01

#ifndef __QNX__
    typedef int mode_t;
#endif


_WCRTLINK int __F_NAME(creat,_wcreat)( const CHAR_TYPE *name, mode_t pmode )
    {
#ifndef __NETWARE__
        return( __F_NAME(open,_wopen)( name, O_RDWR|O_CREAT|O_TRUNC, pmode ) );
#else
        int             acc;

        acc = O_CREAT | O_TRUNC;
        if( ( pmode & S_IWRITE ) && ( pmode & S_IREAD ) ) {
            acc |= O_RDWR;
        } else if( pmode & S_IWRITE ) {
            acc |= O_WRONLY;
        } else if( pmode & S_IREAD ) {
            acc |= O_RDONLY;
        } else if( !pmode ) {
            acc |= O_RDWR;
        }
        if( __F_NAME(access,_waccess)( name, 0 ) ) { /* Delete the file if it exists so that the
            __F_NAME(remove,_wremove)( name );       creation date and time will be reset. */
        }
        return( __F_NAME(open,_wopen)( name, acc, pmode ) );
#endif
    }

