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
* Description:  WGML implement multi letter function &'width( )
*
****************************************************************************/

#include "wgml.h"
#include "gvars.h"

/***************************************************************************/
/*  script string function &'width(                                        */
/*                                                                         */
/*  some logic has to be in sync with scr_singele_func_w in gsfunelu.c     */
/***************************************************************************/

/***************************************************************************/
/*&'width(string<,type>):   The  Width function  returns  the  formatted   */
/*   width of 'string' in units based  on the current Character Per Inch   */
/*   setting.   The returned width is rounded  up if the actual width is   */
/*   not an  exact multiple of a  CPI unit.   An optional  second 'type'   */
/*   operand may be specified to define the  units in which the width is   */
/*   to be  defined:   a  'type' of  "C" or  "CPI" is  default,  "N"  or   */
/*   "Number" is the number of  characters not including overstrikes and   */
/*   escape sequences,  and  "U" or "Units" returns the  width in output   */
/*   Device Units.                                                         */
/*     &'width(abc) ==> 3                                                  */
/*     &'width('A&SYSBS._') ==> 1                                          */
/*     &'width(ABCDEFG,C) ==> width in CPI units                           */
/*     &'width(ABCDEFG,N) ==> number of characters                         */
/*     &'width(ABCDEFG,U) ==> width in Device Units                        */
/*     &'width(AAA,X) ==> invalid 'type' operand                           */
/***************************************************************************/

condcode    scr_width( parm parms[MAX_FUN_PARMS], size_t parmcount, char * * result, int32_t ressize )
{
    char            *   pval;
    char            *   pend;
    char            *   pa;
    char            *   pe;
    size_t              len;
    char                type;
    uint32_t            width;

    ressize = ressize;
    if( (parmcount < 1) || (parmcount > 2) ) {
        return( neg );
    }

    pval = parms[0].start;
    pend = parms[0].stop;

    unquote_if_quoted( &pval, &pend );

    if( pend == pval ) {                // null string width 0
        **result = '0';
        *result += 1;
        **result = '\0';
        return( pos );
    }

    len = pend - pval;

    if( parmcount > 1 ) {               // evalute type
        if( parms[1].stop > parms[1].start ) {// type
            pa = parms[1].start;
            pe = parms[1].stop;

            unquote_if_quoted( &pa, &pe );

            type = tolower( *pa );
            switch( type ) {
            case   'c':                 // CPI
                width = cop_text_width( pval, len, g_curr_font );
                width = (width * CPI + g_resh / 2) / g_resh;
                break;
            case   'u':                 // Device Units
                width = cop_text_width( pval, len, g_curr_font );
                break;
            case   'n':                 // character count
                width = len;
                break;
            default:
                g_err( err_func_parm, "2 (type)" );
                g_info_inp_pos();
                err_count++;
                show_include_stack();
                return( neg );
                break;
            }
        }
    } else {                            // default type c processing
        width = cop_text_width( pval, len, g_curr_font );
        width = (width * CPI + g_resh / 2) / g_resh;
    }

    sprintf( *result, "%d", width );

    *result  += strlen( *result );
    **result = '\0';

    return( pos );
}
