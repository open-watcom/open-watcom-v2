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


#include <wwindows.h>
#include <limits.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <direct.h>
#include <errno.h>
#include "watcom.h"
#include "wrglbl.h"
#include "wrmsg.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define WR_COPY_BUFFER_SIZE  0x7fff
#define WR_BACKUP_CHAR       '@'

/****************************************************************************/
/* static functions                                                         */
/****************************************************************************/
static int  WRCopyBinFile( int, int );

static int  LastError = 0;

int WRAPI WRGetLastError( void )
{
    return( LastError );
}

int WRAPI WRReadEntireFile( WResFileID file, BYTE **data, uint_32 *size )
{
    long int    s;
    int         ok;

    ok = (file != -1 && data != NULL && size != NULL);

    if( ok ) {
        s = filelength( file );
        ok = (s != -1 && s < INT_MAX);
    }

    if( ok ) {
        *size = s;
        *data = (BYTE *)WRMemAlloc( *size );
        ok = (*data != NULL);
    }

    if( ok ) {
        ok = (lseek( file, 0, SEEK_SET ) != -1);
    }

    if( ok ) {
        ok = (read( file, *data, *size ) == *size);
    }

    if( !ok ) {
        if( *data != NULL ) {
            WRMemFree( *data );
            *data = NULL;
        }
        *size = 0;
    }

    return( ok );
}

int WRAPI WRDeleteFile( const char *name )
{
    if( name != NULL && WRFileExists( name ) ) {
        return( !remove( name ) );
    }
    return( FALSE );
}

int WRAPI WRFileExists( const char *name )
{
    return( name != NULL && access( name, R_OK ) == 0 );
}

int WRAPI WRRenameFile( const char *new, const char *old )
{
    char     new_drive[_MAX_DRIVE];
    char     old_drive[_MAX_DRIVE];

    if( new == NULL || old == NULL ) {
        return( FALSE );
    }

    _splitpath( new, new_drive, NULL, NULL, NULL );
    _splitpath( old, new_drive, NULL, NULL, NULL );

    if( stricmp( new_drive, old_drive ) ) {
        if( WRCopyFile( new, old ) ) {
            return( WRDeleteFile( old ) );
        } else {
            return( FALSE );
        }
    } else {
        if( rename( old, new ) == 0 ) {
            return( TRUE );
        }
        LastError = errno;
        return( FALSE );
    }
}

int WRAPI WRBackupFile( const char *name, int use_rename )
{
    char     fn_path[_MAX_PATH];
    char     fn_drive[_MAX_DRIVE];
    char     fn_dir[_MAX_DIR];
    char     fn_name[_MAX_FNAME];
    char     fn_ext[_MAX_EXT + 1];
    size_t   len;
    int      ret;

    if( name == NULL ) {
        return( FALSE );
    }

    _splitpath( name, fn_drive, fn_dir, fn_name, fn_ext );

    len = strlen( fn_ext );

    if( len == 4 ) { // this case is special because in NT _MAX_EXT != 5
        fn_ext[3] = WR_BACKUP_CHAR;
    } else if( len == _MAX_EXT - 1 ) {
        fn_ext[len - 1] = WR_BACKUP_CHAR;
    } else {
        fn_ext[len] = WR_BACKUP_CHAR;
        fn_ext[len + 1] = '\0';
    }

    _makepath( fn_path, fn_drive, fn_dir, fn_name, fn_ext );

    if( use_rename ) {
        ret = WRRenameFile( fn_path, name );
    } else {
        ret = WRCopyFile( fn_path, name );
    }

    if( !ret ) {
        WRPrintErrorMsg( WR_BACKUPFAILED, name, fn_path, strerror( WRGetLastError() ) );
    }

    return( ret );
}

void WRAPI WRFreeTempFileName( char *name )
{
    WRMemFree( name );
}

char * WRAPI WRGetTempFileName( const char *ext )
{
    char    *buf;
    char    tname[L_tmpnam];
    char    *dir;
    size_t  len;
    char    fn_path[_MAX_PATH + 1];
    char    fn_drive[_MAX_DRIVE];
    char    fn_dir[_MAX_DIR];
    char    fn_name[_MAX_FNAME];
    char    fn_ext[_MAX_EXT];
    int     no_tmp;

    if( (dir = getenv( "TMP" )) != NULL || (dir = getenv( "TEMP" )) != NULL ||
        (dir = getenv( "TMPDIR" )) != NULL || (dir = getenv( "TEMPDIR" )) != NULL ) {
        no_tmp = FALSE;
    } else {
        dir = getcwd( (char *)NULL, 0 );
        no_tmp = TRUE;
    }

    len = strlen( dir );
    memcpy( fn_path, dir, len + 1 );
    if( fn_path[len - 1] != '\\' && fn_path[len - 1] != '/' ) {
        fn_path[len] = '\\';
        fn_path[len + 1] = '\0';
    }

    if( dir != NULL ) {
        _splitpath( fn_path, fn_drive, fn_dir, NULL, NULL );
        if( no_tmp ) {
            fn_dir[0] = '\0';
            free( dir );
        }
    } else {
        fn_drive[0] = '\0';
        fn_dir[0] = '\0';
    }

    tmpnam( tname );

    _splitpath( tname, NULL, NULL, fn_name, fn_ext );

    if( ext == NULL ) {
        ext = fn_ext;
    }

    _makepath( fn_path, fn_drive, fn_dir, fn_name, ext );

    len = strlen( fn_path ) + 1;

    buf = (char *)WRMemAlloc( len );
    if( buf != NULL ) {
        memcpy( buf, fn_path, len );
    }

    return( buf );
}

int WRAPI WRCopyFile( const char *dest, const char *src )
{
    uint_8     ret;
    int        dest_handle;
    int        src_handle;

    /* open the resource file that contains the dialog info */
    src_handle = open( src, O_RDONLY | O_BINARY );
    if( src_handle == -1 ) {
        return( FALSE );
    }

    dest_handle = open( dest, O_CREAT | O_WRONLY | O_TRUNC | O_BINARY, S_IWRITE | S_IREAD );
    if( dest_handle == -1 ) {
        LastError = errno;
        return( FALSE );
    }

    lseek( src_handle, 0, SEEK_SET );
    lseek( dest_handle, 0, SEEK_SET );

    ret = WRCopyBinFile( dest_handle, src_handle );

    ret = (close( src_handle ) != -1 && ret);
    ret = (close( dest_handle ) != -1 && ret);

    if( !ret ) {
        LastError = errno;
        WRDeleteFile( dest );
    }

    return( ret );
}

int WRCopyBinFile( int dest, int src )
{
    char        *buf;
    uint_32     file_size;
    uint_32     num_to_copy;
    long int    src_pos;
    long int    dest_pos;
    int         ok;

    buf = (char *)WRMemAlloc( WR_COPY_BUFFER_SIZE );
    if( buf == NULL ) {
        return( FALSE );
    }

    src_pos = lseek( src,  0, SEEK_SET );
    dest_pos = lseek( dest, 0, SEEK_SET );

    ok = ((file_size = filelength( src )) != -1L);

    do {
        if( file_size < WR_COPY_BUFFER_SIZE ) {
            num_to_copy = file_size;
        } else {
            num_to_copy = WR_COPY_BUFFER_SIZE;
        }

        if( ok && num_to_copy != 0 ) {
            if( read( src, buf, num_to_copy ) != (int)num_to_copy ) {
                ok = FALSE;
            }

            if( ok && write( dest, buf, num_to_copy ) != (int)num_to_copy ) {
                ok = FALSE;
            }
        }
        if( !ok ) {
            LastError = errno;
        }

        file_size -= num_to_copy;
    } while ( ok && file_size != 0 );

    if( ok ) {
        lseek( src, src_pos, SEEK_SET );
        lseek( dest, dest_pos, SEEK_SET );
    }

    WRMemFree( buf );

    return( ok );
}
