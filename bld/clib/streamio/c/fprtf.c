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
 * printf -- CLIB formatted output
 */

#include "variety.h"
#include "widechar.h"
#include <stdio.h>
#include <stdarg.h>
#include "rtdata.h"
#include "fileacc.h"
#include "printf.h"

extern  void    __ioalloc( FILE * );
extern  int     __flush( FILE * );


/*
 * file_putc -- write a character to a file
 */
static slib_callback_t file_putc; // setup calling convention
static void __SLIB_CALLBACK file_putc( SPECS __SLIB *specs, int op_char )
{
    __F_NAME(fputc,fputwc)( op_char, (FILE *) specs->_o._dest );
    specs->_o._output_count++;
}


_WCRTLINK int __F_NAME(__fprtf,__fwprtf)( FILE *fp, const CHAR_TYPE *format, va_list arg )
{
    int         not_buffered;
    int         amount_written;
    unsigned    oflag;
    slib_callback_t *tmp;

    _ValidFile( fp, 0 );
    _AccessFile( fp );

    /*** Deal with stream orientation ***/
    #ifndef __NETWARE__
        #ifdef __WIDECHAR__
            if( _FP_ORIENTATION(fp) != _WIDE_ORIENTED ) {
                if( _FP_ORIENTATION(fp) == _NOT_ORIENTED ) {
                    _FP_ORIENTATION(fp) = _WIDE_ORIENTED;
                } else {
                    _ReleaseFile( fp );
                    return( 0 );                /* error return */
                }
            }
        #else
            if( _FP_ORIENTATION(fp) != _BYTE_ORIENTED ) {
                if( _FP_ORIENTATION(fp) == _NOT_ORIENTED ) {
                    _FP_ORIENTATION(fp) = _BYTE_ORIENTED;
                } else {
                    _ReleaseFile( fp );
                    return( 0 );                /* error return */
                }
            }
        #endif
    #endif

    oflag = fp->_flag & (_SFERR|_EOF);                  /* 06-sep-91 */
    fp->_flag &= ~(_SFERR|_EOF);

    if( _FP_BASE(fp) == NULL ) {
        __ioalloc( fp );        /* allocate buffer */
    }
    not_buffered = 0;
    if( fp->_flag & _IONBF ) {
        not_buffered = 1;
        fp->_flag &= ~_IONBF;
        fp->_flag |= _IOFBF;
    }
#if defined(__386__) && defined(__QNX__)
        /* avoid some segment relocations for 32-bit QNX */
        tmp = (void (*)())file_putc;
#else
        tmp = file_putc;
#endif
    amount_written = __F_NAME(__prtf,__wprtf)( fp, format, arg, tmp );
    if( not_buffered ) {
        fp->_flag &= ~_IOFBF;
        fp->_flag |= _IONBF;
        __flush( fp );
    }
    if( ferror( fp ) ) amount_written = -1;             /* 06-sep-91 */
    fp->_flag |= oflag;

    _ReleaseFile( fp );
    return( amount_written );
}
