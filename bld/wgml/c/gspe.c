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
* Description: implement .pe (perform)  script control word
*                        only used options are implemented
*                        i.e.   n ON DELETE
*               not implemented 0 OFF       nested perform
*
*  comments are from script-tso.txt
****************************************************************************/
 
#define __STDC_WANT_LIB_EXT1__  1      /* use safer C library              */
 
#include "wgml.h"
#include "gvars.h"
 
 
/**************************************************************************/
/* PERFORM processes the remainder of the current physical input line (if */
/* any) or the next physical input line (if none) the specified number of */
/* times.                                                                 */
/*                                                                        */
/*      旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커      */
/*      |       |                                                  |      */
/*      |  .PE  |    <1|n|ON|OFF|DELETE>                           |      */
/*      |       |                                                  |      */
/*      읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸      */
/*                                                                        */
/* This control  word does not  cause a  break,  although the  line being */
/* performed may.    Perform provides a  primitive facility  for one-line */
/* loops.                                                                 */
/*                                                                        */
/* <1|n|ON>:  If used with no operand,  the  line to be performed will be */
/*    processed once.    An integer  operand "n"  causes the  line to  be */
/*    processed "n" times,   and "ON" causes it  to be processed up  to a */
/*    maximum of 32,767 times or until  Perform is turned OFF or DELETEd. */
/*    performs within performs are allowed.                               */
/* 0:  An zero operand terminates processing  of the line currently being */
/*    performed but will not terminate an active Perform line.            */
/* <OFF|DELETE>:  An "OFF" operand terminates  all processing of the line */
/*    currently being performed.   DELETE terminates all nested levels of */
/*    performs.                                                           */
/*                                                                        */
/* EXAMPLES                                                               */
/* (1) To produce 20 centered lines of "Hi There!":                       */
/*       .pe 20;.ce;Hi There!                                             */
/* (2) Alternatively,  the "object" of the Perform  may be the next phys- */
/*     ical input line:                                                   */
/*       .pe 20                                                           */
/*       .ce;Hi There!                                                    */
/* (3) To format the first 1000 numbers into 10 paragraphs of 100 each:   */
/*       .sr i = 0;.* number of numbers done                              */
/*       .sr j = 0;.* "paragraph" counter                                 */
/*       .dm GROUP begin                                                  */
/*       .if &j eq 0 .pp                                                  */
/*       .sr i = &i + 1;&i                                                */
/*       .sr j = &j + 1                                                   */
/*       .if &j eq 10 .sr j = 0                                           */
/*       .dm GROUP end                                                    */
/*       .pe on;.GROUP;.if &i ge 1000 .pe delete                          */
/* (4) The same example as above, using nested performs:                  */
/*       .sr i=0;.pe 10;.pp;.pe 100;.sr i=&i+1;&i                         */
/**************************************************************************/
 
 
/***************************************************************************/
/*  fill the data for perform processing                                   */
/***************************************************************************/
 
static  void    init_pe_line( int pe_count )
{
    if( input_cbs->pe_cb.count > 0) {   // count >0 is switch for .pe active
        xx_err( err_nested_pe );
        reset_pe_cb();                  // terminate active .pe
    } else {
        if( get_line( true ) ) {
            input_cbs->pe_cb.count = pe_count;
            input_cbs->pe_cb.ll    =  strlen( buff2 ) + 1;
            input_cbs->pe_cb.line  = mem_alloc( input_cbs->pe_cb.ll );
            strcpy_s( input_cbs->pe_cb.line, input_cbs->pe_cb.ll, buff2 );
        }
    }
    return;
}
 
 
/***************************************************************************/
/*  reset perform   deactivate perform processing                          */
/***************************************************************************/
 
void    reset_pe_cb( void )
{
    input_cbs->pe_cb.count = -1;
    if( input_cbs->pe_cb.line != NULL ) {
        mem_free( input_cbs->pe_cb.line );
        input_cbs->pe_cb.line = NULL;
    }
    return;
}
 
 
void    scr_pe( void )
{
    char        *   pa;
    char        *   p;
    int             len;
    getnum_block    gn;
    condcode        cc;
    char            cwcurr[4];
 
    cwcurr[0] = SCR_char;
    cwcurr[1] = 'p';
    cwcurr[2] = 'e';
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
    if( len == 0 ) {                    // omitted means ON
        init_pe_line( INT_MAX );
    } else {
        gn.argstart = pa;
        gn.argstop = scan_stop + 1;
        gn.ignore_blanks = 0;
 
        cc = getnum( &gn );             // try to get numeric value
        if( cc == notnum ) {
            switch( len ) {
            case 2 :
                if( !strnicmp( "ON", pa, 2 ) ) {
                    init_pe_line( INT_MAX );
                } else {
                    xx_opt_err( cwcurr, pa );
                }
                break;
            case 6 :
                if( !strnicmp( "DELETE", pa, 6 ) ) {
                    reset_pe_cb();
                } else {
                    xx_opt_err( cwcurr, pa );
                }
                break;
            default:
                xx_opt_err( cwcurr, pa );
                break;
            }
        } else {
            scan_start = gn.argstart;
            if( gn.result < 1 ) {
                xx_opt_err( cwcurr, pa );
            } else {
                init_pe_line( gn.result );
            }
        }
    }
    scan_restart = scan_stop +1;
    return;
}
 
