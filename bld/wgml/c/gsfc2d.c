/****************************************************************************
*
*                            Open Watcom Project
*
*  Copyright (c) 2004-2009 The Open Watcom Contributors. All Rights Reserved.
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

#define __STDC_WANT_LIB_EXT1__  1      /* use safer C library              */

#include "wgml.h"
#include "gvars.h"

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

condcode    scr_c2d( parm parms[MAX_FUN_PARMS], size_t parmcount, char * * result, int32_t ressize )
{
    char            *   pval;
    char            *   pend;
    condcode            cc;
    uint32_t            n;
    char                linestr[MAX_L_AS_STR];
    char            *   p;

    if( (parmcount < 1) || (parmcount > 2) ) {// accept 2. parm, but ignore it
        cc = neg;
        return( cc );
    }

    pval = parms[0].a;
    pend = parms[0].e;

    unquote_if_quoted( &pval, &pend );


    n = 0;
    while( pval <= pend ) {
        n *= 256;                      // ignore overflow, let it wrap around
        n += *pval;
        pval++;
    }
    ultoa( n, linestr, 10 );
    p = linestr;
    while( *p && ressize > 0) {
        **result = *p++;
        *result += 1;
        ressize--;
    }
    return( pos );
}
