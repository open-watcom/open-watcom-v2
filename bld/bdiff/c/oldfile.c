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
#include "msg.h"

MY_FILE OldFile;

extern char     *NewName;
extern int      DoPrompt;

extern void MyOpen( MY_FILE *file, int handle, char *name );
extern void Input( MY_FILE *file, void *tmp, foff off, size_t len );
extern void MyClose( MY_FILE *file );
extern int  SecondaryPatchSearch( char *name, char *path );

extern void PatchError( int, ... );
extern void FilePatchError( int, ... );

extern void FileCheck(int, char *);
extern void SeekCheck(long, char *);

static char     new_path[_MAX_PATH];
static char     new_fname[_MAX_FNAME];
static char     new_ext[_MAX_EXT];

static char     path[_MAX_PATH];
static char     drive[_MAX_DRIVE];
static char     dir[_MAX_DIR];
static char     fname[_MAX_FNAME];
static char     ext[_MAX_EXT];

char *SetOld( char *name )
{
    if( name != NULL ) strcpy( path, name );
    return( path );
}

char *FindOld( char *name )
{
    char        temp[_MAX_PATH];
    int         retcode;

    _splitpath( name, drive, dir, fname, ext );
    _makepath( temp, "", "", fname, ext );
    retcode = 1;
    #if defined( INSTALL_PROGRAM )
    {
        retcode = SecondaryPatchSearch( name, path );
    }
    #endif
    if( retcode && path[0] == '\0' ) {
        _searchenv( temp, "PATH", path );
    }
    if( path[ 0 ] == '\0' ) {
        FilePatchError( ERR_CANT_OPEN, temp );
        return( NULL );
    }
    return( path );
}

foff CheckSumOld( foff new_size )
{
    foff        off;
    foff        sum;
    char        ch;

    // Compute old checksum
    off = 0;
    sum = 0;
    while( off != new_size ) {
        Input( &OldFile, &ch, off, 1 );
        ++off;
        sum += ch;
    }
    return( sum );
}

PATCH_RET_CODE OpenOld( foff len, int prompt, foff new_size, foff new_sum )
{
    int         handle;
    foff        actual_len;

    prompt=prompt;
    _splitpath( NewName, NULL, NULL, new_fname, new_ext );
    _splitpath( path, drive, dir, fname, ext );
    _makepath( new_path, drive, dir, new_fname, new_ext );
    NewName = new_path;
#if !defined( INSTALL_PROGRAM )
    {
        char    temp[_MAX_PATH];
        char    msgbuf[MAX_RESOURCE_SIZE];

        if( prompt && DoPrompt ) {
            for( ;; ) {
                GetMsg( msgbuf, MSG_MODIFY );
                cprintf( msgbuf, path );
                while( kbhit() ) getch();
                gets( temp );
                if( tolower( temp[0] ) == 'n' ) {
                    PatchError( ERR_PATCH_ABORTED );
                }
                if( tolower( temp[0] ) == 'y' ) break;
            }
        }
    }
#endif
    handle = open( path, O_RDONLY+O_BINARY, 0 );
    FileCheck( handle, path );
    MyOpen( &OldFile, handle, path );
    actual_len = lseek( handle, 0, SEEK_END );
    SeekCheck( actual_len, path );
    if( actual_len != len
    && (actual_len + sizeof( PATCH_LEVEL )) != len
    && (actual_len - sizeof( PATCH_LEVEL )) != len ) {
        if( actual_len >= new_size ) {
            if( CheckSumOld( new_size ) == new_sum ) {
                MyClose( &OldFile );
                return( PATCH_ALREADY_PATCHED );
            }
        }
        PatchError( ERR_WRONG_SIZE, path, actual_len, len );
        MyClose( &OldFile );
        return( PATCH_BAD_LENGTH );
    }
    SeekCheck( lseek( handle, 0, SEEK_SET ), path );
    return( PATCH_RET_OKAY );
}

byte InOld( foff offset )
{
    byte        tmp;

    Input( &OldFile, &tmp, offset, sizeof(byte) );
    return( tmp );
}

PATCH_RET_CODE CloseOld( int havenew, int dobackup )
{
    char        bak[_MAX_PATH];

    MyClose( &OldFile );
    if( havenew ) {
        _makepath( bak, drive, dir, fname, "bak" );
        remove( bak );
        if( rename( path, bak ) != 0 ) {
            FilePatchError( ERR_CANT_RENAME, path, bak );
            return( PATCH_CANT_RENAME );
        }
        if( rename( NewName, path ) != 0 ) {
            FilePatchError( ERR_CANT_RENAME, NewName, bak );
            return( PATCH_CANT_RENAME );
        }
        if( !dobackup ) remove( bak );
    }
    return( PATCH_RET_OKAY );
}
