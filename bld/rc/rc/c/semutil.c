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
* Description:  Utility routines for semantic actions.
*
****************************************************************************/


#include <string.h>
#include "errors.h"
#include "global.h"
#include "iortns.h"


void ReportCopyError( RcStatus status, int read_msg, char *filename,
                             int err_code ) {
    switch( status ) {
    case RS_READ_ERROR:
        RcError( read_msg, filename, strerror( err_code ) );
        break;
    case RS_READ_INCMPLT:
        RcError( ERR_UNEXPECTED_EOF, filename );
        break;
    case RS_WRITE_ERROR:
        RcError( ERR_WRITTING_RES_FILE, CurrResFile.filename,
                 strerror( err_code ) );
        break;
    default:
        RcError( ERR_INTERNAL, INTERR_UNKNOWN_RCSTATUS );
        break;
    }
}

/*
 * CopyData -
 */
RcStatus CopyData( uint_32 offset, uint_32 length, int handle,
                void *buff, int buffsize, int *err_code )
/***************************************************************************/
{
    int     error;
    int     numread;
    long    seekrc;

    seekrc = RcSeek( handle, offset, SEEK_SET );
    if (seekrc == -1) {
        *err_code = errno;
        return( RS_READ_ERROR );
    }

    while( length > buffsize ) {
        numread = RcRead( handle, buff, buffsize );
        if (numread != buffsize) {
            if( numread == -1 ) {
                *err_code = errno;
                return( RS_READ_ERROR );
            } else {
                return( RS_READ_INCMPLT );
            }
        }
        length -= buffsize;
        error = ResWrite( buff, buffsize, CurrResFile.handle );
        if( error ) {
            *err_code = LastWresErr();
            return( RS_WRITE_ERROR );
        }
    }

    numread = RcRead( handle, buff, length );
    if( numread != length ) {
        if( numread == -1 ) {
            *err_code = errno;
            return( RS_READ_ERROR );
        } else {
            return( RS_READ_INCMPLT );
        }
    }
    error = ResWrite( buff, length, CurrResFile.handle );
    if( error ) {
        *err_code = errno;
        return( RS_WRITE_ERROR );
    }

    return( RS_OK );
} /* CopyData */
