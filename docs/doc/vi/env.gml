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
.* Description:  The VI environment.
.*
.* Date         By              Reason
.* ----         --              ------
.* 06-aug-92    Craig Eisler    initial draft
.* 28-sep-05    L. Haynes       reformatted for hlp, figures
.*
.****************************************************************************/
.chap 'The &edname Environment'
:cmt. .if &e'&dohelp eq 0 .do begin
:cmt. .   .section 'Introduction'
:cmt. .do end
.np
Along with the &edname executable, there are a number of files that are needed:
.*
:DL break.
:DT.ed.cfg
:DD.The
.keyword configuration script
(contains editor setup)
:DT.keys.dat
:DD.Symbolic key names used by the &cmdline commands
.keyref map
:cont.,
.keyref mapbase
:cont.,
.keyref unmap
:cont.,
.keyref execute
and
.keyref keyadd
:period.
:DT.errmsg.dat
:DD.The text strings for &edvi's various error messages.
:DT.error.dat
:DD.Symbolic names for errors that can occur.  These are used for testing
return codes in editor scripts.
:eDL.
.np
The editor searches for its special files as follows:
:OL.
:LI.The editor executable itself (if the files are bound).
:LI.The current directory.
:LI.Directories in the EDPATH environment variable.
:LI.Directories in the PATH environment variable.
:eOL.
.np
The environment variable
.ev EDPATH
is used so that the support files
for &edvi do not have to be in your PATH.  Only &edvi itself has
to be in your PATH.
.* ******************************************************************
.section 'Using Edbind'
.* ******************************************************************
.np
Edbind is a utility designed to place any specified files onto the
end of the editor executable.  The file
:fname.edbind.dat:efname.
contains the list of all files to bind. This would normally include
:fname.ed.cfg:efname.
and all the .dat files.  This eliminates the need to have these files
in your path, and allows &edvi to locate the files faster.
.np
Note that your configuration file must be the FIRST FILE in the
:fname.edbind.dat:efname.
file.  The first file is designated to be the file containing the
.keyword configuration script
:cont.,
the name is irrelevant.
.np
The contents of
:fname.edbind.dat:efname.
might be as follows:
.millust begin
ed._vi
rdme._vi
wrme._vi
rcs._vi
qall._vi
proc._vi
err._vi
chkout._vi
forceout._vi
unlock._vi
mcsel._vi
mlsel._vi
lnum._vi
keys.dat
errmsg.dat
error.dat
.millust end
.np
In this example,
:fname.edbind.dat:efname.
contains the special files
:fname.ed.cfg:efname.,
all the .dat files, and a number of compiled editor scripts (the files
with the ._vi extension).
.np
Usage is as follows:
.millust begin
edbind <editor exe name> (-s)
 
    Options -s: strip info from executable
.millust end
.np
The files in
:fname.edbind.dat:efname.
files are searched for in the following order:
:OL.
:LI.The current directory.
:LI.The directories in the environment variable
.ev EDPATH
:period.
:LI.The directories in the environment variable PATH
:eOL.
.* ******************************************************************
.section 'Invoking the &edname'
.* ******************************************************************
.np
&edvi is invoked from the command prompt with the following possible
set of parameters:
.millust begin
vi [-?-dinqrvz] +<n> -k "keys" [-s <scr> [-p "parm"]] [-t <tag>]
   [-c <cfg>] files
.millust end
The files specified may contain regular expressions.
.np
The parameters cause the following:
.*
:DL break.
.*
:DT.-?         
:DD.Displays the possible options, and what they do.

:DT.--         
:DD.Starts &edvi in
.keyword stdio mode
:period.
In this mode, &edvi reads from standard in to get the file to edit.
When the file is written, the lines are written to standard out.
This is useful for including &edvi in a pipe.

:DT.-d
:DD.Use default configuration.  &edvi will not invoke
:fname.ed.cfg:efname.
when it starts up.

:DT.-i           
:DD.Ignore lost files.  &edvi will not let you start if there are files
to be recovered. This option will cause &edvi to get rid of its
checkpoint file, so that it will no longer complain about files
that have not been recovered.

:DT.-n         
:DD.This option will cause &edvi to read a file as it needs the data, instead
of reading the file all at once.  It is useful if you wish to look at
the first lines of a huge file.  This option overrides any setting of
.keyref readentirefile 1
in your configuration.

:DT.-q         
:DD.Causes &edvi to run in quiet mode (no screen usage).  This is useful
for using &edvi as a batch script processor.

:DT.-r           
:DD.Recover lost files, if there are any.  If this option is specified,
files specified on the command line are ignored.

:DT.-v         
:DD.Causes file edited to be a "view only" file.

:DT.-z         
:DD.Causes &edvi not to terminate a file read when finding a ctrl-z in a file.
This option is the same as the
.keyref ignorectrlz 1
setting.

:DT.+<n>       
:DD.Cause &edvi set the edit buffer to line <n> in the file edited.

:DT.-k "keys"  
:DD.Execute the string
.param keys
as if they were typed from the keyboard. These keystrokes are processed
once &edvi is initialized and all files have been read in.

:DT.-s <scr>   
:DD.Runs the startup script
.param <scr>
once &edvi is initialized and all files have been read in.
Up to 10 startup scripts may be specified.

:DT.-p "parms" 
:DD.Specify the parameters
.param parms
for each startup script. Multiple parms
may be specified as long as they are in double quotes.
The parameters are associated with the most recently specified startup script.

:DT.-t <tag>   
:DD.Edits the file containing the tag
.param <tag>
:period.

:DT.-c <cfg>   
:DD.Runs the configuration script
.param <cfg>
:cont.,
instead of the default
:fname.ed.cfg:efname.
:period.
.*
:eDL.
.* ******************************************************************
.section 'Lost File Recovery'
.* ******************************************************************
.np
&edvi has an autosave feature that periodically makes a backup
copy of the current edit buffer.  This backup is kept in the directory
specified by the
.keyref tmpdir
setting.
.np
The
.keyref autosaveinterval
setting is used to specify the number of seconds between backups of
the current edit buffer.
If
.keyref autosaveinterval
is set to 0, then the autosave feature is disabled.
.np
&edvi keeps a lock file called
.keyword alock_?.fil
in its
.keyref tmpdir
:period.
The question mark ('?') will be a letter.  There may be more than
one lock file, if more than one copy of &edvi is running on
the current machine.
.np
&edvi keeps a checkpoint file called
.keyword asave_?.fil
:period.
The question mark ('?') will be a letter.
Once a file is autosaved, its name is added to this checkpoint file.
When the file is discarded, its name is removed from this checkpoint file.
So, if for any reason, you lose your editing session, this checkpoint
file will contain information about what files were being edited at the time.
Assuming
.keyref tmpdir
has been set to D:\TMP, then a example of what
.keyword asave_?.fil
could contain is:
.millust begin
d:\tmp\aaaaa23j.tmp e:\c\test.c
d:\tmp\baaaa23j.tmp e:\h\test.h
d:\tmp\caaaa23j.tmp c:\autoexec.bat
.millust end
The file e:\c\test.c was being edited, and its backup
is stored in d:\tmp\aaaaa23j.tmp.
.np
The file e:\h\test.c was being edited, and its backup
is stored in d:\tmp\baaaa23j.tmp.
.np
The file c:\autoexec.bat was being edited, and its backup
is stored in d:\tmp\caaaa23j.tmp.
.np
You do not need use this information in order to recover lost files,
however, this is useful to know if you wish to look at the files before
recovering them.
.np
If a checkpoint file exists when &edvi is started, then the following
message will appear:
.millust begin
Files have been lost since your last session, use -r to recover or
-i to ignore
.millust end
&edvi cannot be invoked until either -i or -r is specified.
.np
If -i is specified, then the checkpoint files are erased and
&edvi starts up normally.  The .tmp files that contain the
lost files still remain in the backup directory, however.
.np
If -r is specified, then &edvi recovers the lost files.  Note that
the recovered files must be saved in order to overwrite the original
copy.

