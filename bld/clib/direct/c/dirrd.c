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
* Description:  Directory support for RDOS
*
****************************************************************************/


#include "variety.h"
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include "liballoc.h"
#include <rdos.h>
#include "pathmac.h"


_WCRTLINK int chdir( const char *path )
{
    if ( RdosSetCurDir( path ))
        return 0;
    else
        return 1;
}

_WCRTLINK char *getcwd( char *buf, size_t size )
{
    int drive;
    char *p;
    char cwd[256];

    if( buf == NULL ) {
        size = 256;
        p = lib_malloc( size );
    }
    else
        p = buf;

    drive = RdosGetCurDrive();

    cwd[0] = drive + 'A';
    cwd[1] = DRV_SEP;
    cwd[2] = DIR_SEP;

    if( RdosGetCurDir( drive, &cwd[3] ) )
        return( strncpy( p, cwd, size ) );
    else
        return( NULL );
}

_WCRTLINK int _chdrive( int drive )
{
    int    dnum;

    RdosSetCurDrive( drive );
    dnum = RdosGetCurDrive();
    return( dnum == drive ? 0 : -1 );
}

_WCRTLINK unsigned _getdrive( void ) 
{
    int    dnum;

    dnum = RdosGetCurDrive();
    return( dnum );
}

_WCRTLINK unsigned _getdiskfree( unsigned dnum, struct diskfree_t *df )
{
    unsigned stat;
    long free_units;
    int bytes_per_unit;
    long total_units;
    int disc;
    long start_sector;
    long total_sectors;
    int sector_size;
    int bios_sectors_per_cyl;
    int bios_heads;

    stat = RdosGetDriveInfo( dnum, 
                             &free_units, 
                             &bytes_per_unit, 
                             &total_units );

    if( stat ) {
        stat = RdosGetDriveDiscParam(  dnum,
                                       &disc,
                                       &start_sector,
                                       &total_sectors );
    }

    if( stat ) {
        stat = RdosGetDiscInfo(  disc,
                                 &sector_size,
                                 &total_sectors,
                                 &bios_sectors_per_cyl,
                                 &bios_heads );
    }

    if( stat ) {
        df->total_clusters = total_units;
        df->avail_clusters = free_units;
        df->sectors_per_cluster = bytes_per_unit;
        df->bytes_per_sector = sector_size;                             
    }

    if( stat ) 
        return( 0 );
    else
        return( -1 );
}

static int IsMatch( struct dirent *dir, const char *fname )
{
    char       tmp[NAME_MAX + 1];
    const char *sptr = dir->d_match_mask;
    const char *fptr;
    const char *lsptr = 0;
    const char *lfptr = 0;
    char       ch;

    strcpy( tmp, fname );
    strupr( tmp );
    fptr = tmp;

    if( strlen( sptr ) == 0 )
        return( 1 );

    if( !strcmp( sptr, "*.*" ) )
        return( 1 );

    if( !strcmp( sptr, "*." ) ) {
        if( strchr( fptr, '.' ) )
            return( 0 );
        else
            return( 1 );
    }

        for( ;; ) {
                while( *sptr && *fptr ) {
                        switch( *sptr ) {
                                case '*':
                                        ch = *(sptr + 1);
                                        if( ch ) {
                                                if( ch == *fptr ) {
                                                    lsptr = sptr;
                                                        sptr += 2;
                                                        fptr++;
                                                        lfptr = fptr;
                                                }
                                                else
                                                        fptr++;
                                        }
                                        else
                                                fptr++;
                                        break;
        
                                case '?':
                                        sptr++;
                                        fptr++;
                                        break;

                                default:
                                        if( *sptr == *fptr )    {
                                                sptr++;
                                                fptr++;
                                        } else {
                                                if( lfptr ) {
                                                        fptr = lfptr;
                                                        sptr = lsptr;
                                                        lfptr = 0;
                                                        lsptr = 0;
                                                }
                                                else
                                                        return( 0 );
                                        }
                                        break;
                        }
                }

                if( *sptr == 0 && *fptr == 0 )
                        return( 1 );
                else {
                        if( *sptr == '*' && *(sptr+1) == 0 )
                                return( 1 );

                        if( lfptr ) {
                                fptr = lfptr;
                                sptr = lsptr;
                                lfptr = 0;
                                lsptr = 0;
                        }
                        else
                                return( 0 );
                }
        }
}

static int GetSingleFile( struct dirent *dir )
{
    for( ;; ) {
        if( RdosReadDir( dir->d_handle, 
                     dir->d_entry_nr, 
                     NAME_MAX, 
                     dir->d_name,
                     &dir->d_size, 
                     &dir->d_attr,
                     &dir->d_msb_time,
                     &dir->d_lsb_time ) ) {

            if( IsMatch( dir, dir->d_name ) )
                return( 1 );
            else
                dir->d_entry_nr++;
        } else
            return( 0 );
    }
}

_WCRTLINK struct dirent *opendir( const char *name )
{
    struct dirent   *parent;
    int             handle;
    char            *ptr;
    int             size;
    char            tmp[NAME_MAX + 1];
        
    parent = lib_malloc( sizeof( *parent ) );
    if( parent == NULL )
        return( NULL );

    handle = RdosOpenDir( name );

    if( handle == 0) {
        strcpy( tmp, name );
        ptr = tmp;
        size = strlen( tmp );
        if( size ) {
            ptr += size - 1;
            while( size ) {
                if( IS_DIR_SEP( *ptr ) )
                    break;
                ptr--;
            }
            ptr++;
            strupr( ptr );
            strcpy( parent->d_match_mask, ptr );
            *ptr = 0;
            handle = RdosOpenDir( tmp );
        } else {
            handle = RdosOpenDir( "." );       
            strcpy( tmp, name );
            strupr( tmp );
            strcpy( parent->d_match_mask, tmp );
        }
    } else {
        strcpy( parent->d_match_mask, "*" );    
    }

    if( handle == 0 ) {
        lib_free( parent );
        return( NULL );
    }

    parent->d_handle = handle;
    parent->d_entry_nr = -1;
    return( parent );
}


_WCRTLINK struct dirent *readdir( struct dirent *parent )
{
    if( parent == NULL )
        return( NULL );

    parent->d_entry_nr++;

    if (GetSingleFile( parent ) ) 
        return( parent );
    else
        return( NULL );
}

_WCRTLINK void rewinddir( struct dirent *dirp )
{
    if( dirp )
        dirp->d_entry_nr = -1;
}


_WCRTLINK int closedir( struct dirent *dirp )
{
    if( dirp == NULL )
        return( 1 );

    RdosCloseDir( dirp->d_handle );
    lib_free( dirp );
    return( 0 );
}

_WCRTLINK int mkdir( const char *path )
{
    if( RdosMakeDir( path ))
        return 0;
    else
        return -1;
}

_WCRTLINK int rmdir( const char *path )
{
    if( RdosRemoveDir( path ))
        return 0;
    else
        return -1;
}
