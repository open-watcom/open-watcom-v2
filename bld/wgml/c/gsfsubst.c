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
    int                 stringlen;
    int                 len;
    getnum_block        gn;
    char                padchar;
    char                linestr[MAX_L_AS_STR];

    if( (parmcount < 2) || (parmcount > 4) ) {
        return( neg );
    }

    pval = parms[0].a;
    pend = parms[0].e;

    unquote_if_quoted( &pval, &pend );

    stringlen = pend - pval + 1;        // length of string
    padchar = ' ';                      // default padchar
    len = 0;

    n = 0;                              // default start pos
    gn.ignore_blanks = false;

    if( parmcount > 1 ) {               // evalute start pos
        if( parms[1].e >= parms[1].a ) {// start pos specified
            gn.argstart = parms[1].a;
            gn.argstop  = parms[1].e;
            cc = getnum( &gn );
            if( (cc != pos) || (gn.result == 0) ) {
                if( !ProcFlags.suppress_msg ) {
                    g_err( err_func_parm, "2 (startpos)" );
                    if( input_cbs->fmflags & II_macro ) {
                        ultoa( input_cbs->s.m->lineno, linestr, 10 );
                        g_info( inf_mac_line, linestr, input_cbs->s.m->mac->name );
                    } else {
                        ultoa( input_cbs->s.f->lineno, linestr, 10 );
                        g_info( inf_file_line, linestr, input_cbs->s.f->filename );
                    }
                    err_count++;
                    show_include_stack();
                }
                return( cc );
            }
            n = gn.result - 1;
        }
    }

    if( parmcount > 2 ) {               // evalute length
        if( parms[2].e >= parms[2].a ) {// length specified
            gn.argstart = parms[2].a;
            gn.argstop  = parms[2].e;
            cc = getnum( &gn );
            if( (cc != pos) || (gn.result == 0) ) {
                if( !ProcFlags.suppress_msg ) {
                    g_err( err_func_parm, "3 (length)" );
                    if( input_cbs->fmflags & II_macro ) {
                        ultoa( input_cbs->s.m->lineno, linestr, 10 );
                        g_info( inf_mac_line, linestr, input_cbs->s.m->mac->name );
                    } else {
                        ultoa( input_cbs->s.f->lineno, linestr, 10 );
                        g_info( inf_file_line, linestr, input_cbs->s.f->filename );
                    }
                    err_count++;
                    show_include_stack();
                }
                return( cc );
            }
            len = gn.result;
        }
    }

    if( parmcount > 3 ) {               // isolate padchar
        if( parms[3].e >= parms[3].a ) {
            char *  pa = parms[3].a;
            char *  pe = parms[3].e;

            unquote_if_quoted( &pa, &pe );
            padchar = *pa;
        }
    }

    pval += n;                          // position to startpos
    if( len == 0 ) {                    // no length specified
        len = pend - pval + 1;          // take rest of string
        if( len < 0 ) {                 // if there is one
            len = 0;
        }
    }
    for( k = 0; k < len; k++ ) {
        if( (pval > pend) || (ressize <= 0) ) {
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
