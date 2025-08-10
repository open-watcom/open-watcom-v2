/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  WGML implementation of .bd, .bi, and .us script control
*                    words
*                   interactions are only partially understood/implemented
*
****************************************************************************/


#include "wgml.h"

#include "clibext.h"


/**************************************************************************/
/* BOLD boldfaces the specified number of input text lines.               */
/*                                                                        */
/*      +-------+--------------------------------------------------+      */
/*      |       |                                                  |      */
/*      |  .BD  |    <1|n|ON|OFF|line>                             |      */
/*      |       |                                                  |      */
/*      +-------+--------------------------------------------------+      */
/*                                                                        */
/*                                                                        */
/* n:   The next "n" input text lines are to be made Bold by overstriking */
/*      each character with itself.  If "n" is omitted,  a value  of 1 is */
/*      assumed.                                                          */
/* ON:  All following input text lines are to be made Bold.               */
/* OFF: Terminates  the boldface  effect,  whether  initiated by  "n" or  */
/*      "ON".                                                             */
/* line: The  boldface effect will be  applied to the  resulting "line".  */
/*    If "line"  starts with a control  word indicator,  the  Bold action */
/*    will start one blank after the control word at the start of "line", */
/*    and the result of overstriking must  fit within the maximum allowed */
/*    input line  length.   This restriction does  not apply to  a "line" */
/*    operand of text or text following a numeric or "ON" operand.        */
/*                                                                        */
/* This control word does not cause a break.  BOLD operates independently */
/* of other control words that modify text.    When more than one of .BD, */
/* .BI,  .UC,  .UL,  .UP,  or .US are  in effect,  the result is the best */
/* equivalent of the  sum of the effects.    They may be disabled  in any */
/* order;  the result  will be the best  equivalent of the ones  still in */
/* effect.   The .UD "INCLUDE/IGNORE/SET" characters  may be used to turn */
/* the function on and  off within a "line" or in  subsequent input text. */
/* See .UD for defaults and details.                                      */
/*                                                                        */
/* NOTES                                                                  */
/* (1) For output devices  that support multiple fonts,   the output from */
/*     this  control word  is  not necessarily  bold  but  may merely  be */
/*     selected from a different font.                                    */
/* (2) Text may also be emphasized with the &'bold( function.             */
/* (3) Do not  attempt to use a  numeric expression as a  "line" operand, */
/*     because  it  will be  taken  as  an  "n" operand.    For  example, */
/*     ".BD (1988)" will boldface  the next 1988 input lines  of text and */
/*     not the string  "(1988)".   Use ".BD 1;(1988)" or  ".BD;(1988)" or */
/*     &'bold(1988) instead.                                              */
/*                                                                        */
/**************************************************************************/

/**************************************************************************/
/* BOLD ITALIC overstrikes an input line with itself and underscores it   */
/* for a boldface italic effect.                                          */
/*                                                                        */
/*      +-------+--------------------------------------------------+      */
/*      |       |                                                  |      */
/*      |  .BI  |    <1|n|ON|OFF|line>                             |      */
/*      |       |                                                  |      */
/*      +-------+--------------------------------------------------+      */
/*                                                                        */
/* This control word does not cause a break. The operands are identical   */
/* to those for the .BD control word.                                     */
/*                                                                        */
/* NOTES                                                                  */
/* (1) For output devices  that support multiple fonts,   the output from */
/*     this control word is not necessarily underscored.   It often will  */
/*     display in a bold italic font, for example.                        */
/* (2) Text may also be emphasized with the &'bitalic( function.          */
/*                                                                        */
/**************************************************************************/

/**************************************************************************/
/* UNDERSCORE underscores text in the specified number of input lines.    */
/*                                                                        */
/*      +-------+--------------------------------------------------+      */
/*      |       |                                                  |      */
/*      |  .US  |    <1|n|ON|OFF|line>                             |      */
/*      |       |                                                  |      */
/*      +-------+--------------------------------------------------+      */
/*                                                                        */
/* This control  word does not  cause a  break.   The operands  and other */
/* considerations are identical to those for the other control words that */
/* emphasize text.  See .BD for details.                                  */
/*                                                                        */
/* NOTES                                                                  */
/* (1) For output devices  that support multiple fonts,   the output from */
/*     this control word is not  necessarily underscored.   It often will */
/*     display in an italic font, for example.                            */
/* (2) Text may also be emphasized with the &'italic( function.           */
/*                                                                        */
/**************************************************************************/


/**************************************************************************/
/* The common function does all the work; the sc_xx functions specify     */
/* which control word is being processed                                  */
/* NOTE: the treatment of BI in wgml 4.0 is quite different from what     */
/*       might be expected from the way BI is documented; this attempts   */
/*       to follow a reasonable interpretation of the documentation       */
/*       by treating BI as a quick way to set or clear both BD and US     */
/**************************************************************************/

static void scr_style_common( style_cw_type type, style_cw_info *cw_info )
{
    char            *p;
    char            *pa;
    char            *pb;
    condcode        cc;
    getnum_block    gn;
    unsigned        len;

    if( g_script_style.style == SCT_none ) {  // save current font if no script style
        g_script_style.font = layout_work.defaults.font;
    }

    p = g_scandata.s;     // for use later
    pb = g_scandata.s;    // for use if outputting text
    cc = getarg();      // uses g_tok_start, g_scandata.s and g_scandata.e

    if( cc == CC_omit ) {                  // same as BD 1
        g_script_style.style |= type;
        if( type != SCT_bi ) {
            cw_info->count = 1;
            cw_info->scope = SCS_count;
        }
        pa = g_scandata.s;                // nothing following
    } else {                            // "pos", "quotes", "quote0"
        SkipSpaces( p );                // first token start
        pa = p;
        SkipNonSpaces( p );             // end of token
        len = p - pa;
        SkipSpaces( p );                // second token?
        if( *p == '\0' ) {              // no second token
            if( (len == 3) && (strnicmp( "OFF", pa, len ) == 0) ) { // OFF
                if( type == SCT_bi ) {
                    g_script_style.style &= ~SCT_bd;
                    g_script_style.cw_bd.count = 0;
                    g_script_style.cw_bd.scope = SCS_none;
                    g_script_style.style &= ~SCT_us;
                    g_script_style.cw_us.count = 0;
                    g_script_style.cw_us.scope = SCS_none;
                } else {
                    g_script_style.style &= ~type;
                    cw_info->count = 0;
                    cw_info->scope = SCS_none;
                }
                pa = p;                         // no following text
            } else if( (len == 2) && (strnicmp( "ON", pa, len ) == 0) ) {  // ON
                if( type == SCT_bi ) {
                    g_script_style.style |= SCT_bd;
                    g_script_style.cw_bd.count = 0;
                    g_script_style.cw_bd.scope = SCS_on;
                    g_script_style.style |= SCT_us;
                    g_script_style.cw_us.count = 0;
                    g_script_style.cw_us.scope = SCS_on;
                } else {
                    g_script_style.style |= type;
                    cw_info->count = 0;
                    cw_info->scope = SCS_on;
                }
                pa = p;
            } else {                            // no following text
                gn.arg.s = g_tok_start;
                gn.arg.e = g_scandata.e;
                gn.ignore_blanks = false;
                cc = getnum( &gn );             // try numeric expression evaluation
                if( (cc == CC_notnum) || (cc == CC_neg) ) {
                    if( ProcFlags.concat ) {
                        p = pa;                 // output as text
                    } else {
                        pb++;                   // over blank after control word
                        pa = pb;                // keep spaces at start of text when CO is OFF
                    }
                } else if( gn.result == 0 ) {
                    pa = p;                     // nothing happens at all
                } else {                        // number > 0
                    if( type == SCT_bi ) {
                        g_script_style.style |= SCT_bd;
                        g_script_style.cw_bd.count = gn.result;
                        g_script_style.cw_bd.scope = SCS_count;
                        g_script_style.style |= SCT_us;
                        g_script_style.cw_us.count = gn.result;
                        g_script_style.cw_us.scope = SCS_count;
                    } else {
                        g_script_style.style |= type;
                        cw_info->count = gn.result;
                        cw_info->scope = SCS_count;
                    }
                    pa = p;                     // no following text
                }
            }
        }
    }

    if( *pa != '\0' ) {
        if( type == SCT_bi ) {
            g_script_style.style |= SCT_bd;
            g_script_style.cw_bd.count = 0;
            g_script_style.cw_bd.scope = SCS_line;
            g_script_style.style |= SCT_us;
            g_script_style.cw_us.count = 0;
            g_script_style.cw_us.scope = SCS_line;
        } else {
            g_script_style.style |= type;
            cw_info->count = 0;
            cw_info->scope = SCS_line;
        }
        process_text( pa, g_curr_font );         // submit text on the same line
        if( type == SCT_bi ) {
            g_script_style.style &= ~SCT_bd;
            g_script_style.cw_bd.scope = SCS_none ;
            g_script_style.style &= ~SCT_us;
            g_script_style.cw_us.scope = SCS_none;
        } else {
            g_script_style.style &= ~type;
            cw_info->scope = SCS_none;
        }
        g_curr_font = g_script_style.font;
    }

    scan_restart = g_scandata.e;
    return;

}

void scr_bd( void )
{
    scr_style_common( SCT_bd, &g_script_style.cw_bd );
    return;
}

void scr_bi( void )
{
    scr_style_common( SCT_bi, NULL );   // BI is handled in a special manner
    return;
}

void scr_us( void )
{
    scr_style_common( SCT_us, &g_script_style.cw_us );
    return;
}

/************************************************************************/
/* scr_style_copy() copies the contents of the fields from parameter    */
/* first to those of parameter second                                   */
/* the inline phrase tags use this to interact properly with the        */
/* control words in this file                                           */
/************************************************************************/

void scr_style_copy( script_style_info * first, script_style_info * second )
{
    second->font = first->font;
    second->style = first->style;
    second->cw_bd.count = first->cw_bd.count;
    second->cw_bd.scope = first->cw_bd.scope;
    second->cw_us.count = first->cw_us.count;
    second->cw_us.scope = first->cw_us.scope;

    return;
}

/************************************************************************/
/* scr_style_end() terminates all processing controlled by the control  */
/* words defined in this file                                           */
/* for example, the next tag will cause this function to be called      */
/************************************************************************/

void scr_style_end( void )
{
    g_script_style.style = SCT_none;
    g_script_style.cw_bd.count = 0;
    g_script_style.cw_bd.scope = SCS_none;
    g_script_style.cw_us.count = 0;
    g_script_style.cw_us.scope = SCS_none;

    g_curr_font = g_script_style.font;

    return;
}

/************************************************************************/
/* scr_style_font() adjusts g_script_style for a new input line and     */
/*   returns the resulting font                                         */
/************************************************************************/

font_number scr_style_font( font_number in_font )
{
    font_number     font;

    if( !input_cbs->fm_hh ) {                           // only adjust for SOL
        if( g_script_style.cw_bd.scope == SCS_count ) {   // adjust/remove BD if appropriate
            if( g_script_style.cw_bd.count > 0 ) {
                g_script_style.cw_bd.count --;
            } else {
                g_script_style.cw_bd.scope = SCS_none;
                g_script_style.style &= ~SCT_bd;
            }
        }

        if( g_script_style.cw_us.scope == SCS_count ) {   // adjust/remove US if appropriate
            if( g_script_style.cw_us.count > 0 ) {
                g_script_style.cw_us.count --;
            } else {
                g_script_style.cw_us.scope = SCS_none;
                g_script_style.style &= ~SCT_us;
            }
        }
    }

    if( (g_script_style.style & SCT_bd) && (g_script_style.style & SCT_us) ) {
        font = FONT3;
    } else if( g_script_style.style & SCT_bd ) {
        font = FONT2;
    } else if( g_script_style.style & SCT_us ) {
        font = FONT1;
    } else {                                // no g_script_style
        font = in_font;                     // TBD
        g_curr_font = g_script_style.font;  // restore current font
    }

    return( font );
}
