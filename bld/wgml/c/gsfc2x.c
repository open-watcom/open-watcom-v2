/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2004-2013 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  WGML implement multi letter function &'c2x( )
****************************************************************************/

#define __STDC_WANT_LIB_EXT1__  1      /* use safer C library              */

#include "wgml.h"
#include "gvars.h"

static unsigned char hex( unsigned char c )
{
    static const unsigned char htab[16] = "0123456789ABCDEF";

    if( c < 16 ) {
        return( htab[c] );
    } else {
        return( '0' );
    }
}
/***************************************************************************/
/*  script string function &'c2x()                                         */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/* &'c2x(string):  To convert  a 'string' to its  hexadecimal representa-  */
/*    tion.                                                                */
/*                                                                         */
/* !    examples are all in EBCDIC,       ASCII shown below                */
/*      "&'c2x(abc)" ==> "818283"         "616263"                         */
/*      "&'c2x('1 A')" ==> "F140C1"       "312041"                         */
/*      "&'c2x('')" ==> ""                ""                               */
/*      "&'c2x('X',2)" ==> too many operands                               */
/*                                                                         */
/***************************************************************************/

condcode    scr_c2x( parm parms[MAX_FUN_PARMS], size_t parmcount, char * * result, int32_t ressize )
{
    char            *   pval;
    char            *   pend;

    if( parmcount != 1 ) {              // only 1 parm valid
        return( neg );
    }

    pval = parms[0].start;
    pend = parms[0].stop - 1;

    unquote_if_quoted( &pval, &pend );

    while( (pval <= pend) && (ressize > 1) ) {
        **result = hex( (unsigned)*pval >> 4 );
        *result += 1;
        **result = hex( (unsigned)*pval & 0x0f );
        *result += 1;
        **result = 0;
        ressize -= 2;
        pval++;
    }
    return( pos );
}
