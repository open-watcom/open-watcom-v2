The
.kw INCLUDE
compiler directive or
.kw INCLUDE
statement
may be used to imbed source code into the file being compiled.
Either form may be used.
.exam begin
*$INCLUDE DOS.FI

        INCLUDE 'DOS.FI'
.exam end
.np
When the
.kw INCLUDE
statement is used the name of the file must be placed inside single
quotes (apostrophes).
The file name may include drive, path, and file extension.
The default file extension is :FNAME.&cxt:eFNAME..
.np
It is not necessary to include the drive and path specifiers in the
file specification when the file resides on a different drive or in a
different directory.
&cmpname provides a mechanism for looking up include files which may
be located in various directories and disks of the computer system.
When the drive and path are omitted from a file specification,
&cmpname searches directories for include files in the following
order.
.ix 'include file' 'searching'
.ix 'header file' 'searching'
.ix 'source file' 'searching'
.autopoint
.point
First, the current directory is searched.
.point
.ix 'INCPATH option'
Secondly, each directory listed with the "INCPath" option is searched
(in the order that they were specified).
.point
Thirdly, each directory listed in the
.ev &incvarup
environment variable is searched (in the order that they were
specified).
.endpoint
.np
The compiler will search the directories listed with the "INCPath"
option or in the
.ev &incvarup
environment variable in a manner analogous to that which used by the
operating system when searching for programs by using the
.ev PATH
environment variable.
.np
The "INCPath" option takes the following form.
.millust begin
&sw.INCPath=[&dr4]path;[&dr4]path...
.millust end
.np
The "SET" command is used to define an
.ev &incvarup
environment variable that contains a list of directories.
.ix 'SET' '&incvarup environment variable'
A command of the form
.millust begin
SET &incvarup=[&dr4]path;[&dr4]path...
.millust end
.pc
is issued before running &cmpname the first time.
The brackets indicate that the drive :FNAME.&dr4:eFNAME. is optional and the
ellipsis indicates that any number of paths may be specified.
.np
We illustrate the use of the
.kw INCLUDE
statement in the following example.
.code begin
      subroutine ClearScreen()
      implicit none
      include 'dos.fi'
      integer VIDEO_CALL, SCROLL_UP
      parameter (VIDEO_CALL=16, SCROLL_UP=6)
      DS = ES = FS = GS = 0     ! for safety on 386 DOS extender
      AH = SCROLL_UP            ! scroll up
      AL = 0                    ! blank entire window
      CX = 0                    ! set row,column of upper left
      DX = 24*256 + 80          ! set row,column of lower right
      BH = 7                    ! attribute "white on black"
      call fintr( VIDEO_CALL, regs )
      end
.code end
.pc
The third line of this subroutine contains an
.kw INCLUDE
statement for the file
.fi DOS.FI.
If the above source code is stored in the file
.fi CLRSCR.&langsuffup
in the current directory then we can issue the following commands to
compile the application.
.millust begin
C>set &incvar=c:&pathnam\src\fortran\dos
C>&ccmd16 clsscr
.millust end
.pc
.ix 'SET' '&incvarup environment variable'
In the above example, the "SET" command is used to define the
.ev &incvarup
environment variable.
It specifies that the
.fi &pathnamup\SRC\FORTRAN\DOS
directory is to be searched for include files
that cannot be located in the current directory and that have no
drive or path specified.
The advantage of the
.ev &incvarup
environment variable is that drives and paths can be omitted from the
INCLUDE file specifications in the source code.
This allows the source code to be independent of the disk/directory
structure of your computer system.
