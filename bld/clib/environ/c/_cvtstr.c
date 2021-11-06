/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  wide<->mbcs string conversion function with allocation.
*
****************************************************************************/


#include "variety.h"
#include "widechar.h"
#include <wchar.h>
#include <string.h>
#include <mbstring.h>
#if defined( __NT__ )
    #include <windows.h>
#elif defined( __OS2__ )
    #include <wos2.h>
#endif
#include "thread.h"
#include "rtdata.h"
#include "rterrno.h"
#include "liballoc.h"
#include "_tcsstr.h"
#include "_cvtstr.h"


__F_NAME(wchar_t,char) *__F_NAME(__lib_cvt_mbstowcs_errno,__lib_cvt_wcstombs_errno)( const CHAR_TYPE *in_string )
{
    __F_NAME(wchar_t,char)  *string;
    size_t                  len;

    len = _TCSLEN( in_string ); // length (in characters, not bytes)
    string = lib_malloc( len * __F_NAME(sizeof(wchar_t),MB_CUR_MAX) + __F_NAME(sizeof(wchar_t),1) );
    if( string != NULL ) {
        if( __F_NAME(mbstowcs,wcstombs)( string, in_string, len * __F_NAME(1,MB_CUR_MAX) + 1 ) != (size_t)-1 ) {
            return( string );
        }
        lib_free( string );
    }
    if( string != NULL ) {
        _RWD_errno = ERANGE;
    } else {
        _RWD_errno = ENOMEM;
    }
    return( NULL );
}
