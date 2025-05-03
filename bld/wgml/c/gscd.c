/****************************************************************************
*
*                            Open Watcom Project
*
*  Copyright (c) 2004-2010 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  currently, non-implementation of control word CD
*
****************************************************************************/


#include "wgml.h"


/**************************************************************************/
/* COLUMN DEFINITION defines the number of columns into which the text    */
/* area is to be formatted, and optionally the starting position of each  */
/* relative to the beginning of the output line.                          */
/*                                                                        */
/*       旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커     */
/*       |       |                                                  |     */
/*       |       | SET <n <gmin <gmax>>>                            |     */
/*       |  .CD  |                                                  |     */
/*       |       | <n <h1 <h2 ... h9>>>                             |     */
/*       |       |                                                  |     */
/*       읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸     */
/*                                                                        */
/* SET <n <gmin <gmax>>>: SCRIPT calculates the Column Length (.CL)       */
/*    value and the starting position of each column relative to the Page */
/*    Margin value by dividing the current Line Length (.LL) value into   */
/*    "n" equal columns with "n-1" equal gutters (the space between       */
/*    columns). The value specified for "n" may range from 1 to 9. A      */
/*    signed value specifies a change relative to the number of columns   */
/*    currently in effect. The "gmin" operand allows you to specify the   */
/*    minimum gutter length in horizontal units. A signed value indicates */
/*    a change relative to the current gutter length; if omitted, a       */
/*    value equal to 5% of the Line Length will be used. The "gmax"       */
/*    operand allows you to specify the maximum gutter length in hori-    */
/*    zontal units. A signed value indicates a change relative to the     */
/*    "gmin" value; if omitted, the Line Length value will be used. If    */
/*    only SET is specified, the control word re-defines the COLUMN       */
/*    DEFINITION currently in effect.                                     */
/* <n <h1 <h2 ... h9>>>: The numeric operands following the "n" define    */
/*    the starting position (the displacement) of each column relative to */
/*    the Adjust (.AD) value. A zero displacement means no displacement   */
/*    at all. If a displacement is signed, it means a change relative to  */
/*    the previous displacement in the same control word. If displacement */
/*    operands are omitted, their former values are used. If all          */
/*    operands are omitted, it is a redefinition of the current COLUMN    */
/*    DEFINITION. This form of the COLUMN DEFINITION does not set the     */
/*    width of each column; you must do that yourself, using the Column   */
/*    Length (.CL) control word.                                          */
/*                                                                        */
/* This control word causes a break. The initial number of columns is     */
/* one, with displacements 0, 46, 92, 0, 0, 0, 0, 0, 0. Omitted operands  */
/* in this control word retain their former value.                        */
/*                                                                        */
/* NOTES                                                                  */
/* (1) The .CD control word causes all input text to that point to be     */
/*     printed with the former definition.                                */
/* (2) If the text in one column should overflow past the start of the    */
/*     next column (because of FORMAT NO or overlays), it will be         */
/*     replaced by the text of that next column.                          */
/* (3) This control word is not allowed in Keeps or Footnotes.            */
/* (4) When the SET notation is used, SCRIPT first attempts to calculate  */
/*     column and gutter lengths that produce a displacement of zero for  */
/*     the first column and cause the last character of the last column   */
/*     to be positioned at the Line Length. If that fails, then it will   */
/*     attempt to calculate column and gutter lengths such that there are */
/*     an equal number of blank positions at the beginning and the end of */
/*     the line. If that fails, it will use the "gmin" value (specified   */
/*     or default) for the gutter length and produce a Column Length that */
/*     results in one more blank position at the end of the line than at  */
/*     the beginning of the line.                                         */
/*                                                                        */
/* EXAMPLES                                                               */
/* (1) .ll 132;.cd set 3                                                  */
/*     This example results in a 132-character output line with 3 columns */
/*     of text per page and an appropriate gutter-length calculated by    */
/*     SCRIPT.                                                            */
/* (2) .cd 1 0                                                            */
/*     This is the same as single-column mode. It specifies one column    */
/*     at displacement zero.                                              */
/* (3) .ll 60;.cl 27;.cd 2 0 33                                           */
/*     This specifies a Line Length of 60 and a Column Length of 27.      */
/*     Text will be formatted from column 1 (displacement 0) to column 27 */
/*     and from column 34 (displacement 33) to column 60, leaving a       */
/*     6-character "gutter" or white space between columns.               */
/* (4) .ll 6.5i;.cl 3i;.cd 2 0.0i 3.5i                                    */
/*     This specifies a Line Length of 6.5 inches. The first column text  */
/*     will be formatted from the beginning of the Line Length for 3.0    */
/*     inches and the second column text will be formatted from 3.5       */
/*     inches into the Line Length for 3.0 inches. This will leave a      */
/*     gutter .5 inches wide between the columns.                         */
/**************************************************************************/

/************************************************************************/
/* this simply skips rest of the logical record                         */
/* the reason is that the OW Docs do not need it, but they do use it    */
/* they use it just before INDEX, but, since it is not used inside the  */
/* INDEX section, it has no effect.                                     */
/************************************************************************/

void scr_cd( void )
{
    scan_restart = scan_stop + 1;
    return;
}
