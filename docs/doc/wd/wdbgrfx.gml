.chap *refid=vidrfx Remote File Operations (DOS, NT, OS/2 Only)
.*
.np
.ix 'RFX utility program'
.ix 'RFX see also' 'remote file operations'
.ix 'remote file operations'
.ix 'file operations' 'remote'
Use the
.bd R
.ct emote
.bd F
.ct ile e
.bd X
.ct change program (RFX) to manipulate files on a personal computer
which is connected to your personal computer using a debugger remote
link. You should consult the chapter entitled :HDREF refid='vidrem'.
to familiarize yourself with the concepts of remote debugging.
The types of file operations that are supported on both local
and remote machines include:
.autonote
.note
creating, listing and removing directories
.note
setting the current drive and directory
.note
display, renaming, erasing, and copying files (including PC to PC file
transfers).
.endnote
.np
.ix 'RFX' 'running'
To run RFX, set up your machines as if you are about to do remote debugging.
Start the remote debug server, then start RFX using the the following syntax.
.mbigbox
RFX trap_file[;trap_parm] [rfx_cmd]
.embigbox
.np
:cmt..ix 'trap file'
:cmt..ix 'remote file operations' 'trap file'
The name of a trap file must be specified when running RFX.
.refalso vidrem
.np
See the section entitled :HDREF refid='rfxfile'. for an explanation of
remote and local file names.
.np
You are now ready to copy files back and forth between machines.
.*
.section RFX Commands
.*
.np
When RFX is run without specifying a command, the DOS, NT or OS/2 prompt
will change as illustrated in the following example.
.exam begin
Mon  11-06-1989 15:17:05.84 E:\DOC\UG
E>rfx par
[RFX] Mon  11-06-1989 15:17:12.75 @LE:\DOC\UG
[RFX] E>
.exam end
.pc
Note that the current drive specifier "E" in "E:\DOC\UG" has changed
to "@LE" indicating that the current drive is the local "E" drive.
.np
Any command can be typed in response to the prompt.
RFX recognizes a special set of commands and passes all others on to
DOS, NT or OS/2 for processing.
The following sections describe RFX commands.
.*
.section Set Current Drive - drive:
.*
.mbigbox
drive:
.embigbox
.np
:cmt..ix 'locale' 'local'
:cmt..ix 'locale' 'remote'
The current drive and locale can be set using this command.
The "@L" or "@R" prefix may be used to specify the locale (local or
remote).
.exam begin
d:
.exam end
.pc
Make the "D" disk of the current locale (local or remote) the current
drive.
Since the locale is not specified, it remains unchanged.
.exam begin
@rc:
.exam end
.pc
Make the "C" disk of the remote machine the current drive.
Both locale and disk are specified.
.exam begin
@le:
.exam end
.pc
Make the "E" disk of the local machine the current drive.
Both locale and disk are specified.
.*
.section Change Directory - CHDIR, CD
.*
.mbigbox
chdir dir_spec
cd dir_spec
.embigbox
.np
This command may be used to change the current directory of any disk
on the local or remote machine.
.sy CD
is a short form for
.sy CHDIR.
The "@L" or "@R" prefix may be used to specify the locale (local or
remote).
.exam begin
cd \tmp
.exam end
.pc
Make the "TMP" directory of the current drive the current directory.
.exam begin
cd d:\etc
.exam end
.pc
Make the "ETC" directory of the "D" disk of the current locale (local
or remote) the current directory of that drive.
.exam begin
cd @rc:\demo
.exam end
.pc
Make the "DEMO" directory of the "C" disk of the remote machine the
current directory of that drive.
Both locale and disk are specified.
.exam begin
cd @le:test
.exam end
.pc
Make the "TEST" subdirectory of the current directory of the "E" disk
of the local machine the current directory of that drive.
Both locale and disk are specified.
.*
.section Copy Files - COPY
.*
.mbigbox
copy [/s] src_spec [dst_spec] [/s]
.embigbox
.np
The
.sy COPY
command operates in a manner very similar to the DOS "COPY" and
"XCOPY" commands.
Files may be copied from the local machine to the local or remote
machine.
Similarly files may be copied from the remote machine to the local
or remote machine.
If
.sy /s
is specified then subdirectories are copied as well.
Directories will be created as required for the destination files.
If
.sy dst_spec
is not specified then the default destination will be the current
directory of the other locale (i.e., remote, if the file's locale
is local or, local, if the file's locale is remote).
.exam begin
copy *.for @rd:\tmp
.exam end
.pc
All files of type "FOR" in the current directory are copied to the
"TMP" directory of the "D" disk on the remote machine.
If the current locale is the local machine then files are copied
from the local machine to the remote machine.
If the current locale is the remote machine then files are copied from
the remote machine to the remote machine.
.remark
If your default drive is set to one of the disks on the local
machine then the locale is local (e.g., @LC:, @LD:, @LE:, etc.).
If your default drive is set to one of the disks on the remote
machine then the locale is remote (e.g., @RC:, @RD:, @RE:, etc.).
If your DOS, NT or OS/2 prompt contains the current drive and directory
then it will be easy to identify which locale is current.
.eremark
.exam begin
copy @rd:\tmp\*.for
.exam end
.pc
All files of type "FOR" in the "TMP" directory of the "D" disk on the
remote machine are copied to the current directory of the local
machine.
Whenever a destination is not specified, the current directory of the
opposite locale is used.
If the source locale is the remote machine then files are copied from
the remote to the local machine.
If the source locale is the current machine then files are copied from
the local to the remote machine.
.exam begin
copy @rc:\watcom\*.* /s
.exam end
.pc
All files and subdirectories of the "WATCOM" directory of the "C" disk
on the remote machine are copied to the current directory of the local
machine.
Whenever a destination is not specified, the current directory of the
opposite locale is used.
If the source locale is the remote machine then files are copied from
the remote to the local machine.
If the source locale is the current machine then files are copied from
the local to the remote machine.
Subdirectories are created as required.
.remark
The "COPY" command is most effectively used when copying files from
one machine to the other.
Copying of large amounts of files from one place on the remote machine
to another place on the remote machine could be done more effectively
using the remote machine's DOS, NT or OS/2.
This would eliminate the transfer of data from the remote machine to
the local machine and back to the remote machine.
.eremark
.*
.section List Directory - DIR
.*
.mbigbox
dir [/w] dir_spec [/w]
.embigbox
.np
This command may be used to list the directories of any disk on the
local or remote machine.
Any of the DOS, NT or OS/2 "wild card" characters ("?" and "*") may be
used.
If
.sy /w
is specified then file names are displayed across the screen ("wide")
and the file creation date and time are omitted.
.exam begin
dir \tmp
.exam end
.pc
List the names of files in the "TMP" directory of the current drive.
.exam begin
dir d:\etc
.exam end
.pc
List the names of files in the "ETC" directory of the "D" disk of the
current locale (local or remote).
.exam begin
dir @rc:\demo
.exam end
.pc
List the names of files in the "DEMO" directory of the "C" disk of the
remote machine.
Both locale and disk are specified.
.exam begin
dir @le:test
.exam end
.pc
List the names of files in the "TEST" subdirectory of the current
directory of the "E" disk of the local machine.
If no "TEST" subdirectory exists then the names of all files named
"TEST" will be listed.
Both locale and disk are specified.
.exam begin
dir @le:test.*
.exam end
.pc
List the names of all files named "TEST" in the current directory of
the "E" disk of the local machine.
Both locale and disk are specified.
.*
.section Erase File - ERASE, DEL
.*
.mbigbox
erase [/s] file_spec [/s]
del [/s] file_spec [/s]
.embigbox
.np
This command may be used to erase files from the directories of any
disk on the local or remote machine.
.sy DEL
is a short form for
.sy ERASE.
Any of the DOS, NT or OS/2 "wild card" characters ("?" and "*") may be
used.
If
.sy /s
is specified then subdirectories are also processed.
.exam begin
erase \tmp\*.*
.exam end
.pc
Erase all the files in the "TMP" directory of the current drive.
.exam begin
erase d:\etc\*.lst
.exam end
.pc
Erase all files of type "LST" in the "ETC" directory of the "D" disk
of the current locale (local or remote).
.exam begin
erase @rc:\demo\*.obj
.exam end
.pc
Erase all files of type "OBJ" in the "DEMO" directory of the "C" disk
of the remote machine.
Both locale and disk are specified.
.exam begin
erase @le:trial.*
.exam end
.pc
Erase all files named "TRIAL" of any type in the current directory of
the "E" disk of the local machine.
Both locale and disk are specified.
.*
.section Exit from RFX - EXIT
.*
.mbigbox
exit
.embigbox
.np
This command may be used to exit from RFX and return to the invoking
process.
.*
.section Make Directory - MKDIR, MD
.*
.mbigbox
mkdir dir_spec
md dir_spec
.embigbox
.np
This command may be used to create a directory on any disk on the
local or remote machine.
.sy MD
is a short form for
.sy MKDIR.
The "@L" or "@R" prefix may be used to specify the locale (local or
remote).
.exam begin
md \tmp
.exam end
.pc
Create a "TMP" directory in the root of the current drive.
.exam begin
md d:\etc
.exam end
.pc
Create an "ETC" directory in the root of the "D" disk of the current
locale (local or remote).
.exam begin
md @rc:\demo
.exam end
.pc
Create a "DEMO" directory in the root of the "C" disk of the remote
machine.
Both locale and disk are specified.
.exam begin
md @le:test
.exam end
.pc
Create a "TEST" subdirectory in the current directory of the "E" disk
of the local machine.
Both locale and disk are specified.
.*
.section Rename - RENAME, REN
.*
.mbigbox
rename file_spec new_name
ren file_spec new_name
.embigbox
.np
This command may be used to rename a file in any directory on any disk
on the local or remote machine.
.sy REN
is a short form for
.sy RENAME.
The "@L" or "@R" prefix may be used to specify the locale (local or
remote).
Unlike the DOS "RENAME" command, a file can be moved to a different
directory if the directory is specified in
.sy new_name.
.exam begin
ren test.tmp test1.tmp
.exam end
.pc
Rename the file "TEST.TMP" in the current directory of the current
drive to "TEST1.TMP".
.exam begin
ren d:\etc\test.tmp test1.tmp
.exam end
.pc
Rename the file "TEST.TMP" in the "ETC" directory of the "D" disk of
the current locale (local or remote) to "TEST1.TMP".
.exam begin
ren @rc:\demo\test.tmp test1.tmp
.exam end
.pc
Rename the file "TEST.TMP" in the "DEMO" directory of the "C" disk of
the remote machine to "TEST1.TMP".
Both locale and disk are specified.
.exam begin
ren @le:trial.dat trial1.dat
.exam end
.pc
Rename the file "TRIAL.DAT" in the current directory of the "E" disk of
the local machine to "TRIAL1.DAT".
Both locale and disk are specified.
.exam begin
ren @le:trial.dat ..\trial1.dat
.exam end
.pc
Rename the file "TRIAL.DAT" in the current directory of the "E" disk of
the local machine to "TRIAL1.DAT" and move it to the parent directory.
Both locale and disk are specified.
.*
.section Remove Directory - RMDIR, RD
.*
.mbigbox
rmdir [/s] dir_spec [/s]
rd [/s] dir_spec [/s]
.embigbox
.np
This command may be used to remove one or more directories on any disk
on the local or remote machine.
.sy RD
is a short form for
.sy RMDIR.
The "@L" or "@R" prefix may be used to specify the locale (local or
remote).
If
.sy /s
is specified then subdirectories are also removed.
Before a directory can be removed, it must not contain any files.
.exam begin
rd \tmp
.exam end
.pc
Remove the "TMP" directory from the root of the current drive.
.exam begin
rd d:\etc
.exam end
.pc
Remove the "ETC" directory from the root of the "D" disk of the current
locale (local or remote).
.exam begin
rd @rc:\demo
.exam end
.pc
Remove the "DEMO" directory from the root of the "C" disk of the
remote machine.
Both locale and disk are specified.
.exam begin
rd @le:test
.exam end
.pc
Remove the "TEST" subdirectory from the current directory of the "E"
disk of the local machine.
Both locale and disk are specified.
.*
.section Display File Contents - TYPE
.*
.mbigbox
type dir_spec
.embigbox
.np
This command may be used to list the contents of a file on any disk on
the local or remote machine.
The "@L" or "@R" prefix may be used to specify the locale (local or
remote).
Unlike the DOS "TYPE" command, DOS, NT or OS/2 "wild card" characters ("?"
or "*") may be used.
.exam begin
type \tmp\test.dat
.exam end
.pc
List the contents of the file "TEST.DAT" in the "TMP" directory of the
current drive.
.exam begin
type d:\etc\*.lst
.exam end
.pc
List the contents of all files of type "LST" in the "ETC" directory of
the "D" disk of the current locale (local or remote).
.exam begin
type @rc:\demo\test.c
.exam end
.pc
List the contents of the file "TEST.C" in the "DEMO" directory of the
"C" disk of the remote machine.
Both locale and disk are specified.
.exam begin
type @le:trial.*
.exam end
.pc
List the contents of all files named "TRIAL" of any type in the
current directory of the "E" disk of the local machine.
Both locale and disk are specified.
.*
.section RFX Sample Session
.*
.np
Run serial port server on remote PC specifying a port 1 and a maximum
baud rate of 38,400 baud.
.millust begin
  Tue 11-07-1989 15:29:24.19 C:\
  C>serserv 1.38
.millust end
.np
Run RFX on local PC.
.millust begin
  Tue 11-07-1989 15:30:53.18 E:\DOC\UG
  E>rfx ser
  Link at 38400 baud
.millust end
.np
List directory of remote machine's "F" drive.
.millust begin
  [RFX] Tue  11-07-1989 15:30:59.33 @LE:\DOC\UG
  [RFX] E>dir @rf:
  CLIB         <DIR>     02-01-89  06:43p
  MATH         <DIR>     02-01-89  06:51p
  PCLINT       <DIR>     03-09-89  04:05p
          3 File(s)  16748544 bytes free
.millust end
.np
Switch to remote machine's "F" drive and list files.
.millust begin
  [RFX] Tue  11-07-1989 15:31:11.80 @LE:\DOC\UG
  [RFX] E>@rf:
  [RFX] Tue  11-07-1989 15:31:22.51 @RF:\
  [RFX] F>dir
  CLIB         <DIR>     02-01-89  06:43p
  MATH         <DIR>     02-01-89  06:51p
  PCLINT       <DIR>     03-09-89  04:05p
          3 File(s)  16748544 bytes free
.millust end
.np
Change to subdirectory and list files.
.millust begin
  [RFX] Tue  11-07-1989 15:31:27.73 @RF:\
  [RFX] F>cd clib
  [RFX] Tue  11-07-1989 15:31:47.83 @RF:\CLIB
  [RFX] F>dir
  .            <DIR>     02-01-89  06:43p
  ..           <DIR>     02-01-89  06:43p
  MKCLIB   BAT       95  12-20-88  04:24p
  MKMODEL  BAT      128  02-01-89  04:32p
  MDEF     INC     1831  12-08-88  12:23p
  STRUCT   INC     2487  12-20-88  05:45p
  CLIB     MIF      559  02-01-89  04:42p
  H            <DIR>     02-01-89  06:44p
  SCSD         <DIR>     02-01-89  06:44p
  BCSD         <DIR>     02-01-89  06:44p
  SCBD         <DIR>     02-01-89  06:44p
  BCBD         <DIR>     02-01-89  06:44p
  BCHD         <DIR>     02-01-89  06:44p
  ANSI         <DIR>     02-01-89  06:44p
  DOS          <DIR>     02-01-89  06:47p
  A            <DIR>     02-01-89  06:50p
  C            <DIR>     02-01-89  06:50p
  CGSUPP       <DIR>     02-01-89  06:50p
         18 File(s)  16748544 bytes free
.millust end
.np
List directory of local machine.
.millust begin
  [RFX] Tue  11-07-1989 15:31:51.57 @RF:\CLIB
  [RFX] F>dir @l..\tools\*.c
  CALENDAR C       4378  04-17-89  08:17p
  CLRSCRN  C        233  04-17-89  08:17p
  ERR      C        198  04-17-89  08:30p
  MAIN     C        142  04-17-89  09:14p
  SAMPLE1  C         83  04-14-89  03:56p
  SAMPLE2  C         83  04-14-89  03:57p
  SAMPLE3  C         86  04-14-89  03:58p
  SAMPLE4  C        132  04-14-89  04:05p
  SAMPLE5  C        131  04-14-89  04:05p
  SUB1     C        108  11-03-89  05:11p
  SUB2     C        108  11-03-89  05:11p
  TEST     C        236  05-08-89  07:02p
         12 File(s)   8292352 bytes free
.millust end
.np
Copy file from local machine to current directory of remote machine.
.millust begin
  [RFX] Tue  11-07-1989 15:32:21.99 @RF:\CLIB
  [RFX] F>copy @l..\tools\test.c
  E:..\TOOLS\TEST.C
          1 Files copied        0 Directories created
.millust end
.np
Confirm presence of file.
.millust begin
  [RFX] Tue  11-07-1989 15:32:41.22 @RF:\CLIB
  [RFX] F>dir *.c
  TEST     C        236  05-08-89  07:02p
          1 File(s)  16746496 bytes free
.millust end
.np
Leave RFX.
.millust begin
  [RFX] Tue  11-07-1989 15:32:46.99 @RF:\CLIB
  [RFX] F>exit

  Tue 1989-11-07 15:32:57.20 E:\DOC\UG
  E>
.millust end
