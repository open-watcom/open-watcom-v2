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


/*
 * This file is not indirected with an #include so we can compile this code
 * for multiple platforms without duplicating it in multiple files.
 */

#include "variety.h"
#define __INT64__
#include "int64.h"
#include "widechar.h"
/* gross hack for building 11.0 libraries with 10.6 compiler */
#ifndef __WATCOM_INT64__
    #include <limits.h>         /* a gross hack to make a gross hack work */
    #define __WATCOM_INT64__
    #define __int64             double
#endif
/* most includes should go after this line */
#include <string.h>
#include <sys\stat.h>


#ifdef __WIDECHAR__
 _WCRTLINK int _wfstati64( int handle, struct _wstati64 *buf )
#else
 _WCRTLINK int _fstati64( int handle, struct _stati64 *buf )
#endif
{
    #ifdef __WIDECHAR__
        struct _wstat   buf32;
    #else
        struct _stat    buf32;
    #endif
    int                 rc;
    INT_TYPE            tmp;

    /*** Get the info using non-64bit version ***/
    rc = __F_NAME(_fstat,_wfstat)( handle, &buf32 );
    if( rc == -1 )  return( -1 );

    /*** Convert the info to 64-bit equivalent ***/
    buf->st_dev = buf32.st_dev;
    buf->st_ino = buf32.st_ino;
    buf->st_mode = buf32.st_mode;
    buf->st_nlink = buf32.st_nlink;
    buf->st_uid = buf32.st_uid;
    buf->st_gid = buf32.st_gid;
    buf->st_rdev = buf32.st_rdev;
    _clib_U32ToU64( buf32.st_size, tmp );
    buf->st_size = GET_REALINT64(tmp);
    buf->st_atime = buf32.st_atime;
    buf->st_mtime = buf32.st_mtime;
    buf->st_ctime = buf32.st_ctime;
    buf->st_btime = buf32.st_btime;
    buf->st_attr = buf32.st_attr;
    buf->st_archivedID = buf32.st_archivedID;
    buf->st_updatedID = buf32.st_updatedID;
    buf->st_inheritedRightsMask = buf32.st_inheritedRightsMask;
    buf->st_originatingNameSpace = buf32.st_originatingNameSpace;
    __F_NAME(strcpy,wcscpy)( buf->st_name, buf32.st_name );

    return( rc );
}
