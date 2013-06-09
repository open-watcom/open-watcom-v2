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
* Description:  wgml type definitions for layout data   :LAYOUT ... :eLAYOUT
*
****************************************************************************/
 
#ifndef GTYPELAY_H_INCLUDED
#define GTYPELAY_H_INCLUDED
 
/***************************************************************************/
/*  Layout attributes as enum list                                         */
/*    these are used in LAYOUT tag processing to control the attributes    */
/***************************************************************************/
 
#define pick( name, funci, funco, result ) e_##name,
typedef enum lay_att {
    e_dummy_zero = 0,
#include "glayutil.h"
    e_dummy_max
} lay_att;
 
#undef pick
 
/***************************************************************************/
/*  definitions for number style                                           */
/***************************************************************************/
 
typedef enum num_style {
    h_style     = 0x0001,               // hindu arabic
    a_style     = 0x0002,               // lowercase alphabetic
    b_style     = 0x0004,               // uppercase alphabetic
    c_style     = 0x0080,               // uppercase roman
    r_style     = 0x0010,               // lowercase roman
    char1_style = a_style | b_style | c_style | h_style | r_style,
    xd_style    = 0x0100,               // decimal point follows
    xp_style    = 0x0600,               // in parenthesis
    xpa_style   = 0x0200,               // only left parenthesis
    xpb_style   = 0x0400                // only right parenthesis
} num_style;
 
/***************************************************************************/
/*  definitions for place for :BANNER and :FIG tag                         */
/***************************************************************************/
 
typedef enum bf_place {
    no_place        = 0,
    inline_place,                       // only :FIG
    top_place,
    bottom_place,
    topodd_place,                       // topodd and following only :BANNER
    topeven_place,
    botodd_place,
    boteven_place
} bf_place;
 
/***************************************************************************/
/*  definitions for docsect for :BANNER tag                                */
/*     document sections and :Hx tags                                      */
/***************************************************************************/
 
typedef enum ban_docsect {
    #define pick(e,t,s,n) e,
    #include "bdocsect.h"
    #undef pick
    max_ban                             // has to be last defined value
} ban_docsect;
 
/***************************************************************************/
/*  definitions for frame   :FIG tag and others                            */
/***************************************************************************/
#define xx_str      char
#define str_size    60                  // max string length in layout TBD
                                        // no value found in documentation
typedef enum def_frame_type {
    none,
    box_frame,
    rule_frame,
    char_frame
} def_frame_type;
 
typedef struct def_frame {
    def_frame_type      type;
    xx_str              string[str_size];
} def_frame;
 
/***************************************************************************/
/*  definition for note_string for :NOTE tag and others                    */
/***************************************************************************/
 
/***************************************************************************/
/*  definitions for :Hx tag number form and page position                  */
/*                                                                         */
/***************************************************************************/
 
typedef enum num_form {
    num_none,
    num_prop,
    num_new
} num_form;
 
typedef enum page_pos {
    pos_left,
    pos_right,
    pos_center,
    pos_centre = pos_center
} page_pos;
 
typedef enum page_ej {
    ej_no,
    ej_yes,
    ej_odd,
    ej_even
} page_ej;
 
typedef enum case_t {
    case_mixed,
    case_lower,
    case_upper
} case_t;
 
 
/***************************************************************************/
/*  :ADDRESS  Layout tag data                                               */
/***************************************************************************/
typedef struct address_lay_tag {
    su              left_adjust;        // horizontal space unit
    su              right_adjust;       // horizontal space unit
    su              pre_skip;           // vertical space unit
    font_number     font;               // non negative integer
    page_pos        page_position;      // enum
} address_lay_tag;
 
/***************************************************************************/
/*  :ALINE    Layout tag data                                               */
/***************************************************************************/
typedef struct aline_lay_tag {
    su              skip;           // vertical space unit
} aline_lay_tag;
 
 
/***************************************************************************/
/*  :AUTHOR  Layout tag data                                               */
/***************************************************************************/
typedef struct author_lay_tag {
    su              left_adjust;        // horizontal space unit
    su              right_adjust;       // horizontal space unit
    su              pre_skip;           // vertical space unit
    su              skip;               // vertical space unit
    font_number     font;               // non negative integer
    page_pos        page_position;      // enum
} author_lay_tag;
 
 
/***************************************************************************/
/*  :PAGE    layout tag data                                               */
/***************************************************************************/
typedef struct page_lay_tag {
    su              top_margin;         // vertical space unit
    su              left_margin;        // horizontal space unit
    su              right_margin;       // horizontal space unit
    su              depth;              // vertical space unit
} page_lay_tag;
 
 
/***************************************************************************/
/*  :DEFAULT layout tag data                                               */
/***************************************************************************/
 
typedef struct default_lay_tag {
    su              gutter;             // horizontal space unit
    su              binding;            // horizontal space unit
    font_number     font;               // non-negative integer
    int8_t          spacing;            // positive integer
    int8_t          columns;            // positive integer
    bool            justify;            // yes / no  -> bool
    char            input_esc;          // none or quoted char
} default_lay_tag;
 
/***************************************************************************/
/*  :WIDOW   layout tag data                                               */
/***************************************************************************/
 
typedef struct widow_lay_tag {
    uint8_t         threshold;          // non-negative integer
} widow_lay_tag;
 
 
/***************************************************************************/
/*  :FN       Layout tag data                                              */
/***************************************************************************/
 
typedef struct fn_lay_tag {
    su              line_indent;        // horizontal space unit
    su              align;              // horizontal space unit
    su              pre_lines;          // vertical space unit
    su              skip;               // vertical space unit
    font_number     font;               // non-negative integer
    font_number     number_font;        // non-negative integer
    int8_t          spacing;            // positive integer
    bool            frame;              // rule=1  none=0 bool
    num_style       number_style;       // special enum
} fn_lay_tag;
 
 
/***************************************************************************/
/*  :FNREF    Layout tag data                                              */
/***************************************************************************/
 
typedef struct fnref_lay_tag {
    num_style       number_style;       // special enum
    font_number     font;               // non-negative integer
} fnref_lay_tag;
 
 
/***************************************************************************/
/*  :P and :PC Layout tag data                                             */
/***************************************************************************/
 
typedef struct p_lay_tag {
    su              line_indent;        // horizontal space unit
    su              pre_skip;           // vertical space unit
    su              post_skip;          // vertical space unit
} p_lay_tag;
 
 
/***************************************************************************/
/*  :FIG      Layout tag data                                              */
/***************************************************************************/
 
typedef struct fig_lay_tag {
    su              left_adjust;        // horizontal space unit
    su              right_adjust;       // horizontal space unit
    su              pre_skip;           // vertical space unit
    su              post_skip;          // vertical space unit
    font_number     font;               // non-negative integer
    int8_t          spacing;            // positive integer
    bf_place        default_place;      // special enum
    def_frame       default_frame;      // special
} fig_lay_tag;
 
 
/***************************************************************************/
/*  :XMP      Layout tag data                                              */
/***************************************************************************/
 
typedef struct xmp_lay_tag {
    su              left_indent;        // horizontal space unit
    su              right_indent;       // horizontal space unit
    su              pre_skip;           // vertical space unit
    su              post_skip;          // vertical space unit
    font_number     font;               // non-negative integer
    int8_t          spacing;            // positive integer
} xmp_lay_tag;
 
 
/***************************************************************************/
/*  :NOTE     Layout tag data                                              */
/***************************************************************************/
typedef struct note_lay_tag {
    su              left_indent;        // horizontal space unit
    su              right_indent;       // horizontal space unit
    su              pre_skip;           // vertical space unit
    su              post_skip;          // vertical space unit
    font_number     font;               // non-negative integer
    int8_t          spacing;            // positive integer
    xx_str          string[str_size];   // special string
} note_lay_tag;
 
 
/***************************************************************************/
/*  :APPENDIX Layout tag data                                              */
/***************************************************************************/
 
typedef struct appendix_lay_tag {
    su              indent;             // horizontal space unit
    su              pre_top_skip;       // vertical space unit
    su              pre_skip;           // vertical space unit
    su              post_skip;          // vertical space unit
    su              align;              // horizontal space unit
    font_number     font;               // non-negative integer
    font_number     number_font;        // non-negative integer
    int8_t          spacing;            // positive integer
    num_form        number_form;        // special enum ( none, prop, new )
    page_pos        page_position;      // special enum (left, right, center)
    page_ej         page_eject;         // enum for yes, no, odd, even
    num_style       number_style;       // special enum
    bool            line_break;         // yes, no -> bool
    bool            display_heading;    // yes, no -> bool
    bool            number_reset;       // yes, no -> bool
    case_t          cases;              // lower, upper, mixed
    bool            header;             // yes, no -> bool
    xx_str          string[str_size];   // special string
    bool            page_reset;         // yes, no -> bool
    page_ej         section_eject;      // enum for yes, no, odd, even
    int8_t          columns;            // non-negative integer
} appendix_lay_tag;
 
 
/***************************************************************************/
/*  :H0 - :H6 Layout tag data                                              */
/***************************************************************************/
 
typedef struct hx_lay_tag {
    su              indent;             // horizontal space unit
    su              pre_top_skip;       // vertical space unit
    su              pre_skip;           // vertical space unit
    su              post_skip;          // vertical space unit
    su              align;              // horizontal space unit
    int32_t         headn;              // holds current number if numbered
    symsub      *   headnsub;           // ptr to $HEADNUMx symvar entry
    font_number     font;               // non-negative integer
    font_number     number_font;        // non-negative integer
    int8_t          spacing;            // positive integer
    num_form        number_form;        // special enum ( none, prop, new )
    page_pos        page_position;      // special enum (left, right, center)
    int8_t          group;              // 0 - 9
    num_style       number_style;       // special enum
    page_ej         page_eject;         // enum for yes, no, odd, even
    bool            line_break;         // yes, no -> bool
    bool            display_heading;    // yes, no -> bool
    bool            number_reset;       // yes, no -> bool
    case_t          cases;              // lower, upper, mixed
} hx_lay_tag;
 
 
/***************************************************************************/
/*  :HEADING Layout tag data                                               */
/***************************************************************************/
 
typedef struct heading_lay_tag {
    char            delim;
    bool            stop_eject;         // yes, no -> bool
    bool            para_indent;        // yes, no -> bool
    uint8_t         threshold;          // non-negative integer
    int8_t          max_group;          // non-negative integer
} heading_lay_tag;
 
/***************************************************************************/
/*  :LQ      Layout tag data                                               */
/***************************************************************************/
 
typedef struct lq_lay_tag {
    su              left_indent;        // horizontal space unit
    su              right_indent;       // horizontal space unit
    su              pre_skip;           // vertical space unit
    su              post_skip;          // vertical space unit
    font_number     font;               // non-negative integer
    int8_t          spacing;            // positive integer
} lq_lay_tag;
 
/***************************************************************************/
/* :DT :GT :DTHD :CIT :GD :DDHD :IXPGNUM :IXMAJOR                          */
/*            Layout tag data  these only have a font value                */
/***************************************************************************/
 
typedef struct xx_lay_tag {
    font_number     font;               // non-negative integer
} xx_lay_tag;
 
/***************************************************************************/
/*  :FIGCAP  Layout tag data                                               */
/***************************************************************************/
 
typedef struct figcap_lay_tag {
    su              pre_lines;          // vertical space unit
    font_number     font;               // non-negative integer
    font_number     string_font;        // non-negative integer
    xx_str          string[str_size];   // special string
    char            delim;
} figcap_lay_tag;
 
/***************************************************************************/
/*  :FIGDESC Layout tag data                                               */
/***************************************************************************/
 
typedef struct figdesc_lay_tag {
    su              pre_lines;          // vertical space unit
    font_number     font;               // non-negative integer
} figdesc_lay_tag;
 
/***************************************************************************/
/*  :FIGLIST        Layout tag data                                        */
/***************************************************************************/
 
typedef struct figlist_lay_tag {
    su              left_adjust;        // horizontal space unit
    su              right_adjust;       // horizontal space unit
    su              skip;               // vertical space unit
    int8_t          spacing;            // positive integer
    int8_t          columns;            // positive integer
    int8_t          toc_levels;         // non-negative integer
    xx_str          fill_string[str_size];  // special string
} figlist_lay_tag;
 
/***************************************************************************/
/*  :FLPGNUM        Layout tag data                                        */
/***************************************************************************/
 
typedef struct flpgnum_lay_tag {
    su              size;               // horizontal space unit
    font_number     font;               // non-negative integer
    xx_str          fill_string[str_size];  // special string
} flpgnum_lay_tag;
 
/***************************************************************************/
/*  :DD      Layout tag data                                               */
/***************************************************************************/
 
typedef struct dd_lay_tag {
    su              line_left;          // horizontal space unit
    font_number     font;               // non-negative integer
} dd_lay_tag;
 
/***************************************************************************/
/*  :DATE    Layout tag data                                               */
/***************************************************************************/
typedef struct date_lay_tag {
    xx_str          date_form[str_size];// special string
    su              left_adjust;        // horizontal space unit
    su              right_adjust;       // horizontal space unit
    su              pre_skip;           // vertical space unit
    font_number     font;               // non negative integer
    page_pos        page_position;      // enum
} date_lay_tag;
 
/***************************************************************************/
/*  :DOCNUM  Layout tag data                                               */
/***************************************************************************/
 
typedef struct docnum_lay_tag {
    su              left_adjust;        // horizontal space unit
    su              right_adjust;       // horizontal space unit
    su              pre_skip;           // vertical space unit
    font_number     font;               // non-negative integer
    xx_str          string[str_size];   // special string
    page_pos        page_position;      // special enum (left, right, center)
} docnum_lay_tag;
 
/***************************************************************************/
/*  :ABSTRACT and :PREFACE  Layout tag data                                */
/***************************************************************************/
 
typedef struct abspref_lay_tag {
    su              post_skip;          // vertical space unit
    su              pre_top_skip;       // vertical space unit
    font_number     font;               // non-negative integer
    int8_t          spacing;            // positive integer
    bool            header;             // yes, no -> bool
    xx_str          string[str_size];   // special string
    page_ej         page_eject;         // enum for yes, no, odd, even
    bool            page_reset;         // yes, no -> bool
    int8_t          columns;            // positive integer
} abspref_lay_tag;
 
/***************************************************************************/
/*  :BACKM and :BODY  Layout tag data                                      */
/***************************************************************************/
 
typedef struct backbod_lay_tag {
    su              post_skip;          // vertical space unit
    su              pre_top_skip;       // vertical space unit
    font_number     font;               // non-negative integer
    bool            header;             // yes, no -> bool
    xx_str          string[str_size];   // special string
    page_ej         page_eject;         // enum for yes, no, odd, even
    bool            page_reset;         // yes, no -> bool
                                        // columns leave last
    int8_t          columns;            // positive integer
} backbod_lay_tag;
 
/***************************************************************************/
/*  :LP             Layout tag data                                        */
/***************************************************************************/
 
typedef struct lp_lay_tag {
    su              left_indent;        // horizontal space unit
    su              right_indent;       // horizontal space unit
    su              line_indent;        // horizontal space unit
    su              pre_skip;           // vertical space unit
    su              post_skip;          // vertical space unit
    int8_t          spacing;            // positive integer
} lp_lay_tag;
 
/***************************************************************************/
/*  :INDEX          Layout tag data                                        */
/***************************************************************************/
 
typedef struct index_lay_tag {
    su              post_skip;          // vertical space unit
    su              pre_top_skip;       // vertical space unit
    su              left_adjust;        // horizontal space unit
    su              right_adjust;       // horizontal space unit
    font_number     font;               // non-negative integer
    int8_t          spacing;            // positive integer
    int8_t          columns;            // positive integer
    xx_str          see_string[str_size];// special string
    xx_str          see_also_string[str_size];  // special string
    xx_str          index_string[str_size];// special string
    bool            header;             // yes, no -> bool
    page_ej         page_eject;         // enum for yes, no, odd, even
    bool            page_reset;         // yes, no -> bool
} index_lay_tag;
 
/***************************************************************************/
/*  :IXHEAD         Layout tag data                                        */
/***************************************************************************/
 
typedef struct ixhead_lay_tag {
    su              pre_skip;           // vertical space unit
    su              post_skip;          // vertical space unit
    su              indent;             // horizontal space unit
    font_number     font;               // non-negative integer
    def_frame       frame;              // special
    bool            header;             // yes, no -> bool
} ixhead_lay_tag;
 
/***************************************************************************/
/*  :I1 :I2 :I3     Layout tag data                                        */
/***************************************************************************/
 
typedef struct ix_lay_tag {
    su              pre_skip;           // vertical space unit
    su              post_skip;          // vertical space unit
    su              skip;               // vertical space unit
    su              indent;             // horizontal space unit
    su              wrap_indent;        // horizontal space unit
    font_number     font;               // non-negative integer
    font_number     string_font;        // non-negative integer
    xx_str          index_delim[str_size];  // special string
} ix_lay_tag;
 
/***************************************************************************/
/*  :TITLE   Layout tag data                                               */
/***************************************************************************/
 
typedef struct title_lay_tag {
    su              left_adjust;        // horizontal space unit
    su              right_adjust;       // horizontal space unit
    su              pre_top_skip;       // vertical space unit
    su              skip;               // vertical space unit
    font_number     font;               // non-negative integer
    page_pos        page_position;      // special enum (left, right, center)
} title_lay_tag;
 
/***************************************************************************/
/*  :TITLEP  Layout tag data                                               */
/***************************************************************************/
 
typedef struct titlep_lay_tag {
    int8_t          spacing;            // positive integer
    int8_t          columns;            // positive integer
} titlep_lay_tag;
 
/***************************************************************************/
/*  :TOC            Layout tag data                                        */
/***************************************************************************/
 
typedef struct toc_lay_tag {
    su              left_adjust;        // horizontal space unit
    su              right_adjust;       // horizontal space unit
    int8_t          spacing;            // positive integer
    int8_t          columns;            // positive integer
    int8_t          toc_levels;         // non-negative integer
    xx_str          fill_string[str_size];  // special string
} toc_lay_tag;
 
/***************************************************************************/
/*  :TOCPGNUM       Layout tag data also for :FLPGNUM                      */
/***************************************************************************/
 
typedef struct tocpgnum_lay_tag {
    su              size;               // horizontal space unit
    font_number     font;               // non-negative integer
} tocpgnum_lay_tag;
 
/***************************************************************************/
/*  :TOCH0 - TOCH6  Layout tag data                                        */
/***************************************************************************/
 
typedef struct tochx_lay_tag {
    su              indent;             // horizontal space unit
    su              skip;               // vertical space unit
    su              pre_skip;           // vertical space unit
    su              post_skip;          // vertical space unit
    su              align;              // horizontal space unit
    font_number     font;               // non-negative integer
    bool            display_in_toc;     // yes, no -> bool
    int8_t          group;              // 0 - 9
} tochx_lay_tag;
 
/***************************************************************************/
/*  :SL             Layout tag data                                        */
/***************************************************************************/
 
typedef struct sl_lay_tag {
    su              left_indent;        // horizontal space unit
    su              right_indent;       // horizontal space unit
    su              pre_skip;           // vertical space unit
    su              skip;               // vertical space unit
    su              post_skip;          // vertical space unit
    font_number     font;               // non-negative integer
    int8_t          spacing;            // positive integer
    int8_t          level;              // only level 1 supported
} sl_lay_tag;
 
/***************************************************************************/
/*  :OL             Layout tag data                                        */
/***************************************************************************/
 
typedef struct ol_lay_tag {
    su              left_indent;        // horizontal space unit
    su              right_indent;       // horizontal space unit
    su              pre_skip;           // vertical space unit
    su              skip;               // vertical space unit
    su              post_skip;          // vertical space unit
    su              align;              // horizontal space unit
    font_number     font;               // non-negative integer
    font_number     number_font;        // non-negative integer
    int8_t          spacing;            // positive integer
    int8_t          level;              // only level 1 supported
    num_style       number_style;       // enum special
} ol_lay_tag;
 
/***************************************************************************/
/*  :UL             Layout tag data                                        */
/***************************************************************************/
 
typedef struct ul_lay_tag {
    su              left_indent;        // horizontal space unit
    su              right_indent;       // horizontal space unit
    su              pre_skip;           // vertical space unit
    su              skip;               // vertical space unit
    su              post_skip;          // vertical space unit
    su              align;              // horizontal space unit
    font_number     font;               // non-negative integer
    font_number     bullet_font;        // non-negative integer
    int8_t          spacing;            // positive integer
    char            bullet;             // char
    bool            bullet_translate;   // yes, no -> bool
    int8_t          level;              // only level 1 supported
} ul_lay_tag;
 
/***************************************************************************/
/*  :DL             Layout tag data                                        */
/***************************************************************************/
 
typedef struct dl_lay_tag {
    su              left_indent;        // horizontal space unit
    su              right_indent;       // horizontal space unit
    su              pre_skip;           // vertical space unit
    su              skip;               // vertical space unit
    su              post_skip;          // vertical space unit
    su              align;              // horizontal space unit
    int8_t          spacing;            // positive integer
    bool            line_break;         // yes, no -> bool
    int8_t          level;              // only level 1 supported
} dl_lay_tag;
 
/***************************************************************************/
/*  :GL             Layout tag data                                        */
/***************************************************************************/
 
typedef struct gl_lay_tag {
    su              left_indent;        // horizontal space unit
    su              right_indent;       // horizontal space unit
    su              pre_skip;           // vertical space unit
    su              skip;               // vertical space unit
    su              post_skip;          // vertical space unit
    su              align;              // horizontal space unit
    int8_t          spacing;            // positive integer
    char            delim;              // yes, no -> bool
    int8_t          level;              // only level 1 supported
} gl_lay_tag;
 
/***************************************************************************/
/*  :BANREGION attribute values                                            */
/***************************************************************************/
 
typedef enum reg_pour {
    no_pour,
    last_pour,
    head0_pour,
    head1_pour,
    head2_pour,
    head3_pour,
    head4_pour,
    head5_pour,
    head6_pour
} reg_pour;
 
typedef enum content_enum {
    no_content          =  0,
    author_content,
    bothead_content,
    date_content,
    docnum_content,
    head0_content,
    head1_content,
    head2_content,
    head3_content,
    head4_content,
    head5_content,
    head6_content,
    headnum0_content,
    headnum1_content,
    headnum2_content,
    headnum3_content,
    headnum4_content,
    headnum5_content,
    headnum6_content,
    headtext0_content,
    headtext1_content,
    headtext2_content,
    headtext3_content,
    headtext4_content,
    headtext5_content,
    headtext6_content,
    pgnuma_content,
    pgnumad_content,
    pgnumr_content,
    pgnumrd_content,
    pgnumc_content,
    pgnumcd_content,
    rule_content,
    sec_content,
    stitle_content,
    title_content,
    string_content,
    time_content,
    tophead_content,
    max_content                         // keep as last entry
} content_enum;
 
typedef struct content {
    content_enum    content_type;
    xx_str          string[str_size];
} content;
 
typedef struct script_ban_reg {         // for script format region
    size_t          len;                // preprocessed content
    char        *   string;
} script_ban_reg;
 
/***************************************************************************/
/*  :BANREGION      Layout tag data                                        */
/***************************************************************************/
 
typedef struct region_lay_tag {
    struct region_lay_tag       *   next;   // next banner region
    uint32_t        reg_indent;         // value of 'indent' in base units
    uint32_t        reg_hoffset;        // value of 'hoffset' in base units
    uint32_t        reg_width;          // value of 'width' in base units
    uint32_t        reg_voffset;        // value of 'voffset' in base units
    uint32_t        reg_depth;          // value of 'depth' in base units
    su              indent;             // horizontal space unit or keywords
    su              hoffset;            // horizontal space unit or keywords
    su              width;              // horizontal space unit or extend
    su              voffset;            // vertical space unit
    su              depth;              // vertical space unit
    script_ban_reg  script_region[3];   // speed up processing if script_format
                                        // will be constructed from contents
    font_number     font;               // non negative integer
    int8_t          refnum;             // positive integer
    page_pos        region_position;    // special enum
    reg_pour        pouring;            // special enum
    content         contents;           // what is in the region
    bool            script_format;      // yes no -> bool
} region_lay_tag;
 
/***************************************************************************/
/*  :BANNER         Layout tag data                                        */
/***************************************************************************/
 
typedef struct banner_lay_tag {
    struct banner_lay_tag   *   next;   // next banner
    region_lay_tag          *   region; // banner region
    region_lay_tag          *   top_line;// region containing top line in banner
    uint32_t        ban_left_adjust;    // value of 'left_adjust' in base units
    uint32_t        ban_right_adjust;   // value of 'right_adjust' in base units
    uint32_t        ban_depth;          // value of 'depth' in base units
    su              left_adjust;        // horizontal space unit
    su              right_adjust;       // horizontal space unit
    su              depth;              // vertical space unit
    bf_place        place;              // special enum
    ban_docsect     docsect;            // special enum
//  bf_place        refplace;           // special enum no need to store
//  ban_docsect     refdoc;             // special enum no need to store
} banner_lay_tag;
 
 
 
/***************************************************************************/
/*  Layout data                                             TBD            */
/*  sequence of definitions as seen by :CONVERT output                     */
/***************************************************************************/
 
#ifdef __WATCOMC__
#pragma disable_message( 128 ); // suppress: Warning! W128: 3 padding byte(s) added
#endif

typedef struct layout_data {
    page_lay_tag        page;
    default_lay_tag     defaults;
    widow_lay_tag       widow;
    fn_lay_tag          fn;
    fnref_lay_tag       fnref;
    p_lay_tag           p;
    p_lay_tag           pc;             // :PC same struct as :P
    fig_lay_tag         fig;
    xmp_lay_tag         xmp;
    note_lay_tag        note;
    hx_lay_tag          hx[7];
    heading_lay_tag     heading;
    lq_lay_tag          lq;
    xx_lay_tag          dt;
    xx_lay_tag          gt;
    xx_lay_tag          dthd;
    xx_lay_tag          cit;
    figcap_lay_tag      figcap;
    figdesc_lay_tag     figdesc;
    dd_lay_tag          dd;
    xx_lay_tag          gd;
    xx_lay_tag          ddhd;
    abspref_lay_tag     abstract;
    abspref_lay_tag     preface;
    backbod_lay_tag     body;
    backbod_lay_tag     backm;
    lp_lay_tag          lp;
    index_lay_tag       index;
    xx_lay_tag          ixpgnum;
    xx_lay_tag          ixmajor;
    ixhead_lay_tag      ixhead;
    ix_lay_tag          ix[3];
    toc_lay_tag         toc;
    tocpgnum_lay_tag    tocpgnum;
    tochx_lay_tag       tochx[7];
    figlist_lay_tag     figlist;
    tocpgnum_lay_tag    flpgnum;
    titlep_lay_tag      titlep;
    title_lay_tag       title;
    docnum_lay_tag      docnum;
    date_lay_tag        date;
    author_lay_tag      author;
    address_lay_tag     address;
    aline_lay_tag       aline;
 
    /***********************************************************************/
    /*  The following letter format only layout tags are not supported     */
    /*  and not implemented as the letter format is not used               */
    /***********************************************************************/
 
//  from_lay_tag        from;
//  to_lay_tag          to;
//  attn_lay_tag        attn;
//  subject_lay_tag     subject;
//  letdate_lay_tag     letdate;
//  open_lay_tag        open;
//  close_lay_tag       close;
//  eclose_lay_tag      eclose;
//  distrib_tag         distrib;
 
    appendix_lay_tag    appendix;
    sl_lay_tag          sl;
    ol_lay_tag          ol;
    ul_lay_tag          ul;
    dl_lay_tag          dl;
    gl_lay_tag          gl;
    banner_lay_tag  *   banner;
 
} layout_data;
 
#ifdef __WATCOMC__
#pragma enable_message( 128 );// reenable: Warning! W128: 3 padding byte(s) added
#endif
 
 
 
/***************************************************************************/
/*  parameter structure for parsing layout attributes name + value         */
/***************************************************************************/
 
typedef struct att_args {
    char    *   start[2];
    int         len[2];
    bool        quoted;                 // only for value
} att_args;
 
 
#endif  /* GTYPELAY_H_INCLUDED */
