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
* Description:  WGML tags :H0 :H1 :H2 :H3 :H4 :H5 :H6 processing
*
*                  stitle= not implemented, not used in OW documentation
*
*                  incomplete heading output TBD
****************************************************************************/
#include    "wgml.h"
#include    "gvars.h"

static char hnumx[7] = "$hnumX";


/***************************************************************************/
/*  construct Header numbers  1.2.3.V ...                                  */
/*  and update $headn global variable                                      */
/***************************************************************************/

static  void    update_headnumx( int lvl, char *hnumstr, size_t hnsize )
{
    size_t          pos;
    char            *pn;
    int             rc;

    *hnumstr = 0;
    pos = 0;
    if( layout_work.hx[lvl].number_form == none ) {
        return;                         // no number output
    }

    if( lvl > 0 ) {               // reuse formatted number from previous lvl
       if( (layout_work.hx[lvl].number_form == num_prop) &&
           (layout_work.hx[lvl - 1].headnsub != NULL) ) {

           strcpy( hnumstr, layout_work.hx[lvl - 1].headnsub->value );
           pos = strlen( hnumstr );
       }
    }
    if( pos > 0 ) {             // we have a formatted number from parent lvl
       *(hnumstr + pos) = layout_work.heading.delim;
       pos++;
       *(hnumstr + pos) = 0;
    }
    pn = format_num( layout_work.hx[lvl].headn, hnumstr + pos, hnsize - pos,
                     layout_work.hx[lvl].number_style );
    if( pn != NULL ) {
         pos += strlen( pn );           // all ok
    } else {
         pn = hnumstr + pos;
         *pn = '?';                     // create dummy number
         *(pn + 1) = 0;
         pos++;
    }

    if( layout_work.hx[lvl].headnsub == NULL ) {// first time here
        rc = add_symvar_addr( &global_dict, hnumx, hnumstr, no_subscript, 0,
                              &layout_work.hx[lvl].headnsub );
    } else {
        if( strlen( layout_work.hx[lvl].headnsub->value ) < strlen( hnumstr ) ) {     // need more room
            layout_work.hx[lvl].headnsub->value =
                mem_realloc( layout_work.hx[lvl].headnsub->value, strlen( hnumstr ) + 1 );
        }
        strcpy( layout_work.hx[lvl].headnsub->value, hnumstr );
    }
}


/***************************************************************************/
/*  output hx Header  only  called if display_heading = yes                */
/***************************************************************************/

static  void    hx_header( int hx_lvl, const char *hnumstr, const char *txt )
{
    doc_element     *   cur_el;
    font_number         font_save;
    font_number         font;
    int32_t             width;
    int32_t             widthn;
    text_chars      *   curr_t;
    text_chars      *   curr_tn;
    text_line       *   hd_line;

    hd_line = NULL;
    font_save = g_curr_font;
    spacing = layout_work.hx[hx_lvl].spacing;

    if( layout_work.hx[hx_lvl].line_break ) {
        set_skip_vars( &layout_work.hx[hx_lvl].pre_skip,
                       &layout_work.hx[hx_lvl].pre_top_skip,
                       &layout_work.hx[hx_lvl].post_skip,
                       spacing,
                       layout_work.hx[hx_lvl].number_font );
    } else {
        set_skip_vars( &layout_work.hx[hx_lvl].pre_skip,
                       &layout_work.hx[hx_lvl].pre_top_skip,
                       NULL,
                       spacing,
                       layout_work.hx[hx_lvl].number_font );
    }

    post_space = 0;

    curr_t  = NULL;
    curr_tn = NULL;
    width   = 0;
    widthn  = 0;
    hd_line = alloc_text_line();

    if( layout_work.hx[hx_lvl].number_form != none ) {
        font = layout_work.hx[hx_lvl].number_font;
        curr_tn = alloc_text_chars( hnumstr, strlen( hnumstr ), font );
        curr_tn->width = cop_text_width( curr_tn->text, curr_tn->count, font );
        widthn = curr_tn->width + wgml_fonts[font].spc_width;;

        hd_line->first = curr_tn;
        hd_line->line_height = wgml_fonts[font].line_height;
        hd_line->last = curr_tn;
    }

    if( (txt != NULL) && (*txt != '\0') ) {
        font = layout_work.hx[hx_lvl].font;

        curr_t = alloc_text_chars( txt, strlen( txt ), font );
        curr_t->width = cop_text_width( curr_t->text, curr_t->count, font );
        width = curr_t->width;

        if( hd_line->first == NULL ) {
            hd_line->first = curr_t;
        } else {
            curr_tn->next = curr_t;
            curr_t->prev  = curr_tn;
        }
        hd_line->last = curr_t;
        if( hd_line->line_height < wgml_fonts[font].line_height ) {
            hd_line->line_height = wgml_fonts[font].line_height;
        }
    }
    if( curr_t == NULL ) {
        curr_t = curr_tn;
        curr_tn = NULL;
        width = widthn;
        widthn = 0;
    }
    g_cur_left = g_page_left + conv_hor_unit( &layout_work.hx[hx_lvl].indent )
                             + conv_hor_unit( &layout_work.hx[hx_lvl].align );

    if( layout_work.hx[hx_lvl].page_position == pos_left ) {
        if( curr_tn != NULL ) {
            curr_tn->x_address = g_cur_left;
        }
        curr_t->x_address = g_cur_left + widthn;
    } else {
        if( layout_work.hx[hx_lvl].page_position == pos_center ) {
            curr_t->x_address = g_cur_left + widthn
                             + (g_page_right - g_cur_left - widthn - width) / 2;
        } else {
            curr_t->x_address = g_page_right - width;
        }
        if( curr_tn != NULL ) {
            curr_tn->x_address = curr_t->x_address - widthn;
        }
    }
    if( input_cbs->fmflags & II_research ) {
        test_out_t_line( hd_line );
    }
    cur_el = alloc_doc_el( el_text );
    cur_el->blank_lines = g_blank_lines;
    g_blank_lines = 0;
    cur_el->depth = hd_line->line_height;
    cur_el->subs_skip = g_subs_skip;
    cur_el->top_skip = g_top_skip;
    cur_el->element.text.overprint = ProcFlags.overprint;
    ProcFlags.overprint = false;
    cur_el->element.text.spacing = g_spacing;
    cur_el->element.text.first = hd_line;
    ProcFlags.skips_valid = false;
    hd_line = NULL;

    if( layout_work.hx[hx_lvl].page_eject == ej_no ) {
        insert_col_main( cur_el );
    } else {
        insert_page_width( cur_el );
    }
}

/***************************************************************************/
/*  :H0 - :H6  common processing                                           */
/***************************************************************************/

static  void    gml_hx_common( gml_tag tag, int hx_lvl )
{
    char    *   p;
    char    *   headp;
    bool        idseen;
    bool        stitleseen;
    int         rc;
    int         k;
    size_t      headlen;
    size_t      txtlen;
    char        hnumstr[64];
    ref_entry   *   re;
    ref_entry   *   rwk;
    static char hxstr[4] = ":HX";
    static char htextx[8] = "$htextX";
    static char headx[7]  = "$headX";

    tag = tag;

    *(hxstr + 2) = '0' + hx_lvl;
    htextx[6] = '0' + hx_lvl;
    hnumx[5] = '0' + hx_lvl;
    headx[5] = '0' + hx_lvl;

    switch( hx_lvl ) {
    case   0:
        if( !((ProcFlags.doc_sect == doc_sect_body) ||
            (ProcFlags.doc_sect_nxt == doc_sect_body)) ) {

            g_err( err_tag_wrong_sect, hxstr, ":BODY section" );
            err_count++;
            file_mac_info();
        }
        break;
    case  1:
        if( !((ProcFlags.doc_sect >= doc_sect_body) ||
            (ProcFlags.doc_sect_nxt >= doc_sect_body)) ) {

            g_err( err_tag_wrong_sect, hxstr, ":BODY :APPENDIX :BACKM sections" );
            err_count++;
            file_mac_info();
        }
        break;
    default:
        if( !((ProcFlags.doc_sect >= doc_sect_abstract) ||
            (ProcFlags.doc_sect_nxt >= doc_sect_abstract)) ) {

            g_err( err_tag_wrong_sect, hxstr, ":ABSTRACT section or later" );
            err_count++;
            file_mac_info();
        }
        break;
    }
    if( layout_work.hx[hx_lvl].number_form != num_none ) {
        layout_work.hx[hx_lvl].headn++;
    }

    idseen = false;
    stitleseen = false;
    p = scan_start;
    re = NULL;

    /***********************************************************************/
    /*  Scan attributes  for :Hx                                           */
    /*  id=                                                                */
    /*  stitle=                                                            */
    /***********************************************************************/

    for( ;; ) {
        while( *p == ' ' ) {
            p++;
        }
        if( *p == '\0' || *p == '.'  ) {
            break;                      // tag end found
        }
        if( !strnicmp( "stitle=", p, 7 ) ) {
            p += 6;
            stitleseen = true;

            /***************************************************************/
            /*  Although unsupported scan stitle='xxx'                     */
            /***************************************************************/
            g_warn( wng_unsupp_att, "stitle" );
            wng_count++;
            file_mac_info();

            p = get_att_value( p );

            scan_start = p;
            if( !ProcFlags.tag_end_found ) {
                continue;
            }
            break;
        }

        /*******************************************************************/
        /*  ID='xxxxxxxx'                                                  */
        /*******************************************************************/

        if( !strnicmp( "id=", p, 3 ) ) {
            p += 2;

            p = get_refid_value( p );

            if( val_len > 0 ) {
                idseen = true;          // valid id attribute found
                *(val_start + val_len) = '\0';

                if( re == NULL ) {      // prepare reference entry
                    re = mem_alloc( sizeof( ref_entry ) );
                    init_ref_entry( re, val_start, val_len );
                } else {
                    fill_id( re, val_start, val_len );
                }
            }
            scan_start = p;
            if( !ProcFlags.tag_end_found ) {
                continue;
            }
            break;
        }

        /*******************************************************************/
        /* no more valid attributes, process remaining input as header text*/
        /*******************************************************************/
        break;
    }
    if( *p == '.' ) {                   // tag end ?
        p++;
    }

    /************************************************************************/
    /*  set the global vars $headx, $headnumx, $htextx                      */
    /*    perhaps text translated to upper or lower case                    */
    /************************************************************************/
    while( *p == ' ' ) {                // ignore leading blanks
        p++;
    }
    if( *p ) {                          // text exists
        if( layout_work.hx[hx_lvl].cases == case_lower ) {
            strlwr( p );
        } else if( layout_work.hx[hx_lvl].cases == case_upper ) {
            strupr( p );
        }
    }
    rc = add_symvar( &global_dict, htextx, p, no_subscript, 0 );

    update_headnumx( hx_lvl, hnumstr, sizeof( hnumstr ) );

    txtlen = strlen( p );
    headlen = strlen( hnumstr) + txtlen + 2;
    headp = mem_alloc( headlen );
    if( layout_work.hx[hx_lvl].number_form != num_none ) {
        strcpy( headp, hnumstr); // numbered header
        strcat( headp, " " );
    } else {
        *headp = '\0';
    }
    strcat( headp, p );
    rc = add_symvar( &global_dict, headx, headp, no_subscript, 0 );

    out_msg( " %s\n", headp );          // always verbose output ? TBD

    mem_free( headp );

    /***********************************************************************/
    /*  if id  specified add it to reference dict                          */
    /***********************************************************************/
    if( idseen ) {
        rwk = find_refid( ref_dict, re->id );
        if( !rwk ) {                    // new entry
            if( txtlen > 0 ) {          // text line not empty
                re->u.info.text_cap = mem_alloc( txtlen + 1 );
                strcpy( re->u.info.text_cap, p );
            }
            add_ref_entry( &ref_dict, re );
            re = NULL;                  // free will be done via dictionary
        } else {
            /***************************************************************/
            /*  test for duplicate id                                      */
            /*  it is done with comparing line no only, in the hope that   */
            /*  two identical ids are not specified in different files on  */
            /*  the same line no.                                          */
            /***************************************************************/
            if( re->lineno != rwk->lineno ) {
                g_err( wng_id_xxx, re->id );
                g_info( inf_id_duplicate );
                file_mac_info();
                err_count++;
            }
            if( re->u.info.text_cap != NULL ) {
                mem_free( re->u.info.text_cap );
            }
            mem_free( re );
        }
    }

    if( layout_work.hx[hx_lvl].number_reset ) {
        for( k = hx_lvl + 1; k < 7; k++ ) {
            layout_work.hx[k].headn = 0;// reset following levels
            if( layout_work.hx[k].headnsub != NULL ) {
                *(layout_work.hx[k].headnsub->value) = '\0';
            }
        }
    }

    /***********************************************************************/
    /*  creation of actual heading                                         */
    /***********************************************************************/

    /***********************************************************************/
    /*  eject page(s) if specified                                         */
    /***********************************************************************/
    if( layout_work.hx[hx_lvl].page_eject != ej_no ) {

        if( ProcFlags.page_started ) {
            do_page_out();
            reset_t_page();
        }

        if( !ProcFlags.start_section ) {
            start_doc_sect();
        }
        set_headx_banners( hx_lvl );        // set possible banners
        reset_t_page();                     // and adjust page margins


        if( (layout_work.hx[hx_lvl].page_eject == ej_odd) && (page & 1) ) {
            do_page_out();              // next page would be even
            reset_t_page();
        } else if( (layout_work.hx[hx_lvl].page_eject == ej_even) && !(page & 1) ) {
            do_page_out();              // next page would be odd
            reset_t_page();
        }
    }

    if( layout_work.hx[hx_lvl].display_heading ) {

        hx_header( hx_lvl, hnumstr, p );
    }

    scan_start = scan_stop;
    return;
}

/******************************************************************************/
/*H0, H1, H2, H3, H4, H5, H6                                                  */
/*                                                                            */
/*Format: :Hn [id='id-name']                                                  */
/*            [stitle='character string'].<text line>                         */
/*        (n=0,1)                                                             */
/*                                                                            */
/*Format: :Hn [id='id-name'].<text line>                                      */
/*        (n=0,1,2,3,4,5,6)                                                   */
/*                                                                            */
/*These tags are used to create headings for sections and subsections of text.*/
/*A common convention uses the headings as follows:                           */
/*                                                                            */
/*    :H0 Major part of document.                                             */
/*    :H1 Chapter.                                                            */
/*    :H2 Section.                                                            */
/*    :H3, :H4, :H5, :H6 Subsections.                                         */
/*                                                                            */
/*The specific layout with which a document is formatted will determine the   */
/*format of the headings. Some layouts cause the headings to be automatically */
/*numbered according to a chosen convention. The heading text specified with  */
/*the tag may also be used in the creation of top and/or bottom page banners. */
/*                                                                            */
/*A heading may be used where a basic document element is permitted to appear,*/
/*with the following restrictions:                                            */
/*                                                                            */
/*    1. :h0 tags may only be used in the body of a document.                 */
/*    2. :h1 tags may not be used in the preface or the abstract.             */
/*                                                                            */
/*The stitle attribute allows you to specify a short title for the heading.   */
/*                                                                            */
/*The short title will be used instead of the heading text when creating the  */
/*top and/or bottom page banners. The short title attribute is valid with a   */
/*level one or level zero heading.                                            */
/*                                                                            */
/*The id attribute assigns an identifier name to the heading. The identifier  */
/*name is used when processing a heading reference, and must be unique within */
/*the document.                                                               */
/******************************************************************************/


void    gml_h0( gml_tag tag )
{
    gml_hx_common( tag, 0 );
}

void    gml_h1( gml_tag tag )
{
    gml_hx_common( tag, 1 );
}

void    gml_h2( gml_tag tag )
{
    gml_hx_common( tag, 2 );
}

void    gml_h3( gml_tag tag )
{
    gml_hx_common( tag, 3 );
}

void    gml_h4( gml_tag tag )
{
    gml_hx_common( tag, 4 );
}

void    gml_h5( gml_tag tag )
{
    gml_hx_common( tag, 5 );
}

void    gml_h6( gml_tag tag )
{
    gml_hx_common( tag, 6 );
}

