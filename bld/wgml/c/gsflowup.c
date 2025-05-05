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
* Description:  WGML implement multi letter functions
*                                                     &'lower( )
*                                                     &'upper( )
*
****************************************************************************/


#include "wgml.h"


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

static condcode scr_lowup( parm parms[MAX_FUN_PARMS], unsigned parmcount,
                           char **result, unsigned ressize, bool upper )
{
    tok_type        string;
    condcode        cc;
    int             k;
    int             n;
    int             len;
    getnum_block    gn;

    if( parmcount < 1
      || parmcount > 3 )
        return( neg );

    string.s = parms[0].a;
    string.e = parms[0].e;

    unquote_arg( &string );

    len = string.e - string.s + 1;              // default length

    if( len <= 0 ) {                    // null string nothing to do
        **result = '\0';
        return( pos );
    }

    n   = 0;                            // default start pos
    gn.ignore_blanks = false;

    if( parmcount > 1 ) {               // evalute start pos
        if( parms[1].e >= parms[1].a ) {// start pos specified
            gn.arg.s = parms[1].a;
            gn.arg.e = parms[1].e;
            cc = getnum( &gn );
            if( (cc != pos) || (gn.result > len) ) {
                if( !ProcFlags.suppress_msg ) {
                    xx_source_err_c( err_func_parm, "2 (startpos)" );
                }
                return( cc );
            }
            n = gn.result - 1;
        }
    }

    if( parmcount > 2 ) {               // evalute length for upper
        if( parms[2].e >= parms[2].a ) {// length specified
            gn.arg.s = parms[2].a;
            gn.arg.e = parms[2].e;
            cc = getnum( &gn );
            if( (cc != pos) || (gn.result == 0) ) {
                if( !ProcFlags.suppress_msg ) {
                    xx_source_err_c( err_func_parm, "3 (length)" );
                }
                return( cc );
            }
            len = gn.result;
        }
    }

    for( k = 0; k < n && string.s <= string.e && ressize > 0; k++ ) {          // copy unchanged before startpos
        **result = *string.s++;
        *result += 1;
        ressize--;
    }

    for( k = 0; k < len && string.s <= string.e && ressize > 0; k++ ) {        // translate
        if( upper ) {
           **result = my_toupper( *string.s++ );
        } else {
           **result = my_tolower( *string.s++ );
        }
        *result += 1;
        ressize--;
    }

    for( ; string.s <= string.e && ressize > 0; string.s++ ) {     // copy unchanged
        **result = *string.s;
        *result += 1;
        ressize--;
    }

    **result = '\0';

    return( pos );
}


condcode    scr_lower( parm parms[MAX_FUN_PARMS], unsigned parmcount, char **result, unsigned ressize )
{
    return( scr_lowup( parms, parmcount, result, ressize, false ) );
}

condcode    scr_upper( parm parms[MAX_FUN_PARMS], unsigned parmcount, char **result, unsigned ressize )
{
    return( scr_lowup( parms, parmcount, result, ressize, true ) );
}

