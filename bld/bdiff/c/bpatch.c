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


#include "bdiff.h"
#include "newfile.h"
#include "oldfile.h"
#include "myio.h"
#include "msg.h"

MY_FILE         NewFile;

PATCH_RET_CODE OpenNew( foff len )
{
    FILE                *fd;
    char                *name;
    auto struct stat    statblk;

    len = len;
    name = SetOld( NULL );
    if( stat( name, &statblk ) != 0 ) {
        PatchError( ERR_CANT_GET_ATTRIBUTES, name );
        return( PATCH_CANT_GET_ATTRIBUTES );
    }
    fd = fopen( NewName, "wb" );
    FileCheck( fd, NewName );
    MyOpen( &NewFile, fd, NewName );
    return( PATCH_RET_OKAY );
}

PATCH_RET_CODE CloseNew( foff len, foff actual_sum, bool *havenew )
{
    foff            sum;
    unsigned long   actual_len;
    foff            off;
    byte            ch;

    *havenew = true;
    if( NewFile.dirty ) {
        SeekCheck( fseek( NewFile.fd, NewFile.start, SEEK_SET ), NewName );
        if( fwrite( NewFile.buff, 1, NewFile.len, NewFile.fd ) != NewFile.len ) {
            MyClose( &NewFile );
            FilePatchError( ERR_CANT_WRITE, NewName );
            return( PATCH_CANT_WRITE );
        }
    }
    SeekCheck( fseek( NewFile.fd, 0, SEEK_END ), NewName );
    actual_len = ftell( NewFile.fd );
    if( actual_len != len ) {
        MyClose( &NewFile );
        PatchError( ERR_WRONG_SIZE, NewName, actual_len, len );
        return( PATCH_BAD_LENGTH );
    }
    sum = 0;
    for( off = 0; off < len; ++off ) {
        Input( &NewFile, &ch, off, 1 );
        sum += ch;
    }
    MyClose( &NewFile );
    if( sum != actual_sum ) {
        if( CheckSumOld( len ) == actual_sum ) {
            remove( NewName );
            *havenew = false;
        } else {
            PatchError( ERR_WRONG_CHECKSUM, sum, actual_sum );
            return( PATCH_BAD_CHECKSUM );
        }
    }
    SameDate( NewName, PatchName );
    return( PATCH_RET_OKAY );
}
