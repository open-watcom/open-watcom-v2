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
.* Description:  special keys
.* 14-aug-92    Craig Eisler    initial draft
.* 28-sep-05    L. Haynes               reformatted for hlp
.*
.****************************************************************************/
.*
:DL break.
.*
:DT.\&lt."key"&gt.
:DD.Any special key may be inserted for
.param "key"
:period.
The angle brackets are required.
There are a number of pre-defined keys
symbols that are recognized. These are described in the Appendix
:HDREF refid='symkey'.
:period.

:DT.\e
:DD.Short form for the escape key (rather than \&lt.ESC&gt.).

:DT.\n
:DD.Short form for the enter key (rather than \&lt.ENTER&gt.).

:DT.\h
:DD.If a &cmdline command is used in the sequence, and it follows the
colon (':'), the command is not added to the history.  For example:
.millust begin
:\hdate\n
.millust end
will display the current date and time, but the command will not enter
the command history.

:DT.\x
:DD.If a &cmdline command is used in the sequence, then this stops the
command window from opening.  This prevents the "flashing" of the
command window as it is opened then closed. For example:
.millust begin
\x:date\n
.millust end
will display the current date and time, but the command window will
not be displayed.
.*
:eDL.
