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
#ifdef __NT__
    #include <windows.h>
    #include <winnls.h>
    #include "mbchar.h"
#endif
#include <mbstring.h>
#include "farfunc.h"



/****
***** Convert a wide character to a multibyte character.
****/

_WCRTLINK int _NEARFAR(wctomb,_fwctomb)( char _FFAR *ch, wchar_t wchar )
{
#ifdef __NT__
    int                 rc;
#endif

    /*** Catch special cases ***/
    if( ch == 0 )  return( 0 );

    /*** Convert the character ***/
    #ifdef __NT__
        rc = WideCharToMultiByte( __MBCodePage, WC_COMPOSITECHECK,
                                  (LPCWSTR)&wchar, 1, (LPSTR)ch,
                                  MB_LEN_MAX, NULL, NULL );
        if( rc != FALSE )
            return( rc );
        else
            return( -1 );
    #else                               /* OS/2 and others */
        if( wchar & 0xFF00 ) {
            ch[0] = (wchar&0xFF00) >> 8;        /* store lead byte */
            ch[1] = wchar & 0x00FF;             /* store trail byte */
            return( 2 );                        /* return size in bytes */
        } else {
            ch[0] = wchar & 0x00FF;             /* store char byte */
            return( 1 );                        /* return size in bytes */
        }
    #endif
}
