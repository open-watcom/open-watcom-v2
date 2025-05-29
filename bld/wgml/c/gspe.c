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
* Description: implement .pe (perform)  script control word
*                        only used options are implemented
*                        i.e.   n ON DELETE
*               not implemented 0 OFF       nested perform
*               OFF was implemented to act as DELETE
*               neither is fully implemented, nor (I suppose) is ON,
*               since PE nesting is not supported
*
*  comments are from script-tso.txt
****************************************************************************/


#include "wgml.h"

#include "clibext.h"


/**************************************************************************/
/* PERFORM processes the remainder of the current physical input line (if */
/* any) or the next physical input line (if none) the specified number of */
/* times.                                                                 */
/*                                                                        */
/*      +-------+--------------------------------------------------+      */
/*      |       |                                                  |      */
/*      |  .PE  |    <1|n|ON|OFF|DELETE>                           |      */
/*      |       |                                                  |      */
/*      +-------+--------------------------------------------------+      */
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
        xx_err_exit( ERR_NESTED_PE );
        /* never return */
    }
    if( get_line( true ) ) {
        input_cbs->pe_cb.count = pe_count;
        input_cbs->pe_cb.ll    = strlen( buff2 ) + 1;
        input_cbs->pe_cb.line  = mem_alloc( input_cbs->pe_cb.ll );
        strcpy( input_cbs->pe_cb.line, buff2 );
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
    char            *pa;
    char            *p;
    unsigned        len;
    getnum_block    gn;
    condcode        cc;
    char            cwcurr[4];

    cwcurr[0] = SCR_char;
    cwcurr[1] = 'p';
    cwcurr[2] = 'e';
    cwcurr[3] = '\0';

    p = scandata.s;
    SkipSpaces( p );                    // next word start
    pa = p;
    SkipNonSpaces( p );                 // end of word
    len = p - pa;
    if( len == 0 ) {                    // omitted means ON
        init_pe_line( INT_MAX );
    } else {
        gn.arg.s = pa;
        gn.arg.e = scandata.e;
        gn.ignore_blanks = false;
        cc = getnum( &gn );             // try to get numeric value
        if( cc == CC_notnum ) {
            switch( len ) {
            case 2 :
                if( strnicmp( "ON", pa, 2 ) == 0 ) {
                    init_pe_line( INT_MAX );    // partial implementation (no nesting)
                } else {
                    xx_line_err_exit_cc( ERR_XX_OPT, cwcurr, pa );
                    /* never return */
                }
                break;
            case 3 :
                if( strnicmp( "OFF", pa, 3 ) == 0 ) {
                    reset_pe_cb();              // partial implementation (no nesting)
                } else {
                    xx_line_err_exit_cc( ERR_XX_OPT, cwcurr, pa );
                    /* never return */
                }
                break;
            case 6 :
                if( strnicmp( "DELETE", pa, 6 ) == 0 ) {
                    reset_pe_cb();              // partial implementation (no nesting)
                } else {
                    xx_line_err_exit_cc( ERR_XX_OPT, cwcurr, pa );
                    /* never return */
                }
                break;
            default:
                xx_line_err_exit_cc( ERR_XX_OPT, cwcurr, pa );
                /* never return */
            }
        } else {
            scandata.s = gn.arg.s;
            if( gn.result < 0 ) {
                xx_line_err_exit_c( ERR_VAL_NEG, pa );
                /* never return */
            }
            init_pe_line( gn.result );
        }
    }
    scan_restart = scandata.e;
    return;
}

