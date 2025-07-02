/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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
#include <dirent.h>
#include <rdos.h>
#include "liballoc.h"
#include "pathmac.h"


static int IsMatch( struct dirent *dir, const char *fname )
{
    char       tmp[NAME_MAX + 1];
    const char *sptr = dir->d_match_mask;
    const char *fptr;
    const char *lsptr = 0;
    const char *lfptr = 0;
    char       ch;

    strcpy( tmp, fname );
    _strupr( tmp );
    fptr = tmp;

    if( strlen( sptr ) == 0 )
        return( 1 );

    if( strcmp( sptr, "*.*" ) == 0 )
        return( 1 );

    if( strcmp( sptr, "*." ) == 0 ) {
        if( strchr( fptr, '.' ) ) {
            return( 0 );
        } else {
            return( 1 );
        }
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
                    } else {
                        fptr++;
                    }
                } else {
                    fptr++;
                }
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
                    } else {
                        return( 0 );
                    }
                }
                break;
            }
        }

        if( *sptr == 0 && *fptr == 0 ) {
            return( 1 );
        } else {
            if( *sptr == '*' && *(sptr + 1) == 0 )
                return( 1 );

            if( lfptr ) {
                fptr = lfptr;
                sptr = lsptr;
                lfptr = 0;
                lsptr = 0;
            } else {
                return( 0 );
            }
        }
    }
}

static int GetSingleFile( struct dirent *dir )
{
    struct RdosDirEntry *entry;

    while( dir->d_entry_nr < dir->d_entry_count ) {
        entry = (struct RdosDirEntry *)dir->d_entry_chain;
        if( IsMatch( dir, entry->PathName ) ) {
            dir->d_ino = entry->Inode;
            dir->d_size = entry->Size;
            dir->d_create_time = entry->CreateTime;
            dir->d_modify_time = entry->ModifyTime;
            dir->d_access_time = entry->AccessTime;
            dir->d_attr = entry->Attrib;
            strncpy(dir->d_name, entry->PathName, NAME_MAX);
            dir->d_name[NAME_MAX] = 0;
            return( 1 );
        } else {
            dir->d_entry_nr++;
            dir->d_entry_chain += dir->d_header_size;
            dir->d_entry_chain += entry->PathNameSize;
        }
    }
    return( 0 );
}

_WCRTLINK DIR *opendir( const char *name )
{
    DIR                *parent;
    struct RdosDirInfo dinf;
    int                handle;
    char               *ptr;
    int                size;
    char               tmp[NAME_MAX + 1];

    parent = lib_malloc( sizeof( DIR ) );
    if( parent == NULL )
        return( NULL );

    handle = RdosOpenDir( name, &dinf );

    if( handle == 0 ) {
        strcpy( tmp, name );
        ptr = tmp;
        size = strlen( tmp );
        if( size ) {
            ptr += size - 1;
            while( size ) {
                if( IS_DIR_SEP( *ptr ) )
                    break;
                ptr--;
                size--;
            }
            ptr++;
            _strupr( ptr );
            strcpy( parent->d_match_mask, ptr );
            *ptr = 0;
            handle = RdosOpenDir( tmp, &dinf );
        } else {
            handle = RdosOpenDir( ".", &dinf );
            strcpy( tmp, name );
            _strupr( tmp );
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
    parent->d_entry_nr = 0;
    parent->d_entry_count = dinf.Count;
    parent->d_header_size = dinf.HeaderSize;
    parent->d_entry_head = (char *)dinf.Entry;
    parent->d_entry_chain = (char *)dinf.Entry;
    return( parent );
}


_WCRTLINK struct dirent *readdir( DIR *parent )
{
    if( parent == NULL )
        return( NULL );

    parent->d_entry_nr++;

    if( GetSingleFile( parent ) ) {
        return( parent );
    } else {
        return( NULL );
    }
}

_WCRTLINK void rewinddir( DIR *dirp )
{
    if( dirp != NULL ) {
        dirp->d_entry_nr = 0;
        dirp->d_entry_chain = dirp->d_entry_head;
    }
}


_WCRTLINK int closedir( DIR *dirp )
{
    if( dirp == NULL )
        return( 1 );

    RdosCloseDir( dirp->d_handle );
    lib_free( dirp );
    return( 0 );
}
