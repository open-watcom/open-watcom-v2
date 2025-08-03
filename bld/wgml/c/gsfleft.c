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
* Description:  WGML implement multi letter function &'left( )
*
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/*  script string function &'left(                                         */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/*                                                                         */
/* &'left(string,length<,pad>):   To  generate  a   character  string  of  */
/*    'length' characters  with 'string' in  its start.   If  'length' is  */
/*    longer than 'string' then 'pad' the result.                          */
/*      &'left('ABC D',8) ==> "ABC D   "                                   */
/*      &'left('ABC D',8,'.') ==> "ABC D..."                               */
/*      &'left('ABC  DEF',7) ==> "ABC  DE"                                 */
/*                                                                         */
/* ! optional parm PAD is NOT implemented                                  */
/*                                                                         */
/***************************************************************************/

condcode    scr_left( parm parms[MAX_FUN_PARMS], unsigned parmcount, char **result, unsigned ressize )
{
    tok_type        string;
    int             length;
    char            padchar;
    condcode        cc;
    int             k;
    getnum_block    gn;

    if( parmcount < 2
      || parmcount > 3 )
        return( CC_neg );

    string = parms[0].arg;
    unquote_arg( &string );

    gn.arg = parms[1].arg;
    gn.ignore_blanks = false;
    cc = getnum( &gn );
    if( cc != CC_pos ) {
        if( !ProcFlags.suppress_msg ) {
            xx_source_err_exit_c( ERR_FUNC_PARM, "2 (length)" );
            /* never return */
        }
        return( cc );
    }
    length = gn.result;

    if( length > 0 ) {
        padchar = ' ';          // default padchar
        if( parmcount > 2 ) {   // evaluate padchar
            tok_type pad = parms[2].arg;
            if( unquote_arg( &pad ) > 0 ) {
                padchar = *pad.s;
            }
        }

        k = 0;
        /*
         * copy from string start
         */
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
