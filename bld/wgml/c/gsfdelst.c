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
* Description:  WGML implement multi letter function &'delstr( )
*
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/*  script string function &'delstr(                                       */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/* &'delstr(string,n<,length>):  The  Delete String function  deletes the  */
/*    part of 'string' starting at character  number 'n'.   The number of  */
/*    characters can be specified in 'length' and defaults from character  */
/*    'n' to the end of the string.  If 'string' is less than 'n' charac-  */
/*    ters long, then nothing is deleted.                                  */
/*      &'delstr('abcdef',3) ==> ab                                        */
/*      &'delstr('abcdef',3,2) ==> abef                                    */
/*      &'delstr('abcdef',10) ==> abcdef                                   */
/***************************************************************************/

condcode    scr_delstr( parm parms[MAX_FUN_PARMS], unsigned parmcount, char **result, unsigned ressize )
{
    tok_type        string;
    int             n;
    int             length;
    condcode        cc;
    int             k;
    int             string_len;
    getnum_block    gn;

    if( parmcount < 2
      || parmcount > 3 )
        return( CC_neg );

    string = parms[0].arg;
    string_len = unquote_arg( &string );

    if( string_len > 0 ) {
        gn.ignore_blanks = false;

        n = 0;                                  // default start pos
        if( parms[1].arg.s < parms[1].arg.e ) {// start pos
            gn.arg = parms[1].arg;
            cc = getnum( &gn );
            if( (cc != CC_pos) || (gn.result == 0) ) {
                if( !ProcFlags.suppress_msg ) {
                    xx_source_err_exit_c( ERR_FUNC_PARM, "2 (startpos)" );
                    /* never return */
                }
                return( cc );
            }
            n = gn.result - 1;
        }

        length = string_len;
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
         * copy unchanged string before startpos
         */
        k = 0;
        while( (k < n) && (string.s < string.e) && (ressize > 0) ) {
            *(*result)++ = *string.s++;
            k++;
            ressize--;
        }
        /*
         * skip (don't copy) deleted characters
         */
        while( (length > 0) && (string.s < string.e) ) {
            string.s++;
            length--;
        }
        /*
         * copy rest of string (if any)
         */
        while( (string.s < string.e) && (ressize > 0) ) {
            *(*result)++ = *string.s++;
            ressize--;
        }
    }

    **result = '\0';

    return( CC_pos );
}
