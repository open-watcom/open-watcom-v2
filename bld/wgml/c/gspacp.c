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
* Description: implement .pa (page)  script control word
*                        only used options are implemented
*                        i.e.   ODD EVEN and NOSTART per default
*               not implemented are the other options
*
*                        .cp (conditional page) script control word
*                        only used options are implemented
*                        i.e. .cp numbervalue
*
*
*  comments are from script-tso.txt
****************************************************************************/

#define __STDC_WANT_LIB_EXT1__  1      /* use safer C library              */

#include "wgml.h"
#include "gvars.h"

/**************************************************************************/
/* PAGE EJECT positions output to the top of the next physical page.      */
/*                                                                        */
/*      旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커      */
/*      |       |                                                  |      */
/*      |       |    <n|+n|-n <m|+m|-m>>                           |      */
/*      |  .PA  |    <NOSTART>                                     |      */
/*      |       |    <ODD|EVEN>                                    |      */
/*      |       |                                                  |      */
/*      읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸      */
/*                                                                        */
/* This control word causes a break.  When it is encountered, the rest of */
/* the current page is skipped, any saved footnote lines are printed, the */
/* Footing Space  lines are printed,   and a new  page is begun.    If no */
/* operand is specified then the next  page will be numbered sequentially */
/* one more than the current page number.                                 */
/*                                                                        */
/* <n|+n|-n <m|+m|-m>>:   An operand of  the form  "n" will make  the new */
/*    page be numbered "n".   A "+n" will make the new page number be "n" */
/*    greater than the number of the page on which the .PA occurred,  and */
/*    "-n" will make it  be "n" less.   A second operand  of the form "m" */
/*    may optionally be  specified to make SCRIPT number  and count pages */
/*    in the form  "n.m",  incrementing "m" on each page  instead of "n". */
/*    Thus a chapter or an update  to be page-numbered 3,  3.1,  3.2, ... */
/*    would be started by specifying ".PA 3 0".                           */
/* <NOSTART>:  The current page is terminated  and the next page will not */
/*    be started.  This is the normal action.                             */
/* <ODD|EVEN>:  ODD positions to the top of the next odd-numbered page if */
/*    not already  there.   EVEN positions to  the top of the  next even- */
/*    numbered page if not already there.                                 */
/*                                                                        */
/* NOTES                                                                  */
/* (1) If the  STOP option was specified  when SCRIPT was  invoked,  then */
/*     each time the bottom of a page  is reached,  SCRIPT will eject the */
/*     page and pause to allow you to insert the next sheet of paper.     */
/* (2) If the current page is empty and ".EM NO" is in effect, .PA has no */
/*     effect other  than causing  the page number  to be  incremented or */
/*     reset.   Consequently,  top  running titles are not  printed until */
/*     there is a line about to be printed in the text area.  This allows */
/*     you to  do the  "page eject"  and THEN  make whatever  changes are */
/*     required to the page-layout environment.                           */
/* (3) When numbering  pages with  a decimal  portion,  the  even or  odd */
/*     attribute is  determined by summing the  two portions of  the page */
/*     number.   Thus 3.0 is odd and 3.1 is  even,  4.5 is odd and 4.6 is */
/*     even.                                                              */
/* (4) If the current  page number is "3.4"  then a .PA with  no operands */
/*     would place the output at the top of page "4".                     */
/**************************************************************************/

void    scr_pa( void )
{
    char        *   pa;
    char        *   p;
    int             len;
    char            cwcurr[4];

    cwcurr[0] = SCR_char;
    cwcurr[1] = 'p';
    cwcurr[2] = 'a';
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
    switch( len ) {
    case 7 :
        if( strnicmp( "NOSTART", pa, 7 ) ) {
            xx_opt_err( cwcurr, pa );
            break;
        }
        /* fallthru for NOSTART */
    case 0 :
        do_page_out();                 // default action
        reset_t_page();
        break;
    case 3 :
        if( !strnicmp( "ODD", pa, 3 ) ) {
            do_page_out();
            reset_t_page();
            if( (page & 1) ) {          // next page would be even
                do_page_out();
                reset_t_page();
            }
        } else {
            xx_opt_err( cwcurr, pa );
        }
        break;
    case 4 :
        if( !strnicmp( "EVEN", pa, 4 ) ) {
            do_page_out();
            reset_t_page();
            if( !(page & 1) ) {         // next page would be odd
                do_page_out();
                reset_t_page();
            }
        } else {
            xx_opt_err( cwcurr, pa );
        }
        break;
    default:
        xx_opt_err( cwcurr, pa );
        break;
    }
    scan_restart = scan_stop +1;
    return;
}



/***************************************************************************/
/* CONDITIONAL PAGE positions  output to the top of the  next page column  */
/* if insufficient  lines remain in the  current column (or page,   if in  */
/* single-column mode).                                                    */
/*                                                                         */
/*      旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커       */
/*      |       |                                                  |       */
/*      |       |    <v>                                           |       */
/*      |  .CP  |                                                  |       */
/*      |       |    <BEGIN|INLINE|END <0|w>>                      |       */
/*      |       |                                                  |       */
/*      읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸       */
/*                                                                         */
/* The operands of .CP are identical to those of .CC; see the description  */
/* of .CC for details.   The only  difference is that .CP always operates  */
/* on full pages  instead of columns.   If no operand  is specified,  .CP  */
/* will cause a break and then,  if  the output is not already positioned  */
/* at the  top of a page  it will position to  the top of the  next page,  */
/* independent of whether  single- or multiple-column mode  is in effect.  */
/* It should  therefore be used only  in those circumstances where  it is  */
/* desirable to start a new page.   In other cases,  the .CC control word  */
/* should be used instead.                                                 */
/*                                                                         */
/*                                                                         */
/*                                                                         */
/* CONDITIONAL COLUMN positions  output to the top of the  next column if  */
/* insufficient  lines remain  in the  current  column (or  page,  if  in  */
/* single-column mode).                                                    */
/*                                                                         */
/*      旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커       */
/*      |       |                                                  |       */
/*      |       |    <v>                                           |       */
/*      |  .CC  |                                                  |       */
/*      |       |    <BEGIN|INLINE|END <0|w>>                      |       */
/*      |       |                                                  |       */
/*      읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸       */
/*                                                                         */
/* If no operand is specified,  this control  word will cause a break and  */
/* then,  if the output is not already positioned at the top of a column,  */
/* it will position to the top of the next column.                         */
/*                                                                         */
/* v:  With  a vertical  space operand,  a  check is  made for  "v" space  */
/*    remaining in the  current column.   If "v" space does  not remain a  */
/*    new output  column will be started,   which also makes  the current  */
/*    column ineligible for balancing.  If "v" space or more does remain,  */
/*    no action is performed,  but if Balance Columns is "ON" then output  */
/*    in the  current column may  be split  by later balancing  of column  */
/*    text.  If "v" is not positive then no action is performed.           */
/* <BEGIN|INLINE>:  starts a "conditional column" text block.  If started  */
/*    with "BEGIN",  this control word causes  a break.   If started with  */
/*    "INLINE",  it does not cause a break and any not-yet-printed output  */
/*    line will  become the first  line of  the text block.    All output  */
/*    thereafter becomes part of this "block".                             */
/* END <0|w>:   terminates a  "conditional column"  text block.    If the  */
/*    block was started with "INLINE", no break is caused; the block does  */
/*    not  include the  current  not-yet-printed  output line  (if  any).  */
/*    SCRIPT will  compare the size  of the text  block to the  amount of  */
/*    space left in the current column.1) The "END" operand may be further */
/*    qualified by "w"  vertical space units which defaults  to zero.   A  */
/*    new output column will be started if the text block does not fit or  */
/*    if there  will be  less than "w"  space units  left in  the current  */
/*    column after  the text  block is  printed.   If  insufficient space  */
/*    remains in the current column, SCRIPT ejects to the top of the next  */
/*    column before  printing the block.    Otherwise,  the  block prints  */
/*    immediately.    If a  "conditional" SKIP  or  SPACE terminates  the  */
/*    block,  the conditional  SKIPs or SPACEs are put  into effect AFTER  */
/*    the block is output.                                                 */
/*                                                                         */
/* "In-storage" text  blocks are created  via begin/end sequences  of the  */
/* .CC,  .CP,  .FB,  .FK,  and .FN  control words.   These control words,  */
/* control words that cause column or page ejects, and control words that  */
/* alter the columns-per-page environment (.CD, .MC,  .SC)  may NOT occur  */
/* within an in-storage block.                                             */
/*                                                                         */
/* 1 The amount of  space between the current position in  the column and  */
/*   the first line reserved for footnotes (if any)  or the bottom of the  */
/*   text area (if no footnotes are currently waiting to print).           */
/***************************************************************************/


void    scr_cp( void )
{
    char        *   pa;
    char        *   p;
    int             len;
    getnum_block    gn;
    condcode        cc;
    char            cwcurr[4];

    cwcurr[0] = SCR_char;
    cwcurr[1] = 'c';
    cwcurr[2] = 'p';
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
    if( len > 0 ) {                     // no action if no parm
        gn.argstart = pa;
        gn.argstop  = scan_stop;
        gn.ignore_blanks = 0;

        cc = getnum ( &gn );            // try to get numeric value
        if( cc == notnum ) {
            xx_opt_err( cwcurr, pa );
            scan_restart = scan_stop +1;
        } else {
            if( gn.result > 0 ) {       // ignore values < 1
                if( ((gn.result * wgml_fonts[g_curr_font].line_height) + t_page.cur_depth) > t_page.max_depth ) {
                    do_page_out();
                    reset_t_page();
                }
            }
            scan_restart = gn.argstart;
        }
    } else {
       scan_restart = scan_stop +1;
    }
    return;
}

