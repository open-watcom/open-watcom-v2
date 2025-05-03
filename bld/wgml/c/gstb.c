/****************************************************************************
*
*                            Open Watcom Project
*
*  Copyright (c) 2004-2010 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  implements control word .tb
*               control word .tp is not implemented
*               fill strings are not implemented; fill chars are
*
****************************************************************************/


#include "wgml.h"

#include "clibext.h"


/***************************************************************************/
/* TAB defines tab-stop positions for subsequent output lines, or a user-  */
/* specified character to be recognized as a TAB (hex code 05) in addition */
/* to the TAB key on the terminal.                                         */
/*       旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커      */
/*       |     |                                                    |      */
/*       |     | <n1 n2 n3 ...>                                     |      */
/*       | .TB | <<'string'|char/>n<L|R|C|'char'> ...>              |      */
/*       |     | <SET <char>>                                       |      */
/*       |     |                                                    |      */
/*       읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸      */
/*                                                                         */
/* This control word causes a break.                                       */
/*                                                                         */
/* SET <char>: defines the user-specified single character "char" that     */
/*    SCRIPT is to recognize as a TAB character in addition to hex-code    */
/*    05. The ".TB" control word itself and ". ", ".DO", ".EL", ".SU",     */
/*    ".TH" and ".UR" will not have their operands altered.                */
/*    If used without a "char", it terminates the recognition of any       */
/*    previous user-defined TAB character. (These capabilities are also    */
/*    provided by the ".DC TB" control word.)                              */
/* n1 n2 n3 ...: defines the output-line tab-stop positions, relative to   */
/*    the current left page margin. The tab-stop positions "n" must be     */
/*    specified in ascending order. The "n" values may be specified as     */
/*    absolute positions, or as "+n" meaning "n positions to the right of  */
/*    the previous tab-stop". The fill strings for each tab-stop position  */
/*    will be the "blank", and each tab-stop position will be left-        */
/*    aligning (in the standard typewriter fashion). At the start of       */
/*    SCRIPT processing, tab-stop positions are initially defined as 6,    */
/*    11, 16, ..., and 81. If .TB is specified with no operands, these     */
/*    initial tab-stop positions are re-instated.                          */
/* n<L|R|C|'char'>: specifies the type of alignment desired for the        */
/*    tab-stop position. Each tab-stop position "n" may be followed by     */
/*    an L (left-aligning, the default if the alignment type is not spec-  */
/*    ified), or an R (right-aligning), or a C (centering), or a single    */
/*    delimited character (meaning "align on that character").             */
/* <'string'|char/>n: specifies the fill string or character to be used.   */
/*    If no fill string or character is specified, blanks will be used in  */
/*    filling to that tab-stop position. A single fill character may be    */
/*    specified as a delimited string, in the form                         */
/*      'char'n                                                            */
/*    or in the form                                                       */
/*      char/n                                                             */
/*    but a multi-character fill string must always be specified in the    */
/*    form                                                                 */
/*      'string'n                                                          */
/*    The characters " and / may also be used to delimit the string.       */
/* <'string'|char/>n<L|R|C|'char'>: the complete specification of a        */
/*    tab-stop position, including both the fill string or character and   */
/*    the alignment type. Each of the tab-stop positions on the .TB        */
/*    control line may be specified in this fashion.                       */
/*                                                                         */
/* Tabs in Multiple Column mode are relative to the beginning of the       */
/* column in which they occur. Under Concatenate (.CO) or Format (.FO),    */
/* tabbing beyond the end of the output line still produces the desired    */
/* result.                                                                 */
/*                                                                         */
/* Fill-String Processing                                                  */
/*                                                                         */
/* Tab Characters present in text input lines are expanded by SCRIPT into  */
/* one or more "fill" characters (blanks, if not specified) to the next    */
/* defined tab-stop position on the output line. The horizontal space      */
/* between the end of the previous text and the start of the next text     */
/* (the tabulation gap) is "filled" with the fill string. If this is a     */
/* single character, the character is propagated throughout the tabulation */
/* gap. A multi-character fill string is handled as follows.               */
/*    The fill string is propagated in an internal workarea, and the       */
/* particular column bounds of the tabulation gap are then used to         */
/* extract the portion required to fill the tabulation gap. For example,   */
/* a fill string of "abc" in a tabulation gap from column 5 to 9 inclusive */
/* will result in the character string "bcabc" filling the tabulation      */
/* gap. If the tabulation gap is smaller than the fill string, only        */
/* a subset of the fill string is extracted. The tabulation gap for a      */
/* character-aligning tab-stop is handled by logically tabbing to column   */
/* "n" and then searching for the alignment character, another TAB, or     */
/* end of the input line; the length of the tabulation gap is subsequently */
/* adjusted so this character will be aligned in column n.                 */
/*                                                                         */
/* NOTES                                                                   */
/*                                                                         */
/* (1) Upon encountering a phrase which cannot be centered or right        */
/*     justified on a particular column, then the next tab setting is      */
/*     obtained which allows the desired result. Thus each phrase under    */
/*     an alignment constraint is bounded by the previous and next tab-    */
/*     stop positions specified.                                           */
/*  NOTE:                                                                  */
/*  fill strings are not supported                                         */
/***************************************************************************/

void    scr_tb( void )
{
    bool            relative;
    char        *   p;
    char        *   pa;
    char        *   pb;
    char            quote;
    condcode        cc;
    getnum_block    gn;
    int             i;
    int             len;

    p = scan_start;
    SkipNonSpaces( p );                     // over tb
    SkipSpaces( p );                        // first token
    pa = p;
    SkipNonSpaces( p );                     // token end
    len = p - pa;

    if( (len == 3) && strnicmp( "set", pa, len ) == 0 ) {
        SkipSpaces( p );                    // tab char
        pa = p;
        SkipNonSpaces( p );                 // end tab char
        len = p - pa;
        if( len == 0 ) {
            tab_char = 0x09;                // reset to default value
        } else if( len == 1 ) {
            tab_char = *pa;                 // set to specified char
        } else {
            xx_line_err_c( err_tab_char, pa );
        }
        add_to_sysdir( "$tb", tab_char );
        add_to_sysdir( "$tab", tab_char );
        SkipSpaces( p );                    // end of line
        pa = p;
        SkipNonSpaces( p );                 // end of line
        len = p - pa;                       // should be "0"
        if( len != 0 ) {
            xx_line_err_c( err_tab_char, pa );
        }
    } else {
        user_tabs.current = 0;              // clear user_tabs
        p = pa;                             // reset to start of first tab

        while( *p != '\0' ) {               // tab stop start
            if( user_tabs.current == user_tabs.length) {
                user_tabs.length += TAB_COUNT;  // add space for new tab stops
                user_tabs.tabs = mem_realloc( user_tabs.tabs, user_tabs.length *
                                            sizeof( tab_stop ) );
            }
            i = user_tabs.current;          // initialize (not done elsewhere)
            user_tabs.tabs[i].column = 0;
            user_tabs.tabs[i].fill_char = ' ';
            user_tabs.tabs[i].alignment = al_left;
            quote = ' ';
            pa = p;

            // Parse fill chars/strings

            if( (*p != '+') && !my_isdigit( *p ) ) {    // potential fill char
                if( (*p == '\'') || (*p == '"') || (*p == '/') ) {
                    quote = *p;                         // initial quote found
                    p++;                                // should be fill char
                    if( *p == '\0' || (*p == ' ') ||
                        (*p == '+') || my_isdigit( *p ) ) { // ' " or / only before tab stop position
                        xx_line_err_c( err_right_delim, pa );
                    }
                }
                user_tabs.tabs[i].fill_char = *p;
                pb = p;                 // save position if not fill char
                p++;                    // should be end delimiter
                if( *p == '\0' ) {      // 'c "c or /c only
                    xx_line_err_c( err_right_delim, pa );
                }

                /* fill strings are not allowed -- yet */

                if( ((quote == ' ') && (*p == '/')) ||
                                       ((quote != ' ') && (*p == quote)) ) {
                    p++;                // final quote found
                } else {
                    if( quote != ' ' ) {        // quoted value started
                        while( *p != '\0' ) {   // find final quote
                            if( *p == quote ) {
                                break;
                            }
                            p++;
                        }
                        if( *p == quote ) { // found: fill string
                            xx_line_err_c( err_tab_fill_string, pa );
                        } else if( *p != '\0' ) {   // not found: format error
                            xx_line_err_c( err_right_delim, pa );
                        }
                    } else {                // format error
                        xx_line_err_c( err_inv_text_before_tab, pa );
                    }
                }
            }

            // Parse the tab stop position

            SkipSpaces( p );
            pa = p;                             // tab position start

            gn.ignore_blanks = false;
            gn.argstart = p;
            SkipNonSpaces( p );                 // tab position end plus 1
            if( *p != '\0' && (p > pa) ) {      // as needed by getnum
                p--;                            // *p is last character of tab stop
            }
            while( (p != pa) && !my_isdigit( *p ) ) { // back up over alignment
                p--;
            }
            pb = p + 1;
            gn.argstop = p;
            cc = getnum( &gn );
            p = pb;                             // alignment start
            if( gn.num_sign == ' ' ) {
                relative = false;
            } else {
                if( gn.num_sign == '+' ) {
                    relative = true;
                } else {
                    xx_line_err_c( err_inv_tab_stop, pa );
                }
            }
            if( cc == notnum ) {
                xx_line_err_c( err_inv_text_before_tab, pa );
            } else {
                if( gn.result <= 0 ) {
                    if( relative ) {
                        xx_line_err_c( err_tab_stop_order, pa );
                    } else {
                        xx_line_err_c( err_inv_tab_stop, pa );
                    }
                } else {
                    if( relative && ( i > 0) ) {
                        gn.result *= tab_col;
                        user_tabs.tabs[i].column = user_tabs.tabs[i-1].column +
                                                   gn.result;
                    } else {
                        gn.result --;
                        gn.result *= tab_col;
                        user_tabs.tabs[i].column = gn.result;
                    }
                }
                if( !relative && (i > 0) ) {
                    if( user_tabs.tabs[i].column <= user_tabs.tabs[i-1].column ) {
                        xx_line_err_c( err_tab_stop_order, pa );
                    }
                }
                user_tabs.current++;
            }

            // Parse the alignment

            user_tabs.tabs[i].alignment = al_left;
            if( *p != '\0' && (*p != ' ') ) {   // space ends tab stop

            /* alignment characters are not allowed -- yet */

                pa = p;                         // potential alignment start
                if( (*p == 'c') || (*p == 'C') ) {
                    user_tabs.tabs[i].alignment = al_center;
                    p++;
                } else if( (*p == 'l') || (*p == 'L') ) {
                    user_tabs.tabs[i].alignment = al_left;
                    p++;
                } else if( (*p == 'r') || (*p == 'R') ) {
                    user_tabs.tabs[i].alignment = al_right;
                    p++;
                } else if( *p == '\'' ) {       // possible alignment character
                    p++;
                    if( *p == '\0' || (*p != ' ') ) {  // not end of tab stop
                        if( *p == '\0' ) {             // ' only
                            xx_line_err_c( err_right_delim, pa );
                        }
                        p++;
                        if( *p == '\0' ) {             // 'c only
                            xx_line_err_c( err_right_delim, pa );
                        } else if( *p == '\'' ) {   // definite alignment character
                            xx_line_err_c( err_tab_align_char, pa );
                        } else {                // 'cc with or without more text
                            xx_line_err_c( err_right_delim, pa );
                        }
                    } else {                    // something else
                        xx_line_err_c( err_inv_text_after_tab, pa );
                    }
                    p++;
                } else {                        // something else
                    xx_line_err_c( err_inv_text_after_tab, pa );
                }
            }
            SkipNonSpaces( p );                 // find end of tab stop
            SkipSpaces( p );                    // find next tab position
        }
    }

    scan_restart = scan_stop + 1;
    return;
}
