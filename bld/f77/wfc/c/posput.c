/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  POSIX level i/o support
*
****************************************************************************/


#include "ftnstd.h"
#include <string.h>
#include <limits.h>
#include "fileio.h"
#include "posput.h"
#include "posseek.h"
#include "fileerr.h"


size_t  writebytes( b_file *io, const char *buff, size_t len )
{
    size_t      written;

    if( len == 0 )
        return( 0 );
    written = fwrite( buff, 1, len, io->fp );
    if( written != len && ferror( io->fp ) ) {
        FSetSysErr( io );
        return( 0 );
    }
    io->phys_offset += written;
    if( written < len ) {
        FSetErr( FILEIO_DISK_FULL, io );
        return( 0 );
    }
    return( written );
}


static int SysWrite( b_file *io, const char *b, size_t len )
{
    if( len > 0 ) {
        writebytes( io, b, len );
        if( !IOOk( io ) ) {
            return( -1 );
        }
    }
    return( 0 );
}


void    FPutRecText( b_file *io, const char *b, size_t len, bool nolf )
//=====================================================================
// Put a record to a file with "text" records.
{
    char        tag[2];

    FSetIOOk( io );
    if( SysWrite( io, b, len ) == -1 )
        return;
#if defined( __UNIX__ )
    (void)nolf;

    tag[0] = CHAR_LF;
    len = 1;
#else
    tag[0] = CHAR_CR;
    len = 1;
    if( !nolf ) {
        tag[1] = CHAR_LF;
        ++len;
    }
#endif
    SysWrite( io, tag, len );
}

void    FPutRecFixed( b_file *io, const char *b, size_t len )
//===========================================================
// Put a record to a file with "fixed" records.
{
    FSetIOOk( io );
    SysWrite( io, b, len );
}
