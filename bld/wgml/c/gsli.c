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
* Description: implement .li (literal)  script control word
*
*  comments are from script-tso.txt
****************************************************************************/

#include "wgml.h"

#include "clibext.h"

/***************************************************************************/
/* LITERAL causes  following input  records to be  treated as  text lines  */
/* even if they begin with the control word indicator.                     */
/*                                                                         */
/*      旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커       */
/*      |       |                                                  |       */
/*      |  .LI  |    <1|n|ON|OFF|line>                             |       */
/*      |       |                                                  |       */
/*      읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸       */
/*                                                                         */
/* This control word does not cause a break.                               */
/*                                                                         */
/* line:  causes the specified "line" to be  treated as text,  even if it  */
/*    starts with the control word indicator.                              */
/* <ON|OFF>:  If ON is specified, then all subsequent input lines will be  */
/*    treated as  text until  a physical input  line is  encountered that  */
/*    contains ".LI OFF" starting in column one.                           */
/* <1|n>:  causes the next "n" input lines to be treated as text.   If no  */
/*    operand at  all is  specified,  the  following input  line will  be  */
/*    treated as literal text.                                             */
/*                                                                         */
/* NOTES                                                                   */
/* (1) An input record may begin with two occurrences of the control word  */
/*     indicator character.   This indicates that  the control word which  */
/*     follows always a native control word and never a macro name.        */
/* (2) An input record that begins with  the control word indicator char-  */
/*     acter followed immediately by a backspace character will always be  */
/*     treated as text.                                                    */
/* (3) If the control  word indicator is followed by a  single quote ".'"  */
/*     then any  control word separator characters,   normally semi-colon  */
/*     ";", will be treated as any other character in the record.          */
/* (4) The control word  indicator is normally the  period "." character.  */
/*     It may be altered with the Define Character (.DC LI) control word.  */
/***************************************************************************/


void    scr_li( void )
{
    char        *   pa;
    char        *   p;
    int             len;
    getnum_block    gn;
    condcode        cc;
    char            cwcurr[4];

    cwcurr[0] = SCR_char;
    cwcurr[1] = 'l';
    cwcurr[2] = 'i';
    cwcurr[3] = '\0';

    p = scan_start;
    while( *p && *p == ' ' ) {          // next word start
        p++;
    }
    pa = p;
    while( *p && *p != ' ' ) {          // end of word
        p++;
    }
    len = p - pa;
    if( len == 0 ) {                    // omitted means 1 = next line
        if( !ProcFlags.literal ) {
            li_cnt = 1;
            ProcFlags.literal = true;
            scan_restart = pa;
        }
    } else {
        gn.argstart = pa;
        gn.argstop = scan_stop;
        gn.ignore_blanks = 0;

        cc = getnum( &gn );            // try to get numeric value
        if( cc == notnum ) {
            switch( len ) {
            case 2 :
                if( !strnicmp( "ON", pa, 2 ) ) {
                    if( !ProcFlags.literal ) {
                        li_cnt = LONG_MAX;
                        ProcFlags.literal = true;
                        scan_restart = pa + 2;
                    }
                } else {
                    if( !ProcFlags.literal ) {
                        li_cnt = 1;
                        ProcFlags.literal = true;
                        split_input( scan_start, pa, false );
                        scan_restart = pa;
                    }
                }
                break;
            case 3 :
                if( !strnicmp( "OFF", pa, 3 ) ) {
                    ProcFlags.literal = false;
                    scan_restart = pa + 3;
                } else {
                    if( !ProcFlags.literal ) {
                        li_cnt = 1;
                        ProcFlags.literal = true;
                        split_input( scan_start, pa, false );
                        scan_restart = pa;
                    }
                }
                break;
            default:
                if( !ProcFlags.literal ) {
                    li_cnt = 1;
                    ProcFlags.literal = true;
                    split_input( scan_start, pa, false );
                    scan_restart = pa;
                }
                break;
            }
        } else {                        // .li 1234
            if( !ProcFlags.literal ) {
                if( gn.result > 0 ) {
                    li_cnt = gn.result;
                    ProcFlags.literal = true;
                    scan_restart = gn.argstart;
                }
            } else {
                scan_restart = pa;         // .li already active, treat as text
            }
        }
    }
    return;
}

