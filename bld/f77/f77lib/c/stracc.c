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


//
// STRACC    : String (SCB) access routines.
//

#include "ftnstd.h"
#include "pgmacc.h"


void    TrimStr( string PGM *src, string *res ) {
//===============================================

// Set the 'res' SCB to be the RIGHT and LEFT trimmed value of the
//  'src' SCB.

    res->strptr = src->strptr;
    res->len = src->len;
    for(;;) {
        if( *res->strptr != ' ' ) break;
        if( res->len == 0 ) break;
        res->strptr++;
        res->len--;
    }
    for(;;) {
        if( res->len == 0 ) break;
        if( res->strptr[ res->len-1 ] != ' ' ) break;
        res->len--;
    }
}


void    GetStr( string *str, char *res ) {
//========================================

// Turn a FORTRAN string into a C string.

    pgm_memget( res, str->strptr, str->len );
    res[ str->len ] = NULLCHAR;
}
