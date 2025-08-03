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
* Description: utility functions for arguments
*
****************************************************************************/


#include "wgml.h"

/***************************************************************************/
/* validate and return the character parameter, or raise an error          */
/***************************************************************************/

char parse_char( const char *p, unsigned len )
{
    char        c;

    c = '\0';
    if( len == 1 ) {
        c = p[0];
    } else if( len == 2 ) {         // 2 hex characters
        if( my_isxdigit( p[0] ) && my_isxdigit( p[1] ) ) {
            if( my_isdigit( p[0] ) ) {
                c = p[0] - '0';
            } else {
                c = my_toupper( p[0] ) - 'A' + 10;
            }
            if( my_isdigit( p[1] ) ) {
                c = c * 16 + p[1] - '0';
            } else {
                c = c * 16 + my_toupper( p[1] ) - 'A' + 10;
            }
        } else {
            xx_line_err_exit_ci( ERR_CW_NOT_CHAR, p, len );
            /* never return */
        }
    } else {
        xx_line_err_exit_ci( ERR_CW_NOT_CHAR, p, len );
        /* never return */
    }

    return( c );
}

/***************************************************************************/
/* return true if the character parameter is a string delimiter            */
/***************************************************************************/

bool is_quote_char( char c )
{
    if( c == s_q || c == d_q || c == slash || c == excl  || c == cent ||
        ((c == l_q) && (c != cw_sep_char)) ||
        c == not_c || c == vbar1 || c == vbar2 ) {
        return( true );
    }
    return( false );
}

/***************************************************************************/
/* return true if the character parameter is a string delimiter            */
/***************************************************************************/

bool is_base_quote_char( char c )
{
    if( c == '"'
      || c == '\''
      || c == '`' ) {
        return( true );
    }
    return( false );
}

/***************************************************************************/
/*  scan blank delimited argument perhaps quoted                           */
/*                                                                         */
/*  extension: if unquoted and equalsign, then quoted parm allowed         */
/*            *var="value "                                                */
/*                                                                         */
/*  single-char arguments are special-cased                                */
/***************************************************************************/

condcode getarg( void )
{
    condcode        cc;
    char            *p;
    char            quote;
    char            valquote;

    cc = CC_omit;                          // arg omitted
    if( g_scandata.s < g_scandata.e ) {     // already at end
        p = g_scandata.s;
        while( *p == ' ' && p < g_scandata.e ) {// skip leading blanks
            p++;
        }
        if( p == g_scandata.e ) {
            return( CC_omit );             // nothing found
        }

        if( p == g_scandata.e - 1 ) {     // one character token found
            arg_flen = 1;
            g_tok_start = p;
            g_scandata.s = p + 1;         // address of start for next call
            return( CC_pos );              // arg found
        }

        valquote = ' ';
        g_tok_start = p;

        if( is_quote_char( *p ) ) {     // arg starts with quote
            quote = *p++;
        } else {
            quote = ' ';
        }
        for( ;; p++ ) {
            if( p == g_scandata.e || *p == '\0' ) {
                if( quote != ' ' ) {
                    quote = ' ';
                    p = g_tok_start;    // find end of space-delimited token
                    while( (p < g_scandata.e) && (*p != ' ') ) {
                        p++;
                    }
                }
                break;
            }
            if( *p == quote ) {
                break;
            }
            if( quote == ' ' && (p[0] == '=') && is_quote_char( p[1] ) ) {
                valquote = p[1];
                p += 2;
                for( ; *p != '\0' && p < g_scandata.e; p++ ) {
                    if( *p == valquote ) {
                        p++;
                        break;
                    }
                }
                break;
            }
        }
        if( quote != ' ' ) {
            g_tok_start++;
            g_scandata.s = p + 1;         // address of start for next call
        } else {
            g_scandata.s = p;             // address of start for next call
        }
        arg_flen = p - g_tok_start;     // length of multichar arg
        if( arg_flen > 0 ) {
            if( quote != ' ' ) {
                cc = CC_quotes;            // quoted arg found
            } else {
                cc = CC_pos;               // arg found
            }
        } else {
            if( quote != ' ' ) {
                cc = CC_quotes0;           // Nullstring
            } else {
                cc = CC_omit;              // length zero
            }
        }
    }
    return( cc );
}

/***************************************************************************/
/*  scan quoted string argument                                            */
/*  special for if terms                                                   */
/***************************************************************************/

condcode getqst( void )
{
    condcode        cc;
    char            *p;
    char            quote;

    cc = CC_omit;                          // arg omitted
    if( g_scandata.s < g_scandata.e ) {     // already at end
        p = g_scandata.s;
        while( *p == ' ' && p < g_scandata.e ) {// skip leading blanks
            p++;
        }

        if( p == g_scandata.e ) {
            return( CC_omit );             // nothing found
        }

        quote = ' ';
        g_tok_start = p;
        if( is_quote_char( *p ) ) {
            quote = *p++;               // single or double quotes, vertical bar and cent
        } else {
            quote = ' ';
        }
        for( ; *p != '\0' && p < g_scandata.e; p++ ) {  // look for end of string
            if( *p == quote ) {
                if( quote == ' ' )
                    break;
                if( p[1] == '\0' || p[1] == ' ' ) {
                    break;              // quote followed by blank or null is end
                }
                if( p[1] == quote ) {
                    continue;           // 2 quote chars not end of string
                }
            }
        }
        if( quote != ' ' ) {
            g_tok_start++;
            g_scandata.s = p + 1;         // start address for next call
            arg_flen = p - g_tok_start; // length of arg
        } else {
            g_scandata.s = p;             // address of start for next call
            arg_flen = p - g_tok_start; // length of arg
        }
        if( arg_flen > 0 ) {
            if( quote != ' ' ) {
                if( *p != quote ) {
                    cc = CC_no;            // only start quote found
                } else {
                    cc = CC_quotes;        // quoted arg found
                }
            } else {
                cc = CC_no;                // not quoted
            }
        } else {
            cc = CC_omit;                  // length zero
        }
    }
    return( cc );
}

/*
 * Test character as valid for a GML Tag name
 */

bool is_tag_char( char c )
{
    bool    test;

    test = my_isalnum( c );
    return( test );
}

/*
 * Test character as valid for a GML predefined attribute name
 */

bool is_tag_att_char( char c )
{
    bool    test;

    test = my_isalnum( c );
    if( !test ) {
        test = ( c == '_' );
    }
    return( test );
}

/*
 * Test character as valid for a Layout predefined attribute name
 */

bool is_lay_att_char( char c )
{
    bool    test;

    test = my_isalpha( c );
    if( !test ) {
        test = ( c == '_' );
    }
    return( test );
}

/*
 * Test character as valid for a function name
 */

bool is_function_char( char c )
{
    bool    test;

    test = my_isalnum( c );
    return( test );
}

/*
 * Test character as valid for an identifier name
 */

bool is_id_char( char c )
{
    bool    test;

    test = my_isalnum( c );
    return( test );
}

/*
 * Test character as valid for an label name
 */
bool is_label_char( char c )
{
    bool    test;

    test = my_isalnum( c );
    return( test );
}

/*
 * Test character as valid for an space unit value
 */

bool is_su_char( char c )
{
    bool    test;

    test = my_isalnum( c );
    if( !test ) {
        test = ( c == '.' );
    }
    return( test );
}

/*
 * Test character as valid for a macro name
 */

bool is_macro_char( char c )
{
    bool    test;

    test = my_isalnum( c );
    if( !test ) {
        test = ( c == '@' ) || ( c == '#' ) || ( c == '$' ) || ( c == '_' );
    }
    return( test );
}

/*
 * Test character as valid for a symbol name
 */

bool is_symbol_char( char c )
{
    bool    test;

    test = my_isalnum( c );
    if( !test ) {
        test = ( c == '@' ) || ( c == '#' ) || ( c == '$' ) || ( c == '_' );
    }
    return( test );
}

/*
 * Test character for a full stop character
 */

bool is_stop_char( char c )
{
    bool    test;

    test = ( c == '.' ) || ( c == ':' ) || ( c == '!' ) || ( c == '?' );
    return( test );
}

/*
 * Test character for a space or tab character
 */

bool is_space_tab_char( char c )
{
    return( ( c == ' ' ) || ( c == '\t' ) );
}

/*
 * If first and last character are the same and one of the quote chars
 * the start and end pointers are adjusted
 * but only if a and z are not equal (that is, only if the value has more
 * than one character
 */
int unquote_arg( tok_type *arg )
{
    if( (arg->s < arg->e - 1) && (arg->s[0] == arg->e[-1]) && is_quote_char( arg->s[0] ) ) {
        arg->s += 1;
        arg->e -= 1;
    }
    return( arg->e - arg->s );
}

/*
 * get uppercased GML tag name for internal processing
 */
char *get_tagname( const char *p, char *tagname )
{
    int     i;
    char    tagbuf[TAG_NAME_LENGTH + 1];

    if( tagname == NULL )
        tagname = tagbuf;
    i = 0;
    while( is_tag_char( *p ) ) {
        if( i < TAG_NAME_LENGTH ) {
            tagname[i++] = my_toupper( *p );
        }
        p++;
    }
    tagname[i] = '\0';
    return( (char *)p );
}

char *check_tagname( const char *p, char *tagname )
{
    char    *p1;
    char    c;

    if( *p == GML_char ) {
        p++;
        p1 = get_tagname( p, tagname );
        c = *p1;
        if( p1 != p
          && ( c == ' '
          || c == '.'
          || c == '\0'
          || c == GML_char
          || ProcFlags.layout && c == '\t' ) ) {
            return( p1 );
        }
    }
    return( NULL );
}
