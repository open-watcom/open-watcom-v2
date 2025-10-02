/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Handle based file operations for RDOS.
*
****************************************************************************/


#include "variety.h"
#include "widechar.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <share.h>
#include <time.h>
#include <sys/locking.h>
#include <rdos.h>
#include "rtdata.h"
#include "liballoc.h"
#include "rtinit.h"
#include "iomode.h"
#include "openmode.h"
#include "qread.h"
#include "qwrite.h"
#include "_rdos.h"


struct HandleMap {
    struct RdosFileMap *map;
    int handle;
    int index;
};

static struct HandleMap *StdioMapArr[3] = { NULL, NULL, NULL };
static int              MapCount = 3;
static struct HandleMap **MapArr = StdioMapArr;


static void GrowMapArr( int size )
{
    int                 i;
    int                 count;;
    struct HandleMap  **maparr;

    count = MapCount + MapCount / 2;
    if( count < size )
        count = size;

    maparr = (struct HandleMap **)lib_calloc( 4, count );

    for( i = 0; i < MapCount; i++ )
        maparr[i] = MapArr[i];

    for( i = MapCount; i < count; i++ )
        maparr[i] = 0;

    if( MapCount != 3 )
        lib_free( MapArr );

    MapArr = maparr;
    MapCount = count;
}

static void CreateMap( int handle )
{
    struct RdosFileMap *map;
    int                 mhandle;
    struct HandleMap   *hmap;

    if( handle >= MapCount )
        GrowMapArr( handle + 1 );

    if( handle >= 0 ) {
        map = RdosGetHandleMap( handle, &mhandle );
        if( map ) {
            hmap = (struct HandleMap *)lib_calloc( 1, sizeof( struct HandleMap ) );
            hmap->map = map;
            hmap->handle = mhandle;
            hmap->index = -1;
            MapArr[handle] = hmap;
        }
    }
}

static void FreeMap( int handle )
{
    struct HandleMap    *map;

    if( handle >= 0
      && handle < MapCount ) {
        map = MapArr[handle];
        if( map ) {
            MapArr[handle] = 0;
            lib_free( map );
        }
    }
}

_WCRTLINK int unlink( const CHAR_TYPE *filename )
{
    __ptr_check( filename, 0 );

    if( RdosDeleteFile( filename ) ) {
        return( 0 );
    }
    return( -1 );
}

unsigned _WCNEAR __GetIOMode( int handle )
{
    return( RdosGetHandleMode( handle ) );
}

void _WCNEAR __SetIOMode( int handle, unsigned value )
{
    RdosSetHandleMode( handle, value );
}

int _WCNEAR __SetIOMode_grow( int handle, unsigned value )
{
    return( RdosSetHandleMode( handle, value ) );
}

_WCRTLINK int creat( const CHAR_TYPE *name, mode_t pmode )
{
    unsigned            mode;
    int                 handle;

    mode = O_CREAT | O_TRUNC;
    if( (pmode & S_IWRITE)
      && (pmode & S_IREAD) ) {
        mode |= O_RDWR;
    } else if( pmode & S_IWRITE ) {
        mode |= O_WRONLY;
    } else if( pmode & S_IREAD ) {
        mode |= O_RDONLY;
    } else if( pmode == 0 ) {
        mode |= O_RDWR;
    }

    handle = RdosOpenHandle( name, mode );
    if( handle > 0 )
        CreateMap( handle );

    return( handle );
}

_WCRTLINK int open( const CHAR_TYPE *name, int mode, ... )
{
    int                 permission;
    int                 handle;
    va_list             args;

    va_start( args, mode );
    permission = va_arg( args, int );
    va_end( args );

    handle = RdosOpenHandle( name, mode );
    if( handle > 0 )
        CreateMap( handle );

    return( handle );
}


_WCRTLINK int _sopen( const CHAR_TYPE *name, int mode, int shflag, ... )
{
    int                 handle;
    va_list             args;

    va_start( args, shflag );
    va_end( args );

    handle = RdosOpenHandle( name, mode );
    if( handle > 0 )
        CreateMap( handle );

    return( handle );
}

_WCRTLINK int close( int handle )
{
    FreeMap( handle );
    return( RdosCloseHandle( handle ) );
}

_WCRTLINK int dup( int old_handle )
{
    int         handle;

    handle = RdosDupHandle( old_handle );
    if( handle > 0 )
        CreateMap( handle );
    return( handle );
}

_WCRTLINK int dup2( int handle1, int handle2 )
{
    int                 handle;

    FreeMap( handle2 );
    handle = RdosDup2Handle( handle1, handle2 );

    if( handle > 0 )
        CreateMap( handle );

    return( handle );
}

_WCRTLINK int _eof( int handle )
{
    return( RdosEofHandle( handle ) );
}

_WCRTLINK long long _filelength( int handle )
{
    return( RdosGetHandleSize( handle ) );
}

_WCRTLINK int _chsize( int handle, long long size )
{
    return( RdosSetHandleSize( handle, size ) );
}

_WCRTLINK int fstat( int handle, struct stat *buf )
{
    unsigned long       msb;
    unsigned long       lsb;
    int                 ms;
    int                 us;
    struct tm           tm;
    int                 res = -1;

    buf->st_attr = 0;
    buf->st_archivedID = 0;
    buf->st_updatedID = 0;
    buf->st_inheritedRightsMask = 0;
    buf->st_originatingNameSpace = 0;
    buf->st_nlink = 1;
    buf->st_uid = buf->st_gid = 0;
    buf->st_ino = handle;
    buf->st_mode = 0;
    buf->st_mode |= S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP | S_IWOTH;

    if( RdosIsHandleDevice( handle ) ) {
        res = 0;
        buf->st_dev = 1;
        buf->st_size = 0;
        buf->st_atime = buf->st_ctime = buf->st_mtime = 0;
        buf->st_btime = buf->st_mtime;
        buf->st_mode |= S_IFCHR;
    } else {
        res = RdosGetHandleAccessTime( handle, &msb, &lsb );
        if( res >= 0 ) {
            RdosDecodeMsbTics( msb,
                               &tm.tm_year,
                               &tm.tm_mon,
                               &tm.tm_mday,
                               &tm.tm_hour );

            RdosDecodeLsbTics( lsb,
                               &tm.tm_min,
                               &tm.tm_sec,
                               &ms,
                               &us );

            tm.tm_year -= 1900;
            tm.tm_mon--;
            tm.tm_isdst = -1;
            tm.tm_wday = -1;
            tm.tm_yday = -1;

            buf->st_mtime = mktime( &tm );
            buf->st_atime = buf->st_ctime = buf->st_mtime;
            buf->st_size = _filelength( handle );
            buf->st_mode |= S_IFREG;
            buf->st_btime = buf->st_mtime;
        }
    }

    return( res );
}

_WCRTLINK int _setmode( int handle, int mode )
{
    return( mode );
}

_WCRTLINK int fsync( int handle )
{
    return( -1 );
}

_WCRTLINK int lock( int handle, unsigned long offset, unsigned long nbytes )
{
    return( -1 );
}

_WCRTLINK int unlock( int handle, unsigned long offset, unsigned long nbytes )
{
    return( -1 );
}

_WCRTLINK int (locking)( int handle, int mode, unsigned long nbytes )
{
    return( -1 );
}

_WCRTLINK off_t lseek( int handle, off_t offset, int origin )
{
    off_t       pos;

    switch( origin ) {
    case SEEK_SET:
        RdosSetHandlePos( handle, offset );
        break;

    case SEEK_CUR:
        pos = RdosGetHandlePos( handle );
        pos += offset;
        RdosSetHandlePos( handle, pos );
        break;

    case SEEK_END:
        pos = RdosGetHandleSize( handle );
        pos += offset;
        RdosSetHandlePos( handle, pos );
        break;
    }

    return( RdosGetHandlePos( handle ) );
}

_WCRTLINK off_t _tell( int handle )
{
    return( RdosGetHandlePos( handle ) );
}

static int vfs_find( int handle, struct HandleMap *hm, long long Pos )
{
    struct RdosFileMap *map = hm->map;
    int                Step = 0x80;
    int                Curr = 0;
    unsigned char      index;
    long long          Diff;

    for( ;; )
    {
        if( map->Update )
            RdosUpdateHandle( handle );

        index = map->SortedArr[Curr + Step];
        if( index != 0xFF ) {
            Diff = Pos - map->MapArr[index].Pos;
            if( Diff >= 0 ) {
                Curr += Step;

                if( Diff < map->MapArr[index].Size ) {
                    return( Curr );
                }
            }
        }
        if( Step ) {
            Step = Step >> 1;
        } else {
            break;
        }
    }
    return( -1 );
}

static int vfs_read_one( int handle, int index, char *buf, long long pos, int size )
{
    struct HandleMap        *hm = MapArr[handle];
    struct RdosFileMap      *map = hm->map;
    int                      i;
    int                      diff;
    int                      count = 0;
    char                    *src;
    struct RdosFileMapEntry *entry;

    i = map->SortedArr[index];

    if( i >= 0 ) {
        entry = &map->MapArr[i];
        diff = pos - entry->Pos;

        if( ((long)entry->Base & 0xFFF)
          || (entry->Size & 0xFFF) ) {
            map->Handle->PosArr[hm->handle - 1] = pos;
            count = RdosReadHandle( handle, buf, size );
        } else {
            if( entry->Base
              && diff >= 0 ) {
                count = entry->Size - diff;

                if( count > 0 ) {
                    src = entry->Base + diff;
                    if( count > size )
                        count = size;

                    memcpy( buf, src, count );
                } else {
                    count = 0;
                }
            }
        }
    }

    return( count );
}

static int vfs_read( int handle, void *buffer, unsigned len )
{
    struct HandleMap        *hm = MapArr[handle];
    struct RdosFileMap      *map = hm->map;
    long long                Pos = map->Handle->PosArr[hm->handle - 1];
    long long                TotalSize = map->Info->CurrSize;
    int                      Size = len;
    int                      count;
    int                      diff;
    int                      i;
    int                      ret = 0;
    char                    *ptr = (char *)buffer;

    if( map->Update )
        RdosUpdateHandle( handle );

    if( Pos + Size > TotalSize )
        Size = TotalSize - Pos;

    if( Size < 0 )
        Size = 0;

    RdosEnterFutex( &map->Handle->Futex );

    if( hm->index < 0 )
        hm->index = vfs_find( handle, hm, Pos );

    while( Size ) {
        if( hm->index >= 0 ) {
            count = vfs_read_one(handle, hm->index, ptr, Pos, Size);
            ptr += count;
            Size -= count;
            ret += count;
            Pos += count;
        }

        if( Size ) {
            for( i = 0; i < 10; i++ ) {
                RdosLeaveFutex(&map->Handle->Futex);

                RdosMapHandle( handle, Pos, Size );

                RdosEnterFutex( &map->Handle->Futex );
                hm->index = vfs_find( handle, hm, Pos );
                if( hm->index >= 0 ) {
                    break;
                }
            }

            if( hm->index < 0 ) {
                break;
            }
        }
    }

    RdosLeaveFutex( &map->Handle->Futex );
    map->Handle->PosArr[hm->handle - 1] = Pos;

    return( ret );
}

static int vfs_write_one( int handle, int index, const char *buf, long long pos, int size )
{
    struct HandleMap        *hm = MapArr[handle];
    struct RdosFileMap      *map = hm->map;
    int                      i;
    int                      diff;
    int                      count = 0;
    char                    *dst;
    struct RdosFileMapEntry *entry;
    long long                FileSize;

    i = map->SortedArr[index];

    if( i >= 0 ) {
        entry = &map->MapArr[i];
        diff = pos - entry->Pos;

        if( ((long)entry->Base & 0xFFF)
          || (entry->Size & 0xFFF) ) {
            map->Handle->PosArr[hm->handle - 1] = pos;
            count = RdosWriteHandle( handle, buf, size );
        } else {
            if( entry->Base
              && diff >= 0 ) {
                count = entry->Size - diff;

                if( count > 0 ) {
                    dst = entry->Base + diff;
                    if( count > size )
                        count = size;

                    memcpy( dst, buf, count );

                    FileSize = pos + count;
                    if( FileSize > map->Handle->ReqSize ) {
                        map->Handle->ReqSize = FileSize;
                    }
                } else {
                    count = 0;
                }
            }
        }
    }

    return( count );
}

static int vfs_write( int handle, const void *buffer, unsigned len )
{
    struct HandleMap        *hm = MapArr[handle];
    struct RdosFileMap      *map = hm->map;
    long long                Pos = map->Handle->PosArr[hm->handle - 1];
    long long                TotalSize = map->Info->CurrSize;
    int                      Size = len;
    int                      count;
    int                      diff;
    int                      i;
    int                      ret = 0;
    const char              *ptr = (const char *)buffer;
    struct RdosFileInfo     *info = map->Info;
    long long                Grow;

    if( map->Update )
        RdosUpdateHandle( handle );

    Grow = Pos + Size - info->DiscSize;

    if( Grow > 0 )
        RdosGrowHandle( handle, info->DiscSize, Grow );

    RdosEnterFutex( &map->Handle->Futex );

    if( hm->index < 0
      || Grow > 0 )
        hm->index = vfs_find( handle, hm, Pos );

    while( Size ) {
        if( hm->index >= 0 ) {
            count = vfs_write_one( handle, hm->index, ptr, Pos, Size );
            ptr += count;
            Size -= count;
            ret += count;
            Pos += count;
        }

        if( Size ) {
            for( i = 0; i < 10; i++ ) {
                RdosLeaveFutex( &map->Handle->Futex );

                Grow = Pos + Size - info->DiscSize;

                if( Grow > 0 ) {
                    RdosGrowHandle( handle, info->DiscSize, Grow );
                } else {
                    RdosMapHandle( handle, Pos, Size );
                }
                RdosEnterFutex( &map->Handle->Futex );
                hm->index = vfs_find( handle, hm, Pos );
                if( hm->index >= 0 ) {
                    break;
                }
            }

            if( hm->index < 0 ) {
                break;
            }
        }
    }

    RdosLeaveFutex( &map->Handle->Futex );
    map->Handle->PosArr[hm->handle - 1] = Pos;

    return( ret );
}

int _WCNEAR __qread( int handle, void *buffer, unsigned len )
{
    if( handle >= 0
      && handle < MapCount ) {
        if( MapArr[handle] ) {
            return( vfs_read( handle, buffer, len ) );
        }
    }
    return( RdosReadHandle( handle, buffer, len ) );
}

int _WCNEAR __qwrite( int handle, const void *buffer, unsigned len )
{
    if( handle >= 0
      && handle < MapCount ) {
        if( MapArr[handle] ) {
            return( vfs_write( handle, buffer, len ) );
        }
    }
    return( RdosWriteHandle( handle, buffer, len ) );
}

_WCRTLINK int read( int handle, void *buffer, unsigned len )
{
    if( handle >= 0
      && handle < MapCount ) {
        if( MapArr[handle] ) {
            return( vfs_read( handle, buffer, len ) );
        }
    }
    return( RdosReadHandle( handle, buffer, len ) );
}

_WCRTLINK int write( int handle, const void *buffer, unsigned len )
{
    if( handle >= 0
      && handle < MapCount ) {
        if( MapArr[handle] ) {
            return( vfs_write( handle, buffer, len ) );
        }
    }
    return( RdosWriteHandle( handle, buffer, len ) );
}

static void init( void )
{
    int i;

    for( i = 0; i < 3; i++ ) {
        CreateMap( i );
    }
}

AXI( init, INIT_PRIORITY_RUNTIME )
