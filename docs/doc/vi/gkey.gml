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
.* Description:  Basic usage.
.*
.* Date         By              Reason
.* ----         --              ------
.* 08-aug-92    Craig Eisler    initial draft
.* 01-Oct-05    L. Haynes       reformatted, figures
.*
.****************************************************************************/
.begkey g g
Enters
.keyword text insertion mode
at the current cursor position.  This sets you up in
.param overstrike
or
.param insert
mode, depending on the mode you were in last time you were in
.keyword text insertion mode
:period.
.sp
This key is useful to
.keyref keyadd
in a
.keyword script
:cont.,
to return to the exact same type of
.keyword text insertion mode
the user was in before leaving
.keyword text insertion mode
:period.
.endkey
