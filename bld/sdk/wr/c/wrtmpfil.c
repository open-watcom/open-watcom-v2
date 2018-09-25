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


#include "wrglbl.h"
#include <limits.h>
#include <sys/types.h>
#include <direct.h>
#include <errno.h>
#include "wio.h"
#include "wrmsg.h"
#include "wrmemi.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define WR_COPY_BUFFER_SIZE  0x7fff
#define WR_BACKUP_CHAR       '@'

/****************************************************************************/
/* static functions                                                         */
/****************************************************************************/
static int  LastError = 0;

int WRAPI WRGetLastError( void )
{
    return( LastError );
}

bool WRAPI WRReadEntireFile( const char *fname, BYTE **data, size_t *size )
{
    long int    s;
    bool        ok;
    FILE        *fh;
    size_t      len;

    fh = NULL;
    s = 0;
    len = 0;
    ok = ( fname != NULL && data != NULL && size != NULL );

    if( ok ) {
        ok = ( (fh = fopen( fname, "rb" )) != NULL );
    }

    if( ok ) {
        ok = ( fseek( fh, 0, SEEK_END ) == 0 );
    }

    if( ok ) {
        s = ftell( fh );
        ok = (s != -1 && s < INT_MAX);
    }

    if( ok ) {
        len = (size_t)s;
        *size = len;
        *data = (BYTE *)MemAlloc( len );
        ok = (*data != NULL);
    }

    if( ok ) {
        rewind( fh );
        len = fread( *data, 1, len, fh );
        if( *size != len ) {
            if( feof( fh ) ) {
                *size = len;
            } else {
                ok = false;
            }
        }
    }

    if( fh != NULL ) {
        fclose( fh );
    }

    return( ok );
}

bool WRAPI WRDeleteFile( const char *name )
{
    if( name != NULL && WRFileExists( name ) ) {
        return( remove( name ) == 0 );
    }
    return( false );
}

bool WRAPI WRFileExists( const char *name )
{
    return( name != NULL && access( name, R_OK ) == 0 );
}

bool WRAPI WRCopyFile( const char *dst_name, const char *src_name )
{
    bool        ok;
    FILE        *dst;
    FILE        *src;
    char        *buf;
    size_t      len;
    int         save_errno;

    /* open the resource file that contains the dialog info */
    src = fopen( src_name, "rb" );
    if( src == NULL ) {
        LastError = errno;
        return( false );
    }

    dst = fopen( dst_name, "wb" );
    if( dst == NULL ) {
        LastError = errno;
        fclose( src );
        return( false );
    }

    ok = ( (buf = MemAlloc( WR_COPY_BUFFER_SIZE )) != NULL );
    if( ok ) {
        do {
            len = fread( buf, 1, WR_COPY_BUFFER_SIZE, src );
            if( len != WR_COPY_BUFFER_SIZE ) {
                save_errno = errno;
                if( !feof( src ) ) {
                    ok = false;
                    LastError = save_errno;
                }
            }
            if( ok && fwrite( buf, 1, len, dst ) != len ) {
                ok = false;
                LastError = errno;
            }
        } while ( ok && len == WR_COPY_BUFFER_SIZE );
        MemFree( buf );
    }

    ok = ( fclose( src ) == 0 && ok );
    ok = ( fclose( dst ) == 0 && ok );

    if( !ok ) {
        LastError = errno;
        WRDeleteFile( dst_name );
    }

    return( ok );
}

bool WRAPI WRRenameFile( const char *new, const char *old )
{
    char     new_drive[_MAX_DRIVE];
    char     old_drive[_MAX_DRIVE];

    if( new == NULL || old == NULL ) {
        return( false );
    }

    _splitpath( new, new_drive, NULL, NULL, NULL );
    _splitpath( old, new_drive, NULL, NULL, NULL );

    if( stricmp( new_drive, old_drive ) ) {
        if( WRCopyFile( new, old ) ) {
            return( WRDeleteFile( old ) );
        } else {
            return( false );
        }
    } else {
        if( rename( old, new ) == 0 ) {
            return( true );
        }
        LastError = errno;
        return( false );
    }
}

bool WRAPI WRBackupFile( const char *name, bool use_rename )
{
    char     fn_path[_MAX_PATH];
    char     fn_drive[_MAX_DRIVE];
    char     fn_dir[_MAX_DIR];
    char     fn_name[_MAX_FNAME];
    char     fn_ext[_MAX_EXT + 1];
    size_t   len;
    bool     ret;

    if( name == NULL ) {
        return( false );
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
    MemFree( name );
}

char * WRAPI WRGetTempFileName( const char *ext )
{
    char        *buf;
    char        tname[L_tmpnam];
    const char  *dir;
    size_t      len;
    char        fn_path[_MAX_PATH + 1];
    char        fn_drive[_MAX_DRIVE];
    char        fn_dir[_MAX_DIR];
    char        fn_name[_MAX_FNAME];
    char        fn_ext[_MAX_EXT];

    if( (dir = getenv( "TMP" )) != NULL || (dir = getenv( "TEMP" )) != NULL ||
        (dir = getenv( "TMPDIR" )) != NULL || (dir = getenv( "TEMPDIR" )) != NULL ) {
        strncpy( fn_path, dir, _MAX_PATH );
        fn_path[_MAX_PATH] = '\0';
    } else {
        dir = getcwd( fn_path, sizeof( fn_path ) );
        if( dir == NULL ) {
            fn_path[0] = '\0';
        }
    }
    len = strlen( fn_path );
    if( len > 0 && fn_path[len - 1] != '\\' && fn_path[len - 1] != '/' ) {
        fn_path[len] = '\\';
        fn_path[len + 1] = '\0';
    }

    if( dir != NULL ) {
        _splitpath( fn_path, fn_drive, fn_dir, NULL, NULL );
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

    buf = (char *)MemAlloc( len );
    if( buf != NULL ) {
        memcpy( buf, fn_path, len );
    }

    return( buf );
}
