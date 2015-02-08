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
* Description:  WGML implement multi letter functions
*                                                     &'lower( )
*                                                     &'upper( )
*
****************************************************************************/

#define __STDC_WANT_LIB_EXT1__  1      /* use safer C library              */

#include "wgml.h"
#include "gvars.h"

/***************************************************************************/
/*  script string functions  &'lower() and &'upper()                       */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/*                                                                         */
/* &'lower(string<,n<,length>>):   The  Lowercase  function  returns  the  */
/*    lowercase equivalent of the 'string' operand.   The first character  */
/*    to be lowercased may be specified  by 'n' and the 'length' defaults  */
/*    to the  end of the string.    The conversion to  lowercase includes  */
/*    only the alphabetic characters.                                      */
/*      &'lower(ABC) ==> abc                                               */
/*      &'lower('Now is the time') ==> now is the time                     */
/*      ABC&'lower(TIME FLIES)890 ==> ABCtime flies890                     */
/*      &'lower(ABC)...&'lower(890) ==> abc...890                          */
/*      &'lower(ABCDEFG,3) ==> ABcdefg                                     */
/*      &'lower(ABCDEFG,3,2) ==> ABcdEFG                                   */
/*      &'lower(ONE,TWO,THREE) ==> invalid operands                        */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/*                                                                         */
/* &'upper(string<,n<,length>>):   The  Uppercase  function  returns  the  */
/*    uppercase equivalent of the 'string' operand.   The first character  */
/*    to be uppercased may be specified  by 'n' and the 'length' defaults  */
/*    to the end of the string.    The conversion to uppercase is defined  */
/*    by the Translate Uppercase (.TU) control word.                       */
/*      &'upper(abc) ==> ABC                                               */
/*      &'upper('Now is the time') ==> NOW IS THE TIME                     */
/*      abc&'upper(time flies)xyz ==> abcTIME FLIESxyz                     */
/*      &'upper(abc)...&'upper(xyz) ==> ABC...XYZ                          */
/*      &'upper(abcdefg,3) ==> abCDEFG                                     */
/*      &'upper(abcdefg,3,2) ==> abCDefg                                   */
/*      &'upper(one,two,three) ==> invalid operands                        */
/*                                                                         */
/***************************************************************************/

static condcode scr_lowup( parm parms[MAX_FUN_PARMS], size_t parmcount,
                           char * * result, int32_t ressize, bool upper )
{
    char            *   pval;
    char            *   pend;
    condcode            cc;
    int                 k;
    int                 n;
    int                 len;
    getnum_block        gn;
    char                linestr[MAX_L_AS_STR];

    if( (parmcount < 1) || (parmcount > 3) ) {
        cc = neg;
        return( cc );
    }

    pval = parms[0].start;
    pend = parms[0].stop;

    unquote_if_quoted( &pval, &pend );

    if( pend == pval ) {                // null string nothing to do
        **result = '\0';
        return( pos );
    }

    gn.ignore_blanks = false;

    n = 0;                              // default start pos

    if( parmcount > 1 ) {               // evalute start pos
        if( parms[1].stop > parms[1].start ) {// start pos specified
            gn.argstart = parms[1].start;
            gn.argstop  = parms[1].stop;
            cc = getnum( &gn );
            if( (cc != pos) || (gn.result > len) ) {
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

    len = pend - pval;                  // default length

    if( parmcount > 2 ) {               // evalute length for upper
        if( parms[2].stop > parms[2].start ) {// length specified
            gn.argstart = parms[2].start;
            gn.argstop  = parms[2].stop;
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

    for( k = 0; k < n; k++ ) {          // copy unchanged before startpos
        if( (pval >= pend) || (ressize <= 0) ) {
            break;
        }
        **result = *pval++;
        *result += 1;
        ressize--;
    }

    for( k = 0; k < len; k++ ) {        // translate
        if( (pval >= pend) || (ressize <= 0) ) {
            break;
        }
        if( upper ) {
           **result = toupper( *pval++ );
        } else {
           **result = tolower( *pval++ );
        }
        *result += 1;
        ressize--;
    }

    for( ; pval < pend; pval++ ) {     // copy unchanged
        if( ressize <= 0 ) {
            break;
        }
        **result = *pval;
        *result += 1;
        ressize--;
    }

    **result = '\0';

    return( pos );
}


condcode    scr_lower( parm parms[MAX_FUN_PARMS], size_t parmcount, char * * result, int32_t ressize )
{
    return( scr_lowup( parms, parmcount, result, ressize, 0 ) );
}

condcode    scr_upper( parm parms[MAX_FUN_PARMS], size_t parmcount, char * * result, int32_t ressize )
{
    return( scr_lowup( parms, parmcount, result, ressize, 1 ) );
}
