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

#define __STDC_WANT_LIB_EXT1__  1      /* use safer C library              */

#include "wgml.h"
#include "gvars.h"

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

condcode    scr_translate( parm parms[MAX_FUN_PARMS], size_t parmcount, char * * result, int32_t ressize )
{
    char            *   pval;
    char            *   pend;
    char            *   ptaboa;
    char            *   ptaboe;
    char            *   ptabia;
    char            *   ptabie;
    char                padchar;
    char                c;
    char            *   iptr;
    char            *   optr;
    bool                ifound;
    int                 offset;
    bool                padchar_set;

    if( (parmcount < 1) || (parmcount > 4) ) {
        return( neg );
    }

    pval = parms[0].a;
    pend = parms[0].e;
    unquote_if_quoted( &pval, &pend );

    if( pend - pval + 1 <= 0 ) {        // null string nothing to do
        **result = '\0';
        return( pos );
    }

    ptaboa = parms[1].a;
    ptaboe = parms[1].e;
    if( (parmcount > 1) && (ptaboe >= ptaboa) ) {   // tableo is not empty
        unquote_if_quoted( &ptaboa, &ptaboe );
    } else {
        ptaboa = NULL;
        ptaboe = NULL;
    }

    ptabia = parms[2].a;
    ptabie = parms[2].e;
    if( (parmcount > 2) && (ptabie >= ptabia) ) {   // tablei is not empty
        unquote_if_quoted( &ptabia, &ptabie );
    } else {
        ptabia = NULL;
        ptabie = NULL;
    }

    if( parmcount > 3 ) {               // padchar specified
        char    * pa = parms[3].a;
        char    * pe = parms[3].e;

        unquote_if_quoted( &pa, &pe );
        padchar = *pa;
        padchar_set = true;
    } else {
        padchar = ' ';                  // padchar default is blank
        padchar_set = false;
    }

    if( (ptabia == NULL) && (ptaboa == NULL) && !padchar_set ) {
        while( (pval <= pend) && (ressize > 0) ) {  // translate to upper
            **result = toupper( *pval++ );
            *result += 1;
            ressize--;
        }
    } else {                   // translate as specified in tablei and tableo
        for( ; pval <= pend; pval++ ) {
            c = *pval;
            ifound = false;
            if( ptabia == NULL ) {
                c = padchar;
            } else {
                for( iptr = ptabia; iptr <= ptabie; iptr++ ) {
                    if( c == *iptr ) {
                        ifound = true;  // char found in input table
                        offset = iptr - ptabia;
                        optr = ptaboa + offset;
                        if( optr <= ptaboe ) {
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
            if( ressize <= 0 ) {
                break;
            }
        }
    }

    **result = '\0';

    return( pos );
}
