/****************************************************************************
*
*                            Open Watcom Project
*
*  Copyright (c) 2004-2008 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  WGML layout attribute definitions
*                    and corresponding utility function definitions
*                    to be completed                                TBD
****************************************************************************/

#ifndef pick
    #error macro pick not defined
#endif
//      attribute                 routinename           result type
//      name               input            output

//pick( dummy_zero,        NULL,            NULL,            int )// dummy entry

  pick( abstract_string,   i_xx_string,     o_xx_string,     xx_str          )
  pick( align,             i_space_unit,    o_space_unit,    su              )
  pick( appendix_string,   i_xx_string,     o_xx_string,     xx_str          )
  pick( backm_string,      i_xx_string,     o_xx_string,     xx_str          )
  pick( banner_place,      i_place,         o_place,         bf_place        )
  pick( binding,           i_space_unit,    o_space_unit,    su              )
  pick( body_string,       i_xx_string,     o_xx_string,     xx_str          )
  pick( bullet,            i_char,          o_char,          char            )
  pick( bullet_font,       i_font_number,   o_font_number,   font_number     )
  pick( bullet_translate,  i_yes_no,        o_yes_no,        bool            )
  pick( date_form,         i_date_form,     o_date_form,     xx_str          )
  pick( case,              i_case,          o_case,          case_t          )
  pick( columns,           i_int8,          o_int8,          int8_t          )
  pick( contents,          i_content,       o_content,       content         )
  pick( default_frame,     i_default_frame, o_default_frame, def_frame       )
  pick( default_place,     i_place,         o_place,         bf_place        )
  pick( delim,             i_char,          o_char,          char            )
  pick( depth,             i_space_unit,    o_space_unit,    su              )
  pick( display_heading,   i_yes_no,        o_yes_no,        bool            )
  pick( display_in_toc,    i_yes_no,        o_yes_no,        bool            )
  pick( docsect,           i_docsect,       o_docsect,       ban_docsect     )
  pick( docnum_string,     i_xx_string,     o_xx_string,     xx_str          )
  pick( figcap_string,     i_xx_string,     o_xx_string,     xx_str          )
  pick( fill_string,       i_xx_string,     o_xx_string,     xx_str          )
  pick( font,              i_font_number,   o_font_number,   font_number     )
  pick( frame,             i_frame,         o_frame,         bool            )
  pick( group,             i_int8,          o_int8,          int8_t          )
  pick( gutter,            i_space_unit,    o_space_unit,    su              )
  pick( header,            i_yes_no,        o_yes_no,        bool            )
  pick( hoffset,           i_space_unit,    o_space_unit,    su              )
  pick( indent,            i_space_unit,    o_space_unit,    su              )
  pick( index_delim,       i_xx_string,     o_xx_string,     xx_str          )
  pick( index_string,      i_xx_string,     o_xx_string,     xx_str          )
  pick( input_esc,         i_char,          o_char,          char            )
  pick( ixhead_frame,      i_default_frame, o_default_frame, def_frame       )
  pick( justify,           i_yes_no,        o_yes_no,        bool            )
  pick( left_adjust,       i_space_unit,    o_space_unit,    su              )
  pick( left_indent,       i_space_unit,    o_space_unit,    su              )
  pick( left_margin,       i_space_unit,    o_space_unit,    su              )
  pick( level,             i_int8,          o_int8,          int8_t          )
  pick( line_break,        i_yes_no,        o_yes_no,        bool            )
  pick( line_indent,       i_space_unit,    o_space_unit,    su              )
  pick( line_left,         i_space_unit,    o_space_unit,    su              )
  pick( max_group,         i_int8,          o_int8,          int8_t          )
  pick( note_string,       i_xx_string,     o_xx_string,     xx_str          )
  pick( number_font,       i_font_number,   o_font_number,   font_number     )
  pick( number_form,       i_number_form,   o_number_form,   num_form        )
  pick( number_reset,      i_yes_no,        o_yes_no,        bool            )
  pick( number_style,      i_number_style,  o_number_style,  num_style       )
  pick( page_eject,        i_page_eject,    o_page_eject,    page_ej         )
  pick( page_position,     i_page_position, o_page_position, page_pos        )
  pick( page_reset,        i_yes_no,        o_yes_no,        bool            )
  pick( para_indent,       i_yes_no,        o_yes_no,        bool            )
  pick( place,             i_place,         o_place,         bf_place        )
  pick( preface_string,    i_xx_string,     o_xx_string,     xx_str          )
  pick( pre_lines,         i_space_unit,    o_space_unit,    su              )
  pick( pre_skip,          i_space_unit,    o_space_unit,    su              )
  pick( pre_top_skip,      i_space_unit,    o_space_unit,    su              )
  pick( post_skip,         i_space_unit,    o_space_unit,    su              )
  pick( pouring,           i_pouring,       o_pouring,       reg_pour        )
  pick( refdoc,            i_docsect,       o_docsect,       ban_docsect     )
  pick( refnum,            i_int8,          o_int8,          int8_t          )
  pick( refplace,          i_place,         o_place,         bf_place        )
  pick( region_position,   i_page_position, o_page_position, page_pos        )
  pick( right_adjust,      i_space_unit,    o_space_unit,    su              )
  pick( right_indent,      i_space_unit,    o_space_unit,    su              )
  pick( right_margin,      i_space_unit,    o_space_unit,    su              )
  pick( script_format,     i_yes_no,        o_yes_no,        bool            )
  pick( section_eject,     i_page_eject,    o_page_eject,    page_ej         )
  pick( see_string,        i_xx_string,     o_xx_string,     xx_str          )
  pick( see_also_string,   i_xx_string,     o_xx_string,     xx_str          )
  pick( size,              i_space_unit,    o_space_unit,    su              )
  pick( skip,              i_space_unit,    o_space_unit,    su              )
  pick( spacing,           i_int8,          o_int8,          int8_t          )
  pick( stop_eject,        i_yes_no,        o_yes_no,        bool            )
  pick( string_font,       i_font_number,   o_font_number,   font_number     )
  pick( threshold,         i_uint8,         o_uint8,         uint8_t         )
  pick( toc_levels,        i_int8,          o_int8,          int8_t          )
  pick( top_margin,        i_space_unit,    o_space_unit,    su              )
  pick( voffset,           i_space_unit,    o_space_unit,    su              )
  pick( width,             i_space_unit,    o_space_unit,    su              )
  pick( wrap_indent,       i_space_unit,    o_space_unit,    su              )
//pick( xx_string,         i_xx_string,     o_xx_string,     xx_str          )

//pick( dummy_max,         NULL,            NULL,            int )// dummy entry

#undef  pick
