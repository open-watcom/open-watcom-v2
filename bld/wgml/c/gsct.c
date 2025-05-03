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
* Description: implement .ct (continued text) script control word
*
*
*  comments are from script-tso.txt
****************************************************************************/


#include "wgml.h"


/**************************************************************************/
/* CONTINUED TEXT causes the first character  of the next text input line */
/* to be placed onto the output line immediately following the last char- */
/* acter already on the output line, with no intervening blank.           */
/*                                                                        */
/*      旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커      */
/*      |       |                                                  |      */
/*      |  .CT  |    <line>                                        |      */
/*      |       |                                                  |      */
/*      읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸      */
/*                                                                        */
/* When concatenation is in effect,  each new input text record is joined */
/* to previous  text with an intervening  blank.   Text that occurs  in a */
/* "line" operand with  this control word will be joined  to any previous */
/* text with no intervening blank.   If  no previous text exists then the */
/* "line" operand  will be processed  as normal text.    If concatenation */
/* (.CO)  is not in effect then there  will never be any previous text to */
/* be immediately  continued.   This  can be  contrasted with  the Define */
/* Character "continue" character (.DC CONT)  that  is used to continue a */
/* text line with text that follows.                                      */
/*    This control word does not cause a break.  If it is used without an */
/* operand  line,  the  continuation  of text  is  disabled  even if  the */
/* previous text was to be continued.                                     */
/*                                                                        */
/* EXAMPLES                                                               */
/* (1) This is an example of con                                          */
/*     .ct tinued text.                                                   */
/*     produces:  This is an example of continued text.                   */
/*                                                                        */
/*  Extension: if text starts with . or : process as control line         */
/*                                                                        */
/*  Note: ProcFlags.fsp is used to control whether or not post_space      */
/*        is allowed or is reduced to zero. Not reducing it to zero       */
/*        matches wgml 4.0's behavior, but not the above description.     */
/*                                                                        */
/**************************************************************************/

void    scr_ct( void )
{
    char        *   p;

    p = scan_start;                     // next char after .ct
    if( is_space_tab_char( *p ) && (*p != '\0') && is_space_tab_char( *(p + 1) ) ) {
        ProcFlags.fsp = true;           // keep post_space
        if( post_space == 0 ) {         // ensure it has a non-zero value
            post_space = wgml_fonts[g_curr_font].spc_width;
        }
    }
    if( *p != '\0' ) {                  // line operand specified
        if( ProcFlags.concat ) {
            SkipSpaces( p );
        } else {
            p++;                        // over blank after control word
        }
        if( *p != '\0' ) {
            if( !ProcFlags.fsp ) {                  // not forced space
                if( is_ip_tag( nest_cb->c_tag ) ) { // in inline phrase
                    if( is_ip_tag( nest_cb->prev->c_tag ) ) { // in 2nd inline phrase
                        /* placeholder */
                    } else if( !ProcFlags.inl_text ) {
                        /* placeholder */
                    } else {
                        post_space = 0;
                    }
                } else {
                    post_space = 0;
                }
            }
            ProcFlags.ct = true;
            if( (*p == SCR_char) ||     // script control word follows
                (*p == GML_char) ) {    // GML tag follows
                split_input( scan_start, p, input_cbs->fmflags );   // fixes problem with macro
                input_cbs->hidden_head->fm_symbol = input_cbs->fm_symbol;
                input_cbs->hidden_head->sym_space = input_cbs->sym_space;
                input_cbs->hidden_head->hh_tag = input_cbs->hh_tag;
            } else {
                process_text( p, g_curr_font ); // text follows
            }
        }
    }
    scan_restart = scan_stop + 1;
    return;
}

