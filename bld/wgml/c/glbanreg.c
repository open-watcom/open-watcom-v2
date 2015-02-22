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
* Description: WGML implement :BANREGION :eBANREGION  tags for LAYOUT processing
*
****************************************************************************/

#include "wgml.h"
#include "gvars.h"

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

static  const   int att_count = sizeof( banregion_att );
static  int         count[sizeof( banregion_att )];
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
    reg->refnum = -1;
    reg->pouring = no_pour;
    reg->script_format = false;
    reg->contents.string[0] = '\0';
    for( k = 0; k < att_count; k++ ) {
        count[k] = 0;
    }
    sum_count = 0;

}


/***************************************************************************/
/*  lay_banregion                                                          */
/***************************************************************************/

void    lay_banregion( lay_tag tag )
{
    char        *   p;
    condcode        cc;
    int             k;
    lay_att         curr;
    att_args        l_args;
    bool            cvterr;

    p = scan_start;
    cvterr = false;

    if( !GlobalFlags.firstpass ) {
        scan_start = scan_stop;
        eat_lay_sub_tag();
        return;                         // process during first pass only
    }
    if( ProcFlags.lay_xxx != el_banregion ) {
        if( ProcFlags.lay_xxx == el_banner ) {
            lay_banner_end_prepare();
        }
        ProcFlags.lay_xxx = el_banregion;

        init_banregion_wk( &wk );

    } else {
        if( !strnicmp( ":banregion", buff2, sizeof( ":banregion" ) ) ) {
            err_count++;
            g_err( err_nested_tag, lay_tagname( tag ) );
            file_mac_info();

            while( !ProcFlags.reprocess_line  ) {
                eat_lay_sub_tag();
                if( strnicmp( ":ebanregion", buff2, sizeof( ":ebanregion" ) ) ) {
                    ProcFlags.reprocess_line = false;// not :ebanregion, go on
                }
            }
            return;
        }
    }
    cc = get_lay_sub_and_value( &l_args );              // get att with value
    while( cc == pos ) {
        cvterr = true;
        for( k = 0; k < att_count; k++ ) {
            curr = banregion_att[k];

            if( !strnicmp( att_names[curr], l_args.start[0], l_args.len[0] ) ) {
                p = l_args.start[1];

                if( count[k] ) {
                    cvterr = 1;                  // attribute specified twice
                } else {
                    count[k] += 1;
                    sum_count++;
                    switch( curr ) {
                    case   e_indent:
                        cvterr = i_space_unit( p, curr, &wk.indent );
                        break;
                    case   e_hoffset:
                        cvterr = i_space_unit( p, curr, &wk.hoffset );
                        break;
                    case   e_width:
                        cvterr = i_space_unit( p, curr, &wk.width );
                        break;
                    case   e_voffset:
                        cvterr = i_space_unit( p, curr, &wk.voffset );
                        break;
                    case   e_depth:
                        cvterr = i_space_unit( p, curr, &wk.depth );
                        break;
                    case   e_font:
                        cvterr = i_font_number( p, curr, &wk.font );
                        if( wk.font >= wgml_font_cnt )
                            wk.font = 0;
                        break;
                    case   e_refnum:
                        cvterr = i_int8( p, curr, &wk.refnum );
                        break;
                    case   e_region_position:
                        cvterr = i_page_position( p, curr, &wk.region_position );
                        break;
                    case   e_pouring:
                        cvterr = i_pouring( p, curr, &wk.pouring );
                        break;
                    case   e_script_format:
                        cvterr = i_yes_no( p, curr, &wk.script_format );
                        break;
                    case   e_contents:
                        if( l_args.quoted ) {
                            wk.contents.content_type = string_content;
                            cvterr = i_xx_string( p, curr, wk.contents.string );
                        } else {
                            cvterr = i_content( p, curr, &wk.contents );
                        }
                        break;
                    default:
                        out_msg( "WGML logic error.\n");
                        cvterr = true;
                        break;
                    }
                }
                if( cvterr ) {          // there was an error
                    err_count++;
                    g_err( err_att_val_inv );
                    file_mac_info();
                }
                break;                  // break out of for loop
            }
        }
        cc = get_lay_sub_and_value( &l_args );  // get att with value
    }
    scan_start = scan_stop;
    return;
}

/***************************************************************************/
/*  search region in banner                                                */
/***************************************************************************/

static region_lay_tag * find_region( banner_lay_tag * ban )
{
    region_lay_tag  *   reg;

    reg = ban->region;
    prev_reg = NULL;
    while( reg != NULL ) {
        if( reg->refnum == wk.refnum ) {// found correct region
            break;
        } else {
            prev_reg = reg;
            reg = reg->next;
        }
    }
    return( reg );
}

/***************************************************************************/
/*  lay_ebanregion                                                         */
/***************************************************************************/

void    lay_ebanregion( lay_tag tag )
{
    region_lay_tag  *   reg;
    banner_lay_tag  *   reg_ban;
    int                 k;
    bool                region_deleted;

    if( !GlobalFlags.firstpass ) {
        scan_start = scan_stop;
        eat_lay_sub_tag();
        return;                         // process during first pass only
    }
    if( ProcFlags.lay_xxx == el_banregion ) {   // :banregion was last tag
        ProcFlags.lay_xxx = el_ebanregion;

        prev_reg = NULL;
        reg_ban = NULL;
        region_deleted = false;
        if( sum_count == 1 && wk.refnum > 0  ) {// banregion delete request
            if( del_ban != NULL ) {     // banner delete request, too
                reg_ban = del_ban;
            } else {
                reg_ban = curr_ban;
            }
            reg = find_region( reg_ban );

            if( reg != NULL) {          // banregion delete
               if( prev_reg == NULL ) {
                   reg_ban->region = reg->next;
               } else {
                   prev_reg->next = reg->next;
               }
               mem_free( reg );
               reg = NULL;
               region_deleted = true;   // processing complete
            }
        }
        if( !region_deleted ) {         // no region delete request, or
            prev_reg = NULL;            // region not found
            if( del_ban != NULL ) {
                reg_ban = del_ban;
            } else {
                reg_ban = curr_ban;
            }
            reg = find_region( reg_ban );
            if( reg == NULL ) {         // not found, new region
                reg = mem_alloc( sizeof( region_lay_tag ) );
                memcpy( reg, &wk, sizeof( region_lay_tag ) );
                if( prev_reg == NULL ) {// first region in banner
                    reg_ban->region = reg;
                } else {
                    prev_reg->next = reg;
                }
            } else {                    // modify existing banregion
                for( k = 0; k < att_count; ++k ) {
                    if( count[k] ) {// change specified attribute
                        switch( banregion_att[k] ) {
                        case   e_indent:
                            memcpy( &(reg->indent), &(wk.indent),
                                    sizeof( wk.indent ) );
                            break;
                        case   e_hoffset:
                            memcpy( &(reg->hoffset), &(wk.hoffset),
                                    sizeof( wk.hoffset ) );
                            break;
                        case   e_width:
                            memcpy( &(reg->width), &(wk.width),
                                    sizeof( wk.width ) );
                            break;
                        case   e_voffset:
                            memcpy( &(reg->voffset), &(wk.voffset),
                                    sizeof( wk.voffset ) );
                            break;
                        case   e_depth:
                            memcpy( &(reg->depth), &(wk.depth),
                                    sizeof( wk.depth ) );
                            break;
                        case   e_font:
                            reg->font = wk.font;
                            break;
                        case   e_refnum:
                            reg->refnum = wk.refnum;
                            break;
                        case   e_region_position:
                            reg->region_position = wk.region_position;
                            break;
                        case   e_pouring:
                            reg->pouring = wk.pouring;
                            break;
                        case   e_script_format:
                            reg->script_format = wk.script_format;
                            break;
                        case   e_contents:
                            memcpy( &(reg->contents), &(wk.contents),
                                    sizeof( wk.contents ) );
                            break;
                        default:
                            break;
                        }
                    }
                }
            }
        }

    } else {
        g_err( err_no_lay, &(lay_tagname( tag )[1]), lay_tagname( tag ) );
        err_count++;
        file_mac_info();
    }
    scan_start = scan_stop;
    return;
}
