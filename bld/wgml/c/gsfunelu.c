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
* Description:  WGML single letter functions &e'    &l'     &u'     &w'
*                                             exist, length, upper,  width
*                           &s'        &S'          &x'
*                            subscript, Superscript hex-to-char
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/*  script single letter function &e'         exist                        */
/*              returns   0 or 1 in result                                 */
/***************************************************************************/

static char *scr_single_func_e( char * in, char * end, char * * result )
{
    char            *   pchar;
    sub_index           var_ind;
    symvar              symvar_entry;
    symsub          *   symsubval;
    int                 rc;

    (void)end;

    pchar = scan_sym( in + 3 + (*(in + 3) == '&'), &symvar_entry, &var_ind, NULL, false );

    if( in[3] == '&' ) {            // a string operand is never a symbol
        if( symvar_entry.flags & local_var ) {  // lookup var in dict
            rc = find_symvar_lcl( input_cbs->local_dict, symvar_entry.name,
                                var_ind, &symsubval );
        } else {
            rc = find_symvar( global_dict, symvar_entry.name, var_ind,
                              &symsubval );
        }
    } else {
        rc = 0;
    }

    if( rc == 2 ) {
        **result = '1';             // exists
    } else {
        **result = '0';             // does not exist
    }
    *result  += 1;
    **result = '\0';

    SkipDot( pchar );               // skip optional terminating dot
    return( pchar );
}


/***************************************************************************/
/*  script single letter function &l'         length                       */
/*              returns   length of value or length of name in result      */
/***************************************************************************/

static char *scr_single_func_l( char * in, char * end, char * * result )
{
    char            *   pchar;
    sub_index           var_ind;
    symvar              symvar_entry;
    symsub          *   symsubval;
    int                 rc;
    int                 len;

    if( *(in + 3) == '&' ) {            // symbol name
        pchar = scan_sym( in + 4, &symvar_entry, &var_ind, NULL, false );

        if( symvar_entry.flags & local_var ) {  // lookup var in dict
            rc = find_symvar_lcl( input_cbs->local_dict, symvar_entry.name,
                                var_ind, &symsubval );
        } else {
            rc = find_symvar( global_dict, symvar_entry.name, var_ind,
                              &symsubval );
        }
        if( rc == 2 ) {
            len = strlen( symsubval->value );
        } else {
            len = strlen( symvar_entry.name );
        }
    } else {                            // string
        pchar = in + 3;
        len = 0;
        while( !((*pchar == ' ') || (*pchar == '.') || (pchar == end)) ) {
            len++;
            pchar++;
        }
    }
    sprintf( *result, "%d", len );
    *result  += strlen( *result );
    **result = '\0';

    SkipDot( pchar );               // skip optional terminating dot
    return( pchar );
}


/***************************************************************************/
/*  script single letter function &s'         subscript                    */
/*  script single letter function &S'         superscript                  */
/*                       subscript or superscript is coded in parm fun     */
/***************************************************************************/

static char *scr_single_func_sS( char * in, char * end, char * * result, char fun )
{
    char            *   pchar;
    sub_index           var_ind;
    symvar              symvar_entry;
    symsub          *   symsubval;
    int                 rc;
    char            *   pval;


    **result = function_escape;         // insert function code in buffer
    *result += 1;
    **result = fun;
    *result += 1;

    if( *(in + 3) == '&' ) {            // symbol name
        pchar = scan_sym( in + 4, &symvar_entry, &var_ind, NULL, false );

        if( symvar_entry.flags & local_var ) {  // lookup var in dict
            rc = find_symvar_lcl( input_cbs->local_dict, symvar_entry.name,
                                var_ind, &symsubval );
        } else {
            rc = find_symvar( global_dict, symvar_entry.name, var_ind,
                              &symsubval );
        }
        if( rc == 2 ) {
            pval = symsubval->value;    // variable  found
        } else {
            pval =  symvar_entry.name;  // not found use variable name
        }
        while( *pval ) {
            **result = *pval++;
            *result += 1;
        }
    } else {                            // string
        pchar = in + 3;
        while( !((*pchar == ' ') || (*pchar == '.') || (pchar == end)) ) {
            **result = *pchar++;
            *result += 1;
        }
    }
    **result = function_escape;         // insert function code in buffer
    *result += 1;
    **result = fun | 0x01;              // function end
    *result += 1;
    **result = '\0';

    SkipDot( pchar );                   // skip optional terminating dot
    return( pchar );
}


/***************************************************************************/
/*  script single letter function &u'         upper                        */
/*                                                                         */
/***************************************************************************/

static char *scr_single_func_u( char * in, char * end, char * * result )
{
    char            *   pchar;
    sub_index           var_ind;
    symvar              symvar_entry;
    symsub          *   symsubval;
    int                 rc;
    char            *   pval;
    bool                sym_valid = false;

    if( *(in + 3) == '&' ) {            // symbol name
        pchar = scan_sym( in + 4, &symvar_entry, &var_ind, NULL, false );

        if( symvar_entry.flags & local_var ) {  // lookup var in dict
            rc = find_symvar_lcl( input_cbs->local_dict, symvar_entry.name,
                                var_ind, &symsubval );
        } else {
            rc = find_symvar( global_dict, symvar_entry.name, var_ind,
                              &symsubval );
        }
        if( rc == 2 ) {
            pval = symsubval->value;    // variable found
            sym_valid = true;
            while( *pval ) {
                **result = my_toupper( *pval++ );
                *result += 1;
            }
        }
    }
    if( !sym_valid ) {                  // string or invalid symbol
        pchar = in + 3;
        while( !((*pchar == ' ') || (*pchar == '.') || (pchar == end)) ) {
            **result = my_toupper( *pchar++ );
            *result += 1;
        }
    }
    **result = '\0';

    SkipDot( pchar );               // skip optional terminating dot
    return( pchar );
}


/***************************************************************************/
/*  script single letter function &w'         width                        */
/*              returns   width in CPI rounded up if neccessary            */
/*                                                                         */
/*  some logic has to be in sync with scr_width() in gsfwidth.c            */
/***************************************************************************/

static char *scr_single_func_w( char * in, char * end, char * * result )
{
    char            *   pchar;
    sub_index           var_ind;
    symvar              symvar_entry;
    symsub          *   symsubval;
    int                 rc;
    int                 len;
    uint32_t            width;

    if( *(in + 3) == '&' ) {            // symbol name
        pchar = scan_sym( in + 4, &symvar_entry, &var_ind, NULL, false );

        if( symvar_entry.flags & local_var ) {  // lookup var in dict
            rc = find_symvar_lcl( input_cbs->local_dict, symvar_entry.name,
                                var_ind, &symsubval );
        } else {
            rc = find_symvar( global_dict, symvar_entry.name, var_ind,
                              &symsubval );
        }
        if( rc == 2 ) {
            width = cop_text_width( symsubval->value, strlen( symsubval->value ), g_curr_font );
        } else {

        /*******************************************************************/
        /* for undefined variable calc length of name, & and perhaps *     */
        /*******************************************************************/
            width = cop_text_width( symvar_entry.name, strlen( symvar_entry.name ), g_curr_font )
                    + wgml_fonts[g_curr_font].width_table['&'];

            if( symvar_entry.flags & local_var ) {  // add width of *
                width += wgml_fonts[g_curr_font].width_table['*'];
            }
        }
    } else {                            // string
        pchar = in + 3;
        len = 0;
        while( !((*pchar == ' ') || (*pchar == '.') || (pchar == end)) ) {
            len++;
            pchar++;
        }
        width = cop_text_width( in + 3, len, g_curr_font );
    }
    len = width;
    width = (width * CPI + g_resh / 2) / g_resh;
    sprintf( *result, "%d", width );
    *result  += strlen( *result );
    **result = '\0';

    SkipDot( pchar );               // skip optional terminating dot
    return( pchar );
}


/***************************************************************************/
/*  script single letter function &x'         hex to char                  */
/*              returns   character equivalent of hex pair                 */
/*                                                                         */
/*  Note: multiple concatenated pairs may be processed (sometimes)         */
/*        this version always processes all concatenated pairs, or none    */
/*        invalid operands result in the entire operand being displayed    */
/***************************************************************************/

static char *scr_single_func_x( char * in, char * end, char * * result )
{
    bool        accept;
    char        c;
    char    *   pchar;

    accept = true;
    pchar = in + 3;
    if( *pchar != '&' ) {               // symbols/symbol values are not converted
        if( (end - pchar ) % 2 ) {      // odd number of characters?
            accept = false;
        } else {
            for( ; pchar < end; pchar++ ) { // check for non-hex-digit in input
                if( !my_isxdigit( *pchar ) ) {
                    accept = false;
                    break;
                }
            }
            pchar = in + 3;             // reset to start of input
        }
        if( accept ) {                  // input is acceptable
            for( ; pchar < end; pchar++ ) { // convert input from hex

                c = 0;
                if( my_isdigit( *pchar ) ) {
                    c += *pchar - '0';
                } else {
                    c += my_toupper( *pchar ) - 'A' + 10;
                }
                c *= 16;
                pchar++;
                if( my_isdigit( *pchar ) ) {
                    c += *pchar - '0';
                } else {
                    c += my_toupper( *pchar ) - 'A' + 10;
                }

                **result = c;
                (*result)++;
            }
            **result = '\0';                    // final digit already skipped
            SkipDot( pchar );                   // skip optional terminating dot
        } else {
            for( ; pchar < end; pchar++ )
                *(*result)++ = *pchar;          // copy unrecognized input
            SkipDot( pchar );                   // don't copy terminating dot
            *(*result)++ = *pchar;              // copy possible whitespace
            **result = '\0';
        }
    }

    return( pchar );
}

/***************************************************************************/
/*  script single letter functions unsupported   process to comsume        */
/*                                               variable for scanning     */
/***************************************************************************/

static char *scr_single_func_unsupport( char * in, char * * result )
{
    char        charstr[2];

    (void)result;

    charstr[0] = *(in + 1);
    charstr[1] = '\0';

    // do nothing
    return( in + 3 );
}


/***********************************************************************/
/*  Some single letter functions are implemented here                  */
/*                                                                     */
/*  functions used within the OW doc build system:                     */
/*   &e'  existance of variable 0 or 1                                 */
/*   &l'  length of variable content or if undefined length of name    */
/*   &u'  upper case                                                   */
/*   &w'  width for internal use                                       */
/*   &x'  convert valid hex pairs to characters                        */
/*                                                                     */
/*   &s'  subscript                                                    */
/*   &S'  superscript                                                  */
/*                                                                     */
/*   other single letter functions are not used AFAIK                  */
/*                                                                     */
/***********************************************************************/

char *scr_single_funcs( char * in, char * end, char * * result )
{
    char            *   pw;

    ProcFlags.unresolved = false;
    if( *(in + 2) == '\'' ) {
        switch( *(in + 1) ) {
        case 'e':                       // exist function
        case 'E':
            pw = scr_single_func_e( in, end, result );
            break;
        case 'l':                       // length function
        case 'L':
            pw = scr_single_func_l( in, end, result );
            break;
        case 's':                       // subscript
            pw = scr_single_func_sS( in, end, result, function_subscript );
            break;
        case 'S':                       // superscript
            pw = scr_single_func_sS( in, end, result, function_superscript );
            break;
        case 'u':                       // upper function
        case 'U':
            pw = scr_single_func_u( in, end, result );
            break;
        case 'w':                       // width function
        case 'W':
            pw = scr_single_func_w( in, end, result );
            break;
        case 'x':                       // hex-to-char function
        case 'X':
            pw = scr_single_func_x( in, end, result );
            break;
        default:
            pw = scr_single_func_unsupport( in, result );
            ProcFlags.unresolved = true;
            break;
        }
    } else {
        internal_err( __FILE__, __LINE__ );
    }
    ProcFlags.substituted = true;
    return( pw );
}


