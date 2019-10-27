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


#include "layer0.h"
#include "rcrtns.h"
#include "reserr.h"
#include "trmemcvr.h"

#include "clibext.h"


FILE *res_open( const char *file_name, wres_open_mode omode )
{
    FILE        *fp;

    switch( omode ) {
    default:
    case WRES_OPEN_RO:
        fp = fopen( file_name, "rb" );
        break;
    case WRES_OPEN_RW:
    case WRES_OPEN_NEW:
        fp = fopen( file_name, "wb" );
        break;
    case WRES_OPEN_TMP:
        fp = tmpfile();
        break;
    }
    if( fp == NULL ) {
        WRES_ERROR( WRS_OPEN_FAILED );
    }
    return( fp );
}

bool res_close( FILE *fp )
{
    return( fclose( fp ) != 0 );
}

size_t res_read( FILE *fp, void *buf, size_t size )
{
    return( fread( buf, 1, size, fp ) );
}

size_t res_write( FILE *fp, const void *buf, size_t size )
{
    return( fwrite( buf, 1, size, fp ) );
}

bool res_seek( FILE *fp, long pos, int where )
{
    return( fseek( fp, pos, where ) != 0 );
}

long res_tell( FILE *fp )
{
    return( ftell( fp ) );
}

bool res_ioerr( FILE *fp, size_t rc )
/***********************************/
{
    /* unused parameters */ (void)rc;

    return( ferror( fp ) != 0 );
}

WResSetRtns( res_open, res_close, res_read, res_write, res_seek, res_tell, res_ioerr, RESALLOC, RESFREE );
