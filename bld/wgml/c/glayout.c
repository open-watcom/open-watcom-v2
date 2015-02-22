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
* Description: WGML implement :LAYOUT and :eLAYOUT tags
*
****************************************************************************/
 
#include "wgml.h"
#include "gvars.h"
 
 
 
/***************************************************************************/
/*                                                                         */
/*  Format: :LAYOUT.                                                       */
/*                                                                         */
/* This tag starts a layout section. The layout tag is a special WATCOM    */
/* Script/GML tag used to modify the default layout of the output document.*/
/* More than one layout section may be present, but all layout sections    */
/* must appear before the :gdoc tag. The :elayout tag terminates a layout  */
/* section.                                                                */
/***************************************************************************/
 
/***************************************************************************/
/*                                                                         */
/*  Format: :eLAYOUT.                                                      */
/*                                                                         */
/* This tag signals the end of a layout section. A corresponding :layout   */
/* tag must be previously specified for each :elayout tag.                 */
/***************************************************************************/
 
/***************************************************************************/
/*  gml_layout                                                             */
/***************************************************************************/
 
void    gml_layout( lay_tag tag )
{
    char        *   p;
 
    p = scan_start;
    scan_start = scan_stop;
 
    if( !GlobalFlags.firstpass ) {
        ProcFlags.layout = true;
 
        /*******************************************************************/
        /*  read and ignore all lines up to :eLAYOUT                       */
        /*******************************************************************/
 
        while( !ProcFlags.reprocess_line  ) {
            eat_lay_sub_tag();
            if( strnicmp( ":elayout", buff2, 8 ) ) {
                ProcFlags.reprocess_line = false;   // not :elayout, go on
            }
        }
        return;
    }
 
    if( !ProcFlags.lay_specified ) {
        ProcFlags.lay_specified = true;
        out_msg( "Processing layout\n" );
    }
 
    if( *p == '\0' || *p == '.' ) {
        if( ProcFlags.layout ) {        // nested layout
            err_count++;
            g_err( err_nested_tag, lay_tagname( tag ) );
            file_mac_info();
            return;
        }
        ProcFlags.layout = true;
        return;
    } else {
        err_count++;
        g_err( err_extra_ignored, tok_start, p );
        file_mac_info();
    }
    return;
}
 
 
/***************************************************************************/
/*  lay_elayout     end of layout processing                               */
/***************************************************************************/
 
void    lay_elayout( lay_tag tag )
{
    char        *   p;
 
    p = scan_start;
    scan_start = scan_stop;
 
    if( !GlobalFlags.firstpass ) {
        ProcFlags.layout = false;
        return;                         // process during first pass only
    }
 
    if( *p == '\0' || *p == '.' ) {
        if( !ProcFlags.layout ) {       // not in layout processing
            err_count++;
            g_err( err_no_lay, &(lay_tagname( tag )[1]), lay_tagname( tag ) );
            file_mac_info();
            return;
        }
        ProcFlags.layout = false;
        ProcFlags.lay_xxx = el_zero;
 
        return;
    } else {
        err_count++;
        g_err( err_extra_ignored, tok_start, p );
        file_mac_info();
    }
    return;
}
 
