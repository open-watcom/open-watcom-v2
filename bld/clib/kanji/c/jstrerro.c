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
#include <jstring.h>
#include <stdio.h>
#include <stdlib.h>

/**
*
*  Name:        jstrerror -- エラーメッセージの表示
*
*  Synopsis:    string = jstrerror( errnum );
*
*               unsigned char *string;  エラーメッセージへのポインタ
*               int errnum;             ( errno に対応する)エラー番号
*
*  Description: この関数は errno に入っているエラーコード対応する
*               エラーメッセージへのポインタを返す。
*
*
*  Name:        jstrerror -- print error message
*
*  Synopsis:    string = jstrerror( errnum );
*
*               unsigned char *string;  pointer to error message
*               int errnum;             error number (corresponds to errno)
*
*  Description: This function returns a pointer to an error
*               message corresponding to the error code in errno.
*
**/


unsigned char *jsys_errlist[] = {
/* 0    EZERO           *///"No error",
                            "該当するエラーコードがありません",
/* 1    ENOENT          *///"No such file or directory",
                            "このようなファイルまたはディレクトリはありません",
/* 2    E2BIG           *///"Arg list too big",
                            "引数リストが長すぎます",
/* 3    ENOEXEC         *///"Exec format error",
                            "実行形式エラーです",
/* 4    EBADF           *///"Bad file number",
                            "ファイル番号が不適当です",
/* 5    ENOMEM          *///"Not enough memory",
                            "メモリが不足です",
/* 6    EACCES          *///"Permission denied",
                            "アクセスが禁止されています",
/* 7    EEXIST          *///"File exists",
                            "ファイルがすでに存在します",
/* 8    EXDEV           *///"Cross-device link",
                            "リンクがデバイスをまたがっています",
/* 9    EINVAL          *///"Invalid argument",
                            "引数が間違っています",
/* 10   ENFILE          *///"File table overflow",
                            "これ以上ファイルをオープンできません",
/* 11   EMFILE          *///"Too many open files",
                            "このプロセスではこれ以上ファイルをオープンできません",
/* 12   ENOSPC          *///"No space left on device",
                            "デバイスに余地がありません",
/* 13   EDOM            *///"Argument too large",
                            "数値演算関数の引数がエラーです",
/* 14   ERANGE          *///"Result too large",
                            "数値演算関数の結果が範囲を越えています",
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
};

int jsys_nerr = 40;

_WCRTLINK unsigned char *jstrerror( int errnum )
{
    if( errnum < 0 || errnum > jsys_nerr ) {
        errnum = 0;
    }
    return( jsys_errlist[errnum] );
}
