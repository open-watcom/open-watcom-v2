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
.* 18-aug-92    Craig Eisler    initial draft
.* 28-sept-05   L. Haynes       reformat for hlp
.*
.****************************************************************************/
You can use complex search strings known as
.keyword regular expressions
:period.
Certain characters have special meaning in a regular expression, they are:
.millust begin
^ $ . [ ( ) | ? + * \ ~ @
.millust end
If you wish to search for any of these special characters, you must
place a backslash ('\') before the character.  For example, to search
for:
.millust begin
ab.c$
.millust end
you have to enter:
.millust begin
ab\.c\$
.millust end
For information on regular expressions, see
:HDREF refid='rxchap'.
:period.
