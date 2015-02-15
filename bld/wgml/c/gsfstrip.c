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
* Description:  WGML implement multi letter function &'strip( )
*
****************************************************************************/

#define __STDC_WANT_LIB_EXT1__  1      /* use safer C library              */

#include "wgml.h"
#include "gvars.h"

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

condcode    scr_strip( parm parms[MAX_FUN_PARMS], size_t parmcount, char * * result, int32_t ressize )
{
    char            *   pval;
    char            *   pend;
    char            *   pa;
    char            *   pe;
    char                stripchar;
    char                type;

    if( (parmcount < 1) || (parmcount > 3) ) {
        return( neg );
    }

    pval = parms[0].start;
    pend = parms[0].stop;

    unquote_if_quoted( &pval, &pend );

    if( pend == pval ) {                // null string nothing to do
        **result = '\0';
        return( pos );
    }

    stripchar = ' ';                    // default char to delete
    type      = 'b';                    // default strip both ends

    if( parmcount > 1 ) {               // evalute type
        if( parms[1].stop > parms[1].start ) {// type
            pa  = parms[1].start;
            pe  = parms[1].stop;

            unquote_if_quoted( &pa, &pe );
            type = tolower( *pa );

            switch( type ) {
            case   'b':
            case   'l':
            case   't':
                // type value is valid do nothing
                break;
            default:
                if( !ProcFlags.suppress_msg ) {
                    g_err( err_func_parm, "2 (type)" );
                    g_info_inp_pos();
                    err_count++;
                    show_include_stack();
                }
                return( neg );
                break;
            }
        }
    }

    if( parmcount > 2 ) {               // stripchar
        if( parms[2].stop > parms[2].start ) {
            pa  = parms[2].start;
            pe  = parms[2].stop;

            unquote_if_quoted( &pa, &pe );
            stripchar = *pa;
        }
    }

    if( type != 't' ) {                 // strip leading requested
        for( ; pval < pend; pval++ ) {
            if( *pval != stripchar ) {
                break;
            }
        }
    }

    for( ; pval < pend; pval++ ) {
        if( ressize <= 0 ) {
            break;
        }
        **result = *pval;
        *result += 1;
        ressize--;
    }

    if( type != 'l' ) {                 // strip trailing requested
        while( *(*result - 1) == stripchar ) {
            *result -= 1;
        }
    }

    **result = '\0';

    return( pos );
}
