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
* Description: implement .ty script control word
*
*  comments are from script-tso.txt
****************************************************************************/

#define __STDC_WANT_LIB_EXT1__  1      /* use safer C library              */

#include "wgml.h"
#include "gvars.h"


/**************************************************************************/
/* TYPE displays a line of information at the terminal.                   */
/*                                                                        */
/*      旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커      */
/*      |       |                                                  |      */
/*      |  .TY  |    information                                   |      */
/*      |       |                                                  |      */
/*      읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸      */
/*                                                                        */
/* This control word does not cause a break.  If used with no operands, a */
/* blank line is displayed.   The text  of the line displayed is modified */
/* by the .TR (Translate) characters currently in effect.                 */
/*                                                                        */
/* EXAMPLES                                                               */
/* (1) The .TY  control is useful  immediately preceding a  .TE (Terminal */
/*     Input) or .RV (Read Variable)  control word,  as a prompt for what */
/*     to type in response:                                               */
/*       .ty Type Name and Address in five lines or less:                 */
/*       .te 5                                                            */
/* (2) Prompt user for an input variable:                                 */
/*       .ty Specify number of output columns:                            */
/*       .rv ncols                                                        */
/*       .cd set &ncols                                                   */
/**************************************************************************/

void    scr_ty( void )
{
    char    *   p = scan_start + 1;

    while( *p == ' ' ) {                // wgml 4.0 ignores leading blanks
        p++;                            // let's do the same
    }
    if( *p == '\0' ) {
        p--;
    }
    out_msg( "%s\n", p );        // no output translation, add if needed TBD
    scan_restart = scan_stop + 1;
    return;
}

