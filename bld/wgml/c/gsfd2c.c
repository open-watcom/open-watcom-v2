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
* Description:  WGML implement multi letter function &'d2c( )
*
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/*  script string function &'d2c(                                          */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/* &'d2c(number<,n>):  To  convert a  decimal 'number'  to its  character  */
/*    representation of length 'n'.  The 'number' can be negative only if  */
/*    the length 'n' is specified.                                         */
/*      "&'d2c(129)" ==> "a"          The examples are in EBCDIC           */
/*      "&'d2c(129,1)" ==> "a"                                             */
/*      "&'d2c(129,2)" ==> " a"                                            */
/*      "&'d2c(-127,1)" ==> "a"                                            */
/*      "&'d2c(-127,2)" ==> " a"                                           */
/*      "&'d2c(12,0)" ==> ""                                               */
/*                                                                         */
/***************************************************************************/


condcode    scr_d2c( parm parms[MAX_FUN_PARMS], unsigned parmcount, char **result, unsigned ressize )
{
    tok_type        number;
    int             n;
    condcode        cc;
    int             number_val;
    getnum_block    gn;

    (void)ressize;

    if( parmcount < 1
      || parmcount > 2 )
        return( CC_neg );

    number_val = 0;

    number = parms[0].arg;
    if( unquote_arg( &number ) > 0 ) {
        gn.arg = number;
        gn.ignore_blanks = false;
        cc = getnum( &gn );
        if( cc != CC_pos ) {
            if( !ProcFlags.suppress_msg ) {
                xx_source_err_exit_c( ERR_FUNC_PARM, "1 (number)" );
                /* never return */
            }
            return( cc );
        }
        number_val = gn.result;
        n = 1;
        if( parmcount > 1 ) {                       // evalute length
            gn.arg = parms[1].arg;
            cc = getnum( &gn );
            if( cc != CC_pos ) {
                if( !ProcFlags.suppress_msg ) {
                    xx_source_err_exit_c( ERR_FUNC_PARM, "2 (n)" );
                    /* never return */
                }
                return( cc );
            }
            n = gn.result;
        }
        while( n > 1 && ressize > 0 ) {
            *(*result)++ = ' ';
            n--;
            ressize--;
        }
        if( n > 0 && ressize > 0 ) {
            *(*result)++ = number_val;
            ressize--;
        }
    }

    **result = '\0';

    return( CC_pos );
}
