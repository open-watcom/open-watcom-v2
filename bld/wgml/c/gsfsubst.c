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
* Description:  WGML implement multi letter function &'substr( )
*
****************************************************************************/

#define __STDC_WANT_LIB_EXT1__  1      /* use safer C library              */

#include "wgml.h"
#include "gvars.h"

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

condcode    scr_substr( parm parms[MAX_FUN_PARMS], size_t parmcount, char * * result, int32_t ressize )
{
    char            *   pval;
    char            *   pend;
    condcode            cc;
    int                 k;
    int                 n;
    int                 len;
    getnum_block        gn;
    char                padchar;

    if( (parmcount < 2) || (parmcount > 4) ) {
        return( neg );
    }

    pval = parms[0].start;
    pend = parms[0].stop;

    unquote_if_quoted( &pval, &pend );

    gn.ignore_blanks = false;

    n = 0;                              // default start pos

    if( parmcount > 1 ) {               // evalute start pos
        if( parms[1].stop > parms[1].start ) {// start pos specified
            gn.argstart = parms[1].start;
            gn.argstop  = parms[1].stop;
            cc = getnum( &gn );
            if( (cc != pos) || (gn.result == 0) ) {
                if( !ProcFlags.suppress_msg ) {
                    g_err( err_func_parm, "2 (startpos)" );
                    g_info_inp_pos();
                    err_count++;
                    show_include_stack();
                }
                return( cc );
            }
            n = gn.result - 1;
        }
    }

    len = 0;                            // default length

    if( parmcount > 2 ) {               // evalute length
        if( parms[2].stop > parms[2].start ) {// length specified
            gn.argstart = parms[2].start;
            gn.argstop  = parms[2].stop;
            cc = getnum( &gn );
            if( (cc != pos) || (gn.result == 0) ) {
                if( !ProcFlags.suppress_msg ) {
                    g_err( err_func_parm, "3 (length)" );
                    g_info_inp_pos();
                    err_count++;
                    show_include_stack();
                }
                return( cc );
            }
            len = gn.result;
        }
    }

    padchar = ' ';                      // default padchar

    if( parmcount > 3 ) {               // isolate padchar
        if( parms[3].stop > parms[3].start ) {
            char *pa = parms[3].start;
            char *pe = parms[3].stop;

            unquote_if_quoted( &pa, &pe );
            padchar = *pa;
        }
    }

    pval += n;                          // position to startpos
    if( len == 0 ) {                    // no length specified
        len = pend - pval;              // take rest of string
        if( len < 0 ) {                 // if there is one
            len = 0;
        }
    }
    for( k = 0; k < len; k++ ) {
        if( (pval >= pend) || (ressize <= 0) ) {
            break;
        }
        **result = *pval++;
        *result += 1;
        ressize--;
    }
    for( ; k < len; k++ ) {
        if( ressize <= 0 ) {
            break;
        }
        **result = padchar;
        *result += 1;
        ressize--;
    }

    **result = '\0';

    return( pos );
}
