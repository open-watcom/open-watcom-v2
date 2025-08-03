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
* Description:  WGML implement multi letter function &'min( )
*
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/*  script string function &'min(                                          */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/*                                                                         */
/* &'min(number<,number ...>):  The Minimum function returns the smallest  */
/*    in a list of numbers.                                                */
/*      &'min(5,6,7,8) ==> 5                                               */
/*      &'min(-5,(5*5),0) ==> -5                                           */
/*      &'min('5','6','7') ==> 5                                           */
/*      &'min(1,&'min(2,&'min(3,4))) ==> 1                                 */
/*                                                                         */
/*  ! Maximum number count is 6                                            */
/*                                                                         */
/***************************************************************************/


condcode    scr_min( parm parms[MAX_FUN_PARMS], unsigned parmcount, char **result, unsigned ressize )
{
    tok_type        number;
    condcode        cc;
    unsigned        k;
    getnum_block    gn;
    int             minimum;

    (void)ressize;

    if( parmcount < 2
      || parmcount > 6 )
        return( CC_neg );

    gn.ignore_blanks = false;

    minimum = INT_MAX;
    for( k = 0; k < parmcount; k++ ) {
        number = parms[k].arg;
        if( unquote_arg( &number ) <= 0 ) { // null string nothing to do
            continue;                       // skip empty value
        }
        gn.arg = number;
        cc = getnum( &gn );
        if( !(cc == CC_pos || cc == CC_neg) ) {
            if( !ProcFlags.suppress_msg ) {
                xx_source_err_exit_c( ERR_FUNC_PARM, "" );
                /* never return */
            }
            return( cc );
        }
        if( minimum > gn.result ) {
            minimum = gn.result;        // new minimum
        }
    }

    *result += sprintf( *result, "%d", minimum );

    return( CC_pos );
}
