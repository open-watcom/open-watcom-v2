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
* Description: implement .co concatenate control
*                        .fo format control
*                        .ju justification control
*                                             incomplete TBD
*  comments are from script-tso.txt
****************************************************************************/

#define __STDC_WANT_LIB_EXT1__  1      /* use safer C library              */

#include "wgml.h"
#include "gvars.h"

/****************************************************************************/
/* FORMAT combines the effect of Concatenate and Justify.                   */
/*                                                                          */
/*      旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커        */
/*      |       |                                                  |        */
/*      |       |    <ON|OFF                                       |        */
/*      |  .FO  |     Left|Right|Centre|                           |        */
/*      |       |     INSIDE|OUTSIDE|HALF>                         |        */
/*      |       |                                                  |        */
/*      읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸        */
/*                                                                          */
/* This control word creates a break.  The "OFF" operand is equivalent to   */
/* the control words Concatenate OFF and Justify OFF.  An omitted operand   */
/* is treated as "ON",  which is equivalent to Concatenate ON and Justify   */
/* ON.  The other possible operands specify Concatenate ON and the appro-   */
/* priate mode of Justify.  See the .CO and .JU descriptions for details.   */
/****************************************************************************/

/***************************************************************************/
/* CONCATENATE  enables  or cancels  the  formation  of output  lines  by  */
/* concatenating input lines and truncating  at the nearest word boundary  */
/* to fit on the output line.                                              */
/*                                                                         */
/*      旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커       */
/*      |       |                                                  |       */
/*      |  .CO  |    <ON|OFF>                                      |       */
/*      |       |                                                  |       */
/*      읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸       */
/*                                                                         */
/*                                                                         */
/* ON:  Output lines will be created by adding words from the input lines  */
/*    until a word is found which will  not fit on the output line.   The  */
/*    resulting output line will therefore have as many words as possible  */
/*    without  exceeding the  line-length  restriction (or  column-length  */
/*    restriction, if in multiple-column mode).                            */
/* OFF:  Suppresses  the concatenation action,   which results in  a "one  */
/*    line in,  one  line out" effect that  may exceed the length  of the  */
/*    output line,   depending on  the lengths of  the input  lines.   If  */
/*    justification (.JU)  is  still enabled,  extra blanks  will then be  */
/*    added between words to extend the line to the right margin.          */
/*                                                                         */
/* This control word  causes a break.   An omitted operand  is treated as  */
/* "ON".   Concatenation is "ON" at the  start of SCRIPT processing,  and  */
/* can only be suppressed  by the .CO or .FO control  words with an "OFF"  */
/* operand.   Multiple  blanks that occur  together within an  input line  */
/* will  all be  retained  in the  concatenated  output  line unless  the  */
/* concatenation  process "breaks"  the  input line  at  that point  when  */
/* adding words to the output line.                                        */
/***************************************************************************/

/****************************************************************************/
/* JUSTIFY causes output lines to be padded with inter-word blanks to the   */
/* right margin.                                                            */
/*                                                                          */
/*      旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커        */
/*      |       |                                                  |        */
/*      |       |    <ON|HALF|OFF|                                 |        */
/*      |  .JU  |     Left|Right|Centre|                           |        */
/*      |       |     INSIDE|OUTSIDE>                              |        */
/*      |       |                                                  |        */
/*      읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸        */
/*                                                                          */
/*                                                                          */
/* OFF:  suppresses the  justification process.   If Concatenate  is OFF,   */
/*    input lines  will be  copied to  output lines  on a  "one for  one"   */
/*    basis;  if  Concatenate is  ON,  the  output line  will retain  its   */
/*    "ragged right" style from the point in the concatenation process at   */
/*    which SCRIPT decides it cannot get any more on the output line.       */
/* ON:  On output devices that support "proportional spacing", the inter-   */
/*    word blanks will be equal between all words on the output line.  On   */
/*    other devices,  the following justification algorithm is used until   */
/*    no blanks remain  to be distributed between the  words:   one extra   */
/*    blank is first placed after each period, exclamation mark, or ques-   */
/*    tion mark in the  output line;  then an extra blank  will be placed   */
/*    after any colon  or semi-colon,  and then after any  comma or right   */
/*    parenthesis;  if more blanks must still  be added they are distrib-   */
/*    uted at equidistant places between words.                             */
/* HALF:  produces an effect between  "ragged right" and "full justifica-   */
/*    tion" by only  distributing half of the blanks that  it would under   */
/*    "ON".                                                                 */
/* <Left|Right|Centre|Center>:   causes  the  output  line  to  be  left-   */
/*    justified, right-justified or centered.                               */
/* <INSIDE|OUTSIDE>:   INSIDE  causes  odd-numbered  pages  to  be  left-   */
/*    justified and  even-numbered pages to be  right-justified;  OUTSIDE   */
/*    does the opposite.                                                    */
/*                                                                          */
/* This control word causes a break.   ON  is in effect until a ".JU OFF"   */
/* or ".FO OFF" is encountered.   An omitted  operand is treated as "ON".   */
/* The  Format control  word combines  the functions  of Concatenate  and   */
/* Justify.                                                                 */
/****************************************************************************/


/***************************************************************************/
/*  process .ju setting, .co too if both set                               */
/***************************************************************************/

static void process_co_ju( bool both , char *cwcurr )
{
    char        *   pa;
    char        *   p;
    int             len;

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
    case 0 :                            // omitted means ON
        if( both ) {
            ProcFlags.concat = true;
        }
        ProcFlags.justify = ju_on;
        scan_restart = p;
        break;
    case 2 :                            // only ON valid
        if( !strnicmp( "ON", pa, 2 ) ) {
            if( both ) {
                ProcFlags.concat = true;
            }
            ProcFlags.justify = ju_on;
            scan_restart = pa + len;
        } else {
            xx_opt_err( cwcurr, pa );
        }
        break;
    case 3 :                            // only OFF valid
        if( !strnicmp( "OFF", pa, 3 ) ) {
            if( both ) {
                ProcFlags.concat = false;
            }
            ProcFlags.justify = ju_off;
            scan_restart = pa + len;
        } else {
            xx_opt_err( cwcurr, pa );
        }
        break;
    case 4 :                            // Left or half valid
        if( !strnicmp( "LEFT", pa, 4 ) ) {
            if( both ) {
                ProcFlags.concat = true;
            }

            /***************************************************************/
            /*  .ju left is treated as .ju off by wgml4.0                  */
            /*  system variable &SYSJU is set to OFF                       */
            /***************************************************************/
            ProcFlags.justify = ju_off; // left is like off for wgml 4.0
            scan_restart = pa + len;
        } else {
            if( !strnicmp( "HALF", pa, 4 ) ) {
                if( both ) {
                    ProcFlags.concat = true;
                }
                ProcFlags.justify = ju_half;
                scan_restart = pa + len;
            } else {
                xx_opt_err( cwcurr, pa );
            }
        }
        break;
    case 5 :                            // only Right valid
        if( !strnicmp( "RIGHT", pa, 5 ) ) {
            if( both ) {
                ProcFlags.concat = true;
            }
            ProcFlags.justify = ju_right;
            scan_restart = pa + len;
        } else {
            xx_opt_err( cwcurr, pa );
        }
        break;
    case 6 :                            // center or inside valid
        if( !strnicmp( "CENTER", pa, 6 ) || !strnicmp( "CENTRE", pa, 6 ) ) {
            if( both ) {
                ProcFlags.concat = true;
            }
            ProcFlags.justify = ju_centre;
            scan_restart = pa + len;
        } else {
            if( !strnicmp( "INSIDE", pa, 6 ) ) {
                if( both ) {
                    ProcFlags.concat = true;
                }
                ProcFlags.justify = ju_inside;
                scan_restart = pa + len;
            } else {
                xx_opt_err( cwcurr, pa );
            }
        }
        break;
    case 7 :                            // only outside valid
        if( !strnicmp( "OUTSIDE", pa, 7 ) ) {
            if( both ) {
                ProcFlags.concat = true;
            }
            ProcFlags.justify = ju_outside;
            scan_restart = pa + len;
        } else {
            xx_opt_err( cwcurr, pa );
        }
        break;
    default:
        xx_opt_err( cwcurr, pa );
        break;
    }
    return;
}

/***************************************************************************/
/*  scr_co    implement .co concatenate control word                       */
/***************************************************************************/

void    scr_co( void )
{
    char        *   pa;
    char        *   p;
    int             len;
    char            cwcurr[4 ];

    cwcurr[0] = SCR_char;
    cwcurr[1] = 'c';
    cwcurr[2] = 'o';
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
    case 0 :                            // omitted means ON
        ProcFlags.concat = true;
        scan_restart = pa;
        break;
    case 2 :                            // only ON valid
        if( !strnicmp( "ON", pa, 2 ) ) {
            ProcFlags.concat = true;
            scan_restart = pa + len;
        } else {
            xx_opt_err( cwcurr, pa );
        }
        break;
    case 3 :                            // only OFF valid
        if( !strnicmp( "OFF", pa, 3 ) ) {
            ProcFlags.concat = false;
            scan_restart = pa + len;
        } else {
            xx_opt_err( cwcurr, pa );
        }
        break;
    default:
        xx_opt_err( cwcurr, pa );
        break;
    }
    return;
}


/***************************************************************************/
/*  scr_fo    implement .fo format control                                 */
/***************************************************************************/

void    scr_fo( void )
{
    char            cwcurr[4];

    cwcurr[0] = SCR_char;
    cwcurr[1] = 'f';
    cwcurr[2] = 'o';
    cwcurr[3] = '\0';
    process_co_ju( true, cwcurr );      // .ju and .co processing
}


/***************************************************************************/
/*  scr_ju    implement .ju justify control                                */
/***************************************************************************/

void    scr_ju( void )
{
    char            cwcurr[4];

    cwcurr[0] = SCR_char;
    cwcurr[1] = 'j';
    cwcurr[2] = 'u';
    cwcurr[3] = '\0';
    process_co_ju( false, cwcurr );     // only .ju processing

}
