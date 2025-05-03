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
* Description: WGML implement :BANREGION :eBANREGION  tags for LAYOUT processing
*
****************************************************************************/


#include "wgml.h"

#include "clibext.h"


extern  banner_lay_tag  *   curr_ban;   // in glbanner.c
extern  banner_lay_tag  *   del_ban;    // in glbanner.c

static  region_lay_tag      wk;         // temp for input values
static  region_lay_tag  *   prev_reg;


/***************************************************************************/
/*   :BANREGION attributes                                                 */
/***************************************************************************/
const   lay_att     banregion_att[12] =
    { e_indent, e_hoffset, e_width, e_voffset, e_depth, e_font, e_refnum,
      e_region_position, e_pouring, e_script_format, e_contents, e_dummy_zero };

static  const   int att_count = sizeof( banregion_att ) - 1;    // omit e_dummy_zero from count
static  bool        count[sizeof( banregion_att ) - 1];
static  int         sum_count;

/**************************************************************************************/
/*Define a banner region within a banner. Each banner region specifies a rectangular  */
/*section of the banner. A banner region begins with a :banregion tag and ends with an*/
/*:ebanregion tag. All banner regions are defined after the banner tag attributes and */
/*before the :ebanner tag.                                                            */
/*                                                                                    */
/*:BANREGION                                                                          */
/*        indent = 0                                                                  */
/*        hoffset = left                                                              */
/*        width = extend                                                              */
/*        voffset = 2                                                                 */
/*        depth = 1                                                                   */
/*        font = 0                                                                    */
/*        refnum = 1                                                                  */
/*        region_position = left                                                      */
/*        pouring = last                                                              */
/*        script_format = yes                                                         */
/*        contents = '/&$htext0.// &$pgnuma./'                                        */
/*                                                                                    */
/*indent The indent attribute accepts any valid horizontal space unit. The            */
/*specified space value is added to the value of the horizontal offset                */
/*attribute (hoffset) to determine the start of banner region in the                  */
/*banner if the horizontal offset is specified as left, centre, or center.            */
/*if the horizontal offset is specified as right, the indent value is                 */
/*subtracted from the right margin of the banner.                                     */
/*                                                                                    */
/*hoffset The hoffset attribute specifies the horizontal offset from the left side    */
/*of the banner where the banner region will start. The attribute value               */
/*may be any valid horizontal space unit, or one of the keywords left,                */
/*center, centre, or right. The keyword values remove the                             */
/*dependence upon the left and right adjustment settings of the banner                */
/*that occurs when using an absolute horizontal offset.                               */
/*                                                                                    */
/*width This attribute may be any valid horizontal space unit, or the                 */
/*keyword extend. If the width of the banner region is specified as                   */
/*Layout Tags 135?GML Reference                                                       */
/*extend, the width of the region will be increased until the start of                */
/*another banner region or the right margin of the banner is reached.                 */
/*                                                                                    */
/*voffset This attribute accepts any valid vertical space unit. It specifies the      */
/*vertical offset from the top of the banner for the start of the banner              */
/*region. A value of zero will be the first line of the banner, while the             */
/*value one will be the second line of the banner.                                    */
/*                                                                                    */
/*depth The depth attribute accepts a vertical space unit value. The attribute        */
/*value specifies the number of output lines or vertical space of the                 */
/*banner region.                                                                      */
/*                                                                                    */
/*font This attribute accepts a non-negative integer number. If a font                */
/*number is used for which no font has been defined, WATCOM                           */
/*Script/GML will use font zero. The font numbers from zero to three                  */
/*correspond directly to the highlighting levels specified by the                     */
/*highlighting phrase GML tags. The font attribute defines the font of                */
/*the banner region's contents.                                                       */
/*                                                                                    */
/*refnum This attribute accepts a positive integer number. Each banner region         */
/*must have a unique reference number. If this is the only attribute                  */
/*specified, the banner region is deleted from the banner.                            */
/*                                                                                    */
/*region_position This attribute specifies the position of the data within the        */
/*banner region. The attribute value may be one of the keywords left, center,         */
/*centre, or right.                                                                   */
/*                                                                                    */
/*pouring When the value of the contents attribute is a heading, and a heading        */
/*of the specified level does not appear on the output page, the                      */
/*contents can be 'poured' back to a previous heading level. When                     */
/*the attribute value none is specified, no pouring occurs. In this case,             */
/*the region will be empty. When the attribute value last is specified,               */
/*the last heading appearing in the document with the same level as                   */
/*the heading specified by the contents attribute is used. The attribute              */
/*value headn, where n may have a value of zero through six                           */
/*inclusive, may be specified. In this case, the last heading appearing               */
/*in the document which has a level between zero and the pouring                      */
/*value is used.                                                                      */
/*                                                                                    */
/*script_format This attribute determines if the contents region is processed as a    */
/*Script content string in the same way as the operand of a Script                    */
/*running title control word. If the attribute value is yes, then the                 */
/*value of the content attribute is treated as a Script format title string.          */
/*                                                                                    */
/*contents This attribute defines the content of the banner region. If the            */
/*content value does not fit in the banner region, the value is                       */
/*truncated. Symbols containing the values for each of the content                    */
/*keywords are also listed. Specifying these symbols as part of the                   */
/*string content may be used to create more complex banner region                     */
/*values. Note that when using a symbol in a content string of a                      */
/*banner definition, you will need to protect it from being substituted               */
/*during the definition with the &AMP symbol (ie                                      */
/*&AMP.AUTHOR.). The possible values are:                                             */
/*         author The first author of the document will be used.                      */
/*             The symbol $AUTHOR is also defined with                                */
/*             this value.                                                            */
/*         bothead The last heading on the output page is used.                       */
/*             The symbol $BOTHEAD is also defined with                               */
/*             this value.                                                            */
/*         date The current date will be used.                                        */
/*         docnum The document number will be the content of                          */
/*             the banner region. The symbol $DOCNUM is                               */
/*             also defined with this value.                                          */
/*         HEADn The last heading of level n, where n may have                        */
/*             a value of zero through six inclusive. Both the                        */
/*             heading number and heading text are both                               */
/*             used. The symbols $HEAD0 through                                       */
/*             $HEAD6 are also defined with this value.                               */
/*         HEADNUMn The heading number from the last heading of                       */
/*             level n, where n may have a value of zero                              */
/*             through six inclusive. The symbols $HNUM0                              */
/*             through $HNUM6 are also defined with this                              */
/*             value.                                                                 */
/*         HEADTEXTn The text of the heading from the last heading                    */
/*             of level n, where n may have a value of zero                           */
/*             through six inclusive. If the stitle attribute was                     */
/*             specified for the selected heading, the stitle                         */
/*             value is used. The symbols $HTEXT0 through                             */
/*             $HTEXT6 are also defined with this value.                              */
/*         none The banner region will be empty.                                      */
/*         pgnuma The content of the banner region will be the                        */
/*             page number of the output page in the                                  */
/*             hindu-arabic numbering style. The symbol                               */
/*             $PGNUMA is also defined with this value.                               */
/*         pgnumad The content of the banner region will be the                       */
/*             page number of the output page in the                                  */
/*             hindu-arabic numbering style followed by a                             */
/*             decimal point. The symbol $PGNUMAD is                                  */
/*             also defined with this value.                                          */
/*         pgnumr The content of the banner region will be the                        */
/*             page number of the output page in the lower                            */
/*             case roman numbering style. The symbol :                               */
/*             $PGNUMR is also defined with this value.                               */
/*         pgnumrd The content of the banner region will be the                       */
/*             page number of the output page in the lower                            */
/*             case roman numbering style followed by a :                             */
/*             decimal point. The symbol $PGNUMRD is                                  */
/*             also defined with this value.                                          */
/*         pgnumc The content of the banner region will be the                        */
/*             page number of the output page in the upper                            */
/*             case roman numbering style. The symbol :                               */
/*             $PGNUMC is also defined with this value.                               */
/*         pgnumcd The content of the banner region will be the                       */
/*             page number of the output page in the upper                            */
/*             case roman numbering style followed by a :                             */
/*             decimal point. The symbol $PGNUMCD is                                  */
/*             also defined with this value.                                          */
/*         rule The content of the banner region will be a rule                       */
/*             line which fills the entire region.                                    */
/*             sec The security value specified by the sec                            */
/*             attribute on the :gdoc tag is used. The symbol                         */
/*             $SEC is also defined with this value.                                  */
/*         stitle The stitle attribute value from the first title tag                 */
/*             specified in the front material of the document                        */
/*             is used. If the stitle attribute was not specified,                    */
/*             the title text is used. The symbol $STITLE is                          */
/*             also defined with this value.                                          */
/*         title The text of the first title tag specified in the                     */
/*             front material of the document is used. The                            */
/*             symbol $TITLE is also defined with this value.                         */
/*         string Any character string enclosed in quotation                          */
/*             marks.                                                                 */
/*         time The current time will be used.                                        */
/*         tophead The first heading on the output page is used.                      */
/*             The symbol $TOPHEAD is also defined with                               */
/*             this value.                                                            */
/*                                                                                    */
/*if a banner region does not already exist, then all attributes must be specified.   */
/*If you wish to modify an existing banner region, the refnum attribute will          */
/*uniquely identify the region. When the reference number is that of an               */
/*existing banner region, all other attributes will modify the values of the          */
/*existing banner region.                                                             */
/*To delete a banner region, specify only the refnum attribute. All banner            */
/*regions must be deleted before a banner definition will be removed.                 */
/**************************************************************************************/


/***************************************************************************/
/*  init banregion with no values                                          */
/***************************************************************************/

static  void    init_banregion_wk( region_lay_tag * reg )
{
    int         k;
    char        z0[2] = "0";

    reg->next = NULL;
    reg->reg_indent = 0;
    reg->reg_hoffset = 0;
    reg->reg_width = 0;
    reg->reg_voffset = 0;
    reg->reg_depth = 0;
    lay_init_su( z0, &(reg->indent) );
    lay_init_su( z0, &(reg->hoffset) );
    lay_init_su( z0, &(reg->width) );
    lay_init_su( z0, &(reg->depth) );
    reg->font = 0;
    reg->refnum = 0;
    reg->region_position = pos_left;
    reg->pouring = no_pour;
    reg->script_format = false;
    reg->contents.content_type = string_content;
    reg->contents.string[0] = '\0';
    reg->script_region[0].len = 0;
    reg->script_region[1].len = 0;
    reg->script_region[2].len = 0;
    reg->script_region[0].string = NULL;
    reg->script_region[1].string = NULL;
    reg->script_region[2].string = NULL;
    reg->final_content[0].len = 0;
    reg->final_content[1].len = 0;
    reg->final_content[2].len = 0;
    reg->final_content[0].hoffset = 0;
    reg->final_content[1].hoffset = 0;
    reg->final_content[2].hoffset = 0;
    reg->final_content[0].string = NULL;
    reg->final_content[1].string = NULL;
    reg->final_content[2].string = NULL;

    for( k = 0; k < att_count; k++ ) {
        count[k] = false;
    }
    sum_count = 0;
}


/***************************************************************************/
/*  lay_banregion                                                          */
/***************************************************************************/

void    lay_banregion( const gmltag * entry )
{
    char            *   p;
    condcode            cc;
    int                 k;
    lay_att             curr;
    region_lay_tag  *   reg;

    (void)entry;

    p = scan_start;
    rs_loc = banreg_tag;

    if( !GlobalFlags.firstpass ) {
        scan_start = scan_stop + 1;
        eat_lay_sub_tag();
        return;                         // process during first pass only
    }

    memset( &AttrFlags, 0, sizeof( AttrFlags ) );   // clear all attribute flags
    if( del_ban != NULL ) {             // BANREGION cancels deleteable status
        curr_ban = del_ban;
        del_ban = NULL;
    }

    if( ProcFlags.lay_xxx != el_banregion ) {
        if( !ProcFlags.banner ) {               // not in BANNER/eBANNER block
            xx_err_c( err_tag_expected, "BANNER" );
        }
        ProcFlags.lay_xxx = el_banregion;
        init_banregion_wk( &wk );
    }

    cc = get_attr_and_value();            // get att with value
    while( cc == pos ) {
        for( k = 0; k < att_count; k++ ) {
            curr = banregion_att[k];

            if( !strnicmp( att_names[curr], g_att_val.att_name, g_att_val.att_len ) ) {
                p = g_att_val.val_name;

                if( count[k] ) {
                    if( sum_count == att_count ) {  // all attributes found
                        xx_err( err_lay_text );     // per wgml 4.0: treat as text
                    } else {
                        xx_err( err_att_dup );      // per wgml 4.0: treat as duplicated attribute
                    }
                } else {
                    count[k] = true;
                    sum_count++;
                    switch( curr ) {
                    case   e_indent:
                        if( AttrFlags.indent ) {
                            xx_line_err_ci( err_att_dup, g_att_val.att_name,
                                g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                        }
                        i_space_unit( p, curr, &wk.indent );
                        AttrFlags.indent = true;
                        break;
                    case   e_hoffset:
                        if( AttrFlags.hoffset ) {
                            xx_line_err_ci( err_att_dup, g_att_val.att_name,
                                g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                        }
                        i_space_unit( p, curr, &wk.hoffset );
                        AttrFlags.hoffset = true;
                        break;
                    case   e_width:
                        if( AttrFlags.width ) {
                            xx_line_err_ci( err_att_dup, g_att_val.att_name,
                                g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                        }
                        i_space_unit( p, curr, &wk.width );
                        AttrFlags.width = true;
                        break;
                    case   e_voffset:
                        if( AttrFlags.voffset ) {
                            xx_line_err_ci( err_att_dup, g_att_val.att_name,
                                g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                        }
                        i_space_unit( p, curr, &wk.voffset );
                        AttrFlags.voffset = true;
                        break;
                    case   e_depth:
                        if( AttrFlags.depth ) {
                            xx_line_err_ci( err_att_dup, g_att_val.att_name,
                                g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                        }
                        i_space_unit( p, curr, &wk.depth );
                        AttrFlags.depth = true;
                        break;
                    case   e_font:
                        if( AttrFlags.font ) {
                            xx_line_err_ci( err_att_dup, g_att_val.att_name,
                                g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                        }
                        i_font_number( p, curr, &wk.font );
                        if( wk.font >= wgml_font_cnt ) wk.font = 0;
                        AttrFlags.font = true;
                        break;
                    case   e_refnum:
                        if( AttrFlags.refnum ) {
                            xx_line_err_ci( err_att_dup, g_att_val.att_name,
                                g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                        }
                        i_int8( p, curr, &wk.refnum );
                        if( wk.refnum < 0 ) {           // refnum cannot be negative
                            xx_line_err_c( err_num_too_large, p );
                        } else if( wk.refnum == 0 ) {   // refnum must be greater than zero
                            xx_line_err_c( err_num_zero, p );
                        }
                        AttrFlags.refnum = true;
                        break;
                    case   e_region_position:
                        if( AttrFlags.region_position ) {
                            xx_line_err_ci( err_att_dup, g_att_val.att_name,
                                g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                        }
                        i_page_position( p, curr, &wk.region_position );
                        AttrFlags.region_position = true;
                        break;
                    case   e_pouring:
                        if( AttrFlags.pouring ) {
                            xx_line_err_ci( err_att_dup, g_att_val.att_name,
                                g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                        }
                        i_pouring( p, curr, &wk.pouring );
                        AttrFlags.pouring = true;
                        break;
                    case   e_script_format:
                        if( AttrFlags.script_format ) {
                            xx_line_err_ci( err_att_dup, g_att_val.att_name,
                                g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                        }
                        i_yes_no( p, curr, &wk.script_format );
                        AttrFlags.script_format = true;
                        break;
                    case   e_contents:
                        if( AttrFlags.contents ) {
                            xx_line_err_ci( err_att_dup, g_att_val.att_name,
                                g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                        }
                        if( g_att_val.val_quoted ) {
                            wk.contents.content_type = string_content;
                            i_xx_string( p, curr, wk.contents.string );
                        } else {
                            i_content( p, curr, &wk.contents );
                        }
                        AttrFlags.contents = true;
                        break;
                    default:
                        internal_err( __FILE__, __LINE__ );
                    }
                }
                break;                  // break out of for loop
            }
        }
        cc = get_attr_and_value();            // get att with value
    }

    /*******************************************************/
    /* At this point, end-of-tag has been reached and all  */
    /* attributes provided have been found and processed.  */
    /* First ensure the required attribute is present.     */
    /*******************************************************/

    if( !AttrFlags.refnum ) {                           // refnum was missing
        xx_err( err_att_missing );
    } else if( wk.refnum > curr_ban->next_refnum ) {    // refnum must be, at most, the next value
        xx_err( err_illegal_reg_ref );
    }

    /*******************************************************/
    /* Find the region.                                    */
    /*******************************************************/

    prev_reg = NULL;
    reg = curr_ban->region;
    while( reg != NULL ) {
        if( reg->refnum == wk.refnum ) {// found correct region
            break;
        } else {
            prev_reg = reg;
            reg = reg->next;
        }
    }

    /*******************************************************/
    /* Process the region.                                 */
    /*******************************************************/

    if( reg != NULL ) {                 // region found
        if( sum_count == 1 ) {          // banregion delete request
            if( prev_reg == NULL ) {
                curr_ban->region = reg->next;
            } else {
                prev_reg->next = reg->next;
            }
            mem_free( reg );
            reg = NULL;
        } else {                        // modify existing banregion
            for( k = 0; k < att_count; ++k ) {
                if( count[k] ) {        // change specified attribute
                    switch( banregion_att[k] ) {
                    case e_indent:
                        memcpy( &(reg->indent), &(wk.indent), sizeof( wk.indent ) );
                        break;
                    case e_hoffset:
                        memcpy( &(reg->hoffset), &(wk.hoffset), sizeof( wk.hoffset ) );
                        break;
                    case e_width:
                        memcpy( &(reg->width), &(wk.width), sizeof( wk.width ) );
                        break;
                    case e_voffset:
                        memcpy( &(reg->voffset), &(wk.voffset), sizeof( wk.voffset ) );
                        break;
                    case e_depth:
                        memcpy( &(reg->depth), &(wk.depth), sizeof( wk.depth ) );
                        break;
                    case e_font:
                        reg->font = wk.font;
                        break;
                    case e_refnum:
                        reg->refnum = wk.refnum;
                        break;
                    case e_region_position:
                        reg->region_position = wk.region_position;
                        break;
                    case e_pouring:
                        reg->pouring = wk.pouring;
                        break;
                    case e_script_format:
                        reg->script_format = wk.script_format;
                        break;
                    case e_contents:
                        memcpy( &(reg->contents), &(wk.contents), sizeof( wk.contents ) );
                        break;
                    default:
                        break;
                    }
                }
            }
        }
    } else {                            // new region
        if( sum_count == 1 ) {          // banregion delete request

            /*******************************************************/
            /* If a non-existent region is supposed to be deleted, */
            /* then there is nothing to do.                        */
            /*******************************************************/

        } else if( (sum_count != 11) && (AttrFlags.script_format && (sum_count == 10)) ) {

            /*******************************************************/
            /* A new region must have values for all attributes    */
            /* except script_format.                               */
            /*******************************************************/

            xx_err( err_all_reg_att_rqrd );
        }
        reg = mem_alloc( sizeof( region_lay_tag ) );
        memcpy( reg, &wk, sizeof( region_lay_tag ) );
        if( prev_reg == NULL ) {    // first region in banner
            curr_ban->region = reg;
        } else {
            prev_reg->next = reg;
        }
        curr_ban->next_refnum++;    // next expected refnum value
    }

    /*******************************************************/
    /* Check for attribute inconsistency.                  */
    /*******************************************************/

    if( reg != NULL ) {             // region not deleted
        if( reg->script_format && (reg->contents.content_type != string_content) ) {
            xx_err( err_scr_fmt );
        }
    }

    scan_start = scan_stop + 1;
    return;
}


/***************************************************************************/
/*  lay_ebanregion                                                         */
/***************************************************************************/

void    lay_ebanregion( const gmltag * entry )
{
    rs_loc = banner_tag;
    if( !GlobalFlags.firstpass ) {
        scan_start = scan_stop + 1;
        eat_lay_sub_tag();
        return;                         // process during first pass only
    }
    if( ProcFlags.lay_xxx == el_banregion ) {   // :banregion was last tag
        ProcFlags.lay_xxx = el_ebanregion;
    } else {
        xx_err_cc( err_no_lay, &(entry->tagname[1]), entry->tagname );
    }
    scan_start = scan_stop + 1;
    return;
}
