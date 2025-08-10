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
* Description:  implements control word PP
*
****************************************************************************/


#include "wgml.h"


/**************************************************************************/
/* PARAGRAPH defines the start of a new paragraph.                        */
/*                                                                        */
/*      +-------+--------------------------------------------------+      */
/*      |       |                                                  |      */
/*      |  .PP  | <line>                                           |      */
/*      |       |                                                  |      */
/*      +-------+--------------------------------------------------+      */
/*                                                                        */
/* This control word causes a break, and then internally executes a .SK,  */
/* a .CC 2, and an .IL +3. The "line" operand starts one blank after the  */
/* control word. If the operand is missing, the text for the start of     */
/* the paragraph comes from the next input text line.                     */
/*                                                                        */
/**************************************************************************/

/**************************************************************************/
/* Implements control word PP                                             */
/**************************************************************************/

void scr_pp( void )
{
    char    *   p;

    p = g_scandata.s;

    /* BR is done before scr_pp() is called */

    /* SK */
    g_skip = bin_device->vertical_base_units / LPI; // same computation as used by SK 1
    set_skip_vars( NULL, NULL, NULL, g_text_spacing, g_curr_font );

    /* CC 2 */
    if( (wgml_fonts[g_curr_font].line_height + t_page.cur_depth) > t_page.max_depth ) {
        next_column();
    }

    /* IL +3 */
    g_line_indent = 3 * tab_col;
    t_page.cur_width = t_page.cur_left + g_line_indent;

    post_space = 0;
    if( *p != '\0' ) {
        process_text( p, g_curr_font );
    }

    scan_restart = g_scandata.e;
    return;
}

