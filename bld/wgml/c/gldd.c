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
* Description: WGML implement :DD   tag for LAYOUT processing
*
****************************************************************************/

#define __STDC_WANT_LIB_EXT1__  1      /* use safer C library              */

#include "wgml.h"
#include "gvars.h"

/***************************************************************************/
/*   :DD     attributes                                                    */
/***************************************************************************/
const   lay_att     dd_att[3] =
    { e_line_left, e_font, e_dummy_zero };

/*****************************************************************************/
/*Define the characteristics of the data description entity.                 */
/*                                                                           */
/*:DD                                                                        */
/*        line_left = '0.5i'                                                 */
/*        font = 0                                                           */
/*                                                                           */
/*line_left This attribute accepts any valid horizontal space unit. The      */
/*specified amount of space must be available on the output line after the   */
/*definition term which precedes the data description. If there is not       */
/*enough space, the data description will be started on the next output line.*/
/*                                                                           */
/*font This attribute accepts a non-negative integer number. If a font       */
/*number is used for which no font has been defined, WATCOM                  */
/*Script/GML will use font zero. The font numbers from zero to three         */
/*correspond directly to the highlighting levels specified by the            */
/*highlighting phrase GML tags. The font attribute defines the font of       */
/*the data description.                                                      */
/*                                                                           */
/*****************************************************************************/


/***************************************************************************/
/*  lay_dd                                                                 */
/***************************************************************************/

void    lay_dd( const gmltag * entry )
{
    char        *   p;
    condcode        cc;
    int             k;
    lay_att         curr;
    att_args        l_args;
    int             cvterr;

    p = scan_start;
    cvterr = false;

    if( !GlobalFlags.firstpass ) {
        scan_start = scan_stop + 1;
        eat_lay_sub_tag();
        return;                         // process during first pass only
    }
    if( ProcFlags.lay_xxx != el_dd ) {
        ProcFlags.lay_xxx = el_dd;
    }
    cc = get_lay_sub_and_value( &l_args );  // get att with value
    while( cc == pos ) {
        cvterr = -1;
        for( k = 0, curr = dd_att[k]; curr > 0; k++, curr = dd_att[k] ) {

            if( !strnicmp( att_names[curr], l_args.start[0], l_args.len[0] ) ) {
                p = l_args.start[1];

                switch( curr ) {
                case   e_line_left:
                    cvterr = i_space_unit( p, curr,
                                           &layout_work.dd.line_left );
                    break;
                case   e_font:
                    cvterr = i_int8( p, curr, &layout_work.dd.font );
                    if( layout_work.dd.font >= wgml_font_cnt ) {
                        layout_work.dd.font = 0;
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
    scan_start = scan_stop + 1;
    return;
}

