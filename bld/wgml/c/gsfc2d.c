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
* Description:  WGML implement multi letter function &'c2d( )
*                    2. parm not implemented
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/*  script string function &'c2d()                                         */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/* &'c2d(string<,n>):  To  convert the binary  representation of  a char-  */
/*    acter 'string'  to a decimal number.    The 'string' is  treated as  */
/*    signed when the length 'n' is included.                              */
/* ! examples are all in EBCDIC, ASCII shown below                         */
/*      &'c2d($) ==> 91             36                                     */
/*      &'c2d(a) ==> 129            97                                     */
/*      &'c2d(AA) ==> 49601      16705                                     */
/*      &'c2d('a',1) ==> -127       97 2. parm ignored                     */
/*      &'c2d('a',2) ==> 129        97 2. parm ignored                     */
/*      &'c2d('0a',2) ==> -3967     97 2. parm ignored                     */
/*      &'c2d('0a',1) ==> -127      97 2. parm ignored                     */
/*      &'c2d('X',0) ==> 0          88 2. parm ignored                     */
/*                                                                         */
/* ! 2. parameter not implemented                                          */
/*                                                                         */
/***************************************************************************/

condcode    scr_c2d( parm parms[MAX_FUN_PARMS], unsigned parmcount, char **result, unsigned ressize )
{
    tok_type        string;
    unsigned        n;
    char            linestr[NUM2STR_LENGTH + 1];
    char            *p;

    if( parmcount < 1
      || parmcount > 2 )
        return( CC_neg );

    string = parms[0].arg;
    unquote_arg( &string );

    n = 0;
    while( string.s < string.e ) {
        n *= 256;                      // ignore overflow, let it wrap around
        n += (unsigned char)*string.s;
        string.s++;
    }
    sprintf( linestr, "%d", n );
    p = linestr;
    while( *p != '\0' && ressize > 0 ) {
        *(*result)++ = *p++;
        ressize--;
    }
    return( CC_pos );
}
