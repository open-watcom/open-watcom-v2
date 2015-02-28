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
* Description:  WGML single letter functions &e'    &l'     &u'     &w'
*                                             exist, length, upper,  width
*                           &s'        &S'
*                            subscript, Superscript
****************************************************************************/

#include "wgml.h"


/***************************************************************************/
/*  script single letter function &e'         exist                        */
/*              returns   0 or 1 in result                                 */
/***************************************************************************/

char    *scr_single_func_e( char * in, char * end, char * * result )
{
    char            *   pchar;
    sub_index           var_ind;
    symvar              symvar_entry;
    symsub          *   symsubval;
    int                 rc;

    end   = end;

    pchar = scan_sym( in + 3 + (*(in + 3) == '&'), &symvar_entry, &var_ind );

    if( symvar_entry.flags & local_var ) {  // lookup var in dict
        rc = find_symvar_l( &input_cbs->local_dict, symvar_entry.name,
                            var_ind, &symsubval );
    } else {
        rc = find_symvar( &global_dict, symvar_entry.name, var_ind,
                          &symsubval );
    }
    if( rc == 2 ) {
        **result = '1';                 // exists
    } else {
        **result = '0';                 // does not exist
    }
    *result  += 1;
    **result = '\0';

    if( *pchar == '.' ) {
        pchar++;                    // skip optional terminating dot
    }
    ProcFlags.substituted = true;       // something changed
    return( pchar );
}


/***************************************************************************/
/*  script single letter function &l'         length                       */
/*              returns   length of value or length of name in result      */
/***************************************************************************/

char    *scr_single_func_l( char * in, char * end, char * * result )
{
    char            *   pchar;
    sub_index           var_ind;
    symvar              symvar_entry;
    symsub          *   symsubval;
    int                 rc;
    int                 len;

    if( *(in + 3) == '&' ) {            // symbol name
        pchar = scan_sym( in + 4, &symvar_entry, &var_ind );

        if( symvar_entry.flags & local_var ) {  // lookup var in dict
            rc = find_symvar_l( &input_cbs->local_dict, symvar_entry.name,
                                var_ind, &symsubval );
        } else {
            rc = find_symvar( &global_dict, symvar_entry.name, var_ind,
                              &symsubval );
        }
        if( rc == 2 ) {
            sprintf( *result, "%lu", (unsigned long)strlen( symsubval->value ) );
        } else {
            sprintf( *result, "%lu", (unsigned long)strlen( symvar_entry.name ) );
        }
    } else {                            // string
        pchar = in + 3;
        len = 0;
        while( !((*pchar == ' ') || (*pchar == '.') || (pchar == end)) ) {
            len++;
            pchar++;
        }
        sprintf( *result, "%d", len );
    }
    *result  += strlen( *result );
    **result = '\0';

    if( *pchar == '.' ) {
        pchar++;                    // skip optional terminating dot
    }
    ProcFlags.substituted = true;       // something changed
    return( pchar );
}


/***************************************************************************/
/*  script single letter function &s'         subscript                    */
/*  script single letter function &S'         superscript                  */
/*                       subscript or superscript is coded in parm fun     */
/***************************************************************************/

char    *scr_single_func_sS( char * in, char * end, char * * result, char fun )
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
        pchar = scan_sym( in + 4, &symvar_entry, &var_ind );

        if( symvar_entry.flags & local_var ) {  // lookup var in dict
            rc = find_symvar_l( &input_cbs->local_dict, symvar_entry.name,
                                var_ind, &symsubval );
        } else {
            rc = find_symvar( &global_dict, symvar_entry.name, var_ind,
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

    if( *pchar == '.' ) {
        pchar++;                    // skip optional terminating dot
    }
    ProcFlags.substituted = true;       // something changed
    return( pchar );
}


/***************************************************************************/
/*  script single letter function &u'         upper                        */
/*                                                                         */
/***************************************************************************/

char    *scr_single_func_u( char * in, char * end, char * * result )
{
    char            *   pchar;
    sub_index           var_ind;
    symvar              symvar_entry;
    symsub          *   symsubval;
    int                 rc;
    char            *   pval;

    end   = end;


    if( *(in + 3) == '&' ) {            // symbol name
        pchar = scan_sym( in + 4, &symvar_entry, &var_ind );

        if( symvar_entry.flags & local_var ) {  // lookup var in dict
            rc = find_symvar_l( &input_cbs->local_dict, symvar_entry.name,
                                var_ind, &symsubval );
        } else {
            rc = find_symvar( &global_dict, symvar_entry.name, var_ind,
                              &symsubval );
        }
        if( rc == 2 ) {
            pval = symsubval->value;    // variable found
        } else {
            pval =  symvar_entry.name;  // not found use variable name
        }
        while( *pval ) {
            **result = toupper( *pval++ );
            *result += 1;
        }
    } else {                            // string
        pchar = in + 3;
        while( !((*pchar == ' ') || (*pchar == '.') || (pchar == end)) ) {
            **result = toupper( *pchar++ );
            *result += 1;
        }
    }
    **result = '\0';

    if( *pchar == '.' ) {
        pchar++;                    // skip optional terminating dot
    }
    ProcFlags.substituted = true;       // something changed
    return( pchar );
}


/***************************************************************************/
/*  script single letter function &w'         width                        */
/*              returns   width in CPI rounded up if neccessary            */
/*                                                                         */
/*  some logic has to be in sync with scr_width() in gsfwidth.c            */
/***************************************************************************/

char    *scr_single_func_w( char * in, char * end, char * * result )
{
    char            *   pchar;
    sub_index           var_ind;
    symvar              symvar_entry;
    symsub          *   symsubval;
    int                 rc;
    int                 len;
    uint32_t            width;

    if( *(in + 3) == '&' ) {            // symbol name
        pchar = scan_sym( in + 4, &symvar_entry, &var_ind );

        if( symvar_entry.flags & local_var ) {  // lookup var in dict
            rc = find_symvar_l( &input_cbs->local_dict, symvar_entry.name,
                                var_ind, &symsubval );
        } else {
            rc = find_symvar( &global_dict, symvar_entry.name, var_ind,
                              &symsubval );
        }
        if( rc == 2 ) {
            width = cop_text_width( symsubval->value, strlen( symsubval->value ), g_curr_font );
        } else {

        /*******************************************************************/
        /* for undefined variable calc length of name, & and perhaps *     */
        /*******************************************************************/
            width = cop_text_width( symvar_entry.name, strlen( symvar_entry.name ), g_curr_font )
                    + wgml_fonts[g_curr_font].width.table['&'];

            if( symvar_entry.flags & local_var ) {  // add width of *
                width += wgml_fonts[g_curr_font].width.table['*'];
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
#if 0                                   // perhaps as debug output
    sprintf( *result + strlen( *result ), " %d", len );
#endif
    *result  += strlen( *result );
    **result = '\0';

    if( *pchar == '.' ) {
        pchar++;                    // skip optional terminating dot
    }
    ProcFlags.substituted = true;       // something changed
    return( pchar );
}


/***************************************************************************/
/*  script single letter functions unsupported   process to comsume        */
/*                                               variable for scanning     */
/***************************************************************************/

static  char    *scr_single_func_unsupport( char * in, char * * result )
{
    char        charstr[2];

    result = result;
    charstr[0] = *(in + 1);
    charstr[1] = '\0';
    g_warn( wng_func_unsupport, charstr );
    g_info_inp_pos();

    // do nothing
    return( in + 3 );
}


/***********************************************************************/
/*  Some single letter functions are implemented here                  */
/*                                                                     */
/*  functions used within the OW doc build system:                     */
/*   &e'  existance of variable 0 or 1                                 */
/*   &l'  length of variable content or if undefined length of name    */
/*   &u'  upper                                                        */
/*                                                                     */
/*   &s'  subscript                                                    */
/*   &S'  superscript                                                  */
/*                                                                     */
/*   other single letter functions are not used AFAIK                  */
/*                                                                     */
/*   &w'  width for internal use                                       */
/***********************************************************************/

char    *scr_single_funcs( char * in, char * end, char * * result )
{
    char            *   pw;

    if( *(in + 2) == '\'' ) {
        switch( *(in + 1) ) {
        case  'E' :                     // exist function
        case  'e' :                     // exist function
            pw = scr_single_func_e( in, end, result );
            break;
        case  'L' :                     // length function
        case  'l' :                     // length function
            pw = scr_single_func_l( in, end, result );
            break;
        case  'S' :                     // superscript
            pw = scr_single_func_sS( in, end, result, function_superscript );
            break;
        case  's' :                     // subscript
            pw = scr_single_func_sS( in, end, result, function_subscript );
            break;
        case  'U' :                     // upper function
        case  'u' :                     // upper function
            pw = scr_single_func_u( in, end, result );
            break;
        case  'W' :                     // width function
        case  'w' :                     // width function
            pw = scr_single_func_w( in, end, result );
            break;
        default:
            pw = scr_single_func_unsupport( in, result );
            wng_count++;
            break;
        }
    } else {
        out_msg( "ERR_Logic error in gsfunelu.c\n" );
        err_count++;
        g_suicide();
    }
    ProcFlags.substituted = true;
    return( pw );
}


