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
#include "fileacc.h"
#include "rtdata.h"


extern void __ioalloc( FILE * );
extern int  __flush( FILE * );


_WCRTLINK int __F_NAME(fputs,fputws)( const CHAR_TYPE *s, FILE *fp )    /* put string s to file fp */
{
    const CHAR_TYPE     *start;
    int         c;
    int         not_buffered;
    int         rc;

    #ifndef __WIDECHAR__
        _ValidFile( fp, EOF );
    #else
        _ValidFile( fp, WEOF );
    #endif
    _AccessFile( fp );

    if( _FP_BASE(fp) == NULL ) {
        __ioalloc( fp );                /* allocate buffer */
    }
    not_buffered = 0;
    if( fp->_flag & _IONBF ) {
        not_buffered = 1;
        fp->_flag &= ~_IONBF;
        fp->_flag |= _IOLBF;
    }
    rc = 0;
    start = s;
    while( c = *s ) {
        s++;
        #ifndef __WIDECHAR__
            if( (fputc)( c, fp ) == EOF ) {         /* 23-oct-91 */
                rc = EOF;
                break;
            }
        #else
            if( (fputwc)( c, fp ) == WEOF ) {       /* 23-oct-91 */
                rc = -1;
                break;
            }
        #endif
    }
    if( not_buffered ) {
        fp->_flag &= ~_IOLBF;
        fp->_flag |= _IONBF;
        if( rc == 0 ) rc = __flush( fp );               /* 23-oct-91 */
    }
    if( rc == 0 ) {
        /* return the number of items written */
        /* this is ok by ANSI which says that success is */
        /* indicated by a non-negative return value */
        rc = s - start;
    }
    _ReleaseFile( fp );
    return( rc );
}
