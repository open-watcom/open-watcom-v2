/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
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


#include "wgml.h"


static unsigned char hex( unsigned char c )
{
    static const unsigned char htab[] = "0123456789ABCDEF";

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

condcode    scr_c2x( parm parms[MAX_FUN_PARMS], unsigned parmcount, char **result, unsigned ressize )
{
    tok_type        string;
    unsigned char   c;

    if( parmcount < 1
      || parmcount > 1 )
        return( CC_neg );

    string = parms[0].arg;
    unquote_arg( &string );

    while( ( string.s < string.e ) && ( ressize > 0 ) ) {
        c = *string.s++;
        *(*result)++ = hex( c >> 4 );
        ressize--;
        if( ressize > 0 ) {
            *(*result)++ = hex( c & 0x0f );
            ressize--;
        }
    }
    **result = '\0';
    return( CC_pos );
}
