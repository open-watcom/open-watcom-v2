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
* Description:  WGML implement multi letter function &'width( )
*
****************************************************************************/


#include "wgml.h"


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

condcode    scr_width( parm parms[MAX_FUN_PARMS], unsigned parmcount, char **result, unsigned ressize )
{
    tok_type        string;
    int             string_len;
    char            typechar;
    unsigned        width;

    (void)ressize;

    if( parmcount < 1
      || parmcount > 2 )
        return( CC_neg );

    width = 0;                              // null string width 0

    string = parms[0].arg;
    string_len = unquote_arg( &string );

    if( string_len > 0 ) {
        typechar = 'C';                     // default type is 'c' (CPI)
        if( parmcount > 1 ) {               // evalute typechar
            tok_type type  = parms[1].arg;
            if( unquote_arg( &type ) > 0 ) {
                typechar = my_toupper( *type.s );
            }
        }
        switch( typechar ) {
        case 'C':                 // CPI
            width = cop_text_width( string.s, string_len, g_curr_font );
            width = (width * CPI + g_resh / 2) / g_resh;
            break;
        case 'U':                 // Device Units
            width = cop_text_width( string.s, string_len, g_curr_font );
            break;
        case 'N':                 // character count
            width = string_len;
            break;
        default:
            xx_source_err_exit_c( ERR_FUNC_PARM, "2 (type)" );
            /* never return */
        }
    }

    *result  += sprintf( *result, "%d", width );

    return( CC_pos );
}
