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
* Description:  Implementation of strerror() function.
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "rtdata.h"
#if defined(__NT__)
    #include <windows.h>
#endif
#include "errstr.h"

#if !defined( __NETWARE__ ) && !defined( __WIDECHAR__ )

_WCRTDATA char * _WCDATA _sys_errlist[] = {
#ifdef __LINUX__
    /*  0   EOK             */ "No error",
    /*  1   EPERM           */ "Operation not permitted",
    /*  2   ENOENT          */ "No such file or directory",
    /*  3   ESRCH           */ "No such process",
    /*  4   EINTR           */ "Interrupted system call",
    /*  5   EIO             */ "I/O error",
    /*  6   ENXIO           */ "No such device or address",
    /*  7   E2BIG           */ "Arg list too long",
    /*  8   ENOEXEC         */ "Exec format error",
    /*  9   EBADF           */ "Bad file number",
    /* 10   ECHILD          */ "No child processes",
    /* 11   EAGAIN          */ "Try again",
    /* 12   ENOMEM          */ "Out of memory",
    /* 13   EACCES          */ "Permission denied",
    /* 14   EFAULT          */ "Bad address",
    /* 15   ENOTBLK         */ "Block device required",
    /* 16   EBUSY           */ "Device or resource busy",
    /* 17   EEXIST          */ "File exists",
    /* 18   EXDEV           */ "Cross-device link",
    /* 19   ENODEV          */ "No such device",
    /* 20   ENOTDIR         */ "Not a directory",
    /* 21   EISDIR          */ "Is a directory",
    /* 22   EINVAL          */ "Invalid argument",
    /* 23   ENFILE          */ "File table overflow",
    /* 24   EMFILE          */ "Too many open files",
    /* 25   ENOTTY          */ "Not a typewriter",
    /* 26   ETXTBSY         */ "Text file busy",
    /* 27   EFBIG           */ "File too large",
    /* 28   ENOSPC          */ "No space left on device",
    /* 29   ESPIPE          */ "Illegal seek",
    /* 30   EROFS           */ "Read-only file system",
    /* 31   EMLINK          */ "Too many links",
    /* 32   EPIPE           */ "Broken pipe",
    /* 33   EDOM            */ "Math argument out of domain of func",
    /* 34   ERANGE          */ "Math result not representable",
    /* 35   EDEADLK         */ "Resource deadlock would occur",
    /* 36   ENAMETOOLONG    */ "File name too long",
    /* 37   ENOLCK          */ "No record locks available",
    /* 38   ENOSYS          */ "Function not implemented",
    /* 39   ENOTEMPTY       */ "Directory not empty",
    /* 40   ELOOP           */ "Too many symbolic links encountered",
    /* 41                   */ "",
    /* 42   ENOMSG          */ "No message of desired type",
    /* 43   EIDRM           */ "Identifier removed",
    /* 44   ECHRNG          */ "Channel number out of range",
    /* 45   EL2NSYNC        */ "Level 2 not synchronized",
    /* 46   EL3HLT          */ "Level 3 halted",
    /* 47   EL3RST          */ "Level 3 reset",
    /* 48   ELNRNG          */ "Link number out of range",
    /* 49   EUNATCH         */ "Protocol driver not attached",
    /* 50   ENOCSI          */ "No CSI structure available",
    /* 51   EL2HLT          */ "Level 2 halted",
    /* 52   EBADE           */ "Invalid exchange",
    /* 53   EBADR           */ "Invalid request descriptor",
    /* 54   EXFULL          */ "Exchange full",
    /* 55   ENOANO          */ "No anode",
    /* 56   EBADRQC         */ "Invalid request code",
    /* 57   EBADSLT         */ "Invalid slot",
    /* 58                   */ "",
    /* 59   EBFONT          */ "Bad font file format",
    /* 60   ENOSTR          */ "Device not a stream",
    /* 61   ENODATA         */ "No data available",
    /* 62   ETIME           */ "Timer expired",
    /* 63   ENOSR           */ "Out of streams resources",
    /* 64   ENONET          */ "Machine is not on the network",
    /* 65   ENOPKG          */ "Package not installed",
    /* 66   EREMOTE         */ "Object is remote",
    /* 67   ENOLINK         */ "Link has been severed",
    /* 68   EADV            */ "Advertise error",
    /* 69   ESRMNT          */ "Srmount error",
    /* 70   ECOMM           */ "Communication error on send",
    /* 71   EPROTO          */ "Protocol error",
    /* 72   EMULTIHOP       */ "Multihop attempted",
    /* 73   EDOTDOT         */ "RFS specific error",
    /* 74   EBADMSG         */ "Not a data message",
    /* 75   EOVERFLOW       */ "Value too large for defined data type",
    /* 76   ENOTUNIQ        */ "Name not unique on network",
    /* 77   EBADFD          */ "File descriptor in bad state",
    /* 78   EREMCHG         */ "Remote address changed",
    /* 79   ELIBACC         */ "Can not access a needed shared library",
    /* 80   ELIBBAD         */ "Accessing a corrupted shared library",
    /* 81   ELIBSCN         */ ".lib section in a.out corrupted",
    /* 82   ELIBMAX         */ "Attempting to link in too many shared libraries",
    /* 83   ELIBEXEC        */ "Cannot exec a shared library directly",
    /* 84   EILSEQ          */ "Illegal byte sequence",
    /* 85   ERESTART        */ "Interrupted system call should be restarted",
    /* 86   ESTRPIPE        */ "Streams pipe error",
    /* 87   EUSERS          */ "Too many users",
    /* 88   ENOTSOCK        */ "Socket operation on non-socket",
    /* 89   EDESTADDRREQ    */ "Destination address required",
    /* 90   EMSGSIZE        */ "Message too long",
    /* 91   EPROTOTYPE      */ "Protocol wrong type for socket",
    /* 92   ENOPROTOOPT     */ "Protocol not available",
    /* 93   EPROTONOSUPPORT */ "Protocol not supported",
    /* 94   ESOCKTNOSUPPORT */ "Socket type not supported",
    /* 95   EOPNOTSUPP      */ "Operation not supported on transport endpoint",
    /* 96   EPFNOSUPPORT    */ "Protocol family not supported",
    /* 97   EAFNOSUPPORT    */ "Address family not supported by protocol",
    /* 98   EADDRINUSE      */ "Address already in use",
    /* 99   EADDRNOTAVAIL   */ "Cannot assign requested address",
    /* 100  ENETDOWN        */ "Network is down",
    /* 101  ENETUNREACH     */ "Network is unreachable",
    /* 102  ENETRESET       */ "Network dropped connection because of reset",
    /* 103  ECONNABORTED    */ "Software caused connection abort",
    /* 104  ECONNRESET      */ "Connection reset by peer",
    /* 105  ENOBUFS         */ "No buffer space available",
    /* 106  EISCONN         */ "Transport endpoint is already connected",
    /* 107  ENOTCONN        */ "Transport endpoint is not connected",
    /* 108  ESHUTDOWN       */ "Cannot send after transport endpoint shutdown",
    /* 109  ETOOMANYREFS    */ "Too many references: cannot splice",
    /* 110  ETIMEDOUT       */ "Connection timed out",
    /* 111  ECONNREFUSED    */ "Connection refused",
    /* 112  EHOSTDOWN       */ "Host is down",
    /* 113  EHOSTUNREACH    */ "No route to host",
    /* 114  EALREADY        */ "Operation already in progress",
    /* 115  EINPROGRESS     */ "Operation now in progress",
    /* 116  ESTALE          */ "Stale NFS file handle",
    /* 117  EUCLEAN         */ "Structure needs cleaning",
    /* 118  ENOTNAM         */ "Not a XENIX named type file",
    /* 119  ENAVAIL         */ "No XENIX semaphores available",
    /* 120  EISNAM          */ "Is a named type file",
    /* 121  EREMOTEIO       */ "Remote I/O error",
    /* 122  EDQUOT          */ "Quota exceeded",
    /* 121                  */ "",
    /* 123  ENOMEDIUM       */ "No medium found",
    /* 124  EMEDIUMTYPE     */ "Wrong medium type"
#else
    /* 0    EZERO           */  "No error",
    /* 1    ENOENT          */  "No such file or directory",
    /* 2    E2BIG           */  "Arg list too big",
    /* 3    ENOEXEC         */  "Exec format error",
    /* 4    EBADF           */  "Bad file number",
    /* 5    ENOMEM          */  "Not enough memory",
    /* 6    EACCES          */  "Permission denied",
    /* 7    EEXIST          */  "File exists",
    /* 8    EXDEV           */  "Cross-device link",
    /* 9    EINVAL          */  "Invalid argument",
    /* 10   ENFILE          */  "File table overflow",
    /* 11   EMFILE          */  "Too many open files",
    /* 12   ENOSPC          */  "No space left on device",
    /* 13   EDOM            */  "Argument too large",
    /* 14   ERANGE          */  "Result too large",
    /* 15   EDEADLK         */  "Resource deadlock would occur",
    /* 16   EINTR           */  "System call interrupted",
    /* 17   ECHILD          */  "Child does not exist",
    /* 18   EAGAIN          */  "Resource unavailable, try again",
    /* 19   EBUSY           */  "Device or resource busy",
    /* 20   EFBIG           */  "File too large",
    /* 21   EIO             */  "I/O error",
    /* 22   EISDIR          */  "Is a directory",
    /* 23   ENOTDIR         */  "Not a directory",
    /* 24   EMLINK          */  "Too many links",
    /* 25   ENOTBLK         */  "Block device required",
    /* 26   ENOTTY          */  "Not a character device",
    /* 27   ENXIO           */  "No such device or address",
    /* 28   EPERM           */  "Not owner",
    /* 29   EPIPE           */  "Broken pipe",
    /* 30   EROFS           */  "Read-only file system",
    /* 31   ESPIPE          */  "Illegal seek",
    /* 32   ESRCH           */  "No such process",
    /* 33   ETXTBSY         */  "Text file busy",
    /* 34   EFAULT          */  "Bad address",
    /* 35   ENAMETOOLONG    */  "Filename too long",
    /* 36   ENODEV          */  "No such device",
    /* 37   ENOLCK          */  "No locks available in system",
    /* 38   ENOSYS          */  "Unknown system call",
    /* 39   ENOTEMPTY       */  "Directory not empty",
    /* 40   EILSEQ          */  "Illegal multibyte sequence"
#endif
};

_WCRTDATA int _WCDATA _sys_nerr = ( sizeof( _sys_errlist ) / sizeof( *_sys_errlist ) );

#endif

_WCRTLINK CHAR_TYPE *__F_NAME(strerror,wcserror)( int errnum )
{
#ifdef __WIDECHAR__
    static wchar_t  Wide_Error_String[40];
#endif
    char            *msg;

    if( errnum < 0 || errnum >= _sys_nerr ) {
        msg = UNKNOWN_ERROR;
    } else {
        msg = _sys_errlist[ errnum ];
    }
    return( _AToUni( Wide_Error_String, msg ) );
}

// Note: Windows FORMAT_MESSAGE_MAX_WIDTH_MASK is 255

#if !defined(FORMAT_MESSAGE_MAX_WIDTH_MASK)
    #define FORMAT_MESSAGE_MAX_WIDTH_MASK 255
#endif

#ifdef __WIDECHAR__
static  wchar_t Wide_Error_String[FORMAT_MESSAGE_MAX_WIDTH_MASK+1];
#else
static  char    Error_String[FORMAT_MESSAGE_MAX_WIDTH_MASK+1];
#endif


/*
    char *_strerror( const char *strErrMsg );

    Description from the MSDN:

    If strErrMsg is passed as NULL, _strerror returns a pointer to a
    string containing the system error message for the last library call
    that produced an error. The error-message string is terminated by
    the newline character ('\n'). If strErrMsg is not equal to NULL,
    then _strerror returns a pointer to a string containing (in order)
    your string message, a colon, a space, the system error message for
    the last library call producing an error, and a newline character.
    Your string message can be, at most, 94 bytes long.

    The actual error number for _strerror is stored in the variable
    errno. The system error messages are accessed through the variable
    _sys_errlist, which is an array of messages ordered by error number.
    _strerror accesses the appropriate error message by using the errno
    value as an index to the variable _sys_errlist. The value of the
    variable _sys_nerr is defined as the maximum number of elements in
    the _sys_errlist array. To produce accurate results, call _strerror
    immediately after a library routine returns with an error.
    Otherwise, subsequent calls to strerror or _strerror can overwrite
    the errno value.

    _strerror is not part of the ANSI definition but is instead a
    Microsoft extension to it. Do not use it where portability is
    desired; for ANSI compatibility, use strerror instead.

 */

_WCRTLINK CHAR_TYPE *__F_NAME(_strerror,_wcserror)( const CHAR_TYPE *strErrMsg )
{
    int errnum;

    errnum = _RWD_errno;
#ifdef __WIDECHAR__
    Wide_Error_String[0] = L'\0';
    if( strErrMsg != NULL ) {
        wcsncpy( Wide_Error_String, strErrMsg, 94 );
        Wide_Error_String[94] = L'\0';    // just in case more than 94
        wcscat( Wide_Error_String, L": " );
    }
    wcscat( Wide_Error_String, wcserror( errnum ) );
    wcscat( Wide_Error_String, L"\n" );
    return( Wide_Error_String );
#else
    Error_String[0] = '\0';
    if( strErrMsg != NULL ) {
        strncpy( Error_String, strErrMsg, 94 );
        Error_String[94] = '\0';    // just in case more than 94
        strcat( Error_String, ": " );
    }
    strcat( Error_String, strerror( errnum ) );
    strcat( Error_String, "\n" );
    return( Error_String );
#endif
}

#if defined(__NT__)

_WCRTLINK CHAR_TYPE *__F_NAME(_doserror,_wdoserror)( int errnum )
{
#ifdef __WIDECHAR__
    Wide_Error_String[0] = L'\0';
    FormatMessageW( FORMAT_MESSAGE_IGNORE_INSERTS |
                    FORMAT_MESSAGE_FROM_SYSTEM |
                    FORMAT_MESSAGE_MAX_WIDTH_MASK,
                    NULL,
                    errnum,
                    0,
                    Wide_Error_String,
                    FORMAT_MESSAGE_MAX_WIDTH_MASK,
                    NULL );
    return( Wide_Error_String );
#else
    Error_String[0] = '\0';
    FormatMessageA( FORMAT_MESSAGE_IGNORE_INSERTS |
                    FORMAT_MESSAGE_FROM_SYSTEM |
                    FORMAT_MESSAGE_MAX_WIDTH_MASK,
                    NULL,
                    errnum,
                    0,
                    Error_String,
                    FORMAT_MESSAGE_MAX_WIDTH_MASK,
                    NULL );
    return( Error_String );
#endif
}

#endif
