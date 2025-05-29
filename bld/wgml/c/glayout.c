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
* Description: WGML implement :LAYOUT and :eLAYOUT tags
*
****************************************************************************/


#include "wgml.h"

#include "clibext.h"


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
/*  lay_layout                                                             */
/***************************************************************************/

void    lay_layout( const gmltag * entry )
{
    char            *p;

    p = scandata.s;
    scandata.s = scandata.e;

    if( !GlobalFlags.firstpass ) {
        ProcFlags.layout = true;
        eat_lay_sub_tag();
        return;
    }

    if( ProcFlags.fb_document_done ) {
        xx_err_exit( ERR_LAY_TOO_LATE );
        /* never return */
    }

    if( !ProcFlags.lay_specified ) {
        ProcFlags.lay_specified = true;
        g_info_lm( INF_PROC_LAY );
    }

    if( *p == '\0' || *p == '.' ) {
        if( ProcFlags.layout ) {        // nested layout
            xx_err_exit_c( ERR_NESTED_TAG, entry->tagname );
            /* never return */
        }
        ProcFlags.layout = true;
    } else {
        xx_err_exit_cc( ERR_EXTRA_IGNORED, g_tok_start, p );
        /* never return */
    }
}


/***************************************************************************/
/*  lay_elayout     end of layout processing                               */
/***************************************************************************/

void    lay_elayout( const gmltag * entry )
{
    char            *p;

    p = scandata.s;
    scandata.s = scandata.e;

    if( !GlobalFlags.firstpass ) {
        ProcFlags.layout = false;
        eat_lay_sub_tag();
        return;                         // process during first pass only
    }

    if( *p == '\0' || *p == '.' ) {
        if( !ProcFlags.layout ) {       // not in layout processing
            xx_err_exit_cc( ERR_NO_LAY, &(entry->tagname[1]), entry->tagname );
            /* never return */
        }
        ProcFlags.layout = false;
        ProcFlags.lay_xxx = TL_NONE;
    } else {
        xx_err_exit_cc( ERR_EXTRA_IGNORED, g_tok_start, p );
        /* never return */
    }
}
