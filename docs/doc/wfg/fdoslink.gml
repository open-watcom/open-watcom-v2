.chap Linking with External Subprograms
.*
&product provides a facility that allows access to source and object
code that is not explicitly included in the program you are compiling.
Source code must be contained in files with extension
:FNAME.&cxt.:eFNAME..
A library of source code typically consists of a directory containing
all the source modules that form the library.
This source code is accessed through the use of the
.ev LIBRARY
environment variable.
.np
Object code may either be contained in files with extension
:FNAME.&obj:eFNAME.
or in object libraries, files with extension
:FNAME.&lib.:eFNAME..
Object and library files are accessed by using the "LINK" compiler
option.
.*
.section Libraries of FORTRAN Source Code
.*
.np
In order for &cmpname to locate source files, the &setcmdup command
must be used to define the
.ev LIBRARY
environment variable to a list of directories, each separated by a
semicolon.
The general form of the &setcmdup command to be used is as follows.
.mbigbox
&setcmd library=<source_dir>;<source_dir>...;<source_dir>
.embigbox
.synote
.mnote source_dir
is a directory containing FORTRAN source code.
.endnote
.np
The directories specified in the
.ev LIBRARY
environment variable are searched when an undefined symbol has been
found in your program.
Directories are searched in the order they were specified in the
&setcmdup command.
When an undefined symbol, say
.id SYM,
is detected, &cmpname will attempt to resolve it.
First, if the
.ev LIBRARY
environment variable is not set, an error is issued indicating the
symbol is undefined.
If it is set, each directory is examined in the following way.
.autopoint
.point
&cmpname will attempt to locate the file
:FNAME.sym&cxt:eFNAME..
It this file exists, it will be compiled and linked with the program.
If this file does not exist, the next directory specification in the
.ev LIBRARY
environment variable will be searched.
.point
When all directory specifications in the
.ev LIBRARY
environment variable have been processed and the reference to
.id SYM
has not been resolved, an error message will be issued.
.endpoint
.np
Let us go through an example that demonstrates how to access a library
of FORTRAN source code.
Consider the following program.
.millust begin
    CALL PLOTINIT()
    DO 10 X = -5.0, 5.0, .1
        CALL PLOTPNT( X, X ** 2 )
10  CONTINUE
    CALL PLOT()
    END
.millust end
.np
The above program uses three subroutines, namely
.id PLOTINIT, PLOTPNT,
and
.id PLOT.
If we were to compile this program, an error message for each
subroutine would be issued indicating that they are undefined.
.np
Suppose that the three subroutines used in the above program are part
of a plotting library.
Assume and that the source code for the plotting library is contained
in the files
:FNAME.plotinit&cxt:eFNAME., :FNAME.plotpnt&cxt:eFNAME., and
:FNAME.plot&cxt:eFNAME., and that these files are contained in the
directory :FNAME.&dr3.&pc.plot&pc.source:eFNAME..
Notice that the file names for the files match the name of the
subroutines.
This is required if &cmpname is to be able to resolve the references
to the subroutines.
.remark
If you are running under DOS or are using a FAT file system under
OS/2, the subroutine name will be truncated to eight characters.
.eremark
.np
Let us now show how to resolve these undefined references.
Since the source code is contained in a single directory, namely
:FNAME.&dr3.&pc.plot&pc.source:eFNAME., we set the
.ev LIBRARY
environment variable, using the &setcmdup command, as in the following
command.
.millust begin
&prompt.&setcmd library=&dr3.&pc.plot&pc.source
.millust end
.np
Assume the above program is contained in the file
:FNAME.plotsamp&cxt:eFNAME..
The following command will compile the program successfully.
.millust begin
&prompt.&cmpcmd plotsamp
.millust end
.*
.section Libraries of Object Code
.*
.np
In addition to having libraries of FORTRAN source code, it is also
possible to have libraries of object code.
Since &cmpname can produce object files, let us create object files
from the three source files that are part of the plotting library
discussed in the previous section.
The following commands will compile our source code and generate
object files.
.millust begin
&prompt.&cmpcmd &sw.object plotinit
&prompt.&cmpcmd &sw.object plotpnt
&prompt.&cmpcmd &sw.object plot
.millust end
.np
Three object files will have been created:
:FNAME.plotinit&obj:eFNAME., :FNAME.plotpnt&obj:eFNAME., and
:FNAME.plot&obj:eFNAME..
.np
:I1.&cmpcmdup options:I2.link
In order to link these object files with the sample plotting program
discussed in the previous section, we must use the following form of
the "LINK" option.
.millust begin
/link=<link_file>
.millust end
.np
.mono <link_file>
is a link file that allows you to specify object files to be included
in your application and/or library files to be searched when resolving
undefined references.
Object files are specified by using the "FILE" directive in the link
file. Library files are specified by using the "LIBRARY" directive.
.np
:I1.FILE directive in link files
:I1.link files:I2.FILE directive
The "FILE" directive can be used to specify additional object files
and library modules that are to be linked with your program.
The format of the "FILE" directive (short form "F") is as follows.
.mbigbox
    FILE obj_spec{,obj_spec}

    obj_spec ::= obj_file[(obj_module)]
                       | library_file[(obj_module)]
.embigbox
.synote
.mnote obj_file
is a file specification for the name of an object file.
If no file extension is specified, a file extension of
:FNAME.&obj:eFNAME. is assumed.
.mnote library_file
is a file specification form the name of a library file.
Note that the file extension of the library file (usually
:FNAME.&lib:eFNAME.)
must be specified; otherwise an object file will be assumed.
When a library file is specified, all modules in the library are
included in the program whether they are needed or not.
.mnote obj_module
is the name of an object module defined in an object or library file.
.esynote
.np
:I1.LIBRARY linker directive
:I1.directives:I2.LIBRARY
The "LIBRARY" directive is used to specify the library files to be
searched when resolving references to undefined symbols.
The format of the "LIBRARY" directive (short form "L") is as follows.
.mbigbox
    LIBRARY library_file{,library_file}
.embigbox
.synote
.mnote library_file
is a file specification for the name of a library file.
If no file extension is specified, a file extension of
:FNAME.&lib:eFNAME. is assumed.
.esynote
.np
Let us continue with our plotting example.
Assume that the object files we created using &cmpname are located in
the directory
:FNAME.&dr3.&pc.plot&pc.obj:eFNAME. and the following link file,
called
:FNAME.plot.lnk:eFNAME.,
contains the following directives.
.millust begin
file &dr3.&pc.plot&pc.obj&pc.plotinit&obj
file &dr3.&pc.plot&pc.obj&pc.plotpnt&obj
file &dr3.&pc.plot&pc.obj&pc.plot&obj
.millust end
.np
The following command will compile, link and run the program
successfully.
.millust begin
&prompt.&cmpcmd /link=plot.lnk /run plotsamp
.millust end
.np
The linking phase is actually performed by invoking the &lnkname..
The contents of the link file specified with the "LINK" option are
actually linker directives that will be processed by the &lnkname..
We have previously described the "FILE" and "LIBRARY" linker directive
and how they are to be used to link object files with your
application.
.np
During the linking process, the library file :FNAME.wf386&lib:eFNAME.
(if you are running :FNAME.&cmpcmd.&exe:eFNAME.) or the library file
:FNAME.wf387&lib:eFNAME. (if you are running
:FNAME.&cmp2cmd.&exe:eFNAME.) will be required.
These library files are located in the
:FNAME.&pathnam.&libdir.&pc.dos:eFNAME. directory if you are running
on a DOS-hosted system or in the
:FNAME.&pathnam.&libdir.&pc.os2:eFNAME. directory if you are running
on an OS/2-hosted system.
The
.ev WATCOM
environment variable must be set to :FNAME.&pathnam:eFNAME., the
directory in which the software was installed, in order for these
library files to be located.
.np
Let us now create a library file containing the object files we used
in the above example.
We can use the &libname as in the following example.
.millust begin
&prompt.&libcmd plot +plotinit +plotpnt +plot
.millust end
.np
For more information on the &libname, see the chapter
entitled "The &libname".
.np
A library file called
:FNAME.plot&lib:eFNAME. will be created.
Assume the library is located in the same directory as the object
files.
If we wish to use a library file instead of explicitly specifying the
object files, the contents of
:FNAME.plot.lnk:eFNAME. could be modified as follows.
.millust begin
library &dr3.&pc.plot&pc.obj&pc.plot&lib
.millust end
.np
When we compile our program, &cmpname will extract the object files
from the library file to resolve the references to our plotting
library.
.*
.section Using the WATCOM FORTRAN 77 Graphics Library
.*
.remark
The WATCOM FORTRAN 77 graphics library is only available for DOS.
.eremark
.np
The WATCOM FORTRAN 77 graphics library is an object library of
subroutines and functions that allows you to develop graphical FORTRAN
applications.
The subroutines and functions available to the programmer are
documented in the manual called "WATCOM FORTRAN 77 Graphics Library".
This section describes how to compile and link an application that
uses this graphics library.
.np
Support for the WATCOM FORTRAN 77 graphics library includes an object
library called :FNAME.graph&lib:eFNAME. and a number of FORTRAN
include files called :FNAME.graph&hxt:eFNAME., :FNAME.pg&hxt:eFNAME.,
:FNAME.graphapi&hxt:eFNAME., and :FNAME.pgapi&hxt:eFNAME..
The files :FNAME.graph&hxt:eFNAME. and :FNAME.pg&hxt:eFNAME. define
the data structures and constants that are to be used for developing
graphical applications.
These files are to be included in each subprogram unit that uses any
of the graphics functions or subroutines.
The files :FNAME.graphapi&hxt:eFNAME. and :FNAME.pgapi&hxt:eFNAME.
define the calling conventions for each of the graphics functions and
subroutines and should be included at the top of each source module.
.np
Assuming you installed &cmpname in the :FNAME.&pathnam:eFNAME.
directory, the library file :FNAME.graph&lib:eFNAME. is located in the
:FNAME.&pathnam.&libdir.&pc.dos:eFNAME. directory and the
include files are located in the
:FNAME.&pathnam.&pc.src&pc.fortran&pc.dos:eFNAME. directory.
.np
Consider the following sample FORTRAN graphical application.
It displays the string "WATCOM Graphics" on the screen.
.millust begin
    include 'graphapi.fi'
    include 'graph.fi'
    call _setvideomode( _TEXTC80 )
    call _settextposition( 10, 30 )
    call _outtext( 'WATCOM Graphics'c )
    pause
    call _setvideomode( _DEFAULTMODE )
    end
.millust end
.np
Notice that the character constant in the above program is immediately
followed by the letter "c".
This causes the character data to be terminated with a NUL character,
a requirement when passing character data to the
.id _outtext
graphics function.
.np
In order to compile this program, we must set the
.ev FINCLUDE
environment variable to include the directory containing the graphics
include files.
This is achieved by the following command.
.millust begin
&prompt.&setcmd finclude=&pathnam.&pc.src&pc.fortran&pc.dos
.millust end
.np
We can now compile, link and run the above program.
Assuming it is contained in the file
:FNAME.text&cxt:eFNAME., we can issue the following command.
.millust begin
&prompt.&cmpcmd /link /run text
.millust end
.np
Notice that we did not specify a link file when we used the "LINK"
option.
Normally, we would have created a link file containing a "LIBRARY"
directive for the library file :FNAME.graph&lib:eFNAME. and specified
this file with the "LINK" option.
If you examine the contents of the file :FNAME.graphapi&hxt:eFNAME.,
you will notice that the first line in the file is the following.
.millust begin
c$pragma library graph
.millust end
.np
This form of the PRAGMA compiler directive is used to specify the
library :FNAME.graph&lib:eFNAME. so that it will be searched when
resolving undefined references.
In addition to the graphics library, the library file
:FNAME.wf386&lib:eFNAME. (if you are running
:FNAME.&cmpcmd.&exe:eFNAME.) or the library file
:FNAME.wf387&lib:eFNAME. (if you are running
:FNAME.&cmp2cmd.&exe:eFNAME.)
will be required.
These library files are located in the
:FNAME.&pathnam.&libdir.&pc.dos:eFNAME. directory.
The library file :FNAME.graph&lib:eFNAME. is also located in the
:FNAME.&pathnam.&libdir.&pc.dos:eFNAME.
Note that the same graphics library is used regardless of the compiler
you are using (:FNAME.&cmpcmd.&exe:eFNAME. or
:FNAME.&cmp2cmd.&exe:eFNAME.).
The
.ev WATCOM
environment variable must be set to :FNAME.&pathnam:eFNAME., the
directory in which the software was installed, in order for these
library files to be located.
