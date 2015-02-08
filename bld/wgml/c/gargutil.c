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
* Description: utility functions for arguments:
*         garginit             --- initialize operand scan in buff2 (SCR)
*         garginitdot          --- initialize operand scan in buff2 (GML)
*         getarg               --- scan (quoted) blank delimited argument
*         getqst               --- scan quoted string
*         is_xxx_char          --- test for allowed char
*         is_quote_char        --- test for several quote chars
*         parse_char           --- parse any "char" which can also be in hex
*         unquote_if_quoted    --- adjust ptrs for quoted string
*
****************************************************************************/

#define __STDC_WANT_LIB_EXT1__  1      /* use safer C library              */

#include "wgml.h"
#include "gvars.h"

/***************************************************************************/
/* validate and return the character parameter, or raise an error          */
/***************************************************************************/

char parse_char( const char *pa, size_t len )
{
    const char  *p;
    char        c;

    c = '\0';
    p = pa + len;
    if( len == 2 ) {             // 2 hex characters
        if( isxdigit( *pa ) && isxdigit( *(pa + 1) ) ) {
            for( ; len > 0; len-- ) {
                c *= 16;
                if( isdigit( *pa ) ) {
                    c += *pa - '0';
                } else {
                    c += toupper( *pa ) - 'A' + 10;
                }
                pa++;
            }
        } else {
            xx_line_err_len( err_cw_not_char, pa, len );
            return( c );
        }
    } else {
        if( len != 1 ) {
            xx_line_err_len( err_cw_not_char, pa, len );
            return( c );
        }
        c = *pa;
    }

    return( c );
}


bool    is_quote_char( char c )
{
    if( c == s_q || c == d_q || c == slash || c == excl  || c == cent ||
        ((c == l_q) && (c != CW_sep_char)) ||
        c == not_c || c == vbar1 || c == vbar2 ) {
        return( true );
    } else {
        return( false );
    }
}

void    garginit( void )
{
    char    *   p;

    p = buff2;                          // adress of input buffer
    scan_stop = buff2 + buff2_lg - 1;   // store scan stop address
    while( *p != ' ' && p <= scan_stop ) {// search end of script control word
        p++;
    }
    scan_start = p;                     // store control word end address

    tok_start = NULL;                   // clear token start address
}




void    garginitdot( void )
{
    char    *   p;

    p = buff2;                          // adress of input buffer
    scan_stop = buff2 + buff2_lg - 1;   // store scan stop address
    while( *p != ' ' && *p != '.' && p <= scan_stop ) {// search end of gml tag
        p++;
    }
    scan_start = p;                     // store tag end or space address

    tok_start = NULL;                   // clear token start address
}


/***************************************************************************/
/*  scan blank delimited argument perhaps quoted                           */
/*                                                                         */
/* extension: if unquoted and equalsign, then quoted parm allowed          */
/*            *var="value "                                                */
/*                                                                         */
/***************************************************************************/

condcode    getarg( void )
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
        while( *p && *p == ' ' && p <= scan_stop ) {// skip leading blanks
            p++;
        }
        if( p > scan_stop ) {
            return( omit );             // nothing found
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
                }
                break;
            }
            if( *p == ' ' && quote == '\0' ) {  // unquoted, blank is end
                break;
            }
            if( *p == quote ) {
                break;
            }
            if( quote == '\0' && (*p == '=') && is_quote_char( *(p+1) ) ) {
                valquoted = true;
                valquote = *(p+1);
                p += 2;
                for( ; p <= scan_stop; p++ ) {
                    if( *p == valquote ) {
                        p++;
                        break;
                    }
                    if( *p == '\0' ) {
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
        arg_flen = p - tok_start;       // length of arg
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
/*  scan       quoted string argument       special for if terms           */
/***************************************************************************/

condcode    getqst( void )
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
        while( *p && *p == ' ' && p <= scan_stop ) {// skip leading blanks
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
        for( ; p <= scan_stop; p++ ) {  // look for end of string

            if( *p == '\0' ) {          // null char is end
                break;
            }
            if( quoted ) {
                if( *p == quote ) {
                    if( *(p+1) == '\0' || *(p+1) == ' ' ) {
                        break;      // quote followed by blank or null is end
                    }
                    if( *(p+1) == quote ) {
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
 * Test character as valid for an LAYOUT attribute name
 */
bool    is_lay_att_char( char c )
{
    bool    test;

    test = isalpha( c );
    if( !test ) {
        test = ( c == '_' );
    }
    return( test );
}


/*
 * Test character as valid for a function name
 */
bool    is_function_char( char c )
{
    bool    test;

    test = isalnum( c );
    return( test );
}

/*
 * Test character as valid for an identifier name
 */
bool    is_id_char( char c )
{
    bool    test;

    test = isalnum( c );
    return( test );
}

/*
 * Test character as valid for a macro name
 */
bool    is_macro_char( char c )
{
    bool    test;

    test = isalnum( c );
    if( !test ) {
        test = ( c == '@' ) || ( c == '#' ) || ( c == '$' ) || ( c == '_' );
    }
    return( test );
}

/*
 * Test character as valid for a symbol name
 */
bool    is_symbol_char( char c )
{
    bool    test;

    test = isalnum( c );
    if( !test ) {
        test = ( c == '@' ) || ( c == '#' ) || ( c == '$' ) || ( c == '_' );
    }
    return( test );
}

/*
 * Test character for a full stop character
 */
bool    is_stop_char( char c )
{
    bool    test;

    test = ( c == '.' ) || ( c == ':' ) || ( c == '!' ) || ( c == '?' );
    return( test );
}


/*
 * Test character for a space or tab character
 */
bool    is_space_tab_char( char c )
{

    return( ( c == ' ' ) || ( c == '\t' ) );
}


/*
 * If first and last character are the same and one of the quote chars
 * the start and end pointers are adjusted
 */
void    unquote_if_quoted( char **start, char **stop )
{

    if( *start != *stop && is_quote_char( **start ) && **start == *(*stop - 1) ) {
        *start += 1;
        *stop -= 1;
    }
}

