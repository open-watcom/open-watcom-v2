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
.* 20-aug-92    Craig Eisler    initial draft
.* 03-oct-05    L. Haynes       reformatted for hlp, figures
.*
.****************************************************************************/
.chap *refid=ctags CTAGS
:cmt. .if &e'&dohelp eq 0 .do begin
:cmt. .   .section 'Introduction'
:cmt. .do end
.np
The &edname can utilize something known as tags to help you locate
declarations of various objects in your C, C++, and FORTRAN code.
.np
In C or C++ files, tags will help you locate functions, typedefs, structs,
enums and unions.  In a C++ file, you will also be able to locate classes.
.np
Tags will help you find all function and subroutines in FORTRAN files.
.np
Once you select a tag that you wish to locate (by either using the
&cmdmode key CTRL_] (control close square bracket) or the
&cmdline command
.keyword tag
:cont.), &edname searchs a special tag file for the specified tag.
The name of the tag file is determined by the setting of
.keyword tagfilename
:cont., the default is
.param tags
:period.
.np
If the tag is located in the tag file, then &edname edits the file
that contains the tag that you specified and goes to the line with
the definition on it, highlighting the line.  If the tag is
not found, you will get an error message.
.np
The tag file must be located somewhere in the directories
specified in your PATH or
.ev EDPATH
environment variable.
.np
There is a special program provided for the creation of tag files.  This
program is called
.bd ctags
:period.
It is used as follows:

.millust begin
Usage: ctags [-?admstqvx] [-f<fname>] [files] [@optfile]
    [files]     : source files (may be C, C++, or FORTRAN)
                  file names may contain wild cards (* and ?)
    [@optfile]  : specifies an option file
    Option File Directives:
             option <opts>: any command line options (no dashes).
                            an option line resets the d,m,s and t options.
                            they must be specified on option line to
                            remain in effect
             file <flist> : a list of files, separated by commas
    Options: -?        : print this list
             -a        : append output to existing tags file
             -c        : add classes (C++ files)
             -d        : add all #defines (C,C++ files)
             -f<fname> : specify alternate tag file (default is "tags")
             -m        : add #defines (macros only) (C,C++ files)
             -s        : add structs, enums and unions (C,C++ files)
             -t        : add typedefs (C,C++ files)
             -q        : quiet operation
             -v        : verbose operation
             -x        : add all possible tags (same as -cdst)
    Options may be specified in a CTAGS environment variable
.millust end

