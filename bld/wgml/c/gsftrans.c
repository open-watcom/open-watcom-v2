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
* Description:  WGML implement multi letter function &'translate( )
*
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/*  script string function &'translate(                                    */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/*                                                                         */
/* &'translate(string<,<tableo><,<tablei><,pad>>>):      To     Translate  */
/*    'string' with characters  in 'tablei' to the  corresponding charac-  */
/*    ters in  'tableo'.   The  'pad' character  will extend  'tableo' to  */
/*    equal the length of 'tablei',  if  required.   If both 'tableo' and  */
/*    'tablei' are omitted then 'string' is converted to uppercase.        */
/*      &'translate('abcdef') ==> ABCDEF                                   */
/*      &'translate('abbc','&','b') ==> a&&c                               */
/*      &'translate('abcdef','12','ec') ==> ab2d1f                         */
/*      &'translate('abcdef','12','abcd','.') ==> 12..ef                   */
/*      &'translate('4321','abcd','1234') ==> dcba                         */
/*      &'translate('123abc',,,'$') ==> $$$$$$                             */
/*                                                                         */
/***************************************************************************/

condcode    scr_translate( parm parms[MAX_FUN_PARMS], unsigned parmcount, char **result, unsigned ressize )
{
    tok_type        string;
    tok_type        tableo;
    tok_type        tablei;
    char            padchar;
    char            c;
    char            *iptr;
    char            *optr;
    bool            ifound;
    int             offset;
    bool            padchar_set;

    if( (parmcount < 1) || (parmcount > 4) ) {
        return( neg );
    }

    string.s = parms[0].a;
    string.e = parms[0].e;
    unquote_arg( &string );

    if( string.e - string.s + 1 <= 0 ) {        // null string nothing to do
        **result = '\0';
        return( pos );
    }

    tableo.s = parms[1].a;
    tableo.e = parms[1].e;
    if( (parmcount > 1) && (tableo.e >= tableo.s) ) {   // tableo is not empty
        unquote_arg( &tableo );
    } else {
        tableo.s = NULL;
        tableo.e = NULL;
    }

    tablei.s = parms[2].a;
    tablei.e = parms[2].e;
    if( (parmcount > 2) && (tablei.e >= tablei.s) ) {   // tablei is not empty
        unquote_arg( &tablei );
    } else {
        tablei.s = NULL;
        tablei.e = NULL;
    }

    if( parmcount > 3 ) {               // padchar specified
        tok_type    pad;

        pad.s = parms[3].a;
        pad.e = parms[3].e;
        unquote_arg( &pad );
        padchar = *pad.s;
        padchar_set = true;
    } else {
        padchar = ' ';                  // padchar default is blank
        padchar_set = false;
    }

    if( (tablei.s == NULL) && (tableo.s == NULL) && !padchar_set ) {
        while( (string.s <= string.e) && (ressize > 0) ) {  // translate to upper
            **result = my_toupper( *string.s++ );
            *result += 1;
            ressize--;
        }
    } else {                   // translate as specified in tablei and tableo
        for( ; string.s <= string.e && ressize > 0; string.s++ ) {
            c = *string.s;
            ifound = false;
            if( tablei.s == NULL ) {
                c = padchar;
            } else {
                for( iptr = tablei.s; iptr <= tablei.e; iptr++ ) {
                    if( c == *iptr ) {
                        ifound = true;  // char found in input table
                        offset = iptr - tablei.s;
                        optr = tableo.s + offset;
                        if( optr <= tableo.e ) {
                            **result = *optr;  // take char from output table
                        } else {
                            **result = padchar;// output table too short use padchar
                        }
                        break;
                    }
                }
            }
            if( !ifound ) {
                **result = c;           // not found, leave unchanged
            }
            *result += 1;
            ressize--;
        }
    }

    **result = '\0';

    return( pos );
}
