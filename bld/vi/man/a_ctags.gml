.*
.* (c) Copyright 1992 by WATCOM International Corp.
.*
.* All rights reserved. No part of this publication may be reproduced or
.* used in any form or by any means - graphic, electronic, or mechanical,
.* including photocopying, recording, taping or information storage and
.* retrieval systems - without written permission of WATCOM Publications
.* Limited.
.*
.* Date         By              Reason
.* ----         --              ------
.* 20-aug-92    Craig Eisler    initial draft
.*
:CHAPTER id=ctags.CTAGS
&edname can utilize something known as tags to help you locate
declarations of various objects in your C, C++, and FORTRAN code.
:P.
In C or C++ files, tags will help you locate functions, typedefs, structs,
enums and unions.  In a C++ file, you will also be able to locate classes.
:P.
Tags will help you find all function and subroutines in FORTRAN files.
:P.
Once you select a tag that you wish to locate (by either using the
&cmdmode key CTRL_] (control close square bracket) or the
&cmdline command
:KEYWORD.tag
:CONT.), &edname searchs a special tag file for the specified tag.
The name of the tag file is determined by the setting of
:KEYWORD.tagfilename
:CONT., the default is
:ITALICS.tags
:PERIOD.
:P.
If the tag is located in the tag file, then &edname edits the file
that contains the tag that you specified and goes to the line with
the definition on it, highlighting the line.  If the tag is
not found, you will get an error message.
:P.
The tag file must be located somewhere in the directories
specified in your PATH or
:KEYWORD.EDPATH
environment variable.
:P.
There is a special program provided for the creation of tag files.  This
program is called
:HILITE.ctags
:PERIOD.
It is used as follows:
:ILLUST.
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
:eILLUST.
