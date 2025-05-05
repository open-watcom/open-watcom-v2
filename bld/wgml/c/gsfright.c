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
    condcode        cc;
    int             k;
    int             n;
    int             len;
    getnum_block    gn;
    char            padchar;

    if( (parmcount < 2) || (parmcount > 3) ) {
        return( neg );
    }

    string.s = parms[0].a;
    string.e = parms[0].e;

    unquote_arg( &string );

    len = string.e - string.s + 1;              // total length

    gn.ignore_blanks = false;

    gn.arg.s = parms[1].a;
    gn.arg.e = parms[1].e;
    cc = getnum( &gn );
    if( cc != pos ) {
        if( !ProcFlags.suppress_msg ) {
            xx_source_err_c( err_func_parm, "2 (length)" );
        }
        return( cc );
    }
    n = gn.result;

    if( n > 0 ) {                       // result not nullstring
        if( n > len ) {                 // padding needed
            padchar = ' ';              // default padchar
            if( parmcount > 2 ) {       // pad character specified
                if( parms[2].e >= parms[2].a ) {
                    char * pa = parms[2].a;
                    char * pe = parms[2].e;

                    unquote_arg( &pa, &pe);
                    padchar = *pa;
                }
            }
            for( k = n - len; k > 0 && ressize > 0; k-- ) {
                **result = padchar;
                *result += 1;
                ressize--;
            }
            for( ; string.s <= string.e && ressize > 0; string.s++ ) {
                **result = *string.s;
                *result += 1;
                ressize--;
            }
        } else {                        // no padding

            string.s += len - n;
            for( ; string.s <= string.e && ressize > 0; string.s++ ) {
                **result = *string.s;
                *result += 1;
                ressize--;
            }
        }
    }
    **result = '\0';

    return( pos );
}
