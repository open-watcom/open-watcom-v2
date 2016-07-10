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
* Description:  C Runtime write() and _lwrite() implementation.
*
****************************************************************************/


#include "variety.h"
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <malloc.h>
#include <stddef.h>
#include <string.h>
#if defined(__NT__) || defined(__WINDOWS__)
    #include <windows.h>
#endif
#include "rterrno.h"
#if defined( __DOS__ ) || defined( __WINDOWS__ )
    #include "tinyio.h"
#endif
#include "iomode.h"
#include "fileacc.h"
#include "rtcheck.h"
#include "seterrno.h"
#include "defwin.h"
#include "lseek.h"
#include "thread.h"

#if defined(__NT__)
#ifndef INVALID_SET_FILE_POINTER
#define INVALID_SET_FILE_POINTER 0xFFFFFFFF
#endif
#endif

/*
    The _lwrite function writes data to the specified file.

    Note: This function is provided only for compatibility with 16-bit
    versions of Windows. Win32-based applications should use the WriteFile
    function.

    UINT _lwrite(
      HFILE hFile,  // handle to file
      LPCSTR lpBuffer,  // write data buffer
      UINT uBytes   // number of bytes to write
    );
 */

extern  void    __STKOVERFLOW( void );

/*
    Win32 Note:
    Note that it is not an error to set the file pointer to a position
    beyond the end of the file. The size of the file does not increase
    until you call the SetEndOfFile, WriteFile, or WriteFileEx function. A
    write operation increases the size of the file to the file pointer
    position plus the size of the buffer written, leaving the intervening
    bytes uninitialized.  Hence, the reason for zero-padding the file
    to achieve POSIX conformance.
*/

/*
    POSIX Note:
    When writing to a file that does not support seek operations (pipe,
    device, etc.), the O_APPEND flag is effectively ignored. POSIX does
    not explicitly state this, but it is implied. Also, ESPIPE (illegal
    seek error) is not listed for write(), only pwrite(). Hence we must
    either not attempt to seek on such devices, or ignore the failures.
*/

#define PAD_SIZE 512

typedef union {
    unsigned __int64    _64;
    long                _32[2];
} __i64;

static int zero_pad( int handle )           /* 09-jan-95 */
/*******************************/
{
#if defined(__NT__)
    HANDLE      h;
    DWORD       dw_ptr;
    DWORD       dw_error;
    DWORD       number_of_bytes_written;
    unsigned    write_amt;
    __i64       cur_ptr;
    __i64       end_ptr;
    BOOL        rc;
    char        zeroBuf[PAD_SIZE];

    h = __getOSHandle( handle );
    dw_error = NO_ERROR;

    cur_ptr._64 = 0;
    dw_ptr = SetFilePointer( h, cur_ptr._32[0], &cur_ptr._32[1], FILE_CURRENT );
    if( dw_ptr == INVALID_SET_FILE_POINTER ) { // this might be OK so
        dw_error = GetLastError() ;
    }
    if( dw_error != NO_ERROR )
        return( -1 );
    cur_ptr._32[0] = dw_ptr;

    end_ptr._64 = 0;
    dw_ptr = SetFilePointer( h, end_ptr._32[0], &end_ptr._32[1], FILE_END );
    if( dw_ptr == INVALID_SET_FILE_POINTER ) { // this might be OK so
        dw_error = GetLastError() ;
    }
    if( dw_error != NO_ERROR )
        return( -1 );
    end_ptr._32[0] = dw_ptr;

    memset( zeroBuf, 0x00, PAD_SIZE );

    while( end_ptr._64 < cur_ptr._64 ) {
        if( (end_ptr._64 + PAD_SIZE) < cur_ptr._64 ) {
            write_amt = PAD_SIZE;
        } else {
            write_amt = cur_ptr._64 - end_ptr._64;
        }
        rc = WriteFile( h, zeroBuf, write_amt, &number_of_bytes_written, NULL );
        dw_error = GetLastError() ;
        if( rc == 0 )
            return( -1 );
        end_ptr._64 = end_ptr._64 + write_amt;
    }

    if( cur_ptr._64 != end_ptr._64 ) {
        dw_ptr = SetFilePointer( h, cur_ptr._32[0], &cur_ptr._32[1], FILE_BEGIN );
        if( dw_ptr == INVALID_SET_FILE_POINTER ) { // this might be OK so
            dw_error = GetLastError() ;
        }
        if( dw_error != NO_ERROR ) {
            return( -1 );
        }
    }
    return( 0 );
#else
    int         rc;
    long        curPos, eodPos;
    long        bytesToWrite;
    unsigned    writeAmt;
    char        zeroBuf[PAD_SIZE];

    // Pad with zeros due to lseek() past EOF (POSIX)
    curPos = __lseek( handle, 0L, SEEK_CUR );   /* current offset */
    if( curPos == -1 )
        return( -1 );
    eodPos = __lseek( handle, 0L, SEEK_END );   /* end of data offset */
    if( eodPos == -1 )
        return( -1 );

    if( curPos > eodPos ) {
        bytesToWrite = curPos - eodPos;         /* amount to pad by */
        if( bytesToWrite > 0 ) {                /* only write if needed */
            memset( zeroBuf, 0x00, PAD_SIZE );  /* zero out a buffer */
            do {                                /* loop until done */
                if( bytesToWrite > PAD_SIZE )
                    writeAmt = 512;
                else
                    writeAmt = (unsigned)bytesToWrite;
                rc = write( handle, zeroBuf, writeAmt );
                if( rc < 0 )
                    return( rc );
                bytesToWrite -= writeAmt;       /* more bytes written */
            } while( bytesToWrite != 0 );
        }
    } else {
        curPos = __lseek( handle, curPos, SEEK_SET );
        if( curPos == -1 ) {
            return( -1 );
        }
    }

    return( 0 );                /* return success code */
#endif
}

/*
    The os_write function returns 0 meaning no error, -1 meaning error, or
    ENOSPC meaning no space left on device.
*/

static int os_write( int handle, const void *buffer, unsigned len, unsigned *amt )
/********************************************************************************/
{
    int         rc;
#ifdef DEFAULT_WINDOWING
    LPWDATA     res;
#endif
#if defined(__NT__)
    HANDLE      h;
#elif defined(__OS2__)
    APIRET      rc1;
#else
    tiny_ret_t  rc1;
#endif

    rc = 0;
#ifdef DEFAULT_WINDOWING
    if( _WindowsStdout != 0 && (res = _WindowsIsWindowedHandle( handle )) != 0 ) {
        *amt = _WindowsStdout( res, buffer, len );
    } else
#endif
    {
#if defined(__NT__)
        h = __getOSHandle( handle );
        if( !WriteFile( h, (LPCVOID)buffer, (DWORD)len, (LPDWORD)amt, NULL ) ) {
            return( __set_errno_nt() );
        }
#elif defined(__OS2__)
        rc1 = DosWrite( handle, (PVOID)buffer, (OS_UINT)len, (OS_PUINT)amt );
        if( rc1 ) {
            return( __set_errno_dos( rc1 ) );
        }
#else
        rc1 = TinyWrite( handle, buffer, len );
        if( TINY_ERROR( rc1 ) ) {
            return( __set_errno_dos( TINY_INFO( rc1 ) ) );
        }
        *amt = TINY_LINFO( rc1 );
#endif
    }
    if( *amt != len ) {
        rc = ENOSPC;
        _RWD_errno = rc;
    }
    return( rc );
}

#if defined(__WINDOWS_386__)
  static int __write( int handle, const void *buffer, unsigned len )
#else
  _WCRTLINK int write( int handle, const void *buffer, unsigned len )
#endif
/**********************************************************************/
{
    unsigned        iomode_flags;
    char            *buf;
    unsigned        buf_size;
    unsigned        len_written, i, j;
    int             rc;
#if defined(__NT__)
    HANDLE          h;
    LONG            cur_ptr_low;
    LONG            cur_ptr_high;
    DWORD           rc1;
#elif defined(__OS2__)
    unsigned long   dummy;
    APIRET          rc1;
#else
    tiny_ret_t      rc1;
#endif

    __handle_check( handle, -1 );
    iomode_flags = __GetIOMode( handle );
    if( iomode_flags == 0 ) {
#if defined(__WINDOWS__) || defined(__WINDOWS_386__)
        // How can we write to the handle if we never opened it? JBS
        return( _lwrite( handle, buffer, len ) );
#else
        _RWD_errno = EBADF;
        return( -1 );
#endif
    }
    if( (iomode_flags & _WRITE) == 0 ) {
        _RWD_errno = EACCES;     /* changed from EBADF to EACCES 23-feb-89 */
        return( -1 );
    }

#if defined(__NT__)
    h = __getOSHandle( handle );
#endif

    // put a semaphore around our writes

    _AccessFileH( handle );
    if( (iomode_flags & _APPEND) && (iomode_flags & _ISTTY) == 0 ) {
#if defined(__NT__)
        if( GetFileType( h ) == FILE_TYPE_DISK ) {
            cur_ptr_low = 0;
            cur_ptr_high = 0;
            rc1 = SetFilePointer( h, cur_ptr_low, &cur_ptr_high, FILE_END );
            if( rc1 == INVALID_SET_FILE_POINTER ) { // this might be OK so
                if( GetLastError() != NO_ERROR ) {
                    _ReleaseFileH( handle );
                    return( __set_errno_nt() );
                }
            }
        }
#elif defined(__OS2__)
        rc1 = DosChgFilePtr( handle, 0L, SEEK_END, &dummy );
        // should we explicitly ignore ERROR_SEEK_ON_DEVICE here?
        if( rc1 ) {
            _ReleaseFileH( handle );
            return( __set_errno_dos( rc1 ) );
        }
#else
        rc1 = TinySeek( handle, 0L, SEEK_END );
        if( TINY_ERROR( rc1 ) ) {
            _ReleaseFileH( handle );
            return( __set_errno_dos( TINY_INFO( rc1 ) ) );
        }
#endif
    }

    len_written = 0;
    rc = 0;

    // Pad the file with zeros if necessary
    if( iomode_flags & _FILEEXT ) {
        // turn off file extended flag
        __SetIOMode_nogrow( handle, iomode_flags&(~_FILEEXT) );

        // It is not required to pad a file with zeroes on an NTFS file system;
        // unfortunately it is required on FAT (and probably FAT32). (JBS)
        rc = zero_pad( handle );
    }

    if( rc == 0 ) {
        if( iomode_flags & _BINARY ) {  /* if binary mode */
            rc = os_write( handle, buffer, len, &len_written );
            /* end of binary mode part */
        } else {    /* text mode */
            i = stackavail();
            if( i < 0x00b0 ) {
                __STKOVERFLOW();    /* not enough stack space */
            }
            buf_size = 512;
            if( i < (512 + 48) ) {
                buf_size = 128;
            }
#if defined(__AXP__) || defined(__PPC__)
            buf = alloca( buf_size );
#else
            buf = __alloca( buf_size );
#endif
            j = 0;
            for( i = 0; i < len; ) {
                if( ((const char*)buffer)[i] == '\n' ) {
                    buf[j] = '\r';
                    ++j;
                    if( j == buf_size ) {
                        rc = os_write( handle, buf, buf_size, &j );
                        if( rc == -1 )
                            break;
                        len_written += j;
                        if( rc == ENOSPC )
                            break;
                        len_written = i;
                        j = 0;
                    }
                }
                buf[j] = ((const char*)buffer)[i];
                ++i;
                ++j;
                if( j == buf_size ) {
                    rc = os_write( handle, buf, buf_size, &j );
                    if( rc == -1 )
                        break;
                    len_written += j;
                    if( rc == ENOSPC )
                        break;
                    len_written = i;
                    j = 0;
                }
            }
            if( j ) {
                rc = os_write( handle, buf, j, &i );
                if( rc == ENOSPC ) {
                    len_written += i;
                } else {
                    len_written = len;
                }
            }
            /* end of text mode part */
        }
    }
    _ReleaseFileH( handle );
    if( rc == -1 ) {
        return( rc );
    } else {
        return( len_written );
    }
}


#if defined(__WINDOWS_386__)
#define MAXBUFF 0x8000
_WCRTLINK int write( int handle, const void *buffer, unsigned len )
/*****************************************************************/
{
    unsigned    total = 0;
    unsigned    writeamt;
    int         rc;

    __handle_check( handle, -1 );

    // allow file to be truncated
    if( len == 0 )
        return( __write( handle, buffer, 0 ) );

    while( len > 0 ) {
        if( len > MAXBUFF ) {
            writeamt = MAXBUFF;
        } else {
            writeamt = len;
        }
        rc = __write( handle, buffer, writeamt );
        if( rc == -1 )
            return( rc );
        total += (unsigned)rc;
        if( rc != writeamt )
            return( total );

        len -= writeamt;
        buffer = ((const char *)buffer) + writeamt;
    }
    return( total );
}
#endif
