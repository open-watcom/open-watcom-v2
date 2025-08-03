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
* Description:  WGML implement multi letter function &'right( )
*
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/*  script string function &'right(                                        */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/*                                                                         */
/* &'right(string,length<,pad>):   To  generate  a  character  string  of  */
/*    'length'  characters with  'string' at  the end.    If 'length'  is  */
/*    longer than 'string' then 'pad' the  result,  if less then truncate  */
/*    on the left.                                                         */
/*      &'right('ABC D',8) ==> "   ABC D"                                  */
/*      &'right('ABC D',8,'.') ==> "...ABC D"                              */
/*      &'right('ABC  DEF',7) ==> "BC  DEF"                                */
/*                                                                         */
/***************************************************************************/

condcode    scr_right( parm parms[MAX_FUN_PARMS], unsigned parmcount, char **result, unsigned ressize )
{
    tok_type        string;
    int             length;
    char            padchar;
    condcode        cc;
    int             k;
    int             string_len;
    getnum_block    gn;

    if( parmcount < 2
      || parmcount > 3 )
        return( CC_neg );

    string = parms[0].arg;
    string_len = unquote_arg( &string );

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
        padchar = ' ';              // default padchar
        if( parmcount > 2 ) {       // pad character specified
            tok_type pad = parms[2].arg;
            if( unquote_arg( &pad ) > 0 ) {
                padchar = *pad.s;
            }
        }
        /*
         * output pad characters (if necessary)
         */
        k = string_len;
        while( k < length && ressize > 0 ) {
            *(*result)++ = padchar;
            k++;
            ressize--;
        }
        /*
         * output string from start position
         */
        if( length < string_len ) {
            string.s += string_len - length;
        }
        while( string.s < string.e && ressize > 0 ) {
            *(*result)++ = *string.s++;
            ressize--;
        }
    }

    **result = '\0';

    return( CC_pos );
}
