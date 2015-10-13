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

byte            *NewFile;

PATCH_RET_CODE OpenNew( foff len )
{
    NewFile = _allocate( len );
    if( NewFile == NULL ) {
        //PatchError( ERR_USEREAL );
        return( PATCH_BAD_PATCH );
    }
    memset( NewFile, 0, len );
    return( PATCH_RET_OKAY );
}

PATCH_RET_CODE CloseNew( foff len, foff actual_sum, bool *havenew )
{
    foff        sum;
    foff        off;
    int         fd;
    char        *p;

    *havenew = true;
    off = 0;
    sum = 0;
    p = NewFile;
    while( off != len ) {
        sum += *p;
        ++p;
        ++off;
    }
    if( sum != actual_sum ) {
        *havenew = false;
        if( CheckSumOld( len ) == actual_sum ) {
            return( PATCH_RET_OKAY );
        } else {
            PatchError( ERR_WRONG_CHECKSUM, sum, actual_sum );
            _free( NewFile );
            NewFile = NULL;
            return( PATCH_BAD_CHECKSUM );
        }
    }
    fd = open( NewName, O_BINARY+O_WRONLY+O_CREAT+O_TRUNC, S_IRWXU );
    FileCheck( fd, NewName );
    if( write( fd, NewFile, len ) != len ) {
        *havenew = false;
        FilePatchError( ERR_CANT_WRITE, NewName );
        _free( NewFile );
        NewFile = NULL;
        return( PATCH_CANT_WRITE );
    }
    close( fd );
    SameDate( NewName, PatchName );
    _free( NewFile );
    NewFile = NULL;
    return( PATCH_RET_OKAY );
}
