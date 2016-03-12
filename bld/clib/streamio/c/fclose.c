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
* Description:  Platform independent fclose() implementation.
*
****************************************************************************/


#include "variety.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "rtdata.h"
#include "liballoc.h"
#include "fileacc.h"
#include "tmpfname.h"
#include "lseek.h"
#include "streamio.h"
#include "close.h"
#include "flush.h"


#ifndef __UNIX__
void    (*__RmTmpFileFn)( FILE *fp );
#endif


int __doclose( FILE *fp, int close_handle )
{
    int         ret;

    if( fp->_flag == 0 ) {
        return( -1 );                       /* file already closed */
    }
    ret = 0;
    if( fp->_flag & _DIRTY ) {
        ret = __flush( fp );
    }
    _AccessFile( fp );
/*
 *      02-nov-92 G.Turcotte  Syncronize buffer pointer with the file pointer
 *                        IEEE Std 1003.1-1988 B.8.2.3.2
 *      03-nov-03 B.Oldeman Inlined ftell; we already know the buffer isn't
 *                dirty (because of the flush), so only a "get" applies
 */
    if( fp->_cnt != 0 ) {                   /* if something in buffer */
        __lseek( fileno( fp ), -fp->_cnt, SEEK_CUR );
    }

    if( close_handle ) {
#if defined( __UNIX__ ) || defined( __NETWARE__ ) || defined( __RDOS__ ) || defined( __RDOSDEV__ )
        // we don't get to implement the close function on these systems
        ret |= close( fileno( fp ) );
#else
        ret |= __close( fileno( fp ) );
#endif
    }
    if( fp->_flag & _BIGBUF ) {     /* if we allocated the buffer */
        lib_free( _FP_BASE( fp ) );
        _FP_BASE( fp ) = NULL;
    }
#ifndef __UNIX__
    /* this never happens under UNIX */
    if( fp->_flag & _TMPFIL ) {     /* if this is a temporary file */
        __RmTmpFileFn( fp );
    }
#endif
    _ReleaseFile( fp );
    return( ret );
}

int __shutdown_stream( FILE *fp, int close_handle )
{
    int         ret;

    ret = __doclose( fp, close_handle );
    __freefp( fp );
    return( ret );
}

_WCRTLINK int fclose( FILE *fp )
{
    __stream_link       *link;

    _AccessIOB();
    link = _RWD_ostream;
    for( ;; ) {
        if( link == NULL ) {
            _ReleaseIOB();
            return( -1 );     /* file not open */
        }
        if( link->stream == fp ) break;
        link = link->next;
    }
    _ReleaseIOB();
    return( __shutdown_stream( fp, 1 ) );
}
