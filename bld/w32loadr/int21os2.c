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
#include <dosfunc.h>
#define DOS_FULLPATH 0x60
#include <fcntl.h>
#include <share.h>
#define SH_MASK (SH_COMPAT | SH_DENYRW | SH_DENYWR | SH_DENYRD | SH_DENYNO)
#include <string.h>
#include "tinyio.h"

#define INCL_DOSFILEMGR
#define INCL_DOSDATETIME
#define INCL_DOSMEMMGR
#define INCL_DOSERRORS
#define INCL_16
#define INCL_SUB
#define INCL_ERRORS
#include <wos2.h>

#define CARRY_CLEAR     0
#define CARRY_SET       0x0100          /* carry bit in AH */

static char     _cbyte2;
static unsigned long __DTA;

extern  void    PrintMsg( char *fmt,... );

APIRET __getch( union REGS *r )
{
    KBDKEYINFO  info;

    if( _cbyte2 != 0 ) {
        r->h.al = _cbyte2;
        _cbyte2 = 0;
    } else {
        KbdCharIn( &info, 0, 0 );
        if( info.chChar == 0 || info.chChar == 0xe0 ) {
            _cbyte2 = info.chScan;
        }
        r->h.al = info.chChar;
    }
    return( 0 );
}

APIRET __filedate( union REGS *r )
{
    APIRET      rc;
    FILESTATUS  info;

    if( r->h.al == 0 ) {
        rc = DosQueryFileInfo( r->w.bx, 1, &info, sizeof( FILESTATUS ) );
        if( rc == 0 ) {
            *(FTIME *)&r->w.cx = info.ftimeLastWrite;
            *(FDATE *)&r->w.dx = info.fdateLastWrite;
        }
    } else if( r->h.al == 1 ) {                 /* 01-jun-95 */
        rc = DosQueryFileInfo( r->w.bx, 1, &info, sizeof( FILESTATUS ) );
        if( rc == 0 ) {
            info.ftimeLastWrite = *(FTIME *)&r->w.cx;
            info.fdateLastWrite = *(FDATE *)&r->w.dx;
            if( info.fdateLastAccess.day != 0 ) {
                info.ftimeLastAccess = *(FTIME *)&r->w.cx;
                info.fdateLastAccess = *(FDATE *)&r->w.dx;
            }
            rc = DosSetFileInfo( r->w.bx, 1, &info, sizeof( FILESTATUS ) );
        }
    } else {
        rc = ERROR_INVALID_FUNCTION;
    }
    return( rc );
}


#define FF_LEVEL        1
#define FF_BUFFER       FILEFINDBUF3
#define HANDLE_OF( __find )     ( *( HDIR * )( &(__find)->reserved[0] ) )

static void copydir( struct find_t *buf, FF_BUFFER *dir_buff )
{
    buf->attrib  = dir_buff->attrFile;
    buf->wr_time = *(unsigned short *)&dir_buff->ftimeLastWrite;
    buf->wr_date = *(unsigned short *)&dir_buff->fdateLastWrite;
    buf->size    = dir_buff->cbFile;
    strcpy( buf->name, dir_buff->achName );
}

APIRET __findfirst( union REGS *r )
{
    APIRET      rc;
    FF_BUFFER   dir_buff;
    HDIR        handle = ~0;
    OS_UINT     searchcount;
    struct find_t *buf;

    searchcount = 1;            /* only one at a time */
    rc = DosFindFirst( (PSZ)r->x.edx, (PHFILE)&handle, r->x.ecx,
            (PVOID)&dir_buff, sizeof( dir_buff ), &searchcount, FF_LEVEL );

    buf = (struct find_t *)r->x.ebx;
    if( rc != 0  &&  rc != ERROR_EAS_DIDNT_FIT ) {
        HANDLE_OF( buf ) = ~0;                  /* 15-feb-94 */
        return( rc );
    }
    HANDLE_OF( buf ) = handle;
    copydir( buf, &dir_buff );  /* copy in other fields */
    return( 0 );
}

APIRET __findnext( union REGS *r )
{
    APIRET      rc = 0;
    FF_BUFFER   dir_buff;
    HDIR        handle;
    OS_UINT     searchcount;
    struct find_t *buf;

    buf = (struct find_t *)r->x.edx;
    handle = HANDLE_OF( buf );
    if( handle != ~0 ) {
        if( r->h.al == 0 ) {            /* if FIND_NEXT function */
            searchcount = 1;            /* only one at a time */
            rc = DosFindNext( handle,
                (PVOID)&dir_buff, sizeof( dir_buff ), &searchcount );
            if( rc == 0 ) {
                copydir( buf, &dir_buff );      /* copy in other fields */
            }
        } else {                        /* FIND_CLOSE function */
            rc = DosFindClose( handle );
        }
    }
    return( rc );
}

APIRET __chmod( union REGS *r )
{
    APIRET      rc = 0;
    FILESTATUS  fs;

    rc = DosQueryPathInfo( (PSZ)r->x.edx, FIL_STANDARD,
                                &fs, sizeof( FILESTATUS ) );
    if( r->h.al == 0 ) {                // get file attributes
        r->h.cl = fs.attrFile;
    } else if( r->h.al == 1 ) {         // set file attributes
        rc = -1;
    } else {
        rc = -1;
    }
    return( rc );
}

unsigned __Int21C( union REGS *r )
{
    APIRET      rc;
    ULONG       actiontaken;
    unsigned    open_mode;
    unsigned    share;
    ULONG       open_flag;
    ULONG       len;
    DATETIME    DateTime;

    switch( r->h.ah ) {
    case 6:
        rc = DosWrite( 1, &r->h.dl, 1, (PULONG)&len );
        break;
    case 8:
        rc = __getch( r );
        break;
    case DOS_CUR_DISK:
        rc = DosQueryCurrentDisk( (PULONG)&r->x.eax, &len );
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
    case DOS_GETCWD:
        len = 256;
        rc = DosQueryCurrentDir( r->h.dl, (PBYTE)r->x.esi, &len );
        break;
    case DOS_GET_DATE:
        DosGetDateTime( &DateTime );
        r->h.al = DateTime.weekday;
        r->w.cx = DateTime.year;
        r->h.dh = DateTime.month;
        r->h.dl = DateTime.day;
        rc = 0;
        break;
    case DOS_GET_TIME:
        DosGetDateTime( &DateTime );
        r->h.ch = DateTime.hours;
        r->h.cl = DateTime.minutes;
        r->h.dh = DateTime.seconds;
        r->h.dl = DateTime.hundredths;
        rc = 0;
        break;
    case DOS_CHDIR:                                     /* 17-may-94 */
        rc = DosSetCurrentDir( (PSZ)r->x.edx );
        break;
    case DOS_CREAT:
        rc = DosOpen(   (PSZ)r->x.edx,          // pszFileName
                        (PHFILE)&r->x.eax,      // ppFileHandle
                        &actiontaken,           // pActionTaken
                        0,                      // ulFileSize
                        r->w.cx,                        // ulFileAttribute
                        OPEN_ACTION_CREATE_IF_NEW |
                        OPEN_ACTION_REPLACE_IF_EXISTS,     // ulOpenFlag
                        OPEN_ACCESS_READWRITE | SH_DENYNO, // ulOpenMode
                        0 );                    // ppEABuf
        break;
    case DOS_OPEN:
        open_mode = r->h.al & (O_RDONLY | O_WRONLY | O_RDWR);
        if( open_mode == O_RDONLY ) {
            open_flag = OPEN_ACTION_FAIL_IF_NEW |
                        OPEN_ACTION_OPEN_IF_EXISTS;
        } else if( open_mode == O_WRONLY ) {
            open_flag = OPEN_ACTION_FAIL_IF_NEW |
                        OPEN_ACTION_REPLACE_IF_EXISTS;
        } else {        // open for read/write
            open_flag = OPEN_ACTION_FAIL_IF_NEW |
                        OPEN_ACTION_OPEN_IF_EXISTS;
        }
        share = r->h.al & SH_MASK;
        if( share == SH_COMPAT )  share = SH_DENYNO;
        rc = DosOpen(   (PSZ)r->x.edx,          // pszFileName
                        (PHFILE)&r->x.eax,      // ppFileHandle
                        &actiontaken,           // pActionTaken
                        0,                      // ulFileSize
                        0,                      // ulFileAttribute
                        open_flag,              // ulOpenFlag
                        open_mode | share,      // ulOpenMode
                        0 );                    // ppEABuf
        if( rc == ERROR_OPEN_FAILED )  rc = ERROR_FILE_NOT_FOUND;
        break;
    case DOS_RENAME:
        rc = DosMove( (PSZ)r->x.edx, (PSZ)r->x.edi );
        break;
    case DOS_CLOSE:
        rc = DosClose( r->w.bx );
        break;
    case DOS_READ:
        rc = DosRead( r->w.bx, (void *)r->x.edx, r->x.ecx, (PULONG)&r->x.eax );
        break;
    case DOS_WRITE:
        if( r->x.ecx == 0 ) {
            DosSetFilePtr( r->w.bx, 0, FILE_CURRENT, &len );
            rc = DosSetFileSize( r->w.bx, len );
            r->x.eax = 0;                               /* 07-apr-94 */
        } else {
            rc = DosWrite( r->w.bx, (void *)r->x.edx, r->x.ecx,
                                        (PULONG)&r->x.eax );
        }
        break;
    case DOS_CHMOD:
        rc = __chmod( r );
        break;
    case DOS_UNLINK:                    // delete a file
        rc = DosDelete( (PSZ)r->x.edx );
        break;
    case DOS_LSEEK:
        rc = DosSetFilePtr( r->w.bx, (r->x.ecx << 16) + r->w.dx,
                                r->h.al, (PULONG)&r->x.eax );
        r->x.edx = r->x.eax >> 16;
        break;
    case DOS_ALLOC_SEG:
        rc = DosAllocMem( (PPVOID)&r->x.eax, r->x.ebx,
                        PAG_COMMIT | PAG_READ | PAG_WRITE | PAG_EXECUTE );
        break;
    case DOS_FILE_DATE:
        rc = __filedate( r );
        break;
    case DOS_FULLPATH:
        rc = DosQueryPathInfo( (PSZ)r->x.edx, FIL_QUERYFULLNAME,
                                (char *)r->x.ebx, r->x.ecx );
        break;
    case DOS_IOCTL:
        if( r->h.al == 0 ) {            // Get Device Data
            ULONG       handtype;
            ULONG       flagword;
            rc = DosQueryHType( r->w.bx, &handtype, &flagword );
            r->x.edx = 0;
            if( (handtype & ~HANDTYPE_NETWORK) != HANDTYPE_FILE ) {
                r->x.edx = TIO_CTL_DEVICE;
            }
            break;
        }
        // fall into default case
    case DOS_EXIT:
//      ExitProcess( r->x.eax );
    default:
        PrintMsg( "Unsupported int 21h function AH=%h\r\n", r->h.ah );
        rc = ~0ul;
        break;
    }
    if( rc == 0 )  return( CARRY_CLEAR );
    r->x.eax = rc;
    return( CARRY_SET );
}
