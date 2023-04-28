/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Minimal implementation of DIG client routines.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <process.h>
#include "bool.h"
#include "iopath.h"
#include "digcli.h"
#include "digld.h"
#include "pathlist.h"
#include "dipdump.h"


#if defined( __UNIX__ ) || defined( __DOS__ )
typedef struct char_ring {
    struct char_ring    *next;
    char                name[1];
} char_ring;

static char   *FilePathList = NULL;
#endif

void *DIGCLIENTRY( Alloc )( size_t amount )
{
    return( malloc( amount ) );
}

void *DIGCLIENTRY( Realloc )( void *p, size_t amount )
{
    return( realloc( p, amount) );
}

void DIGCLIENTRY( Free )( void *p )
{
    free( p );
}

FILE *DIGCLIENTRY( Open )( char const *name, dig_open mode )
{
    const char  *fmode;

    /* convert flags. */
    switch( mode & (DIG_OPEN_READ | DIG_OPEN_WRITE) ) {
    case DIG_OPEN_READ:
        fmode = "rb";
        break;
    case DIG_OPEN_WRITE:
        fmode = "wb";
        break;
    case DIG_OPEN_WRITE | DIG_OPEN_READ:
        fmode = "r+b";
        break;
    default:
        return( NULL );
    }
    return( fopen( name, fmode ) );
}

int DIGCLIENTRY( Seek )( FILE *fp, unsigned long p, dig_seek where )
{
    int     whence;

    switch( where ) {
    case DIG_SEEK_ORG:  whence = SEEK_SET; break;
    case DIG_SEEK_CUR:  whence = SEEK_CUR; break;
    case DIG_SEEK_END:  whence = SEEK_END; break;
    default:
        return( true );
    }
    return( fseek( fp, p, whence ) );
}

unsigned long DIGCLIENTRY( Tell )( FILE *fp )
{
    return( ftell( fp ) );
}

size_t DIGCLIENTRY( Read )( FILE *fp, void *b, size_t s )
{
    return( fread( b, 1, s, fp ) );
}

size_t DIGCLIENTRY( Write )( FILE *fp, const void *b, size_t s )
{
    return( fwrite( b, 1, s, fp ) );
}

void DIGCLIENTRY( Close )( FILE *fp )
{
    fclose( fp );
}

void DIGCLIENTRY( Remove )( char const *name, dig_open mode )
{
    /* unused parameters */ (void)mode;

    remove( name );
}

unsigned DIGCLIENTRY( MachineData )( address addr, dig_info_type info_type, dig_elen in_size,
                                        const void *in, dig_elen out_size, void *out )
{
    /* unused parameters */ (void)addr; (void)info_type; (void)in_size; (void)in;
    /* unused parameters */ (void)out_size; (void)out;

    return( 0 ); /// @todo check this out out.
}

#if defined( __UNIX__ ) || defined( __DOS__ )

static char *addPath( char *old_list, const char *path_list )
/***********************************************************/
{
    size_t          len;
    size_t          old_len;
    char            *new_list;
    char            *p;

    new_list = old_list;
    if( path_list != NULL && *path_list != '\0' ) {
        len = strlen( path_list );
        if( old_list == NULL ) {
            p = new_list = malloc( len + 1 );
        } else {
            old_len = strlen( old_list );
            new_list = malloc( old_len + 1 + len + 1 );
            strcpy( new_list, old_list );
            free( old_list );
            p = new_list + old_len;
        }
        while( *path_list != '\0' ) {
            if( p != new_list )
                *p++ = PATH_LIST_SEP;
            path_list = GetPathElement( path_list, NULL, &p );
        }
        *p = '\0';
    }
    return( new_list );
}

void PathInit( void )
/*******************/
{
    char        buff[_MAX_PATH];
    char        *p;

    if( _cmdname( buff ) != NULL ) {
#if defined( __UNIX__ )
        p = strrchr( buff, '/' );
#else
        p = strrchr( buff, '\\' );
#endif
        if( p != NULL ) {
            *p = '\0';
            FilePathList = addPath( FilePathList, buff );
        }
    }
    FilePathList = addPath( FilePathList, getenv( "PATH" ) );
#if defined(__UNIX__)
    FilePathList = addPath( FilePathList, getenv( "WD_PATH" ) );
    FilePathList = addPath( FilePathList, "/usr/watcom/wd" );
    FilePathList = addPath( FilePathList, "/opt/watcom/wd" );
#endif
}

void PathFini( void )
{
    free( FilePathList );
}

size_t DIGLoader( Find )( dig_filetype ftype, const char *name, size_t name_len, const char *defext, char *result, size_t result_len )
/************************************************************************************************************************************/
{
    char        fullname[_MAX_PATH2];
    char        filename[_MAX_PATH2];
    FILE        *fp;
    char        *p;
    char        c;
    size_t      len;

    /* unused parameters */ (void)ftype;

    strncpy( filename, name, name_len );
    filename[name_len] = '\0';
    if( defext != NULL && *defext != '\0' ) {
        _splitpath2( filename, fullname, NULL, NULL, &p, NULL );
        _makepath( filename, NULL, NULL, p, defext );
    }
    if( access( filename, F_OK ) == 0 ) {
        p = filename;
    } else if( path_list != NULL ) {
        strcpy( fullname, filename );
        while( (c = *path_list) != '\0' ) {
            p = fullname;
            do {
                ++path_list;
                if( IS_PATH_LIST_SEP( c ) )
                    break;
                *p = c;
            } while( (c = *path_list) != '\0' );
            c = p[-1];
            if( !IS_PATH_SEP( c ) ) {
                *p++ = DIR_SEP;
            }
            strcpy( p, filename );
            if( access( fullname, F_OK ) == 0 ) {
                p = fullname;
                break;
            }
        }
        if( *path_list == '\0' ) {
            p = "";
        }
    } else {
        p = "";
    }
    len = strlen( p );
    if( result_len > 0 ) {
        result_len--;
        if( result_len > len )
            result_len = len;
        if( result_len > 0 )
            strncpy( result, p, result_len );
        result[result_len] = '\0';
    }
    return( len );
}

FILE *DIGLoader( Open )( const char *filename )
/*********************************************/
{
    return( fopen( filename, "rb" ) );
}

int DIGLoader( Read )( FILE *fp, void *buff, size_t len )
{
    return( fread( buff, 1, len, fp ) != len );
}

int DIGLoader( Seek )( FILE *fp, unsigned long offs, dig_seek where )
{
    return( fseek( fp, offs, where ) );
}

int DIGLoader( Close )( FILE *fp )
{
    return( fclose( fp ) );
}

#endif
