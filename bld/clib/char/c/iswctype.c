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
* Description:  Implementation of iswctype().
*
****************************************************************************/


#include "variety.h"
#include <wctype.h>
#include "intwctyp.h"

/* Determine if the given character is of the specified type. */
_WCRTLINK int iswctype( wint_t wc, wctype_t desc )
{
    switch( desc ) {
    case WCTYPE_ALNUM:   return( iswalnum( wc ) );
    case WCTYPE_ALPHA:   return( iswalpha( wc ) );
    case WCTYPE_BLANK:   return( iswblank( wc ) );
    case WCTYPE_CNTRL:   return( iswcntrl( wc ) );
    case WCTYPE_DIGIT:   return( iswdigit( wc ) );
    case WCTYPE_GRAPH:   return( iswgraph( wc ) );
    case WCTYPE_LOWER:   return( iswlower( wc ) );
    case WCTYPE_PRINT:   return( iswprint( wc ) );
    case WCTYPE_PUNCT:   return( iswpunct( wc ) );
    case WCTYPE_SPACE:   return( iswspace( wc ) );
    case WCTYPE_UPPER:   return( iswupper( wc ) );
    case WCTYPE_XDIGIT:  return( iswxdigit( wc ) );
    default:             return( 0 );
    }
}
