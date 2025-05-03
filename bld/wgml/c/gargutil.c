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
* Description: utility functions for arguments
*
****************************************************************************/


#include "wgml.h"

/***************************************************************************/
/* validate and return the character parameter, or raise an error          */
/***************************************************************************/

char parse_char( const char *p, size_t len )
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
            xx_line_err_ci( err_cw_not_char, p, len );
        }
    } else {
        xx_line_err_ci( err_cw_not_char, p, len );
    }

    return( c );
}

/***************************************************************************/
/* return true if the character parameter is a string delimiter            */
/***************************************************************************/

bool is_quote_char( char c )
{
    if( c == s_q || c == d_q || c == slash || c == excl  || c == cent ||
        ((c == l_q) && (c != CW_sep_char)) ||
        c == not_c || c == vbar1 || c == vbar2 ) {
        return( true );
    } else {
        return( false );
    }
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
    condcode    cc;
    char    *   p;
    char        quote;
    char        valquote;
    bool        quoted;
    bool        valquoted;

    if( scan_stop <= scan_start ) {     // already at end
        cc = omit;                      // arg omitted
    } else {
        p = scan_start;
        while( *p == ' ' && p <= scan_stop ) {// skip leading blanks
            p++;
        }
        if( p > scan_stop ) {
            return( omit );             // nothing found
        }

        if( p == scan_stop) {           // one character token found
            arg_flen = 1;
            tok_start = p;
            scan_start = p + 1;         // address of start for next call
            return( pos );              // arg found
        }

        quote = '\0';
        valquote = '\0';
        quoted = false;
        valquoted = false;
        tok_start = p;

        if( is_quote_char( *p ) ) {     // arg starts with quote
            quote = *p;
            p++;
            quoted = true;
        } else {
            quote = '\0';
            quoted = false;
        }
        for( ;; p++ ) {

            if( p > scan_stop || *p == '\0' ) {
                if( quoted ) {
                    quote = '\0';
                    quoted = false;
                    p = tok_start;              // find end of space-delimited token
                    while( (p < scan_stop) && (*p != ' ') ) {
                        p++;
                    }
                }
                break;
            }
            if( *p == ' ' && quote == '\0' ) {  // unquoted, blank is end
                break;
            }
            if( *p == quote ) {
                break;
            }
            if( quote == '\0' && (p[0] == '=') && is_quote_char( p[1] ) ) {
                valquoted = true;
                valquote = p[1];
                p += 2;
                for( ; *p != '\0' && p <= scan_stop; p++ ) {
                    if( *p == valquote ) {
                        p++;
                        break;
                    }
                }
                break;
            }
        }
        if( quoted ) {
            tok_start++;
            scan_start = p + 1;         // address of start for next call
        } else {
            scan_start = p;             // address of start for next call
        }
        arg_flen = p - tok_start;       // length of multichar arg
        if( arg_flen > 0 ) {
            if( quoted ) {
                cc = quotes;            // quoted arg found
            } else {
                cc = pos;               // arg found
            }
        } else {
            if( quoted ) {
                cc = quotes0;           // Nullstring
            } else {
                cc = omit;              // length zero
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
    condcode    cc;
    char    *   p;
    char        c;
    char        quote;
    bool        quoted;

    if( scan_stop <= scan_start ) {     // already at end
        cc = omit;                      // arg omitted
    } else {
        p = scan_start;
        while( *p == ' ' && p <= scan_stop ) {// skip leading blanks
            p++;
        }

        if( p > scan_stop ) {
            return( omit );             // nothing found
        }

        quote = '\0';
        quoted = false;
        tok_start = p;
        c = *p;
        if( is_quote_char( c ) ) {
            quote = c;      // single or double quotes, vertical bar and cent
            p++;
            quoted = true;
        } else {
            quote = '\0';
            quoted = false;
        }
        for( ; *p != '\0' && p <= scan_stop; p++ ) {  // look for end of string
            if( quoted ) {
                if( *p == quote ) {
                    if( p[1] == '\0' || p[1] == ' ' ) {
                        break;          // quote followed by blank or null is end
                    }
                    if( p[1] == quote ) {
                        continue;       // 2 quote chars not end of string
                    }
                }
            } else {                    // unquoted
                if( *p == ' ' ) {
                    break;              // blank is end
                }
            }
        }
        if( quoted ) {
            tok_start++;
            scan_start = p + 1;         // start address for next call
            arg_flen = p - tok_start;   // length of arg
        } else {
            scan_start = p;             // address of start for next call
            arg_flen = p - tok_start;   // length of arg
        }
        if( arg_flen > 0 ) {
            if( quoted ) {
                if( *p != quote ) {
                    cc = no;            // only start quote found
                } else {
                    cc = quotes;        // quoted arg found
                }
            } else {
                cc = no;                // not quoted
            }
        } else {
            cc = omit;                  // length zero
        }
    }
    return( cc );
}

/*
 * Test character as valid for a predefined attribute name
 */

bool is_att_char( char c )
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

void unquote_if_quoted( char **a, char **z )
{
    if( (*a != *z) && (**a == **z) && is_quote_char( **a ) ) {
        *a += 1;
        *z -= 1;
    }
}

