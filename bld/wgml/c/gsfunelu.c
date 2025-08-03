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

static char *scr_single_func_e( char *args, char *end, char **result )
{
    char            *pchar;
    sub_index       var_ind;
    symvar          symvar_entry;
    symsub          *symsubval;
    int             rc;

    (void)end;

    if( *args == '&' ) {            // a string operand is never a symbol
        pchar = scan_sym( args + 1, &symvar_entry, &var_ind, NULL, false );
        rc = find_symvar_sym( &symvar_entry, var_ind, &symsubval );
    } else {
        pchar = scan_sym( args, &symvar_entry, &var_ind, NULL, false );
        rc = 0;
    }

    if( rc == 2 ) {
        *(*result)++ = '1';             // exists
    } else {
        *(*result)++ = '0';             // does not exist
    }
    **result = '\0';

    SkipDot( pchar );               // skip optional terminating dot
    return( pchar );
}


/***************************************************************************/
/*  script single letter function &l'         length                       */
/*              returns   length of value or length of name in result      */
/***************************************************************************/

static char *scr_single_func_l( char *args, char *end, char **result )
{
    char            *pchar;
    sub_index       var_ind;
    symvar          symvar_entry;
    symsub          *symsubval;
    int             rc;
    unsigned        len;

    if( *args == '&' ) {            // symbol name
        pchar = scan_sym( args + 1, &symvar_entry, &var_ind, NULL, false );
        rc = find_symvar_sym( &symvar_entry, var_ind, &symsubval );
        if( rc == 2 ) {
            len = strlen( symsubval->value );
        } else {
            len = strlen( symvar_entry.name );
        }
    } else {                            // string
        len = 0;
        for( pchar = args; *pchar != ' ' && *pchar != '.' && pchar != end; pchar++ ) {
            len++;
        }
    }

    *result += sprintf( *result, "%d", len );

    SkipDot( pchar );               // skip optional terminating dot
    return( pchar );
}


/***************************************************************************/
/*  script single letter function &s'         subscript                    */
/*  script single letter function &S'         superscript                  */
/*                       subscript or superscript is coded in parm fun     */
/***************************************************************************/

static char *scr_single_func_sS( char *args, char *end, char **result, bool super )
{
    char            *pchar;
    sub_index       var_ind;
    symvar          symvar_entry;
    symsub          *symsubval;
    int             rc;
    char            *pval;

    *(*result)++ = FUNC_escape;         // insert function code in buffer
    if( super ) {
        *(*result)++ = FUNC_superscript_beg;// function superscript start
    } else {
        *(*result)++ = FUNC_subscript_beg;  // function subscript start
    }

    if( *args == '&' ) {            // symbol name
        pchar = scan_sym( args + 1, &symvar_entry, &var_ind, NULL, false );
        rc = find_symvar_sym( &symvar_entry, var_ind, &symsubval );
        if( rc == 2 ) {
            pval = symsubval->value;    // variable  found
        } else {
            pval = symvar_entry.name;   // not found use variable name
        }
        for( ; *pval != '\0'; pval++ ) {
            *(*result)++ = *pval;
        }
    } else {                            // string
        for( pchar = args; *pchar != ' ' && *pchar != '.' && pchar != end; pchar++ ) {
            *(*result)++ = *pchar;
        }
    }
    *(*result)++ = FUNC_escape;         // insert function code in buffer
    if( super ) {
        *(*result)++ = FUNC_superscript_end;    // function superscript end
    } else {
        *(*result)++ = FUNC_subscript_end;    // function subscript end
    }
    **result = '\0';

    SkipDot( pchar );                   // skip optional terminating dot
    return( pchar );
}


/***************************************************************************/
/*  script single letter function &u'         upper                        */
/*                                                                         */
/***************************************************************************/

static char *scr_single_func_u( char *args, char *end, char **result )
{
    char            *pchar;
    sub_index       var_ind;
    symvar          symvar_entry;
    symsub          *symsubval;
    int             rc;
    char            *pval;
    bool            sym_valid;

    sym_valid = false;
    if( *args == '&' ) {            // symbol name
        pchar = scan_sym( args + 1, &symvar_entry, &var_ind, NULL, false );
        rc = find_symvar_sym( &symvar_entry, var_ind, &symsubval );
        if( rc == 2 ) {
            sym_valid = true;           // variable found
            for( pval = symsubval->value; *pval != '\0'; pval++ ) {
                *(*result)++ = my_toupper( *pval );
            }
        }
    }
    if( !sym_valid ) {                  // string or invalid symbol
        for( pchar = args; *pchar != ' ' && *pchar != '.' && pchar != end; pchar++ ) {
            *(*result)++ = my_toupper( *pchar );
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

static char *scr_single_func_w( char *args, char *end, char **result )
{
    char            *pchar;
    sub_index       var_ind;
    symvar          symvar_entry;
    symsub          *symsubval;
    int             rc;
    unsigned        len;
    unsigned        width;

    if( *args == '&' ) {            // symbol name
        pchar = scan_sym( args + 1, &symvar_entry, &var_ind, NULL, false );
        rc = find_symvar_sym( &symvar_entry, var_ind, &symsubval );
        if( rc == 2 ) {
            width = cop_text_width( symsubval->value, strlen( symsubval->value ), g_curr_font );
        } else {

            /*******************************************************************/
            /* for undefined variable calc length of name, & and perhaps *     */
            /*******************************************************************/
            width = cop_text_width( symvar_entry.name, strlen( symvar_entry.name ), g_curr_font )
                    + wgml_fonts[g_curr_font].width.table['&'];

            if( symvar_entry.flags & SF_local_var ) {  // add width of *
                width += wgml_fonts[g_curr_font].width.table['*'];
            }
        }
    } else {                            // string
        len = 0;
        for( pchar = args; *pchar != ' ' && *pchar != '.' && pchar != end; pchar++ ) {
            len++;
        }
        width = cop_text_width( args, len, g_curr_font );
    }
    len = width;
    width = (width * CPI + g_resh / 2) / g_resh;

    *result  += sprintf( *result, "%d", width );

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

static char *scr_single_func_x( char *args, char *end, char **result )
{
    bool            accept;
    char            c;
    char            *pchar;

    accept = true;
    if( *args != '&' ) {               // symbols/symbol values are not converted
        if( ( end - args ) % 2 ) {      // odd number of characters?
            accept = false;
        } else {
            for( pchar = args; pchar < end; pchar++ ) { // check for non-hex-digit in input
                if( !my_isxdigit( *pchar ) ) {
                    accept = false;
                    break;
                }
            }
        }
        if( accept ) {                  // input is acceptable
            for( pchar = args; pchar < end; pchar++ ) { // convert input from hex

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

                *(*result)++ = c;
            }
            **result = '\0';                    // final digit already skipped
            SkipDot( pchar );                   // skip optional terminating dot
        } else {
            for( pchar = args; pchar < end; pchar++ )
                *(*result)++ = *pchar;          // copy unrecognized input
            SkipDot( pchar );                   // don't copy terminating dot
            *(*result)++ = *pchar;              // copy possible whitespace
            **result = '\0';
        }
    } else {
        pchar = args;
    }

    return( pchar );
}

/***************************************************************************/
/*  script single letter functions unsupported   process to comsume        */
/*                                               variable for scanning     */
/***************************************************************************/

static char *scr_single_func_unsupport( const char *funcname, char *args, char *end, char **result )
{
    char        charstr[2];

    (void)result; (void)end;

    charstr[0] = *funcname;
    charstr[1] = '\0';

    // do nothing
    return( args );
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

char *scr_single_funcs( const char *funcname, char *args, char *end, char **result )
{
    char            *pw;

    ProcFlags.unresolved = false;
    switch( *funcname ) {
    case 'e':                       // exist function
    case 'E':
        pw = scr_single_func_e( args, end, result );
        break;
    case 'l':                       // length function
    case 'L':
        pw = scr_single_func_l( args, end, result );
        break;
    case 's':                       // subscript
        pw = scr_single_func_sS( args, end, result, false );
        break;
    case 'S':                       // superscript
        pw = scr_single_func_sS( args, end, result, true );
        break;
    case 'u':                       // upper function
    case 'U':
        pw = scr_single_func_u( args, end, result );
        break;
    case 'w':                       // width function
    case 'W':
        pw = scr_single_func_w( args, end, result );
        break;
    case 'x':                       // hex-to-char function
    case 'X':
        pw = scr_single_func_x( args, end, result );
        break;
    default:
        pw = scr_single_func_unsupport( funcname, args, end, result );
        ProcFlags.unresolved = true;
        break;
    }
    ProcFlags.substituted = true;
    return( pw );
}
