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
#include <errno.h>
#include <mbstring.h>
#include <wchar.h>
#include "farfunc.h"
#include "seterrno.h"


_WCRTLINK int _NEARFAR(mbrtowc,_fmbrtowc)( wchar_t _FFAR *pwc, const char _FFAR *s, size_t n, mbstate_t _FFAR *ps )
{
    int                 rc;

    /*** Check the simple cases ***/
    if( s == NULL )  return( 0 );           /* always in initial state */
    if( n <= 0 )  return( -2 );             /* can't process nothing */

    /*** Check for a valid multibyte character ***/
    rc = _NEARFAR(mbtowc,_fmbtowc)( pwc, s, n );
    if( rc != -1 ) {
        return( rc );
    } else if( n < MB_LEN_MAX  &&  _ismbblead( *s ) ) {
        return( -2 );                       /* incomplete, possibly valid */
    } else {
        __set_errno( EILSEQ );              /* encoding error */
        return( -1 );
    }
}
