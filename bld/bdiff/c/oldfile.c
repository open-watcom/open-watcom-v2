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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "bdiff.h"
#ifdef __QNX__
#include <conio.h>
#endif
#include "oldfile.h"
#include "myio.h"
#include "msg.h"
#include "installp.h"
#include "pathgrp2.h"

#include "clibext.h"


MY_FILE         OldFile;

static char     newName[_MAX_PATH];
static char     oldName[_MAX_PATH];
static pgroup2  pgold;

char *SetOld( const char *name )
{
    if( name != NULL )
        strcpy( oldName, name );
    return( oldName );
}

char *FindOld( const char *name )
{
    char        temp[_MAX_PATH];
    pgroup2     pg;

    _splitpath2( name, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
    _makepath( temp, NULL, NULL, pg.fname, pg.ext );
#ifdef INSTALL_PROGRAM
    if( SecondaryPatchSearch( name, oldName ) ) {
#endif
        if( oldName[0] == '\0' ) {
            _searchenv( temp, "PATH", oldName );
        }
#ifdef INSTALL_PROGRAM
    }
#endif
    if( oldName[0] == '\0' ) {
        FilePatchError( ERR_CANT_OPEN, temp );
        return( NULL );
    }
    return( oldName );
}

foff CheckSumOld( foff new_size )
{
    unsigned long   off;
    foff            sum;
    byte            ch;

    // Compute old checksum
    sum = 0;
    for( off = 0; off < new_size; ++off ) {
        Input( &OldFile, &ch, off, 1 );
        sum += ch;
    }
    return( sum );
}

#define TEMP_FILE_NAME  "bdXXXXXX"

PATCH_RET_CODE OpenOld( foff len, int prompt, foff new_size, foff new_sum )
{
    FILE            *fd;
    int             fh;
    unsigned long   actual_len;

    prompt=prompt;
    _splitpath2( oldName, pgold.buffer, &pgold.drive, &pgold.dir, &pgold.fname, &pgold.ext );
    _makepath( newName, pgold.drive, pgold.dir, TEMP_FILE_NAME, NULL );
    fh = mkstemp( newName );
    if( fh == -1 ) {
        NewName = "";
    } else {
        close( fh );
        NewName = newName;
    }
#ifndef INSTALL_PROGRAM
    {
        char    temp[_MAX_PATH];
        char    msgbuf[MAX_RESOURCE_SIZE];

        if( prompt && DoPrompt ) {
            for( ;; ) {
                GetMsg( msgbuf, MSG_MODIFY );
                cprintf( msgbuf, oldName );
                while( kbhit() )
                    getch();
                fgets( temp, sizeof( temp ), stdin );
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
    fd = fopen( oldName, "rb" );
    FileCheck( fd, oldName );
    MyOpen( &OldFile, fd, oldName );
    SeekCheck( fseek( fd, 0, SEEK_END ), oldName );
    actual_len = ftell( fd );
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
    SeekCheck( fseek( fd, 0, SEEK_SET ), oldName );
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
        _makepath( bak, pgold.drive, pgold.dir, pgold.fname, "bak" );
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
