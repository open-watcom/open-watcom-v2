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
.* 10-aug-92    Craig Eisler    initial draft
.* 28-sep-05    L. Haynes       reformatted, figures, gadget menu invocation
.*
.***************************************************************************
You have now made a number of significant modifications to your file.
To discard the file without saving your changes,
activate the individual file menu (at the top-left corner of the
edit window) either by clicking on the gadget with the mouse or by
pressing
.param ALT_G.
When the menu is up, try selecting the
.keyword Close
item.  You will get the message:
.millust begin
File modified - use :q! to force
.millust end
The command ":q!" being referred to will be discussed later.  The
file can be exited without saving by using the
.keyword Close no save
menu item.  Activate the individual file menu again, and select this
item.  This will discard your changes to the file, and since you
are not editing any other files, you are returned to the operating system
command prompt.
