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
.* Description:  Error Messages.
.*
.* Date         By              Reason
.* ----         --              ------
.* 14-aug-92    Craig Eisler    initial draft
.* 03-oct-05    L. Haynes       reformatted for hlp, figures
.*
.****************************************************************************/
.chap *refid=symkey 'Symbolic Keystrokes'
:cmt. .if &e'&dohelp eq 0 .do begin
:cmt. .   .section 'Introduction'
:cmt. .do end
.np
When mapping keys using the
.keyref map
command, and unmapping keys using the
.keyref unmap
command, it is useful to be able to specify the key that you are
mapping symbolically, especially if it is a function key, a cursor key
or other special key.  There are a number of pre-defined keys
symbols that are recognized when specifying which key is being mapped/unmapped.
.np
The next section describes the symbol used to represent the key, and
what the key actually is.  These symbols are also used throughout
this guide to represent a special key.
.* ******************************************************************
.section 'Symbols and Meaning'
.* ******************************************************************
:INCLUDE file='symkey'
