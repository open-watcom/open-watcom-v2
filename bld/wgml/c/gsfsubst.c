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
* Description:  WGML implement multi letter function &'substr( )
*
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/*  script string function &'substr(                                       */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/*                                                                         */
/* &'substr(string,n<,length<,pad>>):  The Substring function returns the  */
/*    portion of 'string' starting at  character number 'n'.   The number  */
/*    of  characters  to return  may  be  specified  in 'length'  and  it  */
/*    defaults from  character 'n' to the  end of the 'string'.    If the  */
/*    'length' is  present and it  would exceed  the total length  of the  */
/*    'string' then  string is  extended with  blanks or  the user  'pad'  */
/*    character.                                                           */
/*      "&'substr('123456789*',5)" ==> "56789*"                            */
/*      "&'substr('123456789*',5,9)" ==> "56789*   "                       */
/*      "&'substr('123456789*',5,9,'.')" ==> "56789*..."                   */
/*      "&'substr('123456789*',1,3,':')" ==> "123"                         */
/*      .se alpha = 'abcdefghijklmnopqrstuvwxyz'                           */
/*      "&'substr(&alpha,24)" ==> "xyz"                                    */
/*      "&'substr(&alpha,24,1)" ==> "x"                                    */
/*      "&'substr(&alpha,24,5)" ==> "xyz  "                                */
/*      "&'substr(&alpha,24,5,':')" ==> "xyz::"                            */
/*      "&'substr(&alpha,30,5,':')" ==> ":::::"                            */
/*      "&'substr(abcde,0,5)" ==> start column too small                   */
/*      "&'substr(abcde,1,-1)" ==> length too small                        */
/*                                                                         */
/***************************************************************************/

condcode    scr_substr( parm parms[MAX_FUN_PARMS], unsigned parmcount, char **result, unsigned ressize )
{
    tok_type        string;
    int             n;
    int             length;
    char            padchar;
    condcode        cc;
    int             k;
    int             string_len;
    getnum_block    gn;

    if( parmcount < 2
      || parmcount > 4 )
        return( CC_neg );

    string = parms[0].arg;
    string_len = unquote_arg( &string );

    gn.arg = parms[1].arg;
    gn.ignore_blanks = false;
    cc = getnum( &gn );
    if( (cc != CC_pos) || (gn.result == 0) ) {
        if( !ProcFlags.suppress_msg ) {
            xx_source_err_exit_c( ERR_FUNC_PARM, "2 (startpos)" );
            /* never return */
        }
        return( cc );
    }
    n = gn.result - 1;
    if( n > string_len ) {
        n = string_len;
    }

    length = string_len - n;    // default take rest of string
    if( parmcount > 2 ) {       // evalute length
        if( parms[2].arg.s < parms[2].arg.e ) {
            gn.arg = parms[2].arg;
            cc = getnum( &gn );
            if( (cc != CC_pos) || (gn.result == 0) ) {
                if( !ProcFlags.suppress_msg ) {
                    xx_source_err_exit_c( ERR_FUNC_PARM, "3 (length)" );
                    /* never return */
                }
                return( cc );
            }
            length = gn.result;
        }
    }

    if( length > 0 ) {
        padchar = ' ';          // default padchar
        if( parmcount > 3 ) {   // isolate padchar
            tok_type pad = parms[3].arg;
            if( unquote_arg( &pad ) > 0 ) {
                padchar = *pad.s;
            }
        }

        k = 0;
        /*
         * copy from start position
         */
        string.s += n;          // position to startpos or to string end
        while( k < length && string.s < string.e && ressize > 0 ) {
            *(*result)++ = *string.s++;
            k++;
            ressize--;
        }
        /*
         * pad to length (if necessary)
         */
        while( k < length && ressize > 0 ) {
            *(*result)++ = padchar;
            k++;
            ressize--;
        }
    }

    **result = '\0';

    return( CC_pos );
}
