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
* Description:  WGML tags :HP0 :HP1, :HP2, :HP3, :eHPx
*                         :SF  and :eSF processing
*
****************************************************************************/

#include "wgml.h"

#include "clibext.h"

/***************************************************************************/
/*  :HPx :SF common processing                                             */
/***************************************************************************/

static void gml_hp_sf_common( gml_tag gtag, int level )
{
    char    *   p;

    gtag = gtag;
// keep any existing post_space, even if CT follows -- TBD
    if( (input_cbs->fmflags & II_sol) ) {
        ProcFlags.fsp = true;
        if( post_space == 0 ) {
            post_space = wgml_fonts[g_curr_font].spc_width; // TBD
        }
    }

    init_nest_cb( true );

    if( level >= wgml_font_cnt ) {      // invalid font use default
        level = 0;
    }
    nest_cb->font = level;
    g_curr_font = level;

    nest_cb->gtag = gtag;

    scan_err = false;
    p = scan_start;
    if( *p == '.' ) p++;                // over '.'
    if( *p ) {
        process_text( p, g_curr_font );
    }
    if( !ProcFlags.concat && (input_cbs->fmflags & II_eol) ) {
        scr_process_break();            // ensure line is output
    }
    scan_start = scan_stop;
    return;
}


/***************************************************************************/
/*  :HP0  :HP1  :HP2  :HP3                                                 */
/*                                                                         */
/* Format: :HPn. (n=0,1,2,3)                                               */
/* These tags start the highlighting of phrases at one of the four levels  */
/* provided by GML. The actual highlighting to be performed is determined  */
/* by the type of device for which the document is being formatted.        */
/* Examples of highlighting include underlining, displaying in bold face,  */
/* or using a different character shape (such as italics).                 */
/* Highlighting may not be used when the GML layout explicitly determines  */
/* the emphasis to be used, such as in the text of a heading.              */
/* The highlighting tags are paragraph elements. They are used with text   */
/* to create the content of a basic document element, such as a paragraph. */
/* A corresponding :EHPn tag must be specified for each :HPn tag.          */
/***************************************************************************/

void    gml_hp0( gml_tag gtag )
{
    gml_hp_sf_common( gtag, 0 );
}

void    gml_hp1( gml_tag gtag )
{
    gml_hp_sf_common( gtag, 1 );
}

void    gml_hp2( gml_tag gtag )
{
    gml_hp_sf_common( gtag, 2 );
}

void    gml_hp3( gml_tag gtag )
{
    gml_hp_sf_common( gtag, 3 );
}


/***************************************************************************/
/*  :eHPx :eSF common processing                                           */
/***************************************************************************/

static  void    gml_ehp_esf_common( gml_tag egtag )
{
    char    *   p;
    tag_cb  *   wk;

    if( !g_err_gml_etag( egtag ) ) {
        wk = nest_cb;
        nest_cb = nest_cb->prev;
        add_tag_cb_to_pool( wk );
        g_curr_font = nest_cb->font;

        // recompute space at SOL if inline end tag - TBD
        if( (post_space !=0) && (input_cbs->fmflags & II_sol) ) {
            post_space = wgml_fonts[g_curr_font].spc_width;
        }

        scan_err = false;
        p = scan_start;
        if( *p == '.' ) p++;            // over '.'
        if( *p ) {
            process_text( p, g_curr_font );
        }
        if( !ProcFlags.concat && (input_cbs->fmflags & II_eol) ) {
            scr_process_break();        // ensure line is output
        }
    }
    scan_start = scan_stop;
}


/***************************************************************************/
/*                                                                         */
/* EHP0, EHP1, EHP2, EHP3                                                  */
/*                                                                         */
/* Format: :eHPn. (n=0,1,2,3)                                              */
/* These tags end the highlighting of phrases at one of the four levels    */
/* provided by GML.                                                        */
/* Each :ehpn tag must be preceded by a corresponding :hpn tag.            */
/***************************************************************************/

void    gml_ehp0( gml_tag gtag )
{
    gml_ehp_esf_common( gtag );
}

void    gml_ehp1( gml_tag gtag )
{
    gml_ehp_esf_common( gtag );
}

void    gml_ehp2( gml_tag gtag )
{
    gml_ehp_esf_common( gtag );
}

void    gml_ehp3( gml_tag gtag )
{
    gml_ehp_esf_common( gtag );
}

/***************************************************************************/
/*  :esf tag processing                                                    */
/*                                                                         */
/*   Format: :eSF.                                                         */
/*                                                                         */
/*   This tag ends the highlighting of phrases started by the last :sf tag */
/***************************************************************************/

void    gml_esf( gml_tag gtag )
{
    gml_ehp_esf_common( gtag );
}


/***************************************************************************/
/*  :SF  processing                                                        */
/*                                                                         */
/*  Format: :SF font=number.                                               */
/*  The set font tag starts the highlighting of phrases at the level       */
/*  specified by the required attribute font. The actual highlighting      */
/*  to be performed is determined by the type of device for which the      */
/*  document is being formatted. Examples of highlighting include          */
/*  underlining, displaying in bold face, or using a different             */
/*  character shape (such as italics).                                     */
/*  The value of the font attribute is a non-negative integer number.      */
/*  If the specified number is larger than the last defined font for the   */
/*  document, font for zero is used. Highlighting may not be used when     */
/*  the GML layout explicitly determines the emphasis to be used, such     */
/*  as in the text of a heading. The set font tag is a paragraph element.  */
/*  It is used with text to create the content of a basic document element,*/
/*  such as a paragraph. A corresponding :ESF tag must be specified for    */
/*  each :SF tag.                                                          */
/***************************************************************************/

void    gml_sf( gml_tag gtag )
{
    char    *   p;
    char    *   pe;
    long        font;

    p = scan_start;
    p++;
    while( *p == ' ' ) {
        p++;
    }
    if( !strnicmp( "font=", p, 5 ) ) {
        p += 5;
        font = strtol( p, &pe, 10 );
        scan_start = pe;
        if( (font < 0) || (font >= wgml_font_cnt) ) {// invalid font use default
            font = 0;
        }
        gml_hp_sf_common( gtag, font );
    } else {
        err_count++;
        // AT-001 Required attribute not found
        g_err( err_att_missing );
        file_mac_info();
    }
    scan_start = scan_stop;
    return;
}
