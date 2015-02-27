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
/*                                                                         */
/*  Extension: if text starts with . or : process as control line          */
/*                                                                         */
/*                                                                         */
/**************************************************************************/

void    scr_ct( void )
{
    char        *   p;

    p = scan_start;                     // next char after .ct
    if( *p ) {                          // line operand specified
        p++;                            // over space
        if( *p ) {
            if( !ProcFlags.fsp ) {      // preserved forced space
                post_space = 0;
            }
            ProcFlags.ct = true;
            if( (*p == SCR_char) ||     // script control word follows
                (*p == GML_char) ) {    // GML tag follows
                split_input( scan_start, p, false ); // TBD, fixes problem with macro
            } else {
                process_text( p, g_curr_font ); // text follows
            }
        }
    }
    scan_restart = scan_stop;
    return;
}

