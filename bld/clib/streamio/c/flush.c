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
* Description:  Implementation of fflush() helper routine.
*
****************************************************************************/


#include "variety.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#if defined( __OS2__ )
    #include <wos2.h>
#endif
#include "rtdata.h"
#include "fileacc.h"
#include "qwrite.h"
#include "lseek.h"
#include "flush.h"
#include "errorno.h"
#include "thread.h"

#if defined( __NETWARE__ ) && defined( _THIN_LIB )

/* Take flush from LibC */
_WCRTLINK int __flush( FILE *fp )
{
    return( fflush( fp ) );
}

#else

_WCRTLINK int __flush( FILE *fp )
{
    int             len;
    long            offset;
    int             ret;
    unsigned char   *ptr;
    unsigned        amount;

    ret = 0;
    _AccessFile( fp );
    if( fp->_flag & _DIRTY ) {
        fp->_flag &= ~_DIRTY;
        if( (fp->_flag & _WRITE) && (_FP_BASE(fp) != NULL) ) {
            ptr = _FP_BASE(fp);
            amount = fp->_cnt;
            while( amount != 0 && ret == 0 ) {
                len = __qwrite( fileno( fp ), ptr, amount );    /* 02-aug-90 */
                if( len == -1 ) {
                    fp->_flag |= _SFERR;
                    ret = EOF;
                }
#ifndef __UNIX__
                else if( len == 0 ) {
                    _RWD_errno = ENOSPC;
                    fp->_flag |= _SFERR;
                    ret = EOF;
                }
#endif
                ptr += len;
                amount -= len;
            }
        }
    } else if( _FP_BASE(fp) != NULL ) {         /* not dirty */
        /* fseek( fp, ftell(fp), SEEK_SET ); */
        fp->_flag &= ~_EOF;
        if( !(fp->_flag & _ISTTY) ) {
            offset = fp->_cnt;
            if( offset != 0 ) { /* 10-aug-89 JD */
                offset = __lseek( fileno( fp ), -offset, SEEK_CUR );
            }
            if( offset == -1 ) {
                fp->_flag |= _SFERR;
                ret = EOF;
            }
        }
    }
    fp->_ptr = _FP_BASE(fp);   /* reset ptr to start of buffer */
    fp->_cnt = 0;
#if !defined( __NETWARE__ ) && !defined( __OSI__ )
    if( ret == 0  &&  (_FP_EXTFLAGS(fp) & _COMMIT) ) {
        if( fsync( fileno( fp ) ) == -1 ) {
            ret = EOF;
        }
    }
#endif
    _ReleaseFile( fp );
    return( ret );
}

#endif
