.*
.* (c) Copyright 1992 by WATCOM International Corp.
.*
.* All rights reserved. No part of this publication may be reproduced or
.* used in any form or by any means - graphic, electronic, or mechanical,
.* including photocopying, recording, taping or information storage and
.* retrieval systems - without written permission of WATCOM Publications
.* Limited.
.*
.* Date		By		Reason
.* ----		--		------
.* 06-aug-92	Craig Eisler	initial draft
.*
:CHAPTER.&edname Environment
:CMT :SECTION.Introduction
Along with the &edname executable, there are a number of files that are needed:
:DEFLIST.
:DEFITEM.ed.cfg
The
:KEYWORD.configuration script
(contains editor setup)
:DEFITEM.keys.dat
symbolic key names used by the &cmdline commands
:KEYWORD.map
:CONT.,
:KEYWORD.mapbase
:CONT.,
:KEYWORD.unmap
:CONT.,
:KEYWORD.execute
and
:KEYWORD.keyadd
:PERIOD.
:DEFITEM.errmsg.dat
The text strings for &edname's various error messages.
:DEFITEM.error.dat
Symbolic names for errors that can occur.  These are used for testing
return codes in editor scripts.
:eDEFLIST.
:P.
The editor searches for its special files as follows:
:OL.
:LI.The editor executable itself (if the files are bound).
:LI.The current directory.
:LI.Directories in the EDPATH environment variable.
:LI.Directories in the PATH environment variable.
:eOL.
The environment variable
:KEYWORD.EDPATH
is used so that the support files
for &edname do not have to be in your PATH.  Only &edname itself has
to be in your PATH.

:SECTION.Using Edbind
Edbind is a utility designed to place any specified files onto the
end of the editor executable.  The file
:KEYWORD.edbind.dat
contains the list of all files to bind. This would normally include
:KEYWORD.ed.cfg
and all the .dat files.  This eliminates the need to have these files
in your path, and allows &edname to locate the files faster.
:P.
Note that your configuration file must be the FIRST FILE in the
:KEYWORD.edbind.dat
file.  The first file is designated to be the file containing the
:KEYWORD.configuration script
:CONT.,
the name is irrelevant.
:P.
The contents of
:KEYWORD.edbind.dat
might be as follows:
:ILLUST.
ed._vi
:bILLUST.
rdme._vi
:bILLUST.
wrme._vi
:bILLUST.
rcs._vi
:bILLUST.
qall._vi
:bILLUST.
proc._vi
:bILLUST.
err._vi
:bILLUST.
chkout._vi
:bILLUST.
forceout._vi
:bILLUST.
unlock._vi
:bILLUST.
mcsel._vi
:bILLUST.
mlsel._vi
:bILLUST.
lnum._vi
:bILLUST.
keys.dat
:bILLUST.
errmsg.dat
:bILLUST.
error.dat
:eILLUST.
In this example,
:KEYWORD.edbind.dat
contains the special files
:KEYWORD.ed.cfg
:CONT.,
all the .dat files, and a number of compiled editor scripts (the files
with the ._vi extension).

:P.
Usage is as follows:
:ILLUST.
edbind <editor exe name> (-s)
 
    Options -s: strip info from executable
:eILLUST.
:P.
The files in
:KEYWORD.edbind.dat
files are searched for in the following order:
:OL.
:LI.The current directory.
:LI.The directories in the environment variable
:KEYWORD.EDPATH
:PERIOD.
:LI.The directories in the environment variable PATH
:eOL.
:SECTION.Invoking &edname
&edname is invoked from the command prompt with the following possible
set of parameters:
:ILLUST.
vi [-?-dinqrvz] +<n> -k "keys" [-s <scr> [-p "parm"]] [-t <tag>]
   [-c <cfg>] files
:eILLUST.
The files specified may contain regular expressions.
:P.
The parameters cause the following:
:DEFLIST.
:DEFITEM.-?         
Displays the possible options, and what they do.

:DEFITEM.--         
Starts &edname in
:KEYWORD.stdio mode
:PERIOD.
In this mode, &edname reads from standard in to get the file to edit.
When the file is written, the lines are written to standard out.
This is useful for including &edname in a pipe.

:DEFITEM.-d
Use default configuration.  &edname will not invoke
:KEYWORD.ed.cfg
when it starts up.

:DEFITEM.-i	    
Ignore lost files.  &edname will not let you start if there are files
to be recovered. This option will cause &edname to get rid of its
checkpoint file, so that it will no longer complain about files
that have not been recovered.

:DEFITEM.-n         
This option will cause &edname to read a file as it needs the data, instead
of reading the file all at once.  It is useful if you wish to look at
the first lines of a huge file.  This option overrides any setting of
:KEYWORD.readentirefile
in your configuration.

:DEFITEM.-q         
Causes &edname to run in quiet mode (no screen usage).  This is useful
for using &edname as a batch script processor.

:DEFITEM.-r	    
Recover lost files, if there are any.  If this option is specified,
files specified on the command line are ignored.

:DEFITEM.-v         
Causes file edited to be a "view only" file.

:DEFITEM.-z         
Causes &edname not to terminate a file read when finding a ctrl-z in a file.
This option is the same as the
:KEYWORD.ignorectrlz
setting.

:DEFITEM.+<n>       
Cause &edname set the edit buffer to line <n> in the file edited.

:DEFITEM.-k "keys"  
Execute the string
:ITALICS.keys
as if they were typed from the keyboard. These keystrokes are processed
once &edname is initialized and all files have been read in.

:DEFITEM.-s <scr>   
Runs the startup script
:ITALICS.<scr>
once &edname is initialized and all files have been read in.
Up to 10 startup scripts may be specified.

:DEFITEM.-p "parms" 
Specify the parameters
:ITALICS parms
for each startup script. Multiple parms
may be specified as long as they are in double quotes.
The parameters are associated with the most recently specified startup script.

:DEFITEM.-t <tag>   
Edits the file containing the tag
:ITALICS.<tag>
:PERIOD.
:DEFITEM.-c <cfg>   
Runs the
:KEYWORD.configuration script
:ITALICS.<cfg>
:CONT.,
instead of the default
:KEYWORD.ed.cfg
:PERIOD.
:eDEFLIST.

:SECTION.Lost File Recovery
&edname has an autosave feature that periodically makes a backup
copy of the current edit buffer.  This backup is kept in the directory
specified by the
:KEYWORD.tmpdir
setting.
:P.
The
:KEYWORD.autosaveinterval
setting is used to specify the number of seconds between backups of
the current edit buffer.
If
:KEYWORD.autosaveinterval
is set to 0, then the autosave feature is disabled.
:P.
&edname keeps a lock file called
:HILITE.alock_?.fil
in its
:KEYWORD.tmpdir
:PERIOD.
The question mark ('?') will be a letter.  There may be more than
one lock file, if more than one copy of &edname is running on
the current machine.
:P.
&edname keeps a checkpoint file called
:HILITE.asave_?.fil
:PERIOD.
The question mark ('?') will be a letter.
Once a file is autosaved, its name is added to this checkpoint file.
When the file is discarded, its name is removed from this checkpoint file.
So, if for any reason, you lose your editing session, this checkpoint
file will contain information about what files were being edited at the time.
Assuming
:KEYWORD.tmpdir
has been set to D:\TMP, then a example of what
:HILITE.asave_?.fil
could contain is:
:ILLUST.
d:\tmp\aaaaa23j.tmp e:\c\test.c
d:\tmp\baaaa23j.tmp e:\h\test.h
d:\tmp\caaaa23j.tmp c:\autoexec.bat
:eILLUST.
The file e:\c\test.c was being edited, and its backup
is stored in d:\tmp\aaaaa23j.tmp.
:P.
The file e:\h\test.c was being edited, and its backup
is stored in d:\tmp\baaaa23j.tmp.
:P.
The file c:\autoexec.bat was being edited, and its backup
is stored in d:\tmp\caaaa23j.tmp.
:P.
You do not need use this information in order to recover lost files,
however, this is useful to know if you wish to look at the files before
recovering them.
:P.
If a checkpoint file exists when &edname is started, then the following
message will appear:
:ILLUST.
Files have been lost since your last session, use -r to recover or
-i to ignore
:eILLUST.
&edname cannot be invoked until either -i or -r is specified.
:P.
If -i is specified, then the checkpoint files are erased and
&edname starts up normally.  The .tmp files that contain the
lost files still remain in the backup directory, however.
:P.
If -r is specified, then &edname recovers the lost files.  Note that
the recovered files must be saved in order to overwrite the original
copy.
