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
* Description:  Implementation of __fprtf_s() - safe formatted output.
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
#include "saferlib.h"
#include <stdio.h>
#include <stdarg.h>
#include "rtdata.h"
#include "fileacc.h"
#include "printf.h"
#include "fprtf_s.h"
#include "orient.h"

extern  void    __ioalloc( FILE * );
extern  int     __flush( FILE * );


/*
 * file_putc -- write a character to a file
 */
static slib_callback_t file_putc; // setup calling convention
static void __SLIB_CALLBACK file_putc( SPECS __SLIB *specs, OUTC_PARM op_char )
{
    __F_NAME(fputc,fputwc)( (UCHAR_TYPE)op_char, (FILE *)specs->_dest );
    specs->_output_count++;
}


int __F_NAME(__fprtf_s,__fwprtf_s)( FILE * __restrict stream,
                         const CHAR_TYPE * __restrict format, va_list arg )
{
    int             not_buffered;
    int             amount_written;
    unsigned        oflag;
    const char      *msg = NULL;    /* doubles as error indicator */

    /* Check for runtime-constraints before grabbing file lock */
    /* stream   not null */
    /* format   not null */
    if( __check_constraint_nullptr_msg( msg, stream )  &&
        __check_constraint_nullptr_msg( msg, format ) ) {

        _ValidFile( stream, 0 );
        _AccessFile( stream );

        /*** Deal with stream orientation ***/
        ORIENT_STREAM(stream,0);

        oflag = stream->_flag & (_SFERR | _EOF);
        stream->_flag &= ~(_SFERR | _EOF);

        if( _FP_BASE(stream) == NULL ) {
            __ioalloc( stream );            /* allocate buffer */
        }
        not_buffered = 0;
        if( stream->_flag & _IONBF ) {
            not_buffered = 1;
            stream->_flag &= ~_IONBF;
            stream->_flag |= _IOFBF;
        }
        amount_written = __F_NAME(__prtf_s,__wprtf_s)( stream, format, arg, &msg, file_putc );
        if( not_buffered ) {
            stream->_flag &= ~_IOFBF;
            stream->_flag |= _IONBF;
            __flush( stream );
        }
        if( ferror( stream ) )
            amount_written = -1;
        stream->_flag |= oflag;

        _ReleaseFile( stream );
    }
    if( msg != NULL ) {
        /* There was a constraint violation; call the handler */
        __rtct_fail( __func__, msg, NULL );
        amount_written = -1;
    }
    return( amount_written );
}
