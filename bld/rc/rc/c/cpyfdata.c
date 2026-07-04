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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "global.h"
#include "rctypes.h"
#include "rcerrors.h"
#include "pass2.h"
#include "rcrtns.h"
#include "exeutil.h"
#include "cpyfdata.h"


RcStatus CopyFilesData( FILE *src_fp, FILE *dst_fp, uint_32 length, void *buffer, size_t blen )
/**********************************************************************************************
 * NB When an error occurs the function MUST return without altering errno
 */
{
    size_t          numread;
    size_t          bufflen;

    if( length == 0 ) {
        return( RS_PARAM_ERROR );
    }
    for( bufflen = blen; length > 0; length -= bufflen ) {
        if( bufflen > length )
            bufflen = length;
        numread = RESREAD( src_fp, buffer, bufflen );
        if( numread != bufflen ) {
            return( RESIOERR( src_fp, numread ) ? RS_READ_ERROR : RS_READ_INCMPLT );
        }
        if( RESWRITE( dst_fp, buffer, numread ) != numread ) {
            return( RS_WRITE_ERROR );
        }
    }
    return( RS_OK );
}

#if 0
RcStatus CopyFilesDataTilEOF( FILE *src_fp, FILE *dst_fp, void *buffer, size_t blen )
/*********************************************************
 * NB When an error occurs the function MUST return without altering errno
 */
{
    size_t      numread;

    while( (numread = RESREAD( src_fp, buffer, blen )) != 0 ) {
        if( numread != blen
          && RESIOERR( src_fp, numread ) ) {
            return( RS_READ_ERROR );
        }
        if( RESWRITE( dst_fp, buffer, numread ) != numread ) {
            return( RS_WRITE_ERROR );
        }
    }

    return( RS_OK );
}
#endif
