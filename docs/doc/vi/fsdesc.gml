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
.* Description:  Directory listing text.
.*
.* Date		By		Reason
.* ----		--		------
.* 12-aug-92	Craig Eisler	initial draft
.* 28-sep-05	L. Haynes		reformatted
.****************************************************************************/
From this listing, you may pick a file, another directory, or another
drive (the available drives are at the end of the listing).
A directory is indicated
by the leading backslash ('\'). If a directory is chosen in this window,
then the list of files in that directory is displayed.  This
list also contains all the drives available (which are enclosed in
square brackets, e.g. [c:]). If you select a drive, the list of files in the
current directory on that drive is displayed.
.np
For files and directories, each line indicates the file name,
the various attributes of the file ([d]irectory, [a]rchive, [h]idden,
[s]ystem, [r]eadable, [w]riteable, e[x]ecutable), the file size in
bytes, the date and time of the last file update. Some sample lines are:
.millust begin
  test.c        -a--rw-    25586  08/16/92  08:14
  bar.c         -a--r--      639  02/27/92  13:25
 \tmpdir        d---rw-        0  08/16/92  19:05
  [c:]
.millust end
