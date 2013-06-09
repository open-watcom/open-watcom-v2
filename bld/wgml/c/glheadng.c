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
* Description: WGML implement :HEADING tag for LAYOUT processing
*
****************************************************************************/

#define __STDC_WANT_LIB_EXT1__  1      /* use safer C library              */

#include "wgml.h"
#include "gvars.h"

/***************************************************************************/
/*   :HEADING  attributes                                                  */
/***************************************************************************/
const   lay_att     heading_att[6] =
    { e_delim, e_stop_eject, e_para_indent, e_threshold, e_max_group,
      e_dummy_zero };


/*********************************************************************************/
/*Specify information which applies to headings in general.                      */
/*                                                                               */
/*:HEADING                                                                       */
/*        delim = '.'                                                            */
/*        stop_eject = no                                                        */
/*        para_indent = no                                                       */
/*        threshold = 2                                                          */
/*        max_group = 10                                                         */
/*                                                                               */
/*delim The delim attribute sets the heading number delimiter to a specific      */
/*character.                                                                     */
/*                                                                               */
/*stop_eject This attribute accepts the keyword values yes and no. If the value  */
/*yes is specified, a heading which would force the beginning of a               */
/*new page will not cause a page ejection if it immediately follows              */
/*another heading.                                                               */
/*                                                                               */
/*para_indent This attribute accepts the keyword values yes and no. If the value */
/*no is specified, the indentation of the first line in a paragraph after a      */
/*heading is suppressed.                                                         */
/*                                                                               */
/*threshold This attribute accepts as a value a non-negative integer number.     */
/*The specified value indicates the minimum number of text lines                 */
/*which must fit on the page. The heading will be forced to the next             */
/*page or column if the threshold requirements are not met by the                */
/*following document element. The threshold attribute of the heading             */
/*overrides the default threshold specified by the :widow tag.                   */
/*                                                                               */
/*max_group This attribute accepts a positive integer number. If a group of      */
/*headings are forced to a new page or column because of threshold               */
/*requirements, the specified value will limit the number of headings            */
/*forced as a group.                                                             */
/*********************************************************************************/


/***************************************************************************/
/*  lay_heading                                                            */
/***************************************************************************/

void    lay_heading( const gmltag * entry )
{
    char        *   p;
    condcode        cc;
    int             k;
    lay_att         curr;
    att_args        l_args;
    int             cvterr;

    entry = entry;
    p = scan_start;

    if( !GlobalFlags.firstpass ) {
        scan_start = scan_stop + 1;
        eat_lay_sub_tag();
        return;                         // process during first pass only
    }
    if( ProcFlags.lay_xxx != el_heading ) {
        ProcFlags.lay_xxx = el_heading;
    }

    cc = get_lay_sub_and_value( &l_args );  // get att with value
    while( cc == pos ) {
        cvterr = -1;
        for( k = 0, curr = heading_att[k]; curr > 0; k++, curr = heading_att[k] ) {

            if( !strnicmp( att_names[curr], l_args.start[0], l_args.len[0] ) ) {
                p = l_args.start[1];

                switch( curr ) {
                case   e_delim:
                    cvterr = i_char( p, curr, &layout_work.heading.delim );
                    break;
                case   e_stop_eject:
                    cvterr = i_yes_no( p, curr,
                                           &layout_work.heading.stop_eject );
                    break;
                case   e_para_indent:
                    cvterr = i_yes_no( p, curr,
                                           &layout_work.heading.para_indent );
                    break;
                case   e_threshold:
                    cvterr = i_uint8( p, curr, &layout_work.heading.threshold );
                    break;
                case   e_max_group:
                    cvterr = i_int8( p, curr, &layout_work.heading.max_group );
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
        cc = get_lay_sub_and_value( &l_args );  // get one with value
    }
    scan_start = scan_stop + 1;
    return;
}

