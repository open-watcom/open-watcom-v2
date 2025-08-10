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
* Description:  wgml type definitions for layout data   :LAYOUT ... :eLAYOUT
*
****************************************************************************/

#ifndef GTYPELAY_H_INCLUDED
#define GTYPELAY_H_INCLUDED

/***************************************************************************/
/*  Layout attributes as enum list                                         */
/*    these are used in LAYOUT tag processing to control the attributes    */
/***************************************************************************/

typedef enum lay_att {
    #define pick( name, funci, funco, result ) e_##name,
    #include "glayutil.h"
    #undef pick
} lay_att;

/***************************************************************************/
/*  definitions for place for :BANNER and :FIG tag                         */
/***************************************************************************/

typedef enum ban_place {
    #define pick(text,en)   en,
    #include "bplaces.h"
    #undef pick
    max_place
} ban_place;

/***************************************************************************/
/*  definitions for docsect for :BANNER tag                                */
/*     document sections and :Hx tags                                      */
/***************************************************************************/

typedef enum ban_docsect {
    #define pick(ban,gml,text,len)   ban,
    #include "bdocsect.h"
    #undef pick
    max_ban
} ban_docsect;

/***************************************************************************/
/*  document sections for banner definition                                */
/***************************************************************************/

typedef struct  ban_sections {
    const char      *name;
    ban_docsect     type;
} ban_sections;

/***************************************************************************/
/*  definitions for frame   :FIG tag and others                            */
/***************************************************************************/
#define xx_str          char
#define STRBLK_SIZE     120             // max string length in layout TBD
                                        // no value found in documentation
typedef enum def_frame_type {
    FRAME_none,
    FRAME_box,
    FRAME_rule,
    FRAME_char
} def_frame_type;

typedef struct def_frame {
    def_frame_type      type;
    xx_str              string[STRBLK_SIZE + 1];
} def_frame;

/***************************************************************************/
/*  definitions for :Hx tag number form and page position                  */
/*                                                                         */
/***************************************************************************/

typedef enum num_form {
    FORM_none,
    FORM_prop,
    FORM_new
} num_form;

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
    page_pos        page_position;      // enum
    font_number     font;               // non negative integer
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
    page_pos        page_position;      // enum
    font_number     font;               // non negative integer
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
    unsigned        def_gutter;         // gutter value as a number
    su              gutter;             // horizontal space unit
    su              binding;            // horizontal space unit
    text_space      spacing;            // positive integer
    int8_t          columns;            // positive integer
    font_number     font;               // non-negative integer
    bool            justify;            // yes / no  -> bool
    char            input_esc;          // none or quoted char
} default_lay_tag;


/***************************************************************************/
/*  :WIDOW   layout tag data                                               */
/***************************************************************************/

typedef struct widow_lay_tag {
    uint16_t        threshold;          // matches wgml 4.0
} widow_lay_tag;


/***************************************************************************/
/*  :FN       Layout tag data                                              */
/***************************************************************************/

typedef struct fn_lay_tag {
    su              line_indent;        // horizontal space unit
    su              align;              // horizontal space unit
    su              pre_lines;          // vertical space unit
    su              skip;               // vertical space unit
    text_space      spacing;            // positive integer
    font_number     font;               // non-negative integer
    font_number     number_font;        // non-negative integer
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
    text_space      spacing;            // positive integer
    font_number     font;               // non-negative integer
    ban_place       default_place;      // special enum
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
    text_space      spacing;            // positive integer
    font_number     font;               // non-negative integer
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
    text_space      spacing;            // positive integer
    xx_str          string[STRBLK_SIZE + 1];    // special string
    xx_str          text[STRBLK_SIZE + 1];      // special string
    xx_str          spaces[STRBLK_SIZE + 1];    // special string
} note_lay_tag;


/***************************************************************************/
/*  :APPENDIX Layout tag data                                              */
/***************************************************************************/

typedef struct appendix_lay_tag {
    xx_str          string[STRBLK_SIZE + 1];    // special string
    bool            page_reset;                 // yes, no -> bool
    int8_t          columns;                    // non-negative integer
    page_ej         section_eject;              // enum for yes, no, odd, even
} appendix_lay_tag;


/***************************************************************************/
/*  :H0 - :H6 Layout tag data                                              */
/*  Expanded to include section heading data common to Hn tags             */
/*  and to support APPENDIX                                                */
/***************************************************************************/

typedef struct hx_head_lay_tag {        // attributes common to Hx and APPENDIX
    su              align;              // horizontal space unit
    su              indent;             // horizontal space unit
    su              pre_skip;           // vertical space unit
    font_number     number_font;        // non-negative integer
    case_t          hd_case;            // lower, upper, mixed
    num_form        number_form;        // special enum ( none, prop, new )
    num_style       number_style;       // special enum
    page_ej         page_eject;         // enum for yes, no, odd, even
    page_pos        line_position;      // special enum (left, right, center)
    bool            display_heading;    // yes, no -> bool
    bool            line_break;         // yes, no -> bool
    bool            number_reset;       // yes, no -> bool
} hx_head_lay_tag;

typedef struct hx_sect_lay_tag {        // attributes common to Hx and all Section tags
    su              post_skip;          // vertical space unit
    su              pre_top_skip;       // vertical space unit
    text_space      spacing;            // positive integer
    font_number     text_font;          // non-negative integer
    bool            header;             // yes, no -> bool
} hx_sect_lay_tag;

typedef struct hx_lay_tag {
    int8_t          group;              // 0 - 9
    hx_head_lay_tag hx_head[HDS_abstract];
    hx_sect_lay_tag hx_sect[HDS_max];
} hx_lay_tag;


/***************************************************************************/
/*  :HEADING Layout tag data                                               */
/***************************************************************************/

typedef struct heading_lay_tag {
    char            delim;
    bool            stop_eject;         // yes, no -> bool
    bool            para_indent;        // yes, no -> bool
    uint16_t        threshold;          // per wgml 4.0
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
    text_space      spacing;            // positive integer
    font_number     font;               // non-negative integer
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
    su              pre_lines;                  // vertical space unit
    font_number     font;                       // non-negative integer
    xx_str          string[STRBLK_SIZE + 1];    // special string
    font_number     string_font;                // non-negative integer
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
    su              left_adjust;                    // horizontal space unit
    su              right_adjust;                   // horizontal space unit
    su              skip;                           // vertical space unit
    text_space      spacing;                        // positive integer
    int8_t          columns;                        // positive integer
    int8_t          toc_levels;                     // non-negative integer
    xx_str          fill_string[STRBLK_SIZE + 1];   // special string
} figlist_lay_tag;

/***************************************************************************/
/*  :FLPGNUM        Layout tag data                                        */
/***************************************************************************/

typedef struct flpgnum_lay_tag {
    su              size;                           // horizontal space unit
    font_number     font;                           // non-negative integer
    xx_str          fill_string[STRBLK_SIZE + 1];   // special string
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
    xx_str          date_form[STRBLK_SIZE + 1]; // special string
    su              left_adjust;                // horizontal space unit
    su              right_adjust;               // horizontal space unit
    su              pre_skip;                   // vertical space unit
    page_pos        page_position;              // enum
    font_number     font;                       // non negative integer
} date_lay_tag;

/***************************************************************************/
/*  :DOCNUM  Layout tag data                                               */
/***************************************************************************/

typedef struct docnum_lay_tag {
    su              left_adjust;                // horizontal space unit
    su              right_adjust;               // horizontal space unit
    su              pre_skip;                   // vertical space unit
    xx_str          string[STRBLK_SIZE + 1];    // special string
    page_pos        page_position;              // special enum (left, right, center)
    font_number     font;                       // non-negative integer
} docnum_lay_tag;

/***************************************************************************/
/*  :ABSTRACT and :PREFACE  Layout tag data                                */
/***************************************************************************/

typedef struct abspref_lay_tag {
    xx_str          string[STRBLK_SIZE + 1];    // special string
    bool            page_reset;                 // yes, no -> bool
    int8_t          columns;                    // positive integer
    page_ej         page_eject;                 // enum for yes, no, odd, even
} abspref_lay_tag;

/***************************************************************************/
/*  :BACKM and :BODY  Layout tag data                                      */
/***************************************************************************/

typedef struct backbod_lay_tag {
    xx_str          string[STRBLK_SIZE + 1];    // special string
    bool            page_reset;                 // yes, no -> bool
    int8_t          columns;                    // positive integer
    page_ej         page_eject;                 // enum for yes, no, odd, even
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
    text_space      spacing;            // positive integer
} lp_lay_tag;

/***************************************************************************/
/*  :INDEX          Layout tag data                                        */
/***************************************************************************/

typedef struct index_lay_tag {
    su              left_adjust;        // horizontal space unit
    su              right_adjust;       // horizontal space unit
    xx_str          index_string[STRBLK_SIZE + 1];      // special string
    xx_str          see_string[STRBLK_SIZE + 1];        // special string
    xx_str          see_also_string[STRBLK_SIZE + 1];   // special string
    int8_t          columns;            // positive integer
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
    def_frame       frame;              // special
    font_number     font;               // non-negative integer
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
    xx_str          index_delim[STRBLK_SIZE + 1];  // special string
    font_number     font;               // non-negative integer
    font_number     string_font;        // non-negative integer
} ix_lay_tag;

/***************************************************************************/
/*  :TITLE   Layout tag data                                               */
/***************************************************************************/

typedef struct title_lay_tag {
    su              left_adjust;        // horizontal space unit
    su              right_adjust;       // horizontal space unit
    su              pre_top_skip;       // vertical space unit
    su              skip;               // vertical space unit
    page_pos        page_position;      // special enum (left, right, center)
    font_number     font;               // non-negative integer
} title_lay_tag;

/***************************************************************************/
/*  :TITLEP  Layout tag data                                               */
/***************************************************************************/

typedef struct titlep_lay_tag {
    text_space      spacing;            // positive integer
    int8_t          columns;            // positive integer
} titlep_lay_tag;

/***************************************************************************/
/*  :TOC            Layout tag data                                        */
/***************************************************************************/

typedef struct toc_lay_tag {
    su              left_adjust;        // horizontal space unit
    su              right_adjust;       // horizontal space unit
    text_space      spacing;            // positive integer
    int8_t          columns;            // positive integer
    int8_t          toc_levels;         // non-negative integer
    xx_str          fill_string[STRBLK_SIZE + 1];  // special string
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

typedef struct sl_lay_level {
    struct sl_lay_level *   next;       // next-level sl tag
    su              left_indent;        // horizontal space unit
    su              right_indent;       // horizontal space unit
    su              pre_skip;           // vertical space unit
    su              skip;               // vertical space unit
    su              post_skip;          // vertical space unit
    text_space      spacing;            // positive integer
    font_number     font;               // non-negative integer
    int8_t          level;              // level of this tag
} sl_lay_level;

typedef struct sl_lay_tag {
    struct sl_lay_level *   first;      // first-level sl tag
    uint8_t                 max_level;  // maximum level
} sl_lay_tag;

/***************************************************************************/
/*  :OL             Layout tag data                                        */
/***************************************************************************/

typedef struct ol_lay_level {
    struct ol_lay_level *   next;       // next-level ol tag
    su              left_indent;        // horizontal space unit
    su              right_indent;       // horizontal space unit
    su              pre_skip;           // vertical space unit
    su              skip;               // vertical space unit
    su              post_skip;          // vertical space unit
    su              align;              // horizontal space unit
    text_space      spacing;            // positive integer
    font_number     font;               // non-negative integer
    num_style       number_style;       // enum special
    font_number     number_font;        // non-negative integer
    int8_t          level;              // level of this tag
} ol_lay_level;

typedef struct ol_lay_tag {
    struct ol_lay_level *   first;      // first-level ol tag
    uint8_t                 max_level;  // maximum level
} ol_lay_tag;

/***************************************************************************/
/*  :UL             Layout tag data                                        */
/***************************************************************************/

typedef struct ul_lay_level {
    struct ul_lay_level *   next;       // next-level ul tag
    su              left_indent;        // horizontal space unit
    su              right_indent;       // horizontal space unit
    su              pre_skip;           // vertical space unit
    su              skip;               // vertical space unit
    su              post_skip;          // vertical space unit
    su              align;              // horizontal space unit
    text_space      spacing;            // positive integer
    font_number     font;               // non-negative integer
    char            bullet;             // char
    bool            bullet_translate;   // yes, no -> bool
    font_number     bullet_font;        // non-negative integer
    int8_t          level;              // level of this tag
} ul_lay_level;

typedef struct ul_lay_tag {
    struct ul_lay_level *   first;      // first-level ul tag
    uint8_t                 max_level;  // maximum level
} ul_lay_tag;

/***************************************************************************/
/*  :DL             Layout tag data                                        */
/***************************************************************************/

typedef struct dl_lay_level {
    struct dl_lay_level *   next;       // next-level dl tag
    su              left_indent;        // horizontal space unit
    su              right_indent;       // horizontal space unit
    su              pre_skip;           // vertical space unit
    su              skip;               // vertical space unit
    su              post_skip;          // vertical space unit
    su              align;              // horizontal space unit
    text_space      spacing;            // positive integer
    bool            line_break;         // yes, no -> bool
    int8_t          level;              // level of this tag
} dl_lay_level;

typedef struct dl_lay_tag {
    struct dl_lay_level *   first;      // first-level dl tag
    uint8_t                 max_level;  // maximum level
} dl_lay_tag;

/***************************************************************************/
/*  :GL             Layout tag data                                        */
/***************************************************************************/

typedef struct gl_lay_level {
    struct gl_lay_level *   next;       // next-level gl tag
    su              left_indent;        // horizontal space unit
    su              right_indent;       // horizontal space unit
    su              pre_skip;           // vertical space unit
    su              skip;               // vertical space unit
    su              post_skip;          // vertical space unit
    su              align;              // horizontal space unit
    text_space      spacing;            // positive integer
    char            delim;              // delimiter char
    int8_t          level;              // level of this tag
} gl_lay_level;

typedef struct gl_lay_tag {
    struct gl_lay_level *   first;      // first-level sl tag
    uint8_t                 max_level;  // maximum level
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

typedef struct reg_content {
    content_enum    content_type;
    xx_str          string[STRBLK_SIZE + 1];
} reg_content;

typedef struct script_ban_reg {         // for script format region
    unsigned        len;                // split into sub-fields
    char            *string;
} script_ban_reg;

typedef struct final_reg_content {      // final fully-preprocessed content
    unsigned        size;               // suitable for final trim and output
    unsigned        hoffset;
    char            *string;
} final_reg_content;

typedef uint8_t         refnum_type;

/***************************************************************************/
/*  :BANREGION      Layout tag data                                        */
/***************************************************************************/

typedef struct region_lay_tag {
    struct  region_lay_tag  *   next;       // next banner region
    unsigned            reg_indent;         // value of 'indent' in base units
    unsigned            reg_hoffset;        // value of 'hoffset' in base units
    unsigned            reg_width;          // value of 'width' in base units
    unsigned            reg_voffset;        // value of 'voffset' in base units
    unsigned            reg_depth;          // value of 'depth' in base units
    space_units         reg_h_type;         // type of reg_hoffset: left, middle, right
    su                  indent;             // horizontal space unit or keywords
    su                  hoffset;            // horizontal space unit or keywords
    su                  width;              // horizontal space unit or extend
    su                  voffset;            // vertical space unit
    su                  depth;              // vertical space unit
    font_number         font;               // non negative integer
    refnum_type         refnum;             // positive integer
    page_pos            region_position;    // special enum
    reg_pour            pouring;            // special enum
    script_ban_reg      script_region[3];   // speed up processing if script_format
                                            // will be constructed from contents
    reg_content         contents;           // what is in the region
    final_reg_content   final_content[3];   // fully resolved content from contents or script_region
    bool                script_format;      // yes no -> bool
} region_lay_tag;

/***************************************************************************/
/*  banner region group                                                    */
/*  Note: each group corresponds to one vertical position in the banner    */
/*        plus the line_height corresponding to the region font            */
/*        this puts regions with fonts with different line_heights into    */
/*        different groups, ultimately sorting the doc_elements into the   */
/*        order needed to output them moving strictly down the page        */
/***************************************************************************/

typedef struct ban_reg_group {
    struct  ban_reg_group       *   next;       // next banner region group
            region_lay_tag      *   first;      // first BANREGION
            unsigned                voffset;    // value of 'voffset' in base units
            unsigned                line_height;// line height (in base units) of the region font
            unsigned                max_depth;  // largest value of 'depth' in base units
} ban_reg_group;

/***************************************************************************/
/*  :BANNER         Layout tag data                                        */
/***************************************************************************/

typedef struct banner_lay_tag {
    struct banner_lay_tag   *   next;       // next banner
    region_lay_tag          *   region;     // region list (in refnum order)
    ban_reg_group           *   by_line;    // group list (in vertical position order)
    unsigned        ban_left_adjust;        // value of 'left_adjust' in base units
    unsigned        ban_right_adjust;       // value of 'right_adjust' in base units
    unsigned        ban_depth;              // value of 'depth' in base units
    refnum_type     next_refnum;            // next expected refnum value
    su              left_adjust;            // horizontal space unit
    su              right_adjust;           // horizontal space unit
    su              depth;                  // vertical space unit
    ban_place       place;                  // special enum
    ban_docsect     docsect;                // special enum
    content_enum    style;                  // page number style defined by banner, if any
//  ban_place       refplace;               // special enum no need to store
//  ban_docsect     refdoc;                 // special enum no need to store
} banner_lay_tag;



/***************************************************************************/
/*  Layout data                                             TBD            */
/*  sequence of definitions as seen by :CONVERT output                     */
/***************************************************************************/

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
    hx_lay_tag          hx;
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
    tochx_lay_tag       tochx[HLVL_MAX];
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

#endif  /* GTYPELAY_H_INCLUDED */
