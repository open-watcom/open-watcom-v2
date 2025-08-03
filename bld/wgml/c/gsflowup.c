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
    int             n;
    int             length;
    condcode        cc;
    int             k;
    int             string_len;
    getnum_block    gn;

    if( parmcount < 1
      || parmcount > 3 )
        return( CC_neg );

    string = parms[0].arg;
    string_len = unquote_arg( &string );

    if( string_len > 0 ) {                          // null string nothing to do
        gn.ignore_blanks = false;
        n = 0;                                      // default start pos
        if( parmcount > 1 ) {                       // evalute start pos
            if( parms[1].arg.s < parms[1].arg.e ) {// start pos specified
                gn.arg = parms[1].arg;
                cc = getnum( &gn );
                if( (cc != CC_pos) || (gn.result > string_len) ) {
                    if( !ProcFlags.suppress_msg ) {
                        xx_source_err_exit_c( ERR_FUNC_PARM, "2 (startpos)" );
                        /* never return */
                    }
                    return( cc );
                }
                n = gn.result - 1;
            }
        }

        length = string_len;                        // default length
        if( parmcount > 2 ) {                       // evalute length
            if( parms[2].arg.s < parms[2].arg.e ) {// length specified
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
        /*
         * copy unchanged string to start position
         */
        for( k = 0; k < n && string.s < string.e && ressize > 0; k++ ) {          // copy unchanged before startpos
            *(*result)++ = *string.s++;
            ressize--;
        }
        /*
         * change length of characters to lower/upper case
         */
        for( k = 0; k < length && string.s < string.e && ressize > 0; k++ ) {        // translate
            if( upper ) {
               *(*result)++ = my_toupper( *string.s++ );
            } else {
               *(*result)++ = my_tolower( *string.s++ );
            }
            ressize--;
        }
        /*
         * copy rest of string (if any)
         */
        for( ; string.s < string.e && ressize > 0; string.s++ ) {
            *(*result)++ = *string.s;
            ressize--;
        }
    }

    **result = '\0';

    return( CC_pos );
}


condcode    scr_lower( parm parms[MAX_FUN_PARMS], unsigned parmcount, char **result, unsigned ressize )
{
    return( scr_lowup( parms, parmcount, result, ressize, false ) );
}

condcode    scr_upper( parm parms[MAX_FUN_PARMS], unsigned parmcount, char **result, unsigned ressize )
{
    return( scr_lowup( parms, parmcount, result, ressize, true ) );
}
