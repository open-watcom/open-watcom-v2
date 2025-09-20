/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2025 The Open Watcom Contributors. All Rights Reserved.
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
#include "seterrno.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
#if defined(__NT__)
    #include <windows.h>
#elif defined( __OS2__ )
    #include <wos2.h>
#elif defined( __WINDOWS__ )
    #include <windows.h>
    #include "tinyio.h"
#elif defined( __DOS__ )
    #include "tinyio.h"
#endif
#include "iomode.h"
#include "fileacc.h"
#include "rtcheck.h"
#include "defwin.h"
#include "lseek.h"
#include "thread.h"
#include "stkoverf.h"


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

static int _WCNEAR zero_pad( int handle )
/***************************************/
{
#if defined(__NT__)
    HANDLE      osfh;
    DWORD       pos;
    DWORD       number_of_bytes_written;
    unsigned    write_amt;
    __i64       cur_ptr;
    __i64       end_ptr;
    char        zeroBuf[PAD_SIZE];

    osfh = __getOSHandle( handle );

    cur_ptr._64 = 0;
    pos = SetFilePointer( osfh, cur_ptr._32[0], &cur_ptr._32[1], FILE_CURRENT );
    if( pos == INVALID_SET_FILE_POINTER ) {
        // this might be OK so need check error
        if( GetLastError() != NO_ERROR ) {
            return( -1 );
        }
    }
    cur_ptr._32[0] = pos;

    end_ptr._64 = 0;
    pos = SetFilePointer( osfh, end_ptr._32[0], &end_ptr._32[1], FILE_END );
    if( pos == INVALID_SET_FILE_POINTER ) {
        // this might be OK so need check error
        if( GetLastError() != NO_ERROR ) {
            return( -1 );
        }
    }
    end_ptr._32[0] = pos;

    if( end_ptr._64 < cur_ptr._64 ) {
        unsigned long long  len;

        memset( zeroBuf, 0, PAD_SIZE );
        len = end_ptr._64 - cur_ptr._64;
        write_amt = PAD_SIZE;
        while( len > 0 ) {
            if( len < PAD_SIZE ) {
                write_amt = len;
            }
            if( WriteFile( osfh, zeroBuf, write_amt, &number_of_bytes_written, NULL ) == 0 ) {
                return( -1 );
            }
            end_ptr._64 += write_amt;
            len -= write_amt;
        }
    }

    if( cur_ptr._64 != end_ptr._64 ) {
        pos = SetFilePointer( osfh, cur_ptr._32[0], &cur_ptr._32[1], FILE_BEGIN );
        if( pos == INVALID_SET_FILE_POINTER ) {
            // this might be OK so need check error
            if( GetLastError() != NO_ERROR ) {
                return( -1 );
            }
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

    if( curPos > eodPos ) {                     /* only write if needed */
        memset( zeroBuf, 0, PAD_SIZE );         /* zero out a buffer */
        writeAmt = PAD_SIZE;
        bytesToWrite = curPos - eodPos;         /* amount to pad by */
        while( bytesToWrite > 0 ) {             /* loop until done */
            if( bytesToWrite < PAD_SIZE )
                writeAmt = (unsigned)bytesToWrite;
            rc = write( handle, zeroBuf, writeAmt );
            if( rc < 0 )
                return( rc );
            bytesToWrite -= writeAmt;           /* more bytes written */
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

static int _WCNEAR os_write( int handle, const void *buffer, unsigned len, unsigned *amt )
/****************************************************************************************/
{
    int         rc;
#if defined(__NT__)
#elif defined(__OS2__)
    APIRET      rc1;
#else
    tiny_ret_t  rc1;
#endif
#ifdef DEFAULT_WINDOWING
    LPWDATA     res;
#endif

    rc = 0;
#ifdef DEFAULT_WINDOWING
    if( _WindowsStdout != NULL
      && (res = _WindowsIsWindowedHandle( handle )) != NULL ) {
        *amt = _WindowsStdout( res, buffer, len );
    } else {
#endif
#if defined(__NT__)
        if( WriteFile( __getOSHandle( handle ), (LPCVOID)buffer, (DWORD)len, (LPDWORD)amt, NULL ) == 0 ) {
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
#ifdef DEFAULT_WINDOWING
    }
#endif
    if( *amt != len ) {
        rc = ENOSPC;
        lib_set_errno( rc );
    }
    return( rc );
}

#if defined(__WINDOWS_386__)
  static int _WCNEAR __write( int handle, const void *buffer, unsigned len )
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
    HANDLE          osfh;
    LONG            cur_ptr_low;
    LONG            cur_ptr_high;
    DWORD           error;
#elif defined(__OS2__)
    unsigned long   dummy;
    APIRET          rc1;
#else
    tiny_ret_t      rc1;
#endif

    __handle_check( handle, -1 );
    iomode_flags = __GetIOMode( handle );
    if( iomode_flags == 0 ) {
#if defined(__WINDOWS__)
        // How can we write to the handle if we never opened it? JBS
        return( _lwrite( handle, buffer, len ) );
#else
        lib_set_errno( EBADF );
        return( -1 );
#endif
    }
    if( (iomode_flags & _WRITE) == 0 ) {
        lib_set_errno( EACCES );     /* changed from EBADF to EACCES 23-feb-89 */
        return( -1 );
    }

#if defined(__NT__)
    osfh = __getOSHandle( handle );
#endif

    // put a semaphore around our writes

    _AccessFileH( handle );
    if( (iomode_flags & _APPEND)
      && (iomode_flags & _ISTTY) == 0 ) {
#if defined(__NT__)
        if( GetFileType( osfh ) == FILE_TYPE_DISK ) {
            cur_ptr_low = 0;
            cur_ptr_high = 0;
            if( SetFilePointer( osfh, cur_ptr_low, &cur_ptr_high, FILE_END ) == INVALID_SET_FILE_POINTER ) {
                // this might be OK so need check error
                error = GetLastError() ;
                if( error != NO_ERROR ) {
                    _ReleaseFileH( handle );
                    return( __set_errno_dos( error ) );
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
        __SetIOMode( handle, iomode_flags & (~_FILEEXT) );

        // It is not required to pad a file with zeroes on an NTFS file system;
        // unfortunately it is required on FAT (and probably FAT32).
        rc = zero_pad( handle );
    }

    if( rc == 0 ) {
        if( iomode_flags & _BINARY ) {  /* if binary mode */
            rc = os_write( handle, buffer, len, &len_written );
            /* end of binary mode part */
        } else {    /* text mode */
            #define BUF_SHORT    0x0080
            #define BUF_LONG     0x0200
            #define STACK_MARGIN 0x0030
            i = stackavail();
            if( i < BUF_SHORT + STACK_MARGIN ) {
                __STKOVERFLOW();    /* not enough stack space */
            }
            buf_size = BUF_LONG;
            if( i < BUF_LONG + STACK_MARGIN ) {
                buf_size = BUF_SHORT;
            }
            buf = __alloca( buf_size );
            #undef BUF_SHORT
            #undef BUF_LONG
            #undef STACK_MARGIN
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
    unsigned    total;
    unsigned    writeamt;
    unsigned    rc;

    __handle_check( handle, -1 );

    // allow file to be truncated
    if( len == 0 )
        return( __write( handle, buffer, 0 ) );

    total = 0;
    writeamt = MAXBUFF;
    while( len > 0 ) {
        if( len < MAXBUFF )
            writeamt = len;
        rc = __write( handle, buffer, writeamt );
        if( (int)rc == -1 )
            return( -1 );
        total += rc;
        if( rc != writeamt )
            break;
        len -= writeamt;
        buffer = ((const char *)buffer) + writeamt;
    }
    return( total );
}
#endif
