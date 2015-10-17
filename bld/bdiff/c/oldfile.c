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
#include "oldfile.h"
#include "myio.h"
#include "msg.h"
#include "installp.h"

MY_FILE         OldFile;

static char     newName[_MAX_PATH];
static char     new_fname[_MAX_FNAME];
static char     new_ext[_MAX_EXT];

static char     oldName[_MAX_PATH];
static char     old_drive[_MAX_DRIVE];
static char     old_dir[_MAX_DIR];
static char     old_fname[_MAX_FNAME];
static char     old_ext[_MAX_EXT];

char *SetOld( const char *name )
{
    if( name != NULL )
        strcpy( oldName, name );
    return( oldName );
}

char *FindOld( const char *name )
{
    char        temp[_MAX_PATH];

    _splitpath( name, old_drive, old_dir, old_fname, old_ext );
    _makepath( temp, "", "", old_fname, old_ext );
#if defined( INSTALL_PROGRAM )
    if( SecondaryPatchSearch( name, oldName ) && oldName[0] == '\0' ) {
#else
    if( oldName[0] == '\0' ) {
#endif
        _searchenv( temp, "PATH", oldName );
    }
    if( oldName[0] == '\0' ) {
        FilePatchError( ERR_CANT_OPEN, temp );
        return( NULL );
    }
    return( oldName );
}

foff CheckSumOld( foff new_size )
{
    foff        off;
    foff        sum;
    byte        ch;

    // Compute old checksum
    sum = 0;
    for( off = 0; off < new_size; ++off ) {
        Input( &OldFile, &ch, off, 1 );
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
    _splitpath( oldName, old_drive, old_dir, old_fname, old_ext );
    _makepath( newName, old_drive, old_dir, new_fname, new_ext );
    NewName = newName;
#if !defined( INSTALL_PROGRAM )
    {
        char    temp[_MAX_PATH];
        char    msgbuf[MAX_RESOURCE_SIZE];

        if( prompt && DoPrompt ) {
            for( ;; ) {
                GetMsg( msgbuf, MSG_MODIFY );
                cprintf( msgbuf, oldName );
                while( kbhit() )
                    getch();
                gets( temp );
                if( tolower( temp[0] ) == 'n' ) {
                    PatchError( ERR_PATCH_ABORTED );
                }
                if( tolower( temp[0] ) == 'y' ) {
                    break;
                }
            }
        }
    }
#endif
    handle = open( oldName, O_RDONLY + O_BINARY, 0 );
    FileCheck( handle, oldName );
    MyOpen( &OldFile, handle, oldName );
    actual_len = lseek( handle, 0, SEEK_END );
    SeekCheck( actual_len, oldName );
    if( actual_len != len
      && (actual_len + sizeof( PATCH_LEVEL )) != len
      && (actual_len - sizeof( PATCH_LEVEL )) != len ) {
        if( actual_len >= new_size ) {
            if( CheckSumOld( new_size ) == new_sum ) {
                MyClose( &OldFile );
                return( PATCH_ALREADY_PATCHED );
            }
        }
        PatchError( ERR_WRONG_SIZE, oldName, actual_len, len );
        MyClose( &OldFile );
        return( PATCH_BAD_LENGTH );
    }
    SeekCheck( lseek( handle, 0, SEEK_SET ), oldName );
    return( PATCH_RET_OKAY );
}

byte InOld( foff offset )
{
    byte        tmp;

    Input( &OldFile, &tmp, offset, sizeof( byte ) );
    return( tmp );
}

PATCH_RET_CODE CloseOld( bool havenew, bool dobackup )
{
    char        bak[_MAX_PATH];

    MyClose( &OldFile );
    if( havenew ) {
        _makepath( bak, old_drive, old_dir, old_fname, "bak" );
        remove( bak );
        if( rename( oldName, bak ) != 0 ) {
            FilePatchError( ERR_CANT_RENAME, oldName, bak );
            return( PATCH_CANT_RENAME );
        }
        if( rename( NewName, oldName ) != 0 ) {
            FilePatchError( ERR_CANT_RENAME, NewName, bak );
            return( PATCH_CANT_RENAME );
        }
        if( !dobackup ) {
            remove( bak );
        }
    }
    return( PATCH_RET_OKAY );
}
