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
* Description:  WGML implement multi letter function &'strip( )
*
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/*  script string function &'strip(                                        */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/*                                                                         */
/* &'strip(string<,<type><,char>>):   To remove  leading and/or  trailing  */
/*    'char's from a 'string'.  The 'type' specified may be 'L' to remove  */
/*    leading characters,  'T' for trailing  characters,  or 'B' for both  */
/*    leading and trailing characters.   If omitted the default 'type' is  */
/*    'B'.   All leading  or trailing characters matching  'char' will be  */
/*    removed, the default 'char' being a blank.                           */
/*      "&'strip('  the dog  ')" ==> "the dog"                             */
/*      "&'strip('  the dog  ','L')" ==> "the dog  "                       */
/*      "&'strip('  the dog  ','t')" ==> "  the dog"                       */
/*      "&'strip('a-b--',,'-')" ==> "a-b"                                  */
/*      "&'strip(--a-b--,,-)" ==> "a-b"                                    */
/*                                                                         */
/***************************************************************************/

condcode    scr_strip( parm parms[MAX_FUN_PARMS], unsigned parmcount, char **result, unsigned ressize )
{
    tok_type        string;
    char            stripchar;
    char            typechar;

    if( parmcount < 1
      || parmcount > 3 )
        return( CC_neg );

    string = parms[0].arg;
    if( unquote_arg( &string ) > 0 ) {    // null string nothing to do
        typechar  = 'B';                        // default strip both ends
        if( parmcount > 1 ) {
            tok_type type = parms[1].arg;       // evalute type
            if( unquote_arg( &type ) > 0 ) {
                typechar = my_toupper( *type.s );
            }
            switch( typechar ) {
            case 'B':
            case 'L':
            case 'T':
                // type value is valid do nothing
                break;
            default:
                if( !ProcFlags.suppress_msg ) {
                    xx_source_err_exit_c( ERR_FUNC_PARM, "2 (type)" );
                    /* never return */
                }
                return( CC_neg );
            }
        }

        stripchar = ' ';                    // default char to delete
        if( parmcount > 2 ) {               // stripchar
            tok_type strip = parms[2].arg;
            if( unquote_arg( &strip ) > 0 ) {
                stripchar = *strip.s;
            }
        }
        /*
         * strip leading requested
         */
        if( typechar != 'T' ) {
            for( ; string.s < string.e; string.s++ ) {
                if( *string.s != stripchar ) {
                    break;
                }
            }
        }
        /*
         * copy string body
         */
        for( ; string.s < string.e && ressize > 0; string.s++ ) {
            *(*result)++ = *string.s;
            ressize--;
        }
        /*
         * strip trailing requested
         */
        if( typechar != 'L' ) {
            while( *(*result - 1) == stripchar ) {
                *result -= 1;
            }
        }
    }

    **result = '\0';

    return( CC_pos );
}
