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
    int             tableo_len;
    int             offset;

    if( parmcount < 1
      || parmcount > 4 )
        return( CC_neg );

    string = parms[0].arg;
    if( unquote_arg( &string ) > 0 ) {
        tableo.s = NULL;
        tableo.e = NULL;
        tableo_len = 0;
        if( parmcount > 1 ) {
            tok_type table = parms[1].arg;
            tableo_len = unquote_arg( &table );
            if( tableo_len > 0 ) {    // tableo is not empty
                tableo = table;
            }
        }
        tablei.s = NULL;
        tablei.e = NULL;
        if( parmcount > 2 ) {
            tok_type table = parms[2].arg;
            if( unquote_arg( &table ) > 0 ) {   // tableo is not empty
                tablei = table;
            }
        }
        padchar = ' ';                          // padchar default is blank
        if( parmcount > 3 ) {
            tok_type pad = parms[3].arg;
            if( unquote_arg( &pad ) > 0 ) {     // padchar specified
                padchar = *pad.s;
            }
        }

        if( (tablei.s == NULL) && (tableo.s == NULL) && padchar == '\0' ) {
            while( (string.s < string.e) && (ressize > 0) ) {  // translate to upper
                *(*result)++ = my_toupper( *string.s++ );
                ressize--;
            }
        } else {                   // translate as specified in tablei and tableo
            while( string.s < string.e && ressize > 0 ) {
                c = *string.s++;
                if( tablei.s == NULL ) {
                    c = padchar;
                } else {
                    for( iptr = tablei.s; iptr < tablei.e; iptr++ ) {
                        if( c == *iptr ) {
                            offset = iptr - tablei.s;
                            if( offset < tableo_len ) {
                                c = *(tableo.s + offset);  // take char from output table
                            } else {
                                c = padchar;    // output table too short use padchar
                            }
                            break;
                        }
                    }
                }
                *(*result)++ = c;
                ressize--;
            }
        }
    }

    **result = '\0';

    return( CC_pos );
}
