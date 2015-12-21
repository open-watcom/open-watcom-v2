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
#include <mbstring.h>
#include <wchar.h>
#include "farfunc.h"


_WCRTLINK size_t _NEARFAR(wcsrtombs,_fwcsrtombs)( char _FFAR *dst, const wchar_t _FFAR * _FFAR *src, size_t len, mbstate_t _FFAR *ps )
{
    int                     bytesConverted = 0;
    const wchar_t _FFAR *   wcPtr = *src;
    unsigned char           mbc[MB_LEN_MAX+1];
    int                     rc;

    /*** Process the characters, one by one ***/
    if( dst != NULL ) {
        while( len > 0 ) {
            rc = _NEARFAR(wcrtomb,_fwcrtomb)( (char *)mbc, *wcPtr, ps );
            if( rc > 0 ) {
                if( *mbc != '\0' ) {
                    if( len >= rc ) {
                        _NEARFAR(_mbccpy,_fmbccpy)( (unsigned char _FFAR *)dst, mbc );
                        dst = (char _FFAR *)_NEARFAR(_mbsinc,_fmbsinc)( (unsigned char _FFAR *)dst );
                        wcPtr++;
                        bytesConverted += rc;
                        len -= rc;
                    } else {
                        len = 0;
                    }
                } else {
                    break;
                }
            } else {
                return( (size_t)-1 );
            }
        }
        if( *dst == '\0' ) {
            *src = NULL;
        } else {
            *src = wcPtr;
        }
    } else {
        for( ;; ) {
            rc = _NEARFAR(wcrtomb,_fwcrtomb)( (char *)mbc, *wcPtr, ps );
            if( rc > 0 ) {
                if( *mbc != '\0' ) {
                    wcPtr++;
                    bytesConverted += rc;
                } else {
                    break;
                }
            } else {
                return( (size_t)-1 );
            }
        }
    }
    return( bytesConverted );
}
