.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
:set symbol="function"  value="function".
:set symbol="functions" value="functions".
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
:set symbol="function"  value="subprogram".
:set symbol="functions" value="subprograms".
.do end
.*
.chap *refid=mmodels Memory Models
.*
.section Introduction
.*
.np
This chapter describes the various memory models supported by
&cmpname..
Each memory model is distinguished by two properties; the code model
used to implement &function calls and the data model used to reference
data.
.*
.section Code Models
.*
.np
There are two code models;
.autopoint
.point
the small code model and
.point
the big code model.
.endpoint
.pc
:I1.small code model
:I1.code models:I2.small
:I1.near call
A small code model is one in which all calls to &functions are made
with
.us near calls.
In a near call, the destination address is 16 bits and is relative
to the segment value in segment register CS.
Hence, in a small code model, all code comprising your program,
including library &functions, must be less than 64K.
.if '&lang' eq 'FORTRAN 77' .do begin
&cmpname does not support the small code model.
.do end
.np
:I1.big code model
:I1.code models:I2.big
:I1.far call
A big code model is one in which all calls to &functions are made with
.us far calls.
In a far call, the destination address is 32 bits (a segment value and
an offset relative to the segment value).
This model allows the size of the code comprising your program to
exceed 64K.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.autonote Notes:
.note
If your program contains less than 64K of code, you should use a memory
model that employs the small code model.
This will result in smaller and faster code since near calls are
smaller instructions and are processed faster by the CPU.
.endnote
.do end
.*
.section Data Models
.*
.np
There are three data models;
.autopoint
.point
the small data model,
.point
the big data model and
.point
the huge data model.
.endpoint
.pc
:I1.small data model
:I1.data models:I2.small
A small data model is one in which all references to data are made
with
.us near pointers.
Near pointers are 16 bits;
all data references are made relative to the segment value in
segment register DS.
Hence, in a small data model, all data comprising your program must be
less than 64K.
.np
:I1.big data model
:I1.data models:I2.big
A big data model is one in which all references to data are made with
.us far pointers.
Far pointers are 32 bits
(a segment value and an offset relative to the segment value).
This removes the 64K limitation on data size imposed by the small
data model.
However, when a far pointer is incremented, only the offset is adjusted.
&cmpname assumes that the offset portion of a far pointer will not
be incremented beyond 64K.
The compiler will assign an object to a new segment if the grouping
of data in a segment will cause the object to cross a segment boundary.
Implicit in this is the requirement that no individual object
exceed 64K bytes.
For example, an array containing 40,000 integers does not fit into
the big data model.
An object such as this should be described as
.us huge.
.np
:I1.huge data model
:I1.data models:I2.huge
A huge data model is one in which all references to data are made
with far pointers.
This is similar to the big data model.
However, in the huge data model, incrementing a far pointer will
adjust the offset
.us and
the segment if necessary.
The limit on the size of an object pointed to by a far pointer
imposed by the big data model is removed in the huge data model.
.autonote Notes:
.if '&lang' eq 'FORTRAN 77' .do begin
.note
The huge data model has the same characteristics as the big data
model, but formal array arguments are assumed to exceed 64K bytes.
You should use the huge data model whenever any arrays in your
application exceed 64K bytes in size.
.do end
.note
If your program contains less than 64K of data, you should use the
small data model.
This will result in smaller and faster code since references using
near pointers produce fewer instructions.
.note
The huge data model should be used only if needed.
The code generated in the huge data model is not very efficient since
a run-time routine is called in order to increment far pointers.
This increases the size of the code significantly and increases
execution time.
.endnote
.*
.section Summary of Memory Models
.*
.np
As previously mentioned, a memory model is a combination of a code model
and a data model.
The following table describes the memory models supported by &cmpname..
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
:I1.tiny memory model
:I1.memory models:I2.tiny
:I1.small memory model
:I1.memory models:I2.small
:I1.compact memory model
:I1.memory models:I2.compact
.do end
:I1.medium memory model
:I1.memory models:I2.medium
:I1.large memory model
:I1.memory models:I2.large
:I1.huge memory model
:I1.memory models:I2.huge
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.millust begin
Memory      Code        Data        Default     Default
Model       Model       Model       Code        Data
                                    Pointer     Pointer
--------    --------    --------    --------    --------
tiny        small       small       near        near
small       small       small       near        near
medium      big         small       far         near
compact     small       big         near        far
large       big         big         far         far
huge        big         huge        far         huge
.millust end
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.millust begin
Memory      Code        Data        Default     Default
Model       Model       Model       Code        Data
                                    Pointer     Pointer
--------    --------    --------    --------    --------
medium      big         small       far         near
large       big         big         far         far
huge        big         huge        far         huge
.millust end
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.if '&machsys' ne 'QNX' .do begin
.*
.section Tiny Memory Model
.*
.np
In the tiny memory model, the application's code and data must total
less than 64K bytes in size.
All code and data are placed in the same segment.
Use of the tiny memory model allows the creation of a COM file for
the executable program instead of an EXE file.
For more information, see the section entitled
"Creating a Tiny Memory Model Application" in this chapter.
.do end
.do end
.*
.section Mixed Memory Model
.*
.np
:I1.mixed memory model
:I1.memory models:I2.mixed
A mixed memory model application combines elements from the various
code and data models.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
A mixed memory model application might be characterized as one that
uses the
.kw near
.ct ,
.kw far
or
.kw huge
keywords when describing some of its &functions or data objects.
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
A mixed memory model application might be characterized as one that
includes arrays which are larger than 64K bytes.
.do end
.np
For example, a medium memory model application that uses some
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
far pointers to data
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
arrays which exceed 64K bytes in total size
.do end
can be described as a mixed memory model.
In an application such as this, most of the data is in a 64K segment
(DGROUP) and hence can be referenced with near pointers relative to
the segment value in segment register DS.
This results in more efficient code being generated and better
execution times than one can expect from a big data model.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
Data objects outside of the DGROUP segment are described with the
.kw far
keyword.
.do end
.*
.section Linking Applications for the Various Memory Models
.*
.np
Each memory model requires different run-time and floating-point
libraries.
Each library assumes a particular memory model and should be linked
only with modules that have been compiled with the same memory model.
The following table lists the libraries that are to be used to link an
application that has been compiled for a particular memory model.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.if '&machsys' eq 'QNX' .do begin
.millust begin
Memory  Run-time      Floating-Point  Floating-Point
Model   Library       Calls Library   Library (80x87)
------- --------      --------------  ---------------

small   clibs.lib     maths.lib       math87s.lib
                                      +(no)emu87.lib*

medium  clibm.lib     mathm.lib       math87m.lib
                                      +(no)emu87.lib*

compact clibc.lib     mathc.lib       math87c.lib
                                      +(no)emu87.lib*

large   clibl.lib     mathl.lib       math87l.lib
                                      +(no)emu87.lib*

huge    clibh.lib     mathh.lib       math87h.lib
                                      +(no)emu87.lib*
.millust end
.do end
.el .do begin
.millust begin
Memory  Run-time      Floating-Point  Floating-Point
Model   Library       Calls Library   Library (80x87)
------- --------      --------------  ---------------
tiny    CLIBS.LIB     MATHS.LIB       MATH87S.LIB
        +CSTART_T.OBJ                 +(NO)EMU87.LIB*

small   CLIBS.LIB     MATHS.LIB       MATH87S.LIB
                                      +(NO)EMU87.LIB*

medium  CLIBM.LIB     MATHM.LIB       MATH87M.LIB
                                      +(NO)EMU87.LIB*

compact CLIBC.LIB     MATHC.LIB       MATH87C.LIB
                                      +(NO)EMU87.LIB*

large   CLIBL.LIB     MATHL.LIB       MATH87L.LIB
                                      +(NO)EMU87.LIB*

huge    CLIBH.LIB     MATHH.LIB       MATH87H.LIB
                                      +(NO)EMU87.LIB*
.millust end
.do end
.np
* One of
.fi emu87.lib
or
.fi noemu87.lib
will be used with the 80x87 math
libraries depending on the use of the "fpi" (include emulation) or
.if &version gt 90 .do begin
"fpi87"
.do end
.el .do begin
.if '&machine' eq '80386' .do begin
"fpi87/fpi287"
.do end
.if '&machine' eq '8086' .do begin
"fpi87/fpi387"
.do end
.do end
(do not include emulation) options.
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.code begin
Library         Memory           Floating-point
                model            model
-------         ------           --------------
flibm.lib       /mm              /fpc
flibl.lib       /ml, /mh         /fpc
flib7m.lib      /mm              /fpi, /fpi87
flib7l.lib      /ml, /mh         /fpi, /fpi87
clibm.lib       /mm              /fpc, /fpi, /fpi87
clibl.lib       /ml, /mh         /fpc, /fpi, /fpi87
mathm.lib       /mm,             /fpc
mathl.lib       /ml, /mh         /fpc
math87m.lib     /mm,             /fpi, /fpi87
math87l.lib     /ml, /mh         /fpi, /fpi87
emu87.lib       /mm, /ml, /mh    /fpi
noemu87.lib     /mm, /ml, /mh    /fpi87
.code end
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.if '&machsys' ne 'QNX' .do begin
.*
.section Creating a Tiny Memory Model Application
.*
.np
Tiny memory model programs are created by compiling all modules with
the small memory model option and linking in the special
initialization file "CSTART_T.OBJ".
This file is found in the WATCOM C
.fi LIB286\DOS
directory.
It must be the first object file specified when linking the program.
:cmt.exe2. Once the program is linked, the "EXE2COM" program (provided with
:cmt.exe2. WATCOM C) can be run to convert the EXE file into a COM file.
:cmt.exe2. :I1.EXE2COM:I2.command line format
:cmt.exe2. :I1.command line format:I2.EXE2COM
:cmt.exe2. .mbigbox
:cmt.exe2. EXE2COM [option] exe_file [output_file]
:cmt.exe2. .embigbox
:cmt.exe2. .pc
:cmt.exe2. The square brackets [ ] denote items which are optional.
:cmt.exe2. .begpoint
:cmt.exe2. .mnote option
:cmt.exe2. is an option of the form "/<number>" which is used to define the
:cmt.exe2. starting offset of the COM file.
:cmt.exe2. By default, it is 0x0100.
:cmt.exe2. In certain cases, you might wish to specify a different starting offset.
:cmt.exe2. For example, when creating a device driver file (file extension ".SYS"),
:cmt.exe2. the starting offset should be 0x0000.
:cmt.exe2. The syntax for
:cmt.exe2. .sy <number>
:cmt.exe2. follows the C syntax for integer constants.
:cmt.exe2. .mnote exe_file
:cmt.exe2. is the DOS file specification of the executable file that will be
:cmt.exe2. converted to a COM file.
:cmt.exe2. If omitted, a file extension of "EXE" is assumed.
:cmt.exe2. If the period "." is specified but not the extension, the file is
:cmt.exe2. assumed to have no file extension.
:cmt.exe2. .mnote output_file
:cmt.exe2. is the optional DOS file specification of the COM file to
:cmt.exe2. be created.
:cmt.exe2. If omitted, a file extension of "COM" is assumed.
:cmt.exe2. If the period "." is specified but not the extension, the file is
:cmt.exe2. assumed to have no file extension.
:cmt.exe2. If
:cmt.exe2. .sy output_file
:cmt.exe2. is not specified then the output file will have the same name as
:cmt.exe2. .sy exe_file
:cmt.exe2. except that the file extension will be "COM".
:cmt.exe2. .endpoint
.np
The following sequence will create the executable file
"MYPROG.COM" from the file "MYPROG.C":
.exam begin
C>wcc myprog /ms
C>wlink system com file myprog
.exam end
.pc
Most of the details of linking a "COM" program are handled by the
"SYSTEM COM" directive (see the
.fi wlsystem.lnk
file for details).
When linking a "COM" program, the message "Stack segment not found" is
issued.
This message may be ignored.
:cmt.exe2. .np
:cmt.exe2. The following command demonstrates the creation of a device driver
:cmt.exe2. file.
:cmt.exe2. .exam begin
:cmt.exe2. C>exe2com /0 mouse mdriver.sys
:cmt.exe2. .exam end
:cmt.exe2. .pc
:cmt.exe2. The file "MOUSE.EXE" is converted to COM file format and is stored in
:cmt.exe2. the file "MDRIVER.SYS".
:cmt.exe2. The starting offset is 0.
:cmt.exe2. .*
:cmt.exe2. .section EXE2COM Error Diagnostics
:cmt.exe2. .*
:cmt.exe2. .np
:cmt.exe2. The following error messages may be issued by EXE2COM.
:cmt.exe2. .begnote $break
:cmt.exe2. .mnote Usage: EXE2COM [/start_offset] file [outfile]
:cmt.exe2. The command line was entered incorrectly.
:cmt.exe2. .mnote Expecting filename
:cmt.exe2. A file name was not specified on the command line.
:cmt.exe2. .mnote Unable to open %s
:cmt.exe2. The specified file %s could not be opened.
:cmt.exe2. Check that the file exists.
:cmt.exe2. .mnote EXE file too large
:cmt.exe2. The executable (EXE) file contains more than 64K of code and data.
:cmt.exe2. .mnote STACK segment ignored
:cmt.exe2. The "STACK" segment in an EXE file is not included in the COM file.
:cmt.exe2. .mnote Invalid start address
:cmt.exe2. The entry point offset of the EXE file does not correspond to the
:cmt.exe2. starting offset of the COM file.
:cmt.exe2. The default starting offset for a COM file is 0x0100 unless
:cmt.exe2. otherwise specified on the command line.
:cmt.exe2. .mnote EXE file contains relocation entries
:cmt.exe2. To convert an EXE file to a COM file, the executable file must not
:cmt.exe2. contain relocation entries.
:cmt.exe2. .mnote Not a valid EXE file
:cmt.exe2. The specified file does not have the proper format of an EXE file.
:cmt.exe2. .mnote Not enough memory
:cmt.exe2. Not enough memory is available to run the program.
:cmt.exe2. .endnote
.do end
.do end
.im wmemlay
