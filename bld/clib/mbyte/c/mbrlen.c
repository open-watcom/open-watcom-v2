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
#include <wchar.h>
#include "farfunc.h"



/****
***** Determines the number of bytes contained in the multibyte character
***** pointed to by 's'.  Returns -2 if the next 'n' bytes form an
***** incomplete but possibly valid multibyte character, -1 if an encoding
***** error occurs (in which case errno will be set to EILSEQ), 0 if the
***** next 'n' or fewer bytes form the multibyte character corresponding to
***** the wide null character, or otherwise some positive value indicating
***** the number of bytes which form the valid multibyte character.
****/

_WCRTLINK int _NEARFAR(mbrlen,_fmbrlen)( const char _FFAR *s, size_t n, mbstate_t _FFAR *ps )
{
    return( _NEARFAR(mbrtowc,_fmbrtowc)( NULL, s, n, NULL ) );
}
