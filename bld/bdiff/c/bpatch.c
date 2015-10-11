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
#include <sys/stat.h>
#include "newfile.h"
#include "oldfile.h"

MY_FILE         NewFile;

extern char     *NewName;
extern char     *PatchName;

extern void MyOpen( MY_FILE *file, int handle, char *name );
extern void Input( MY_FILE *file, void *tmp, foff off, size_t len );
extern void MyClose( MY_FILE *file );
extern void SameDate( char *file, char *as );

extern void FileCheck(int, char *);
extern void SeekCheck(long, char *);

extern void PatchError( int, ... );
extern void FilePatchError( int, ... );

PATCH_RET_CODE OpenNew( foff len )
{
    int         handle;
    char                        *name;
    auto struct stat            statblk;

    len = len;
    name = SetOld( NULL );
    if( stat( name, &statblk ) != 0 ) {
        PatchError( ERR_CANT_GET_ATTRIBUTES, name );
        return( PATCH_CANT_GET_ATTRIBUTES );
    }
    handle = open(NewName, O_RDWR+O_BINARY+O_CREAT+O_TRUNC, statblk.st_mode);
    FileCheck( handle, NewName );
    MyOpen( &NewFile, handle, NewName );
    return( PATCH_RET_OKAY );
}

PATCH_RET_CODE CloseNew( foff len, foff actual_sum, int *havenew )
{
    foff        sum;
    foff        actual_len;
    foff        off;
    char        ch;

    *havenew = 1;
    if( NewFile.dirty ) {
        SeekCheck( lseek( NewFile.handle, NewFile.start, SEEK_SET ), NewName );
        if( write(NewFile.handle, NewFile.buff, NewFile.len ) != NewFile.len ) {
            MyClose( &NewFile );
            FilePatchError( ERR_CANT_WRITE, NewName );
            return( PATCH_CANT_WRITE );
        }
    }
    actual_len = lseek( NewFile.handle, 0, SEEK_END );
    SeekCheck( actual_len, NewName );
    if( actual_len != len ) {
        MyClose( &NewFile );
        PatchError( ERR_WRONG_SIZE, NewName, actual_len, len );
        return( PATCH_BAD_LENGTH );
    }
    off = 0;
    sum = 0;
    while( off != len ) {
        Input( &NewFile, &ch, off, 1 );
        ++off;
        sum += ch;
    }
    MyClose( &NewFile );
    if( sum != actual_sum ) {
        if( CheckSumOld( len ) == actual_sum ) {
            remove( NewName );
            *havenew = 0;
        } else {
            PatchError( ERR_WRONG_CHECKSUM, sum, actual_sum );
            return( PATCH_BAD_CHECKSUM );
        }
    }
    SameDate( NewName, PatchName );
    return( PATCH_RET_OKAY );
}
