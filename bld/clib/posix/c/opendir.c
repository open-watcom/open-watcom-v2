/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2026      The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Implementation for opendir() for Linux.
*
****************************************************************************/


#include "variety.h"
#include <stdlib.h>
#include <fcntl.h>
#include <dirent.h>
#include "liballoc.h"
#include "dirstrea.h"


_WCRTLINK DIR *opendir( const char *dirname )
{
    DIR *dirp = lib_malloc( sizeof( *dirp ) );

    dirp->fd = open( dirname, O_RDONLY | O_NONBLOCK | O_DIRECTORY );
    if( dirp->fd == -1 ) {
        lib_free( dirp );
        return( NULL );
    }
    dirp->bufofs = 0;
    return( dirp );
}
