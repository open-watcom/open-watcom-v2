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
.* Description:  Marks.
.*
.* Date         By              Reason
.* ----         --              ------
.* 28-sep-92    Craig Eisler    initial draft
.* 28-sept-05   L. Haynes       reformat for hlp
.*
.****************************************************************************/
:OL.
:LI.A movement command. See the section
:HDREF refid='mvement'.
for a full description of all movement commands.
If a movement command is specified, then the range that the command
will act on is from the current position to the position that would
be achieved by using the movement command.
:LI.A search command:
:UL compact.
:LI./ (forward slash)
:LI.? (question mark)
:LI.n
:LI.N
:eUL.
See the section
:HDREF refid='srching'
.if '&chapref' eq '1' .do begin
.se chapref = 0
in the chapter
:HDREF page=no refid='modes'
.do end
for a full description of the searching commands.
If a search command is specified, then the range that the command
will act on is from the current position to the position that would
be achieved by using the search command.
:LI.The current selected (highlighted) region.  In this case,
.param &lt.oper&gt.
is the
.param r
key.
:LI.The same character as the command character.  This causes the
command to operate on the current line.
:eOL.

