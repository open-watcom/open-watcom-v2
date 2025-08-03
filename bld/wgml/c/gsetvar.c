/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 200--2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  implement .se and .sr script control words
*
****************************************************************************/


#include "wgml.h"

/* Special WGML 4.0 compatibility: When scanning symbol names,
 * the scanning stops after 10 characters (not including the '&').
 * It is thus possible to reference symbols with maximum-length
 * names without properly separating them, such as
 *   &longsymname
 * and &longsymnam will be resolved if it exists.
 */
static char *get_symbol_name( const char *p, char *symname, bool splittable )
{
    int     i;

    i = 0;
    while( is_symbol_char( *p ) ) {
        if( i == SYM_NAME_LENGTH
          && splittable )
            /* break */;
        if( i < SYM_NAME_LENGTH ) {
            if( i == 3
              && symname[0] == 's'
              && symname[1] == 'y'
              && symname[2] == 's' ) {
                symname[0] = '$';   // create sys shortcut $
                i = 1;
            }
            symname[i++] = my_tolower( *p );
        }
        p++;
    }
    symname[i] = '\0';
    return( (char *)p );
}

/*****************************************************************************/
/* construct symbol name and optionally subscript from input                 */
/* Note: p must point to a symbol name, as no trailing "=<value" is required */
/*****************************************************************************/

char *scan_sym( char *p, symvar *sym, sub_index *subscript, char **result, bool splittable )
{
    char            *pend;
    int             p_level;
    char            *psave;
    char            quote;
    char            *sym_start;
    int             rc;
    sub_index       var_ind;
    symsub          *symsubval;

    (void)result;

    psave = p;
    g_scan_err = false;
    sym->next = NULL;
    sym->flags = SF_none;
    *subscript = SI_no_subscript;       // not subscripted

    SkipSpaces( p );                    // skip over spaces
    if( *p == d_q
      || *p == s_q
      || *p == l_q ) {
        quote = *p++;                   // skip over start quote
    } else {
        quote = '\0';
    }
    if( *p == '*' ) {                   // local var
        p++;                            // skip over '*' character
        sym->flags |= SF_local_var;
    }
    sym_start = p;
    p = get_symbol_name( p, sym->name, splittable );
    if( p == sym_start ) {              // special for &*
        if( *p != ampchar ) {           // not &*&xx construct
            if( (sym->flags & SF_local_var)
              && (input_cbs->fmflags & II_tag_mac) ) {
                strcpy( sym->name, MAC_STAR_NAME );
            } else if( (sym->flags & SF_local_var)
              && (input_cbs->fmflags & II_file) ) {
                strcpy( sym->name, MAC_STAR_NAME );
            } else {
                g_scan_err = true;
            }
        }
    } else if( p > sym_start + SYM_NAME_LENGTH ) {
        /*
         * SC--074 For the symbol '%s'
         *     The length of a symbol cannot exceed ten characters
         */
        if( !g_scan_err ) {
            g_scan_err = true;
            if( !ProcFlags.suppress_msg ) {
                symbol_name_length_err_exit( sym_start );
                /* never return */
            }
        }
    }
    if( quote != '\0'
      && quote == *p ) {
        p++;                                // skip over end quote
    }
    pend = p;                               // char after symbol name if not subscripted

    if( !g_scan_err
      && (*p == '(') ) {      // subscripted ?
        // find true end of subscript
        psave = p;
        p_level = 0;
        while( *p != '\0' ) {               // to end of buffer
            if( *p == '(' ) {
                p_level++;
            } else if( *p == ')' ) {
                p_level--;
                if( p_level == 0 ) {
                    break;
                }
            }
            p++;
        }
        pend = p + 1;                       // character after outermost ')'

        if( p_level > 0 ) {                 // at least one missing ')'
            /* Note: missing ')' is not an error in wgml 4.0 */
            g_scan_err = true;
        } else {
            p = psave + 1;
            if( *p == ')' ) {               // () is auto increment
                p++;
                var_ind = 0;
                rc = find_symvar_sym( sym, var_ind, &symsubval );
                if( rc > 0 ) {              // variable exists use last_auto_inc
                    *subscript = symsubval->base->last_auto_inc + 1;
                } else {
                    *subscript = 1;         // start with index 1
                }
                sym->flags |= SF_auto_inc | SF_subscripted;
            } else if( *p == '*' ) {        // * concatenates all elements
                p++;
                if( *p == '+' ) {
                    *subscript = SI_pos_subscript; // positive indices only
                    p++;
                } else if( *p == '-' ) {
                    *subscript = SI_neg_subscript; // negative indices only
                    p++;
                } else {
                    *subscript = SI_all_subscript; // all indices
                }
                if( *p != ')' ) {
                    g_scan_err = true;
                }
            } else {
                char            *pa;
                char            valbuf[BUF_SIZE + 1];
                condcode        cc;
                getnum_block    gn;
                unsigned        len;

                len = pend - p - 1;
                if( len > BUF_SIZE )
                    len = BUF_SIZE;
                strncpy( valbuf, p, len );
                valbuf[len] = '\0';
                pa = valbuf;
                ProcFlags.unresolved = false;
                finalize_subscript( &pa, splittable );
                if( ProcFlags.unresolved ) {
                    g_scan_err = true;
                } else {
                    gn.arg.s = valbuf;
                    gn.arg.e = valbuf;
                    while( *gn.arg.e != '\0' && (*gn.arg.e != ')') ) {
                        gn.arg.e++;
                    }
                    gn.ignore_blanks = false;
                    cc = getnum( &gn );     // try numeric expression evaluation
                    if( cc == CC_pos
                      || cc == CC_neg ) {
                        *subscript = gn.result;
                        if( *p == ')' ) {
                            p++;
                        }
                        SkipDot( p );
                        sym->flags |= SF_subscripted;
                    } else {
                        if( !g_scan_err
                          && !ProcFlags.suppress_msg ) {
                            xx_line_err_exit_c( ERR_SUB_INVALID, p );
                            /* never return */
                        }
                        g_scan_err = true;
                    }
                }
            }
        }
        if( g_scan_err ) {
            p = psave;
        } else {
            p = pend;
        }
    }
    return( p );
}

/***************************************************************************/
/*  processing  SET                                                        */
/*                                                                         */
/*                          = <character string>                           */
/*         .SE       symbol = <numeric expression>                         */
/*                          <OFF>                                          */
/*  symbol may be subscripted (3) or () for auto increment 1 - n           */
/*                                                                         */
/*  This summarizes SR, since SE extends SR and SR is not implemented      */
/*         .se x1     off                                                  */
/*         .se x2a    'string (Note 1)                                     */
/*         .se x2b    'string (Note 1)                                     */
/*         .se x3a =  string                                               */
/*         .se x3b =  'string (Note 2)                                     */
/*         .se x4c =  'string'(Note 2)                                     */
/*         .se n1  =  1234                                                 */
/*         .se n2a =  (1+(2+5)/6)                                          */
/*         .se n2b =  -1+(2+5)/6)                                          */
/*  Note 1: these apply only to ', the first of which must be present      */
/*          at least one space must precede the initial '                  */
/*  Note 2: these apply to all delimiters: ', ", /, |, !, ^, 0x9b and,     */
/*          apparently, 0xdd and 0x60                                      */
/*          the final delimiter must be followed by a space or '\0'        */
/*          this means that 'abc'def' is a seven-character delimited value */
/*          provided it is at the end of the line or followed by a space   */
/*                                                                         */
/*  Except for local symbol *, spaces are removed from the end of the line */
/*  before further processing (so that spaces inside delimiters are not    */
/*  afftected, but any following the closing delimiter are)                */
/*                                                                         */
/* NOTE: it must use g_scandata pointers instead of relation to              */
/*       null terminating character because it is called from macro        */
/*       parameters processing for approriate variable setup in input      */
/*       buffer without null termination.                                  */
/*                                                                         */
/***************************************************************************/

void    scr_se( void )
{
    char            *p;
    char            *valstart;
    int             rc;
    sub_index       subscript;
    symsub          *symsubval;
    symvar          sym;
    unsigned        len;
    getnum_block    gn;

    subscript = SI_no_subscript;                       // not subscripted
    g_scan_err = false;
    p = scan_sym( g_scandata.s, &sym, &subscript, NULL, false );

    if( strcmp( MAC_STAR_NAME, sym.name ) != 0 ) {  // remove trailing blanks from all symbols except *
        while( g_scandata.e-- > p && *g_scandata.e == ' ' )
            /* empty */;
        g_scandata.e++;
    }

    if( ProcFlags.blanks_allowed ) {
        SkipSpacesTok( p, g_scandata.e );                        // skip over spaces
    }
    if( p >= g_scandata.e ) {
        if( !ProcFlags.suppress_msg ) {
            xx_line_err_exit_c( ERR_EQ_EXPECTED, p);
            /* never return */
        }
        g_scan_err = true;
    }
    if( !g_scan_err ) {
        if( *p == ')' ) {
            p++;
        }
        valstart = p;
        if( *p == '=' ) {                       // all other cases have no equal sign (see above)
            p++;
            if( ProcFlags.blanks_allowed ) {
                SkipSpacesTok( p, g_scandata.e ); // skip over spaces to value
            }
            valstart = p;
            len = g_scandata.e - p;
            if( is_quote_char( *valstart ) ) {  // quotes ?
                p++;
                while( p < g_scandata.e ) {
                    /*
                     * TODO! ????
                     * remove final character, if it matches the start character
                     * look for quote end (must match and be at eol or followed by a space)
                     *
                     *  if( (*valstart == *p) && (!*(p+1) || (*(p+1) == ' ')) ) {
                     */
                    if( (*valstart == p[0])
                      && p + 1 >= g_scandata.e ) {
                        break;
                    }
                    ++p;
                }
                if( (valstart < p)
                  && (*p == *valstart) ) { // delete quotes if more than one character
                    valstart++;
                }
                len = p - valstart;
            } else {                                // numeric or undelimited string
                condcode        cc;

                gn.arg.s = p;
                gn.arg.e = g_scandata.e;
                gn.ignore_blanks = true;
                cc = getnum( &gn );             // try numeric expression evaluation
                if( cc == CC_pos
                  || cc == CC_neg ) {
                    valstart = gn.resultstr;
                    len = gn.length;
                }                               // if notnum treat as character value
            }
            rc = add_symvar_sym( &sym, valstart, len, subscript, sym.flags );
        } else if( *p == '\'' ) {               // \' may introduce valid value
            if( *(p - 1) == ' ' ) {             // but must be preceded by a space
                p++;
                while( p < g_scandata.e && (*valstart != *p) ) {  // look for final \'
                    p++;
                }
                valstart++;                                 // delete initial \'
                rc = add_symvar_sym( &sym, valstart, p - valstart, subscript, sym.flags );
            } else {                                        // matches wgml 4.0
                if( !ProcFlags.suppress_msg ) {
                    xx_line_err_exit_c( ERR_EQ_EXPECTED, p );
                    /* never return */
                }
                g_scan_err = true;
            }
        } else if( strncmp( p, "off", 3 ) == 0 ) {       // OFF
            p += 3;
            rc = find_symvar_sym( &sym, subscript, &symsubval );
            if( rc == 2 ) {
                symsubval->base->flags |= SF_deleted;
            }
        } else {
            if( !ProcFlags.suppress_msg ) {
                xx_warn_cc( WNG_MISS_INV_VALUE, sym.name, p );
            }
            g_scan_err = true;
        }
    }
    scan_restart = g_scandata.e;
    return;
}
