.****************************************************************************
.*
.*                            Open Watcom Project
.*
.*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
.*
.*  ========================================================================
.*
.*    This file contains Original Code and/or Modifications of Original
.*    Code as defined in and that are subject to the Sybase Open Watcom
.*    Public License version 1.0 (the 'License'). You may not use this file
.*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
.*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
.*    provided with the Original Code and Modifications, and is also
.*    available at www.sybase.com/developer/opensource.
.*
.*    The Original Code and all software distributed under the License are
.*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
.*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
.*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
.*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
.*    NON-INFRINGEMENT. Please see the License for the specific language
.*    governing rights and limitations under the License.
.*
.*  ========================================================================
.*
.* Description:  Selecting text using the keyboard.
.*
.* Date         By              Reason
.* ----         --              ------
.* 10-aug-92    Craig Eisler    initial draft
.* 28-sept-05   L. Haynes       reformat for hlp
.*
.****************************************************************************/
:DL break.
:DT.SHIFT_UP (shifted cursor up key)
:DD.Starts or continues selection and moves up to the previous
line.  The new line is selected.

:DT.SHIFT_DOWN (shifted cursor down key)
:DD.Starts or continues selection and moves down to the next line.
The new line is selected.

:DT.SHIFT_LEFT (shifted cursor left key)
:DD.Starts or continues selection and moves left to the next
character.  The new character is selected.

:DT.SHIFT_RIGHT (shifted cursor right key)
:DD.Starts or continues selection and moves right to the previous
character. The new character is selected.

:DT.CTRL_R
:DD.Starts text selection, if no text is selected.  The current character is
highlighted. If a region is already selected, then this cancels the
selected region.

:DT.ESC
:DD.Cancels the current selection.
:eDL.
