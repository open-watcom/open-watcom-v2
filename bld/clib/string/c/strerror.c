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


#include "variety.h"
#include "widechar.h"
#include <string.h>
#include <errno.h>
#include "rtdata.h"
#if defined(__NT__)
#include <windows.h>
#endif

#define UNKNOWN_ERROR   "unknown error"

#if defined(__NETWARE__) || defined(__WIDECHAR__)

extern int _WCNEAR      sys_nerr;
extern char             *sys_errlist[];
#define _sys_nerr       sys_nerr
#define _sys_errlist    sys_errlist

#else

// change this if more messages added
#define SYS_NERR_MSGS   (EILSEQ + 1)

int _WCNEAR _sys_nerr = SYS_NERR_MSGS;

char *_sys_errlist[ SYS_NERR_MSGS ] = {
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
    /* if more are added, be sure to update _sys_nerr accordingly */
};
#endif

_WCRTLINK CHAR_TYPE *__F_NAME(strerror,wcserror)( int errnum )
{
    #if defined(__WIDECHAR__ )
        static wchar_t Wide_Error_String[40];
    #endif
    char        *msg;

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

#if defined(__WIDECHAR__ )
static wchar_t Wide_Error_String[FORMAT_MESSAGE_MAX_WIDTH_MASK+1];
#else
static char Error_String[FORMAT_MESSAGE_MAX_WIDTH_MASK+1];
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
#if defined(__WIDECHAR__ )
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
#if defined(__WIDECHAR__ )
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
