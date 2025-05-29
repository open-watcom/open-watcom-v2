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
* Description: implement .in (indent)  script control word
*
*  comments are from script-tso.txt
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/*  round indent to get whole characters                                   */
/*  can be relative i.e. negative                                          */
/***************************************************************************/

static  int round_indent( su * work )
{
    return( conv_hor_unit( work, g_curr_font ) * CPI / g_resh * g_resh / CPI );
}

/***************************************************************************/
/* INDENT causes  text to  be indented  with respect  to the  left and/or  */
/* right ends of the output line.                                          */
/*                                                                         */
/*      +-------+--------------------------------------------------+       */
/*      |       |                                                  |       */
/*      |  .IN  |    <0|h|+h|-h|*> <0|i|+i|-i|*>                   |       */
/*      |       |                                                  |       */
/*      +-------+--------------------------------------------------+       */
/*                                                                         */
/* These indents remain in effect for all subsequent output lines,         */
/*   Including new paragraphs, new footnotes, and new pages.   It remains  */
/*   in effect even if ".FO NO" is specified,                              */
/*   until reset by the next occurrence of the .IN control word.           */
/*                                                                         */
/* <0|h|+h|-h|*>:  For the first operand, a value of the form "h" indents  */
/*    the start of all  subsequent output "h" spaces to the  right of the  */
/*    start of the  line.   An operand of  the form "+h" adds  "h" to the  */
/*    current left  indent.   An operand of  the form "-h"  subtracts "h"  */
/*    from the  current left  indent or makes  it zero  if the  result is  */
/*    negative.   An  operand of  the form  "*" leaves  the current  left  */
/*    indent unchanged.                                                    */
/* <0|i|+i|-i|*>:  The second  operand specifies a "right  indent" value.  */
/*    A value  of the  form "i" or  "+i" will move  the right  indent "i"  */
/*    spaces to the right of its current  position.   A value of the form  */
/*    "-i" will  move the  right indent  "i" spaces  to the  left of  its  */
/*    current position.    A value of the  form "0-i" will set  the right  */
/*    indent "i" spaces to the left end  of the output line.   A value of  */
/*    the form "*" leaves the current right indent unchanged.              */
/*                                                                         */
/* This control word causes a break.   The initial value for the Left and  */
/* Right Indent is  zero.   Specifying ".IN 0 0" or  ".IN" will terminate  */
/* any left and right indents currently in effect.  An .IN will terminate  */
/* any Offset  (.OF),  Indent Line (.IL)   or Undent (.UN)   currently in  */
/* effect unless a first operand of "*" is specified.                      */
/*                                                                         */
/* ! .of .il .un are not used in OW documentation                          */
/* ! a right indent value of the form "0-i" is not used / implemented      */
/*                                                                         */
/***************************************************************************/

void    scr_in( void )
{
    const char      *pa;
    const char      *p;
    int             len;
    char            cwcurr[4];
    bool            scanerr;
    su              indentwork;
    int             newindentl;
    int             newindentr;
    bool            su_relative;

    static  int oldindent;

    cwcurr[0] = SCR_char;
    cwcurr[1] = 'i';
    cwcurr[2] = 'n';
    cwcurr[3] = '\0';

    su_relative = false;
    p = scandata.s;
    SkipSpaces( p );                    // next word start
    pa = p;
    SkipNonSpaces( p );                 // end of word
    newindentl = 0;
    newindentr = 0;
    len = p - pa;
    if( len > 0 ) {                     // omitted means reset to default
        if( *pa == '*' ) {              // keep old indent value
            newindentl = g_indentl;     // prepare keeping old values
            p = pa + 1;
        } else {
            p = pa;
            scanerr = cw_val_to_su( &p, &indentwork );
            if( scanerr ) {
                xx_line_err_exit_c( ERR_SPC_NOT_VALID, pa );
                /* never return */
            }
            su_relative = indentwork.su_relative;
            newindentl = round_indent( &indentwork );
        }
        SkipSpaces( p );
        if( *p == '\0' ) {              // zero right indent
        } else if( *p == '*' ) {        // keep old indentr value
            newindentr = g_indentr;
            p++;
        } else {
            pa = p;
            scanerr = cw_val_to_su( &p, &indentwork );
            if( scanerr ) {
                xx_line_err_exit_cc( ERR_MISS_INV_OPT_VALUE, cwcurr, pa );
                /* never return */
            }
            /***************************************************************/
            /*  indent right is always relative or 0 for reset to default  */
            /***************************************************************/
            if( indentwork.su_whole + indentwork.su_dec != 0) {
                newindentr = g_indentr + round_indent( &indentwork );
            }
        }
    }
    g_indentl = newindentl;
    g_indentr = newindentr;
    /*
     * Reset margin(s) to reflect the current IN offsets
     */
    if( su_relative ) {
        if( ProcFlags.in_reduced ) {
            t_page.cur_left = oldindent;
        }
        t_page.cur_left += g_indentl;
    } else {
        t_page.cur_left = g_indentl;
    }
    t_page.max_width = t_page.last_pane->col_width + g_indentr;
    /*
     * Reduce t_page.cur_left to 0 if g_indentl made it negative
     */
    ProcFlags.in_reduced = false;       // flag, if on, is active until next IN
    if( ( (int)t_page.page_left + t_page.cur_left ) < 0 ) {
        oldindent = t_page.cur_left;
        t_page.cur_left = 0;
        ProcFlags.in_reduced = true;    // set flag to record virtual reduction of in value
    }
    t_page.cur_width = t_page.cur_left;

    scan_restart = (char *)p;
    return;
}
