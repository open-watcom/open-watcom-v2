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
 
#define __STDC_WANT_LIB_EXT1__  1      /* use safer C library              */
 
#include "wgml.h"
#include "gvars.h"
 
/***************************************************************************/
/*  round indent to get whole characters                                   */
/*  can be relative i.e. negative                                          */
/***************************************************************************/
 
static  int32_t round_indent( su * work )
{
    return( conv_hor_unit( work ) * CPI / g_resh * g_resh / CPI );
}
 
/***************************************************************************/
/* INDENT causes  text to  be indented  with respect  to the  left and/or  */
/* right ends of the output line.                                          */
/*                                                                         */
/*      旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커       */
/*      |       |                                                  |       */
/*      |  .IN  |    <0|h|+h|-h|*> <0|i|+i|-i|*>                   |       */
/*      |       |                                                  |       */
/*      읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸       */
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
/*                                                                         */
/***************************************************************************/
 
void    scr_in( void )
{
    char        *   pa;
    char        *   p;
    int             len;
    char            cwcurr[4];
    bool            scanerr;
    su              indentwork;
    int32_t         newindent;
    int32_t         newindentr;
 
    cwcurr[0] = SCR_char;
    cwcurr[1] = 'i';
    cwcurr[2] = 'n';
    cwcurr[3] = '\0';
 
    p = scan_start;
    while( *p && *p != ' ' ) {          // over cw
        p++;
    }
    while( *p && *p == ' ' ) {          // next word start
        p++;
    }
    pa = p;
 
    while( *p && *p != ' ' ) {          // end of word
        p++;
    }
    len = p - pa;
    if( len == 0 ) {                    // omitted means reset to default
        newindent = 0;
        newindentr = 0;
    } else {
        newindent  = g_indent;          // prepare keeping old values
        newindentr = g_indentr;
        if( *pa == '*' ) {              // keep old indent value
            p = pa + 1;
        } else {
 
            p = pa;
            scanerr = to_internal_SU( &p, &indentwork );
            if( scanerr ) {
                g_err( err_miss_inv_opt_value, cwcurr, pa );
                err_count++;
                show_include_stack();
            } else {
                newindent = round_indent( &indentwork );
                if( indentwork.su_relative ) {
                    newindent += g_indent;
                }
                if( newindent < 0 ) {
                    newindent = 0;      // minimum value
                }
            }
        }
        while( *p == ' ' ) {
            p++;
        }
        if( *p == '*' ) {               // keep old indentr value
            p++;
        } else {
            pa = p;
            scanerr = to_internal_SU( &p, &indentwork );
            if( scanerr ) {
                g_err( err_miss_inv_opt_value, cwcurr, pa );
                err_count++;
                show_include_stack();
            } else {
 
            /***************************************************************/
            /*  indent right is always relative or 0 for reset to default  */
            /***************************************************************/
 
                if( indentwork.su_whole + indentwork.su_dec != 0) {
                    newindentr = g_indentr + round_indent( &indentwork );
                } else {
                    newindentr = 0;
                }
            }
        }
    }
    g_indent = newindent;
    g_indentr = newindentr;
 
    g_page_right = g_page_right_org + g_indentr;
    ProcFlags.keep_left_margin = false;
    set_h_start();                      // apply new values
    scan_restart = p;
    return;
}
 
