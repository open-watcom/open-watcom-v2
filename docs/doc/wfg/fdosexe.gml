.chap Compiling, Linking, and Executing
.*
.section Introduction
.*
.np
&product can operate in the following ways.
.autopoint
.point
compile, link and execute your source program in a single step
.point
generate an object file
.point
generate an executable file
.endpoint
.np
We have dealt with the single-step approach to compiling, linking and
executing your program in previous sections.
This chapter will mainly deal with the generation of object files and
executable files.
.*
.section Generating Object Files
.*
.np
The "OBJECT" option instructs the compiler to generate an object file.
These object files can later be linked into applications that wish to
use the subprograms they define.
See the chapter entitled "Linking with External Subprograms" for more
information on this topic.
.np
Suppose we have three source files,
:FNAME.sub1&cxt:eFNAME., :FNAME.sub2&cxt:eFNAME. and :FNAME.sub3&cxt:eFNAME..
In order to generate object files for each of these files, we can issue the
following commands.
.millust begin
&prompt.&cmpcmd/object sub1
&prompt.&cmpcmd/object sub2
&prompt.&cmpcmd/object sub3
.millust end
.*
.section Generating Executable Files
.*
.np
Once a FORTRAN 77 program has been developed, it may be desirable to
generate the executable form of the program so that it can be made
available to others who do not have the &cmpname compiler.
.ix 'executable files'
&cmpname will generate an executable file when the "LINK" option is
specified on the command line.
.np
Suppose that an application is contained in the file
:FNAME.sample&cxt:eFNAME..
To generate an executable file issue the following command.
.millust begin
&prompt.&cmpcmd/link sample
.millust end
.pc
The executable image of the program will be contained in the file
:FNAME.sample&exe:eFNAME. in the current directory of the default disk.
Note that the program is not executed when an executable file is
generated.
To run the program, simply type:
.millust begin
&prompt.sample
.millust end
.np
The executable file can be generated and executed in one step by
specifying the "RUN" option in addition to the "LINK" option as in the
following example.
.millust begin
&prompt.&cmpcmd/link/run sample
.millust end
.autonote Notes:
.note
You should ensure that there is enough unused space on your disk
for the resultant executable file.
.note
Since the application is not executed by &cmpname when an executable
file is requested, certain compile-time options will be ignored (e.g.,
LOGIO).
.endnote
