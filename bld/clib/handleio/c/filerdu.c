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
* Description:  Handle based file operations for RDOS.
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <share.h>
#include <time.h>
#include <rdos.h>
#include "rtdata.h"
#include "liballoc.h"
#include "rtinit.h"
#include "iomode.h"
#include "openmode.h"
#include "_rdos.h"

#define CONSOLE "CON"
#define FILE_INHERIT "[FILE]"

#define MAX_INHERIT_SPACE   0x4000

/* values for type */
#define HANDLE_TYPE_INVALID 0
#define HANDLE_TYPE_INPUT   1
#define HANDLE_TYPE_OUTPUT  2
#define HANDLE_TYPE_FILE    3

typedef struct rdos_handle_type
{
    int         rdos_handle;
    unsigned    mode;
    int         ref_count;
    char        type;
    char       *file_name;
} rdos_handle_type;

rdos_handle_type *console_in;
rdos_handle_type *console_out;
rdos_handle_type **handle_ptr;
int handle_count;
int handle_section;

static rdos_handle_type *AllocHandleObj( const char *file_name )
{
    rdos_handle_type *h;
    char *buf = 0;

    if( file_name )
        buf = _fullpath( 0, file_name, 0 );

    h = ( rdos_handle_type * )lib_malloc( sizeof( rdos_handle_type ) );
    h->rdos_handle = HANDLE_TYPE_FILE;
    h->mode = 0;
    h->type = 0;
    h->ref_count = 1;
    h->file_name = buf;
    return( h );
}

static void FreeHandleObj( rdos_handle_type *h)
{
    h->ref_count--;

    if( h->ref_count == 0 ) {
        if( h->file_name )
            lib_free( h->file_name );
        if( h->rdos_handle )
            RdosCloseFile( h->rdos_handle );
        lib_free( h );
    }
}

static void GrowHandleArr( void )
{
    int                 i;
    int                 new_count;
    rdos_handle_type  **new_ptr;

    new_count = 1 + 3 * handle_count / 2;
    new_ptr = ( rdos_handle_type ** )lib_malloc( new_count * sizeof( rdos_handle_type * ) );

    for( i = 0; i < handle_count; i++ )
        new_ptr[i] = handle_ptr[i];
    
    for( i = handle_count; i < new_count; i++ )
        new_ptr[i] = 0;

    lib_free( handle_ptr );
    handle_ptr = new_ptr;
    handle_count = new_count;
}
    
static int AllocHandleEntry( rdos_handle_type *obj )
{
    int i;

    RdosEnterSection( handle_section );

    for( i = 0; i < handle_count; i++)
        if( handle_ptr[i] == 0 )
            break;

    if( i == handle_count )
        GrowHandleArr();

    handle_ptr[i] = obj;

    RdosLeaveSection( handle_section );

    return( i );
}
    
static rdos_handle_type *FreeHandleEntry( int handle )
{
    rdos_handle_type *obj = 0;
    
    RdosEnterSection( handle_section );

    if( handle >= 0 && handle < handle_count ) {
        if( handle_ptr[handle] ) {
            obj = handle_ptr[handle];
            handle_ptr[handle] = 0;
        }
    }

    RdosLeaveSection( handle_section );

    return( obj );
}
    
static int ReplaceHandleEntry( int handle, rdos_handle_type *new_obj )
{
    int                 ok = 0;
    rdos_handle_type   *obj = 0;
    
    RdosEnterSection( handle_section );

    if( handle >= 0 && handle < handle_count ) {
        if( handle_ptr[handle] )
            obj = handle_ptr[handle];
            
        handle_ptr[handle] = new_obj;
        ok = 1;
    }

    RdosLeaveSection( handle_section );

    if( obj )
        FreeHandleObj( obj );

    return ( ok );
}
    
static char GetHandle( int handle, int *rdos_handle, unsigned *mode )
{
    char type;

    RdosEnterSection( handle_section );

    type = 0;
    *rdos_handle = 0;
    *mode = 0;
    
    if( handle >= 0 && handle < handle_count ) {
        if( handle_ptr[handle] ) {
            *rdos_handle = handle_ptr[handle]->rdos_handle;
            *mode = handle_ptr[handle]->mode;
            type = handle_ptr[handle]->type;
        }
    }
    
    RdosLeaveSection( handle_section );

    return( type );
}

static void ProcessFile( char *filename, int handle, unsigned mode )
{
    rdos_handle_type *obj;
    int rdos_handle;
    int org_handle;
    
    rdos_handle = RdosOpenFile( filename, 0 );

    if( rdos_handle ) {
        obj = AllocHandleObj( filename );
    }

    if( obj ) {
        obj->rdos_handle = rdos_handle;
        obj->mode = mode;
        obj->type = HANDLE_TYPE_FILE;
        org_handle = AllocHandleEntry( obj );
        dup2( org_handle, handle );
        close( org_handle );
    }
}

static void ProcessFileInherit( const char *inherit )
{
    const char *ptr;
    int size;
    char filename[300];
    char *fp;
    int handle;
    unsigned mode;

    size = strlen( FILE_INHERIT );
    inherit += size;
    inherit++;

    ptr = inherit;

    while( ptr && *ptr ) {
        size = 0;
        fp = filename;
        while( size < 300 && *ptr != '=' ) {
            *fp = *ptr;
            fp++;
            ptr++;
        }
        *fp = 0;
        ptr++;
        
        if( sscanf( ptr, "%d,%d", &handle, &mode ) == 2 )
            ProcessFile( filename, handle, mode );

        ptr = strchr( ptr, 0xd );        
        if( ptr )
            ptr++;
    }
}

static void ProcessInherit( const char *inherit )
{
    const char *ptr;
    int size;

    while( *inherit ) {
        ptr = strstr( inherit, FILE_INHERIT );
        if( ptr )
            ProcessFileInherit( ptr );

        size = strlen( inherit );
        inherit += size;
        inherit++;
    }
}

void InitHandle( void )
{
    int i;
    rdos_handle_type *h;
    const char *inherit;
    const char *ptr;

    handle_section = RdosCreateSection();    
    handle_count = 5;
    handle_ptr = ( rdos_handle_type ** )lib_malloc( handle_count * sizeof( rdos_handle_type * ) );

    for( i = 0; i < handle_count; i++ )
        handle_ptr[i] = 0;

    h = AllocHandleObj( 0 );
    h->type = HANDLE_TYPE_INPUT;
    h->mode = _READ;
    handle_ptr[0] = h;

    h->ref_count++;
    console_in = h;

    h = AllocHandleObj( 0 );
    h->type = HANDLE_TYPE_OUTPUT;
    h->mode = _WRITE;
    handle_ptr[1] = h;

    h->ref_count++;
    console_out = h;

    h->ref_count++;
    handle_ptr[2] = h;

    inherit = RdosGetOptions();

    if( inherit )
        ProcessInherit( inherit );
}

void FiniHandle( void )
{
    int i;

    for( i = 0; i < handle_count; i++ )
        if( handle_ptr[i] )
            FreeHandleObj( handle_ptr[i] );

    lib_free( handle_ptr );

    RdosDeleteSection( handle_section );
}

static char *AddInherit( char *ptr, int handle, rdos_handle_type *obj, int *remain )
{
    char str[40];
    int len;

    if( !obj->rdos_handle )
        return( ptr );

    if (!obj->file_name )
        return( ptr );

    if( *remain ) {
        *ptr = 0;
        (*remain)--;
    }
    else
        return( ptr );

    sprintf( str, "=%d,%d", handle, obj->mode );
    len = strlen( obj->file_name );
    len += strlen( str );

    if( len <= *remain ) {
        strcpy( ptr, obj->file_name );
        ptr += strlen( obj->file_name );
        strcpy( ptr, str );
        ptr += strlen( str );
        *ptr = 0xd;
        ptr++;
        (*remain) -= len;
    }    
    return( ptr );    
}                

char * __CreateInheritString( void ) 
{
    char               *str;
    char               *ptr;
    int                 handle;
    int                 remain;
    int                 count;
    rdos_handle_type   *obj;

    count = 0;

    str = lib_malloc( MAX_INHERIT_SPACE );

    remain = MAX_INHERIT_SPACE - 2 - sizeof( FILE_INHERIT );
    ptr = str;
    strcpy( ptr, FILE_INHERIT );
    ptr += strlen( FILE_INHERIT );
    *ptr = 0xd;
    ptr++;

    RdosEnterSection( handle_section );

    for( handle = 0; handle < handle_count; handle++ ) {
        obj = handle_ptr[handle];
        if( obj ) {
            if( obj->file_name ) {
                count++;
                ptr = AddInherit( ptr, handle, obj, &remain );                 
            }
        }
    }
    
    RdosLeaveSection( handle_section );

    if( count ) {    
        *ptr = 0;
        ptr++;
        *ptr = 0;
        return( str );
    } else {
        lib_free( str );
        return( 0 );
    }
}

_WCRTLINK int unlink( const CHAR_TYPE *filename ) 
{
    __ptr_check( filename, 0 );

    if( RdosDeleteFile( filename ) )
        return( 0 );
    else
        return( -1 );  
}

unsigned __GetIOMode( int handle )
{
    unsigned mode = 0;
    
    RdosEnterSection( handle_section );

    if( handle >= 0 && handle < handle_count )
        if( handle_ptr[handle] )
            mode = handle_ptr[handle]->mode;

    RdosLeaveSection( handle_section );

    return( mode );
}

void __SetIOMode_nogrow( int handle, unsigned value )
{
    RdosEnterSection( handle_section );

    if( handle >= 0 && handle < handle_count )
        if( handle_ptr[handle] )
            handle_ptr[handle]->mode = value;

    RdosLeaveSection( handle_section );
}

signed __SetIOMode( int handle, unsigned value )
{
    signed ret = -1;

    RdosEnterSection( handle_section );

    if( handle >= 0 && handle < handle_count )
        if( handle_ptr[handle] ) {
            handle_ptr[handle]->mode = value;
            ret = handle;
        }

    RdosLeaveSection( handle_section );

    return( ret );
}

static int open_base( const CHAR_TYPE *name, int mode )
{
    int                 handle;
    rdos_handle_type   *obj;
    int                 rdos_handle;
    int                 rwmode;
    unsigned            iomode_flags;

    rwmode = mode & OPENMODE_ACCESS_MASK;
    iomode_flags = 0;
    if( mode == O_RDWR )                iomode_flags |= _READ | _WRITE;
    if( rwmode == O_RDONLY)             iomode_flags |= _READ;
    if( rwmode == O_WRONLY)             iomode_flags |= _WRITE;
    if( mode & O_APPEND )               iomode_flags |= _APPEND;
    if( mode & (O_BINARY|O_TEXT) )
        if( mode & O_BINARY )           iomode_flags |= _BINARY;

    if( !strcmp( name, CONSOLE ) ) {
        if( mode & O_CREAT )
            obj = console_out;
        else
            obj = console_in;
        obj->ref_count++;
        handle = AllocHandleEntry( obj );
        return( handle );
    }
    else {
        if( mode & O_CREAT ) {
            if( mode & O_EXCL ) {
                rdos_handle = RdosOpenFile( name, 0 );
                if( rdos_handle ) {
                    RdosCloseFile( rdos_handle );
                    return( -1 );
                }
            }
            rdos_handle = RdosCreateFile( name, 0 );    
        }
        else
            rdos_handle = RdosOpenFile( name, 0 );    

        if( rdos_handle )
            if( mode & O_TRUNC )
                RdosSetFileSize( rdos_handle, 0 );
    
        if( rdos_handle ) {
            obj = AllocHandleObj( name );
            if( obj ) {
                obj->rdos_handle = rdos_handle;
                obj->mode = iomode_flags;
                obj->type = HANDLE_TYPE_FILE;
                handle = AllocHandleEntry( obj );
                return( handle );
            }
        }
    }
    return( -1 );
}

_WCRTLINK int creat( const CHAR_TYPE *name, mode_t pmode )
{
    unsigned mode;
    
    mode = O_CREAT | O_TRUNC;
    if( (pmode & S_IWRITE) && (pmode & S_IREAD) ) {
        mode |= O_RDWR;
    } else if( pmode & S_IWRITE ) {
        mode |= O_WRONLY;
    } else if( pmode & S_IREAD ) {
        mode |= O_RDONLY;
    } else if( !pmode ) {
        mode |= O_RDWR;
    }

    return( open_base( name, mode ) );
}

_WCRTLINK int open( const CHAR_TYPE *name, int mode, ... )
{
    int                 permission;
    va_list             args;
    
    va_start( args, mode );
    permission = va_arg( args, int );
    va_end( args );

    return( open_base( name, mode ) );
}


_WCRTLINK int sopen( const CHAR_TYPE *name, int mode, int shflag, ... )
{
    va_list             args;

    va_start( args, shflag );
    va_end( args );

    return( open_base( name, mode ) );
}

_WCRTLINK int close( int handle )
{
    rdos_handle_type   *obj;

    obj = FreeHandleEntry( handle );

    if( obj ) {
        FreeHandleObj( obj );
        return( 0 );
    }

    return( -1 );
}

_WCRTLINK int dup( int handle )
{
    rdos_handle_type   *obj = 0;

    RdosEnterSection( handle_section );

    if( handle >= 0 && handle < handle_count )
        if( handle_ptr[handle] )
            obj = handle_ptr[handle];

    if( obj )
        obj->ref_count++;
    
    RdosLeaveSection( handle_section );

    if( obj )
        return( AllocHandleEntry( obj ) );
    else
        return( -1 );
}

_WCRTLINK int dup2( int handle1, int handle2 )
{
    rdos_handle_type   *obj = 0;

    if( handle1 == handle2 )
        return( handle2 );

    if( handle2 < 0 )
        return( -1 );

    while( handle2 >= handle_count )
        GrowHandleArr( );
        
    RdosEnterSection( handle_section );

    if( handle1 >= 0 && handle1 < handle_count )
        if( handle_ptr[handle1] )
            obj = handle_ptr[handle1];

    if( obj )
        obj->ref_count++;
    
    RdosLeaveSection( handle_section );

    if( obj ) {
        if( ReplaceHandleEntry( handle2, obj ) )
            return( handle2 );
        else
            obj->ref_count--;
    }

    return( -1 );
}

_WCRTLINK int eof( int handle )
{
    char        type;
    int         rdos_handle;
    unsigned    mode;

    type = GetHandle( handle, &rdos_handle, &mode );

    switch ( type ) {
    case HANDLE_TYPE_FILE:
        if( RdosGetFileSize( rdos_handle ) == RdosGetFilePos( rdos_handle ) )
            return( 1 );
        else
            return( 0 );        

    case HANDLE_TYPE_INPUT:
        return( !RdosPollKeyboard( ) );

    case HANDLE_TYPE_OUTPUT:
        return( 1 );

    default:
        return( -1 );
    }
}

_WCRTLINK long filelength( int handle )
{
    char        type;
    int         rdos_handle;
    unsigned    mode;

    type = GetHandle( handle, &rdos_handle, &mode );

    switch ( type ) {
    case HANDLE_TYPE_FILE:
        return( RdosGetFileSize( rdos_handle ) );

    case HANDLE_TYPE_INPUT:
    case HANDLE_TYPE_OUTPUT:
        return( 0 );

    default:
        return( -1 );
    }
}

static void GrowRdosFile( int rdos_handle, long diff )
{
    char *buf;
    int wr_size;
    long curr_pos;
    long curr_size;

    buf = lib_malloc( 0x1000 );
    memset( buf, 0, 0x1000 );

    curr_pos = RdosGetFilePos( rdos_handle );
    curr_size = RdosGetFileSize( rdos_handle );
    RdosSetFilePos( rdos_handle, curr_size );
    while( diff ) {
        wr_size = diff;
        if( diff > 0x1000 ) 
            wr_size = 0x1000;
 
        RdosWriteFile( rdos_handle, buf, wr_size );
        diff -= wr_size;
    }
    RdosSetFilePos( rdos_handle, curr_pos );            
    lib_free( buf );
}

_WCRTLINK int chsize( int handle, long size )
{
    char        type;
    int         rdos_handle;
    unsigned    mode;
    long        curr_size;

    type = GetHandle( handle, &rdos_handle, &mode );

    switch ( type ) {
    case HANDLE_TYPE_FILE:
        curr_size = RdosGetFileSize( rdos_handle );
        if( size > curr_size )
            GrowRdosFile( rdos_handle, size - curr_size );
        else
            RdosSetFileSize( rdos_handle, size );
        return( 0 );

    default:
        return( -1 );
    }
}

_WCRTLINK int fstat( int handle, struct stat *buf )
{
    unsigned long       msb;
    unsigned long       lsb;
    int                 ms;
    int                 us;
    struct tm           tm;
    char                type;
    int                 rdos_handle;
    unsigned            mode;

    type = GetHandle( handle, &rdos_handle, &mode );

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

    switch ( type ) {
    case HANDLE_TYPE_FILE:
        buf->st_dev = RdosIsDevice( rdos_handle );
        buf->st_rdev = buf->st_dev;

        if( buf->st_dev ) {
            buf->st_size = 0;
            buf->st_atime = buf->st_ctime = buf->st_mtime = 0;
            buf->st_mode |= S_IFCHR;
        } else {                /* file */
            RdosGetFileTime( rdos_handle, &msb, &lsb );
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
            buf->st_size = filelength( handle );
            buf->st_mode |= S_IFREG;
        }
        buf->st_btime = buf->st_mtime;
        return( 0 );

    case HANDLE_TYPE_INPUT:
    case HANDLE_TYPE_OUTPUT:
        buf->st_rdev = buf->st_dev = 1;
        buf->st_size = 0;
        buf->st_atime = buf->st_btime = buf->st_ctime = buf->st_mtime = 0;
        buf->st_mode |= S_IFCHR;
        return( 0 );

    default:
        return( -1 );
    }    
}

_WCRTLINK int setmode( int handle, int mode )
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
    char        type;
    int         rdos_handle;
    unsigned    mode;

    type = GetHandle( handle, &rdos_handle, &mode );

    switch ( type ) {
    case HANDLE_TYPE_FILE:
        switch( origin ) {
        case SEEK_SET:
            RdosSetFilePos( rdos_handle, offset );
            break;
            
        case SEEK_CUR:
            pos = RdosGetFilePos( rdos_handle );
            pos += offset;
            RdosSetFilePos( rdos_handle, pos );
            break;

        case SEEK_END:
            pos = RdosGetFileSize( rdos_handle );
            pos += offset;
            RdosSetFilePos( rdos_handle, pos );
            break;
        }
        return( RdosGetFilePos( rdos_handle ) );            

    default:
        return( -1 );
    }
}

_WCRTLINK off_t tell( int handle )
{
    char        type;
    int         rdos_handle;
    unsigned    mode;

    type = GetHandle( handle, &rdos_handle, &mode );

    switch ( type ) {
    case HANDLE_TYPE_FILE:
        return( RdosGetFilePos( rdos_handle ) );

    default:
        return( -1 );
    }
}

int __qread( int handle, void *buffer, unsigned len )
{
    int         i;
    char       *ptr = (char *)buffer;
    char        type;
    int         rdos_handle;
    unsigned    mode;

    type = GetHandle( handle, &rdos_handle, &mode );

    switch ( type ) {
    case HANDLE_TYPE_FILE:
        return( RdosReadFile( rdos_handle, buffer, len ) );

    case HANDLE_TYPE_INPUT:
        for( i = 2; i < len; i++ ) {
            *ptr = ( char )RdosReadKeyboard( );    
            RdosWriteChar( *ptr );
            if( *ptr == 0xd ) {
                ptr++;
                *ptr = 0xa;
                RdosWriteChar( *ptr );
                break;
            }
            else
                ptr++;
        }
        return( i );

    case HANDLE_TYPE_OUTPUT:
        return( 0 );            

    default:
        return( -1 );
    }
}

int __qwrite( int handle, const void *buffer, unsigned len )
{
    char        type;
    int         rdos_handle;
    unsigned    mode;
    int         pos;
    int         size;

    type = GetHandle( handle, &rdos_handle, &mode );

    switch ( type ) {
    case HANDLE_TYPE_FILE:
        if( mode & _APPEND ) {
            size = RdosGetFileSize( rdos_handle );
            RdosSetFilePos( rdos_handle, size );
        }
        else {
            pos = RdosGetFilePos( rdos_handle );
            size = RdosGetFileSize( rdos_handle );
            if (pos > size )
                GrowRdosFile( rdos_handle, pos - size );
        }
        return( RdosWriteFile( rdos_handle, buffer, len ) );

    case HANDLE_TYPE_OUTPUT:
        RdosWriteSizeString( ( const char * )buffer, len );
        return( len );

    case HANDLE_TYPE_INPUT:
        return( 0 );

    default:
        return( -1 );
    }
}

_WCRTLINK int read( int handle, void *buffer, unsigned len )
{
    return( __qread( handle, buffer, len ) );
}

_WCRTLINK int write( int handle, const void *buffer, unsigned len )
{
    return( __qwrite( handle, buffer, len ) );
}

AXI(InitHandle,INIT_PRIORITY_LIBRARY);
AYI(FiniHandle,INIT_PRIORITY_LIBRARY);
