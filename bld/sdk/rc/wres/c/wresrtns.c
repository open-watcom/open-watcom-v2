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


#include <unistd.h>
#if defined( __QNX__ ) || defined( UNIX )
#include <fcntl.h>
#endif
#include <malloc.h>
#include "wressetr.h"
#ifdef UNIX
    #include "clibext.h"
    #include <stdlib.h>  // malloc for AIX
#endif

#if defined( LINUX )

#include <sys/types.h>

// Linux does not have a tell call
// use this equivelent one instead
off_t linux_tell( int fd )
{
        return( lseek( fd, 0, SEEK_CUR ) );
}

WResSetRtns( open, close, read, write, lseek, linux_tell, malloc, free );
#else
WResSetRtns( open, close, read, write, lseek, tell, malloc, free );
#endif
