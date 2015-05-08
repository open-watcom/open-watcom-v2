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
#include <stddef.h>
#include <stdio.h>
#include "rtdata.h"
#include "seterrno.h"
#include "errorno.h"
#include "thread.h"

/**
*
*  Name:        jperror -- エラーメッセージの表示
*
*  Synopsis:    void jperror( msg );
*
*               unsigned char *msg;     ユーザーメッセージの文字列
*
*  Description: この関数は変数 errno にあるエラーコードに対応するエラー
*               メッセージを表示します。引数 msg で与えられる文字列が
*               まず、始めに表示されます。それにつづいてコロン（：）と、
*               エラーメッセージが表示されます。もしエラー番号が不適格
*               な番号であるある時には、エラーコード０のメッセージが
*               表示されます。
*
*
*  Name:        jperror -- print error message
*
*  Synopsis:    void jperror( msg );
*
*               unsigned char *msg;
*
*  Description: This function prints an error message corresponding to the
*               error code stored in errno.  The string supplied as the
*               msg argument is printed first, followed by a colon and
*               the error message.  If the error number is invalid, the
*               message for error code 0 is used.
*
**/

_WCRTLINK void jperror( const char *s )
{
    if( s != NULL && *s != '\0' ) {
        fputs( s, stderr );
        fputs( ": ", stderr );
    }
    fputs( jstrerror( _RWD_errno ), stderr );
    fputc( '\n', stderr );
}
