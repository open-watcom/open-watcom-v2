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
* Description: WGML implement :DL :GL :OL :SL :UL tags for LAYOUT processing
*
*
*                              only level 1 is supported            TBD
*
****************************************************************************/

#define __STDC_WANT_LIB_EXT1__  1      /* use safer C library              */

#include "wgml.h"
#include "gvars.h"

/***************************************************************************/
/*   :DL       attributes                                                  */
/***************************************************************************/
const   lay_att     dl_att[10] =
    { e_level, e_left_indent, e_right_indent, e_pre_skip, e_skip,
      e_spacing, e_post_skip, e_align, e_line_break, e_dummy_zero };

/***************************************************************************/
/*   :GL       attributes                                                  */
/***************************************************************************/
const   lay_att     gl_att[10] =
    { e_level, e_left_indent, e_right_indent, e_pre_skip, e_skip,
      e_spacing, e_post_skip, e_align, e_delim, e_dummy_zero };

/***************************************************************************/
/*   :OL       attributes                                                  */
/***************************************************************************/
const   lay_att     ol_att[12] =
    { e_level, e_left_indent, e_right_indent, e_pre_skip, e_skip,
      e_spacing, e_post_skip, e_font, e_align, e_number_style, e_number_font,
      e_dummy_zero };

/***************************************************************************/
/*   :SL       attributes                                                  */
/***************************************************************************/
const   lay_att     sl_att[9] =
    { e_level, e_left_indent, e_right_indent, e_pre_skip, e_skip,
      e_spacing, e_post_skip, e_font, e_dummy_zero };

/***************************************************************************/
/*   :UL       attributes                                                  */
/***************************************************************************/
const   lay_att     ul_att[13] =
    { e_level, e_left_indent, e_right_indent, e_pre_skip, e_skip,
      e_spacing, e_post_skip, e_font, e_align, e_bullet, e_bullet_translate,
      e_bullet_font, e_dummy_zero };


/********************************************************************************/
/*Define the characteristics of the definition list entity.                     */
/*:DL                                                                           */
/*        level = 1                                                             */
/*        left_indent = 0                                                       */
/*        right_indent = 0                                                      */
/*        pre_skip = 1                                                          */
/*        skip = 1                                                              */
/*        spacing = 1                                                           */
/*        post_skip = 1                                                         */
/*        align = '1i'                                                          */
/*        line_break = no                                                       */
/*                                                                              */
/*level This attribute accepts a positive integer number. If not specified, a   */
/*level value of '1'. is assumed. Each list level is separately               */
/*specified. For example, if two levels of the ordered list are                 */
/*specified, the :dl tag will be specified twice in the layout. When            */
/*some attributes for a new level of a list are not specified, the default      */
/*values for those attributes will be the values of the first level. Since      */
/*list levels may not be skipped, each new level of list must be                */
/*sequentially defined from the last specified level.                           */
/*if there is an ordered, simple, and second ordered list nested                */
/*together in the document, the simple and first ordered list will both         */
/*be from level one, while the last ordered list will be level two. The         */
/*appropriate level number is selected based on the nesting level of a          */
/*particular list type. If a list type is nested beyond the levels              */
/*specified in the layout, the levels are "cycled". For example, if there       */
/*are two levels of ordered list specified in the layout, and there are         */
/*three ordered lists nested, the third level of ordered list will use the      */
/*attributes of the level one ordered list. A fourth nested list would          */
/*use the attributes of the level two.                                          */
/*                                                                              */
/*left_indent This attribute accepts any valid horizontal space unit. The left  */
/*indent value is added to the current left margin. The left margin will        */
/*be reset to its previous value at the end of the definition list.             */
/*                                                                              */
/*right_indent This attribute accepts any valid horizontal space unit. The right*/
/*indent value is subtracted from the current right margin. The right           */
/*margin will be reset to its previous value at the end of the definition       */
/*list.                                                                         */
/*                                                                              */
/*pre_skip This attribute accepts vertical space units. A zero value means that */
/*no lines are skipped. If the skip value is a line unit, it is multiplied      */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for         */
/*more information). The resulting amount of space is skipped before            */
/*the definition list. The pre-skip will be merged with the previous            */
/*document entity's post-skip value. If a pre-skip occurs at the               */
/*beginning of an output page, the pre-skip value has no effect.                */
/*                                                                              */
/*skip This attribute accepts vertical space units. A zero value means that     */
/*no lines are skipped. If the skip value is a line unit, it is multiplied      */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for         */
/*more information). The resulting amount of space is skipped                   */
/*between each item of the definition list.                                     */
/*                                                                              */
/*spacing This attribute accepts a positive integer number. The spacing         */
/*determines the number of blank lines that are output between text             */
/*lines. If the line spacing is two, each text line will take two lines in      */
/*the output. The number of blank lines between text lines will                 */
/*therefore be the spacing value minus one. The spacing attribute               */
/*defines the line spacing within the items of the definition list.             */
/*                                                                              */
/*post_skip This attribute accepts vertical space units. A zero value means that*/
/*no lines are skipped. If the skip value is a line unit, it is multiplied      */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for         */
/*more information). The resulting amount of space is skipped after             */
/*the definition list. The post-skip will be merged with the next               */
/*document entity's pre-skip value. If a post-skip occurs at the end of         */
/*an output page, any remaining part of the skip is not carried over to         */
/*the next output page.                                                         */
/*                                                                              */
/*align This attribute accepts any valid horizontal space unit. The align       */
/*value specifies the amount of space reserved for the definition term.         */
/*After the definition term is produced, the align value is added to the        */
/*current left margin. The left margin will be reset to its previous            */
/*value after the definition list item.                                         */
/*                                                                              */
/*line_break This attribute accepts the keyword values yes and no. If the value */
/*yes is specified, the data description starts a new line after the            */
/*definition term if the length of the term is larger than align value. If      */
/*the value no is specified, the definition term is allowed to intrude          */
/*into the data description area.                                               */
/********************************************************************************/

/*********************************************************************************/
/*Define the characteristics of the glossary list entity.                        */
/*:GL                                                                            */
/*        level = 1                                                              */
/*        left_indent = 0                                                        */
/*        right_indent = 0                                                       */
/*        pre_skip = 1                                                           */
/*        skip = 1                                                               */
/*        spacing = 1                                                            */
/*        post_skip = 1                                                          */
/*        align = 0                                                              */
/*        delim = ':'                                                            */
/*                                                                               */
/*level This attribute accepts a positive integer number. If not specified, a    */
/*level value of '1'. is assumed. Each list level is separately                  */
/*specified. For example, if two levels of the ordered list are                  */
/*specified, the :gl tag will be specified twice in the layout. When             */
/*some attributes for a new level of a list are not specified, the default       */
/*values for those attributes will be the values of the first level. Since       */
/*list levels may not be skipped, each new level of list must be                 */
/*sequentially defined from the last specified level.                            */
/*if there is an ordered, simple, and second ordered list nested                 */
/*together in the document, the simple and first ordered list will both          */
/*be from level one, while the last ordered list will be level two. The          */
/*appropriate level number is selected based on the nesting level of a           */
/*particular list type. If a list type is nested beyond the levels               */
/*specified in the layout, the levels are "cycled". For example, if there        */
/*are two levels of ordered list specified in the layout, and there are          */
/*three ordered lists nested, the third level of ordered list will use the       */
/*attributes of the level one ordered list. A fourth nested list would           */
/*use the attributes of the level two.                                           */
/*                                                                               */
/*left_indent This attribute accepts any valid horizontal space unit. The left   */
/*indent value is added to the current left margin. The left margin will         */
/*be reset to its previous value at the end of the glossary list.                */
/*right_indent This attribute accepts any valid horizontal space unit. The right */
/*indent value is subtracted from the current right margin. The right            */
/*margin will be reset to its previous value at the end of the glossary          */
/*list.                                                                          */
/*                                                                               */
/*pre_skip This attribute accepts vertical space units. A zero value means that  */
/*no lines are skipped. If the skip value is a line unit, it is multiplied       */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for          */
/*more information). The resulting amount of space is skipped before             */
/*the glossary list. The pre-skip will be merged with the previous               */
/*document entity's post-skip value. If a pre-skip occurs at the                 */
/*beginning of an output page, the pre-skip value has no effect.                 */
/*                                                                               */
/*skip This attribute accepts vertical space units. A zero value means that      */
/*no lines are skipped. If the skip value is a line unit, it is multiplied       */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for          */
/*more information). The resulting amount of space is skipped                    */
/*between each item of the glossary list.                                        */
/*                                                                               */
/*spacing This attribute accepts a positive integer number. The spacing          */
/*determines the number of blank lines that are output between text              */
/*lines. If the line spacing is two, each text line will take two lines in       */
/*the output. The number of blank lines between text lines will                  */
/*therefore be the spacing value minus one. The spacing attribute                */
/*defines the line spacing within the items of the glossary list.                */
/*                                                                               */
/*post_skip This attribute accepts vertical space units. A zero value means that */
/*no lines are skipped. If the skip value is a line unit, it is multiplied       */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for          */
/*more information). The resulting amount of space is skipped after              */
/*the glossary list. The post-skip will be merged with the next                  */
/*document entity's pre-skip value. If a post-skip occurs at the end of          */
/*an output page, any remaining part of the skip is not carried over to          */
/*the next output page.                                                          */
/*                                                                               */
/*align This attribute accepts any valid horizontal space unit. The align        */
/*value specifies the amount of space reserved for the glossary term.            */
/*After the glossary term is produced, the align value is added to the           */
/*current left margin. The left margin will be reset to its previous             */
/*value after the glossary list item.                                            */
/*delim The quoted character value is used to separate the glossary term         */
/*from the glossary description.                                                 */
/*********************************************************************************/

/*********************************************************************************/
/*Define the characteristics of the ordered list entity.                         */
/*:OL                                                                            */
/*        level = 1                                                              */
/*        left_indent = 0                                                        */
/*        right_indent = 0                                                       */
/*        pre_skip = 1                                                           */
/*        skip = 1                                                               */
/*        spacing = 1                                                            */
/*        post_skip = 1                                                          */
/*        font = 0                                                               */
/*        align = '0.4i'                                                         */
/*        number_style = hd                                                      */
/*        number_font = 0                                                        */
/*                                                                               */
/*level This attribute accepts a positive integer number. If not specified, a    */
/*level value of '1'. is assumed. Each list level is separately                  */
/*specified. For example, if two levels of the ordered list are                  */
/*specified, the :ol tag will be specified twice in the layout. When             */
/*some attributes for a new level of a list are not specified, the default       */
/*values for those attributes will be the values of the first level. Since       */
/*list levels may not be skipped, each new level of list must be                 */
/*sequentially defined from the last specified level.                            */
/*if there is an ordered, simple, and second ordered list nested                 */
/*together in the document, the simple and first ordered list will both          */
/*be from level one, while the last ordered list will be level two. The          */
/*appropriate level number is selected based on the nesting level of a           */
/*particular list type. If a list type is nested beyond the levels               */
/*specified in the layout, the levels are "cycled". For example, if there        */
/*are two levels of ordered list specified in the layout, and there are          */
/*three ordered lists nested, the third level of ordered list will use the       */
/*attributes of the level one ordered list. A fourth nested list would           */
/*use the attributes of the level two.                                           */
/*                                                                               */
/*left_indent This attribute accepts any valid horizontal space unit. The left   */
/*indent value is added to the current left margin. The left margin will         */
/*be reset to its previous value at the end of the ordered list.                 */
/*right_indent This attribute accepts any valid horizontal space unit. The right */
/*indent value is subtracted from the current right margin. The right            */
/*margin will be reset to its previous value at the end of the ordered           */
/*list.                                                                          */
/*                                                                               */
/*pre_skip This attribute accepts vertical space units. A zero value means that  */
/*no lines are skipped. If the skip value is a line unit, it is multiplied       */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for          */
/*more information). The resulting amount of space is skipped before             */
/*the ordered list. The pre-skip will be merged with the previous                */
/*document entity's post-skip value. If a pre-skip occurs at the                 */
/*beginning of an output page, the pre-skip value has no effect.                 */
/*                                                                               */
/*skip This attribute accepts vertical space units. A zero value means that      */
/*no lines are skipped. If the skip value is a line unit, it is multiplied       */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for          */
/*more information). The resulting amount of space is skipped                    */
/*between list items.                                                            */
/*                                                                               */
/*spacing This attribute accepts a positive integer number. The spacing          */
/*determines the number of blank lines that are output between text              */
/*lines. If the line spacing is two, each text line will take two lines in       */
/*the output. The number of blank lines between text lines will                  */
/*therefore be the spacing value minus one. The spacing attribute                */
/*defines the line spacing within the list item.                                 */
/*                                                                               */
/*post_skip This attribute accepts vertical space units. A zero value means that */
/*no lines are skipped. If the skip value is a line unit, it is multiplied       */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for          */
/*more information). The resulting amount of space is skipped after              */
/*the ordered list. The post-skip will be merged with the next                   */
/*document entity's pre-skip value. If a post-skip occurs at the end of          */
/*an output page, any remaining part of the skip is not carried over to          */
/*the next output page.                                                          */
/*                                                                               */
/*font This attribute accepts a non-negative integer number. If a font           */
/*number is used for which no font has been defined, WATCOM                      */
/*Script/GML will use font zero. The font numbers from zero to three             */
/*correspond directly to the highlighting levels specified by the                */
/*highlighting phrase GML tags. The font attribute defines the font of           */
/*the ordered list text. The font value is linked to the left_indent,            */
/*right_indent, pre_skip, post_skip and skip attributes (see "Font               */
/*Linkage" on page 77).                                                          */
/*                                                                               */
/*align This attribute accepts any valid horizontal space unit. The align        */
/*value specifies the amount of space reserved for the list item                 */
/*number. After the list item number is produced, the align value is             */
/*added to the current left margin. The left margin will be reset to its         */
/*previous value after the list item.                                            */
/*                                                                               */
/*number_style This attribute sets the number style of the list item number.     */
/*(See "Number Style" on page 121).                                              */
/*                                                                               */
/*number_font This attribute accepts a non-negative integer number. If a font    */
/*number is used for which no font has been defined, WATCOM                      */
/*Script/GML will use font zero. The font numbers from zero to three             */
/*correspond directly to the highlighting levels specified by the                */
/*highlighting phrase GML tags. The number font attribute defines                */
/*the font of the list item number. The font value is linked to the align        */
/*attibute (see "Font Linkage" on page 77).                                      */
/*********************************************************************************/

/********************************************************************************/
/*Define the characteristics of the simple list entity.                         */
/*:SL                                                                           */
/*        level = 1                                                             */
/*        left_indent = 0                                                       */
/*        right_indent = 0                                                      */
/*        pre_skip = 1                                                          */
/*        skip = 1                                                              */
/*        spacing = 1                                                           */
/*        post_skip = 1                                                         */
/*        font = 0                                                              */
/*                                                                              */
/*level This attribute accepts a positive integer number. If not specified, a   */
/*level value of '1'. is assumed. Each list level is separately                 */
/*specified. For example, if two levels of the ordered list are                 */
/*specified, the :sl tag will be specified twice in the layout. When            */
/*some attributes for a new level of a list are not specified, the default      */
/*values for those attributes will be the values of the first level. Since      */
/*list levels may not be skipped, each new level of list must be                */
/*sequentially defined from the last specified level.                           */
/*if there is an ordered, simple, and second ordered list nested                */
/*together in the document, the simple and first ordered list will both         */
/*be from level one, while the last ordered list will be level two. The         */
/*appropriate level number is selected based on the nesting level of a          */
/*particular list type. If a list type is nested beyond the levels              */
/*specified in the layout, the levels are "cycled". For example, if there       */
/*are two levels of ordered list specified in the layout, and there are         */
/*three ordered lists nested, the third level of ordered list will use the      */
/*attributes of the level one ordered list. A fourth nested list would          */
/*use the attributes of the level two.                                          */
/*                                                                              */
/*left_indent This attribute accepts any valid horizontal space unit. The left  */
/*indent value is added to the current left margin. The left margin will        */
/*be reset to its previous value at the end of the simple list.                 */
/*                                                                              */
/*right_indent This attribute accepts any valid horizontal space unit. The right*/
/*indent value is subtracted from the current right margin. The right           */
/*margin will be reset to its previous value at the end of the simple           */
/*list.                                                                         */
/*                                                                              */
/*pre_skip This attribute accepts vertical space units. A zero value means that */
/*no lines are skipped. If the skip value is a line unit, it is multiplied      */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for         */
/*more information). The resulting amount of space is skipped before            */
/*the simple list. The pre-skip will be merged with the previous                */
/*document entity's post-skip value. If a pre-skip occurs at the                */
/*beginning of an output page, the pre-skip value has no effect.                */
/*                                                                              */
/*skip This attribute accepts vertical space units. A zero value means that     */
/*no lines are skipped. If the skip value is a line unit, it is multiplied      */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for         */
/*more information). The resulting amount of space is skipped                   */
/*between list items.                                                           */
/*                                                                              */
/*spacing This attribute accepts a positive integer number. The spacing         */
/*determines the number of blank lines that are output between text             */
/*lines. If the line spacing is two, each text line will take two lines in      */
/*the output. The number of blank lines between text lines will                 */
/*therefore be the spacing value minus one. The spacing attribute               */
/*defines the line spacing within the list items.                               */
/*                                                                              */
/*post_skip This attribute accepts vertical space units. A zero value means that*/
/*no lines are skipped. If the skip value is a line unit, it is multiplied      */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for         */
/*more information). The resulting amount of space is skipped after             */
/*the simple list. The post-skip will be merged with the next                   */
/*document entity's pre-skip value. If a post-skip occurs at the end of         */
/*an output page, any remaining part of the skip is not carried over to         */
/*the next output page.                                                         */
/*                                                                              */
/*font This attribute accepts a non-negative integer number. If a font          */
/*number is used for which no font has been defined, WATCOM                     */
/*Script/GML will use font zero. The font numbers from zero to three            */
/*correspond directly to the highlighting levels specified by the               */
/*highlighting phrase GML tags. The font attribute defines the font of          */
/*the simple list text. The font value is linked to the left_indent,            */
/*right_indent, pre_skip, post_skip and skip attributes (see "Font              */
/*Linkage" on page 77).                                                         */
/********************************************************************************/

/********************************************************************************/
/*Define the characteristics of the unordered list entity.                      */
/*:UL                                                                           */
/*        level = 1                                                             */
/*        left_indent = 0                                                       */
/*        right_indent = 0                                                      */
/*        pre_skip = 1                                                          */
/*        skip = 1                                                              */
/*        spacing = 1                                                           */
/*        post_skip = 1                                                         */
/*        font = 0                                                              */
/*        align = '0.4i'                                                        */
/*        bullet = '*'                                                          */
/*        bullet_translate = yes                                                */
/*        bullet_font = 0                                                       */
/*                                                                              */
/*level This attribute accepts a positive integer number. If not specified, a   */
/*level value of '1'. is assumed. Each list level is separately                 */
/*specified. For example, if two levels of the ordered list are                 */
/*specified, the :ul tag will be specified twice in the layout. When            */
/*some attributes for a new level of a list are not specified, the default      */
/*values for those attributes will be the values of the first level. Since      */
/*list levels may not be skipped, each new level of list must be                */
/*sequentially defined from the last specified level.                           */
/*if( there is an ordered, simple, and second ordered list nested ) {           */
/*together in the document, the simple and first ordered list will both         */
/*be from level one, while the last ordered list will be level two. The         */
/*appropriate level number is selected based on the nesting level of a          */
/*particular list type. If a list type is nested beyond the levels              */
/*specified in the layout, the levels are "cycled". For example, if there       */
/*are two levels of ordered list specified in the layout, and there are         */
/*three ordered lists nested, the third level of ordered list will use the      */
/*attributes of the level one ordered list. A fourth nested list would          */
/*use the attributes of the level two.                                          */
/*                                                                              */
/*left_indent This attribute accepts any valid horizontal space unit. The left  */
/*indent value is added to the current left margin. The left margin will        */
/*be reset to its previous value at the end of the unordered list.              */
/*                                                                              */
/*right_indent This attribute accepts any valid horizontal space unit. The right*/
/*indent value is subtracted from the current right margin. The right           */
/*margin will be reset to its previous value at the end of the unordered        */
/*list.                                                                         */
/*                                                                              */
/*pre_skip This attribute accepts vertical space units. A zero value means that */
/*no lines are skipped. If the skip value is a line unit, it is multiplied      */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for         */
/*more information). The resulting amount of space is skipped before            */
/*the unordered list. The pre-skip will be merged with the previous             */
/*document entity's post-skip value. If a pre-skip occurs at the                */
/*beginning of an output page, the pre-skip value has no effect.                */
/*                                                                              */
/*skip This attribute accepts vertical space units. A zero value means that     */
/*no lines are skipped. If the skip value is a line unit, it is multiplied      */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for         */
/*more information). The resulting amount of space is skipped                   */
/*between list items.                                                           */
/*                                                                              */
/*spacing This attribute accepts a positive integer number. The spacing         */
/*determines the number of blank lines that are output between text             */
/*lines. If the line spacing is two, each text line will take two lines in      */
/*the output. The number of blank lines between text lines will                 */
/*therefore be the spacing value minus one. The spacing attribute               */
/*defines the line spacing within the list items.                               */
/*                                                                              */
/*post_skip This attribute accepts vertical space units. A zero value means that*/
/*no lines are skipped. If the skip value is a line unit, it is multiplied      */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for         */
/*more information). The resulting amount of space is skipped after             */
/*the unordered list. The post-skip will be merged with the next                */
/*document entity's pre-skip value. If a post-skip occurs at the end of         */
/*an output page, any remaining part of the skip is not carried over to         */
/*the next output page.                                                         */
/*                                                                              */
/*font This attribute accepts a non-negative integer number. If a font          */
/*number is used for which no font has been defined, WATCOM                     */
/*Script/GML will use font zero. The font numbers from zero to three            */
/*correspond directly to the highlighting levels specified by the               */
/*highlighting phrase GML tags. The font attribute defines the font of          */
/*the unordered list text. The font value is linked to the left_indent,         */
/*right_indent, pre_skip, post_skip and skip attributes (see "Font              */
/*Linkage" on page 77).                                                         */
/*                                                                              */
/*align This attribute accepts any valid horizontal space unit. The align       */
/*value specifies the amount of space reserved for the list item bullet.        */
/*After the list item bullet is produced, the align value is added to the       */
/*current left margin. The left margin will be reset to its previous            */
/*value after the list item.                                                    */
/*                                                                              */
/*bullet This attribute specifies the single character value which annotates an */
/*unordered list item.                                                          */
/*                                                                              */
/*bullet_translate This attribute accepts the keyword values yes and no.        */
/*If 'yes' is specified, input translation is performed on the annotation       */
/*character.                                                                    */
/*                                                                              */
/*bullet_font This attribute accepts a non-negative integer number. If a font   */
/*number is used for which no font has been defined, WATCOM                     */
/*Script/GML will use font zero. The font numbers from zero to three            */
/*correspond directly to the highlighting levels specified by the               */
/*highlighting phrase GML tags. The bullet_font attribute defines the           */
/*font of the annotation character. The font value is linked to the             */
/*align attibute (see "Font Linkage" on page 77).                               */
/********************************************************************************/


/***************************************************************************/
/*  lay_dl                                                                 */
/***************************************************************************/

void    lay_dl( lay_tag tag )
{
    char            *   p;
    condcode            cc;
    int                 k;
    lay_att             curr;
    att_args            l_args;
    int                 cvterr;

    tag = tag;
    p = scan_start;
    cvterr = false;

    if( !GlobalFlags.firstpass ) {
        scan_start = scan_stop;
        eat_lay_sub_tag();
        return;                         // process during first pass only
    }
    if( ProcFlags.lay_xxx != el_dl ) {
        ProcFlags.lay_xxx = el_dl;
    }
    cc = get_lay_sub_and_value( &l_args );  // get att with value
    while( cc == pos ) {
        cvterr = -1;
        for( k = 0, curr = dl_att[k]; curr > 0; k++, curr = dl_att[k] ) {

            if( !strnicmp( att_names[curr], l_args.start[0], l_args.len[0] ) ) {
                p = l_args.start[1];

                switch( curr ) {
                case   e_level:
                    cvterr = i_int8( p, curr, &layout_work.dl.level );
                    if( layout_work.dl.level != 1) {
                       cvterr = 1;      // only level 1 allowed /supported  TBD
                    }
                    break;
                case   e_left_indent:
                    cvterr = i_space_unit( p, curr,
                                           &layout_work.dl.left_indent );
                    break;
                case   e_right_indent:
                    cvterr = i_space_unit( p, curr,
                                           &layout_work.dl.right_indent );
                    break;
                case   e_pre_skip:
                    cvterr = i_space_unit( p, curr,
                                           &layout_work.dl.pre_skip );
                    break;
                case   e_skip:
                    cvterr = i_space_unit( p, curr,
                                           &layout_work.dl.skip );
                    break;
                case   e_spacing:
                    cvterr = i_int8( p, curr, &layout_work.dl.spacing );
                    break;
                case   e_post_skip:
                    cvterr = i_space_unit( p, curr,
                                           &layout_work.dl.post_skip );
                    break;
                case   e_align:
                    cvterr = i_space_unit( p, curr,
                                           &layout_work.dl.align );
                    break;
                case   e_line_break:
                    cvterr = i_yes_no( p, curr,
                                       &layout_work.dl.line_break );
                    break;
                default:
                    out_msg( "WGML logic error.\n");
                    cvterr = true;
                    break;
                }
                if( cvterr ) {          // there was an error
                    err_count++;
                    g_err( err_att_val_inv );
                    file_mac_info();
                }
                break;                  // break out of for loop
            }
        }
        if( cvterr < 0 ) {
            err_count++;
            g_err( err_att_name_inv );
            file_mac_info();
        }
        cc = get_lay_sub_and_value( &l_args );  // get att with value
    }
    scan_start = scan_stop;
    return;
}

/***************************************************************************/
/*  lay_gl                                                                 */
/***************************************************************************/

void    lay_gl( lay_tag tag )
{
    char            *   p;
    condcode            cc;
    int                 k;
    lay_att             curr;
    att_args            l_args;
    int                 cvterr;

    tag = tag;
    p = scan_start;
    cvterr = false;

    if( !GlobalFlags.firstpass ) {
        scan_start = scan_stop;
        eat_lay_sub_tag();
        return;                         // process during first pass only
    }
    if( ProcFlags.lay_xxx != el_gl ) {
        ProcFlags.lay_xxx = el_gl;
    }
    cc = get_lay_sub_and_value( &l_args );  // get att with value
    while( cc == pos ) {
        cvterr = -1;
        for( k = 0, curr = gl_att[k]; curr > 0; k++, curr = gl_att[k] ) {

            if( !strnicmp( att_names[curr], l_args.start[0], l_args.len[0] ) ) {
                p = l_args.start[1];

                switch( curr ) {
                case   e_level:
                    cvterr = i_int8( p, curr, &layout_work.gl.level );
                    if( layout_work.gl.level != 1) {
                       cvterr = 1;      // only level 1 allowed /supported  TBD
                    }
                    break;
                case   e_left_indent:
                    cvterr = i_space_unit( p, curr,
                                           &layout_work.gl.left_indent );
                    break;
                case   e_right_indent:
                    cvterr = i_space_unit( p, curr,
                                           &layout_work.gl.right_indent );
                    break;
                case   e_pre_skip:
                    cvterr = i_space_unit( p, curr, &layout_work.gl.pre_skip );
                    break;
                case   e_skip:
                    cvterr = i_space_unit( p, curr, &layout_work.gl.skip );
                    break;
                case   e_spacing:
                    cvterr = i_int8( p, curr, &layout_work.gl.spacing );
                    break;
                case   e_post_skip:
                    cvterr = i_space_unit( p, curr,
                                           &layout_work.gl.post_skip );
                    break;
                case   e_align:
                    cvterr = i_space_unit( p, curr, &layout_work.gl.align );
                    break;
                case   e_delim:
                    cvterr = i_char( p, curr, &layout_work.gl.delim );
                    break;
                default:
                    out_msg( "WGML logic error.\n");
                    cvterr = true;
                    break;
                }
                if( cvterr ) {          // there was an error
                    err_count++;
                    g_err( err_att_val_inv );
                    file_mac_info();
                }
                break;                  // break out of for loop
            }
        }
        if( cvterr < 0 ) {
            err_count++;
            g_err( err_att_name_inv );
            file_mac_info();
        }
        cc = get_lay_sub_and_value( &l_args );  // get att with value
    }
    scan_start = scan_stop;
    return;
}

/***************************************************************************/
/*  lay_ol                                                                 */
/***************************************************************************/

void    lay_ol( lay_tag tag )
{
    char            *   p;
    condcode            cc;
    int                 k;
    lay_att             curr;
    att_args            l_args;
    int                 cvterr;

    tag = tag;
    p = scan_start;
    cvterr = false;

    if( !GlobalFlags.firstpass ) {
        scan_start = scan_stop;
        eat_lay_sub_tag();
        return;                         // process during first pass only
    }
    if( ProcFlags.lay_xxx != el_ol ) {
        ProcFlags.lay_xxx = el_ol;
    }
    cc = get_lay_sub_and_value( &l_args );  // get att with value
    while( cc == pos ) {
        cvterr = -1;
        for( k = 0, curr = ol_att[k]; curr > 0; k++, curr = ol_att[k] ) {

            if( !strnicmp( att_names[curr], l_args.start[0], l_args.len[0] ) ) {
                p = l_args.start[1];

                switch( curr ) {
                case   e_level:
                    cvterr = i_int8( p, curr, &layout_work.ol.level );
                    if( layout_work.ol.level != 1) {
                       cvterr = 1;      // only level 1 allowed /supported  TBD
                    }
                    break;
                case   e_left_indent:
                    cvterr = i_space_unit( p, curr,
                                           &layout_work.ol.left_indent );
                    break;
                case   e_right_indent:
                    cvterr = i_space_unit( p, curr,
                                           &layout_work.ol.right_indent );
                    break;
                case   e_pre_skip:
                    cvterr = i_space_unit( p, curr, &layout_work.ol.pre_skip );
                    break;
                case   e_skip:
                    cvterr = i_space_unit( p, curr, &layout_work.ol.skip );
                    break;
                case   e_spacing:
                    cvterr = i_int8( p, curr, &layout_work.ol.spacing );
                    break;
                case   e_post_skip:
                    cvterr = i_space_unit( p, curr,
                                           &layout_work.ol.post_skip );
                    break;
                case   e_font:
                    cvterr = i_font_number( p, curr, &layout_work.ol.font );
                    if( layout_work.ol.font >= wgml_font_cnt ) {
                        layout_work.ol.font = 0;
                    }
                    break;
                case   e_align:
                    cvterr = i_space_unit( p, curr, &layout_work.ol.align );
                    break;
                case   e_number_style:
                    cvterr = i_number_style( p, curr,
                                             &layout_work.ol.number_style );
                    break;
                case   e_number_font:
                    cvterr = i_font_number( p, curr, &layout_work.ol.number_font );
                    if( layout_work.ol.number_font >= wgml_font_cnt ) {
                        layout_work.ol.number_font = 0;
                    }
                    break;
                default:
                    out_msg( "WGML logic error.\n");
                    cvterr = true;
                    break;
                }
                if( cvterr ) {          // there was an error
                    err_count++;
                    g_err( err_att_val_inv );
                    file_mac_info();
                }
                break;                  // break out of for loop
            }
        }
        if( cvterr < 0 ) {
            err_count++;
            g_err( err_att_name_inv );
            file_mac_info();
        }
        cc = get_lay_sub_and_value( &l_args );  // get att with value
    }
    scan_start = scan_stop;
    return;
}

/***************************************************************************/
/*  lay_sl                                                                 */
/***************************************************************************/

void    lay_sl( lay_tag tag )
{
    char            *   p;
    condcode            cc;
    int                 k;
    lay_att             curr;
    att_args            l_args;
    int                 cvterr;

    tag = tag;
    p = scan_start;
    cvterr = false;

    if( !GlobalFlags.firstpass ) {
        scan_start = scan_stop;
        eat_lay_sub_tag();
        return;                         // process during first pass only
    }
    if( ProcFlags.lay_xxx != el_sl ) {
        ProcFlags.lay_xxx = el_sl;
    }
    cc = get_lay_sub_and_value( &l_args );  // get att with value
    while( cc == pos ) {
        cvterr = -1;
        for( k = 0, curr = sl_att[k]; curr > 0; k++, curr = sl_att[k] ) {

            if( !strnicmp( att_names[curr], l_args.start[0], l_args.len[0] ) ) {
                p = l_args.start[1];

                switch( curr ) {
                case   e_level:
                    cvterr = i_int8( p, curr, &layout_work.sl.level );
                    if( layout_work.sl.level != 1) {
                       cvterr = 1;      // only level 1 allowed /supported  TBD
                    }
                    break;
                case   e_left_indent:
                    cvterr = i_space_unit( p, curr,
                                           &layout_work.sl.left_indent );
                    break;
                case   e_right_indent:
                    cvterr = i_space_unit( p, curr,
                                           &layout_work.sl.right_indent );
                    break;
                case   e_pre_skip:
                    cvterr = i_space_unit( p, curr, &layout_work.sl.pre_skip );
                    break;
                case   e_skip:
                    cvterr = i_space_unit( p, curr, &layout_work.sl.skip );
                    break;
                case   e_spacing:
                    cvterr = i_int8( p, curr, &layout_work.sl.spacing );
                    break;
                case   e_post_skip:
                    cvterr = i_space_unit( p, curr,
                                           &layout_work.sl.post_skip );
                    break;
                case   e_font:
                    cvterr = i_font_number( p, curr, &layout_work.sl.font );
                    if( layout_work.sl.font >= wgml_font_cnt ) {
                        layout_work.sl.font = 0;
                    }
                    break;
                default:
                    out_msg( "WGML logic error.\n");
                    cvterr = true;
                    break;
                }
                if( cvterr ) {          // there was an error
                    err_count++;
                    g_err( err_att_val_inv );
                    file_mac_info();
                }
                break;                  // break out of for loop
            }
        }
        if( cvterr < 0 ) {
            err_count++;
            g_err( err_att_name_inv );
            file_mac_info();
        }
        cc = get_lay_sub_and_value( &l_args );  // get att with value
    }
    scan_start = scan_stop;
    return;
}

/***************************************************************************/
/*  lay_ul                                                                 */
/***************************************************************************/

void    lay_ul( lay_tag tag )
{
    char            *   p;
    condcode            cc;
    int                 k;
    lay_att             curr;
    att_args            l_args;
    int                 cvterr;

    tag = tag;
    p = scan_start;
    cvterr = false;

    if( !GlobalFlags.firstpass ) {
        scan_start = scan_stop;
        eat_lay_sub_tag();
        return;                         // process during first pass only
    }
    if( ProcFlags.lay_xxx != el_ul ) {
        ProcFlags.lay_xxx = el_ul;
    }
    cc = get_lay_sub_and_value( &l_args );  // get att with value
    while( cc == pos ) {
        cvterr = -1;
        for( k = 0, curr = ul_att[k]; curr > 0; k++, curr = ul_att[k] ) {

            if( !strnicmp( att_names[curr], l_args.start[0], l_args.len[0] ) ) {
                p = l_args.start[1];

                switch( curr ) {
                case   e_level:
                    cvterr = i_int8( p, curr, &layout_work.ul.level );
                    if( layout_work.ul.level != 1) {
                       cvterr = 1;      // only level 1 allowed /supported  TBD
                    }
                    break;
                case   e_left_indent:
                    cvterr = i_space_unit( p, curr,
                                           &layout_work.ul.left_indent );
                    break;
                case   e_right_indent:
                    cvterr = i_space_unit( p, curr,
                                           &layout_work.ul.right_indent );
                    break;
                case   e_pre_skip:
                    cvterr = i_space_unit( p, curr, &layout_work.ul.pre_skip );
                    break;
                case   e_skip:
                    cvterr = i_space_unit( p, curr, &layout_work.ul.skip );
                    break;
                case   e_spacing:
                    cvterr = i_int8( p, curr, &layout_work.ul.spacing );
                    break;
                case   e_post_skip:
                    cvterr = i_space_unit( p, curr,
                                           &layout_work.ul.post_skip );
                    break;
                case   e_font:
                    cvterr = i_font_number( p, curr, &layout_work.ul.font );
                    if( layout_work.ul.font >= wgml_font_cnt ) {
                        layout_work.ul.font = 0;
                    }
                    break;
                case   e_align:
                    cvterr = i_space_unit( p, curr, &layout_work.ul.align );
                    break;
                case   e_bullet:
                    cvterr = i_char( p, curr,
                                     &layout_work.ul.bullet );
                    break;
                case   e_bullet_translate:
                    cvterr = i_yes_no( p, curr,
                                       &layout_work.ul.bullet_translate );
                    break;
                case   e_bullet_font:
                    cvterr = i_font_number( p, curr, &layout_work.ul.bullet_font );
                    if( layout_work.ul.bullet_font >= wgml_font_cnt ) {
                        layout_work.ul.bullet_font = 0;
                    }
                    break;
                default:
                    out_msg( "WGML logic error.\n");
                    cvterr = true;
                    break;
                }
                if( cvterr ) {          // there was an error
                    err_count++;
                    g_err( err_att_val_inv );
                    file_mac_info();
                }
                break;                  // break out of for loop
            }
        }
        if( cvterr < 0 ) {
            err_count++;
            g_err( err_att_name_inv );
            file_mac_info();
        }
        cc = get_lay_sub_and_value( &l_args );  // get att with value
    }
    scan_start = scan_stop;
    return;
}

