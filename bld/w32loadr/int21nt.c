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


#include <dos.h>
#include <i86.h>
#include <ctype.h>
#include <dosfunc.h>
#include <direct.h>
#define DOS_FULLPATH 0x60
#include <fcntl.h>
#include <share.h>
#define SH_MASK (SH_COMPAT | SH_DENYRW | SH_DENYWR | SH_DENYRD | SH_DENYNO)
#include <string.h>
#include "tinyio.h"

#include <windows.h>

#define CARRY_CLEAR     0
#define CARRY_SET       0x0100          /* carry bit in AH */

#define MAX_HANDLES     64              /* maximum number of file handles */
HANDLE  __FileHandleIDs[ MAX_HANDLES ];

int     MaxHandle = 20;
int     ErrorCode;
#define E_NOHANDLES     4

static unsigned long __DTA;
#define HANDLE_OF(dirp) ( *( HANDLE * )( &(((char *)(dirp))[0]) ) )
#define ATTR_OF(dirp)   ( *( DWORD * )( &(((char *)(dirp))[4]) ) )
extern  void    __GetNTDirInfo( struct find_t *dirp, LPWIN32_FIND_DATA ffb );
extern  BOOL    __NTFindNextFileWithAttr( HANDLE h, DWORD attr,
                                          LPWIN32_FIND_DATA ffb );
extern  void    PrintMsg( char *fmt,... );

void __MakeDOSDT( FILETIME *NT_stamp, unsigned short *d, unsigned short *t );
void __FromDOSDT( unsigned short d, unsigned short t, FILETIME *NT_stamp );


BOOL __open_file( union REGS *r,
                  DWORD access,
                  DWORD share,
                  DWORD create_disp,
                  DWORD fileattr )
{
    HANDLE      h;
    int         handle;

    handle = 0;
    for(;;) {
        if( __FileHandleIDs[ handle ] == 0 ) break;
        handle++;
        if( handle == MaxHandle ) {
            ErrorCode = E_NOHANDLES;    // indicate no more file handles
            return( FALSE );
        }
    }
    h = CreateFile( (LPTSTR) r->x.edx, access, share, NULL,
                        create_disp, fileattr,  NULL );
    if( h == (HANDLE)-1 )  return( FALSE );
    __FileHandleIDs[ handle ] = h;
    r->x.eax = handle;
    return( TRUE );
}

BOOL __create( union REGS *r )
{
    DWORD       desired_access;
    DWORD       attr;

    if( r->x.ecx & _A_RDONLY ) {
        desired_access = GENERIC_READ;
        attr = FILE_ATTRIBUTE_READONLY;
    } else {
        desired_access = GENERIC_READ | GENERIC_WRITE;
        attr = FILE_ATTRIBUTE_NORMAL;
    }
    if( r->x.ecx & _A_HIDDEN ) {
        attr |= FILE_ATTRIBUTE_HIDDEN;
    }
    if( r->x.ecx & _A_SYSTEM ) {
        attr |= FILE_ATTRIBUTE_SYSTEM;
    }
    return( __open_file( r, desired_access, 0, CREATE_ALWAYS, attr ) );
}

BOOL __open( union REGS *r )
{
    DWORD       desired_access;
    DWORD       share_mode;
    DWORD       create_disp;
    DWORD       attr;
    int         open_mode;
    int         share;

    open_mode = r->h.al & (O_RDONLY | O_WRONLY | O_RDWR);
    if( open_mode == O_RDONLY ) {
        create_disp = OPEN_EXISTING;
        desired_access = GENERIC_READ;
        attr = FILE_ATTRIBUTE_READONLY;
    } else if( open_mode == O_WRONLY ) {
        create_disp = OPEN_EXISTING;
        desired_access = GENERIC_WRITE;
        attr = FILE_ATTRIBUTE_NORMAL;
    } else {    // open for read/write
        create_disp = OPEN_ALWAYS;
        desired_access = GENERIC_READ | GENERIC_WRITE;
        attr = FILE_ATTRIBUTE_NORMAL;
    }
    share = r->h.al & SH_MASK;
    if( share == SH_COMPAT || share == SH_DENYNO ) {
        share_mode = FILE_SHARE_READ | FILE_SHARE_WRITE;
    } else if( share == SH_DENYWR ) {
        share_mode = FILE_SHARE_READ;
    } else if( share == SH_DENYRD ) {
        share_mode = FILE_SHARE_WRITE;
    } else {
        share_mode = 0;
    }
    attr = FILE_ATTRIBUTE_NORMAL;
    return __open_file( r, desired_access, share_mode, create_disp, attr );
}

BOOL __getch( union REGS *r )
{
    DWORD       read;
    HANDLE      h;
    DWORD       mode;
    BOOL        rc;

    h = __FileHandleIDs[ 0 ];
    GetConsoleMode( h, &mode );
    SetConsoleMode( h, 0 );
    rc = ReadFile( h, &r->h.al, 1, &read, NULL );
    SetConsoleMode( h, mode );
    return( rc );
}

BOOL __filedate( union REGS *r )
{
    BOOL        rc;
    HANDLE      h;
    FILETIME    ctime,atime,wtime;

    if( r->h.al == 0 ) {
        h = __FileHandleIDs[ r->w.bx ];
        rc = GetFileTime( h, &ctime, &atime, &wtime );
        if( rc != FALSE ) {
            __MakeDOSDT( &wtime, &r->w.dx, &r->w.cx );
        }
    } else if( r->h.al == 1 ) {                 /* 01-jun-95 */
        h = __FileHandleIDs[ r->w.bx ];
        rc = GetFileTime( h, &ctime, &atime, &wtime );
        if( rc != FALSE ) {
            __FromDOSDT( r->w.dx, r->w.cx, &wtime );
            atime = wtime;
            rc = SetFileTime( h, &ctime, &atime, &wtime );
        }
    } else {
        rc = FALSE;
    }
    return( rc );
}

BOOL __fullpath( union REGS *r )
{
    BOOL        rc;
    LPTSTR      fp;

    if( strcmp( (char *)r->x.edx, "con" ) == 0 ) {
        strcpy( (char *)r->x.ebx, "con" );
        return( TRUE );
    }
    rc = GetFullPathName( (char *)r->x.edx, r->x.ecx, (char *)r->x.ebx, &fp );
    return( rc );
}

void __MakeDOSDT( FILETIME *NT_stamp, unsigned short *d, unsigned short *t )
{
    FILETIME local_ft;

    FileTimeToLocalFileTime( NT_stamp, &local_ft );
    FileTimeToDosDateTime( &local_ft, d, t );
}

void __FromDOSDT( unsigned short d, unsigned short t, FILETIME *NT_stamp )
{
    FILETIME local_ft;

    DosDateTimeToFileTime( d, t, &local_ft );
    LocalFileTimeToFileTime( &local_ft, NT_stamp );
}

void __GetNTDirInfo( struct find_t *dirp, WIN32_FIND_DATA *ffd )
{
    __MakeDOSDT( &ffd->ftLastWriteTime, &dirp->wr_date, &dirp->wr_time );
    dirp->attrib = ffd->dwFileAttributes;
    dirp->size   = ffd->nFileSizeLow;
    strncpy( dirp->name, ffd->cFileName, NAME_MAX );
//    dirp->name[NAME_MAX] = '\0';
}

BOOL __NTFindNextFileWithAttr( HANDLE h, DWORD attr, WIN32_FIND_DATA *ffd )
{
    // include normal files unless we are searching for volume ids
    attr |= ( FILE_ATTRIBUTE_NORMAL
            | FILE_ATTRIBUTE_ARCHIVE
            | FILE_ATTRIBUTE_READONLY );
    if( attr & _A_VOLID ) {
        attr &= ~_A_VOLID;
    }
    for(;;) {
        if( ffd->dwFileAttributes == 0 ) {
            // Win95 seems to return 0 for the attributes sometimes?
            // In that case, treat as a normal file
            return( TRUE );
        }
        if( attr & ffd->dwFileAttributes ) return( TRUE );
        if( !FindNextFile( h, ffd ) ) return( FALSE );
    }
}

BOOL __findfirst( union REGS *r )
{
    BOOL                rc = FALSE;
    HANDLE              handle;
    struct find_t       *buf;
    WIN32_FIND_DATA     ffd;
    char                *p;

    buf = (struct find_t *)r->x.ebx;
    handle = FindFirstFile( (LPTSTR)r->x.edx, &ffd );
    if( handle == (HANDLE)-1 ) {
        HANDLE_OF( buf ) = (HANDLE)~0;
    } else {
        // 02-aug-95: Another problem: this time compressed files on NT3.51
        // file was backed up so that it doesn't have the archive bit on
        // file is compressed, so it doesn't have the normal bit on
        // FindFirstFile found a file matching the name
        // scan the name to see if it contains any wildcard characters
        // if and only if it contains wildcard chars, check attr
        rc = TRUE;
        p = (char *)r->x.edx;
        while( *p != '\0' ) {
            if( *p == '*' || *p == '?' ) {      // if wildcard character
                rc = __NTFindNextFileWithAttr( handle, r->x.ecx, &ffd );
                break;
            }
            ++p;
        }
        if( rc == TRUE ) {
            HANDLE_OF( buf ) = handle;
            ATTR_OF( buf ) = r->x.ecx;
            __GetNTDirInfo( buf, &ffd );
        }
    }
    return( rc );
}

BOOL __findnext( union REGS *r )
{
    BOOL                rc = FALSE;
    HANDLE              handle;
    struct find_t       *buf;
    WIN32_FIND_DATA     ffd;

    buf = (struct find_t *)r->x.edx;
    handle = HANDLE_OF( buf );
    if( handle != (HANDLE)~0 ) {
        if( r->h.al == 0 ) {            /* if FIND_NEXT function */
            if( FindNextFile( handle, &ffd ) ) {
                if( __NTFindNextFileWithAttr( handle, ATTR_OF(buf), &ffd ) ) {
                    __GetNTDirInfo( buf, &ffd );
                    rc = TRUE;
                }
            }
        } else {                        /* FIND_CLOSE function */
            rc = FindClose( handle );
        }
    }
    return( rc );
}

BOOL __chmod( union REGS *r )
{
    BOOL        rc = FALSE;
    LONG        attr;

    if( r->h.al == 0 ) {                // get file attributes
        attr = GetFileAttributes( (LPTSTR) r->x.edx );
        if( attr != -1 ) {
            r->h.cl = attr;
            rc = TRUE;
        }
    } else if( r->h.al == 1 ) {         // set file attributes
        rc = FALSE;
    } else {
        rc = FALSE;
    }
    return( rc );
}

static int getCurrDrive( void )
{
    char        buff[MAX_PATH];

    GetCurrentDirectory( sizeof( buff ), buff );
    return( tolower( buff[0] ) - 'a' );
}

unsigned __Int21C( union REGS *r )
{
    BOOL        rc;
    HANDLE      h;
    DWORD       len;
    SYSTEMTIME  DateTime;

    ErrorCode = 0;
    switch( r->h.ah ) {
    case 6:
        h = __FileHandleIDs[ 1 ];
        rc = WriteFile( h, &r->h.dl, 1, (LPDWORD)&len, NULL );
        break;
    case 8:
        rc = __getch( r );
        break;
    case DOS_CUR_DISK:
        r->x.eax = getCurrDrive();
        rc = TRUE;
        break;
    case DOS_SET_DTA:
        __DTA = r->x.edx;       // remember DTA address
        rc = 0;
        break;
    case DOS_FIND_FIRST:
        rc = __findfirst( r );
        break;
    case DOS_FIND_NEXT:
        rc = __findnext( r );
        break;
    case DOS_CHMOD:
        rc = __chmod( r );
        break;
    case DOS_GETCWD:
        rc = GetCurrentDirectory( 256, (void *)r->x.esi );
        if( rc ) {
            // DOS int 21h function doesn't return the X:\ part of the CWD
            char *b = (char*) r->x.esi;
            size_t len = strlen( b + 3 );
            memmove( b, b + 3, len + 1 );
        }
        break;
    case DOS_GET_DATE:
        GetLocalTime( &DateTime );
        r->h.al = DateTime.wDayOfWeek;
        r->w.cx = DateTime.wYear;
        r->h.dh = DateTime.wMonth;
        r->h.dl = DateTime.wDay;
        rc = TRUE;
        break;
    case DOS_GET_TIME:
        GetLocalTime( &DateTime );
        r->h.ch = DateTime.wHour;
        r->h.cl = DateTime.wMinute;
        r->h.dh = DateTime.wSecond;
        r->h.dl = DateTime.wMilliseconds / 10;
        rc = TRUE;
        break;
    case DOS_CHDIR:                                     /* 17-may-94 */
        rc = SetCurrentDirectory( (LPTSTR) r->x.edx );
        break;
    case DOS_CREAT:
        rc = __create( r );
        break;
    case DOS_OPEN:
        rc = __open( r );
        break;
    case DOS_RENAME:
        rc = MoveFile( (LPTSTR)r->x.edx, (LPTSTR)r->x.edi );
        break;
    case DOS_CLOSE:
        h = __FileHandleIDs[ r->w.bx ];
        __FileHandleIDs[ r->w.bx ] = 0;
        rc = CloseHandle( h );
        break;
    case DOS_READ:
        h = __FileHandleIDs[ r->w.bx ];
        rc = ReadFile( h, (void *)r->x.edx, r->x.ecx, (LPDWORD)&r->x.eax, NULL );
        break;
    case DOS_WRITE:
        h = __FileHandleIDs[ r->w.bx ];
        if( r->x.ecx == 0 ) {
            r->x.eax = 0;                               /* 07-apr-94 */
            rc = SetEndOfFile( h );
        } else {
            rc = WriteFile( h, (void *)r->x.edx, r->x.ecx,
                                (LPDWORD)&r->x.eax, NULL );
        }
        break;
    case DOS_UNLINK:                    // delete a file
        rc = DeleteFile( (LPTSTR)r->x.edx );
        break;
    case DOS_LSEEK:
        h = __FileHandleIDs[ r->w.bx ];
        r->x.eax = SetFilePointer( h, (r->x.ecx << 16) + r->w.dx, 0, r->h.al );
        r->x.edx = r->x.eax >> 16;
        if( r->x.eax == ~0ul ) {
            rc = FALSE;
        } else {
            rc = TRUE;
        }
        break;
    case DOS_ALLOC_SEG:
        r->x.eax = (DWORD)LocalAlloc( LMEM_FIXED, r->x.ebx );
        if( r->x.eax == 0 )  return( CARRY_SET );
        return( CARRY_CLEAR );
    case DOS_FILE_DATE:
        rc = __filedate( r );
        break;
    case DOS_FULLPATH:
        rc = __fullpath( r );
        break;
    case DOS_EXIT:
        ExitProcess( r->x.eax );
    case DOS_IOCTL:
        if( r->h.al == 0 ) {            // Get Device Data
            r->x.edx = 0;
            h = __FileHandleIDs[ r->w.bx ];
            if( GetFileType( h ) == FILE_TYPE_CHAR ) {
                r->x.edx = TIO_CTL_DEVICE;
            }
            rc = TRUE;
            break;
        }
        // fall into default case
    default:
        PrintMsg( "Unsupported int 21h function AH=%h\r\n", r->h.ah );
        return( CARRY_SET );
    }
    if( rc != FALSE )  return( CARRY_CLEAR );
    if( ErrorCode != 0 ) {
        r->x.eax = ErrorCode;
    } else {
        r->x.eax = GetLastError();
    }
    return( CARRY_SET );
}

void __InitInt21()
{
    __FileHandleIDs[ 0 ] = GetStdHandle( STD_INPUT_HANDLE );
    __FileHandleIDs[ 1 ] = GetStdHandle( STD_OUTPUT_HANDLE );
    __FileHandleIDs[ 2 ] = GetStdHandle( STD_ERROR_HANDLE );
}
