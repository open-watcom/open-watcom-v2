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
* Description: implement .oc (output comment) script control word
*
*
*  comments are from script-tso.txt
****************************************************************************/

#define __STDC_WANT_LIB_EXT1__  1      /* use safer C library              */

#include "wgml.h"
#include "gvars.h"


/**************************************************************************/
/* OUTPUT COMMENT causes one line of  information to be inserted into the */
/* output as a comment.                                                   */
/*                                                                        */
/*      ------------------------------------------------------------      */
/*      |       |                                                  |      */
/*      |  .OC  |    <information>                                 |      */
/*      |       |                                                  |      */
/*      ------------------------------------------------------------      */
/*                                                                        */
/* This  control word  does not  cause a  break.   If  specified with  no */
/* operand, no action is taken.  The operand line is not itself formatted */
/* and  does not  add to  the count  of lines  on the  output page  being */
/* formatted.   The  comment line  is immediately  written to  the output */
/* file.   Thus  if it  were to occur  within a  paragraph or  within the */
/* BEGIN/END range of an "in-storage" text  block (.CC,  .FN,  etc),  the */
/* position of the comment  in the output would not agree  with its posi- */
/* tion in the input.   This would  be most noticeable in multiple-column */
/* mode.   This  control word would normally  be used when  the formatted */
/* output is  to be re-processed by  another program,  an editor,   or an */
/* intelligent output device.                                             */
/*                                                                        */
/* NOTES                                                                  */
/* (1) If multiple  passes are in effect,   the Output Comments  are only */
/*     written during the last pass.                                      */
/* (2) Output Comments are  processed even if the current page  is not to */
/*     be displayed because of FROM page options.                         */
/**************************************************************************/

void    scr_oc( void )
{
    char        *   p;

    if( GlobalFlags.lastpass ) {
        p = scan_start;                 // next char after .oc
        if( *p ) {                      // line operand specified
            p++;                        // over space
            if( *p ) {
                ob_direct_out( p );
                if( input_cbs->fmflags & II_research ) {
                    out_msg( p );       // TBD
                    out_msg( "<-.oc.oc.oc\n" ); // TBD
                }
            }
        }
    }
    scan_restart = scan_stop;
    return;
}

