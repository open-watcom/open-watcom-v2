.*
.*
.dirctv DEBUG
.*
.np
The "DEBUG" directive is used to tell the &lnkname to generate
debugging information in the executable file.
This extra information in the executable file is used by the
&dbgname..
The format of the "DEBUG" directive (short form "D") is as follows.
.mbigbox
    DEBUG dbtype [dblist] |
    DEBUG [dblist]
~b
    dbtype ::= DWARF | WATCOM | CODEVIEW | NOVELL
    dblist ::= [db_option{,db_option}]
    db_option ::= LINES | TYPES | LOCALS | ALL
~b
DEBUG NOVELL only
    db_option ::= ONLYEXPORTS | REFERENCED
.embigbox
.np
The &lnkname supports four types of debugging information,
"DWARF" (the default), "WATCOM", "CODEVIEW", or "NOVELL".
.begpoint $break
.point DWARF
.ix 'DEBUG options' 'DWARF'
(short form "D") specifies that all object files contain DWARF format
debugging information and that the executable file will contain DWARF
debugging information.
.np
This debugging format is assumed by default when none is specified.
.point WATCOM
.ix 'DEBUG options' 'Watcom'
(short form "W") specifies that all object files contain Watcom format
debugging information and that the executable file will contain Watcom
debugging information.
This format permits the selection of specific classes of debugging
information (
.ct;.us dblist
.ct )
which are described below.
.point CODEVIEW
.ix 'DEBUG options' 'CODEVIEW'
(short form "C") specifies that all object files contain CodeView (CV4)
format debugging information and that the executable file will contain
CodeView debugging information.
.np
.ix 'CVPACK'
.ix 'Debugging Information Compactor'
It will be necessary to run the Microsoft Debugging Information
Compactor, CVPACK, on the executable that it has created.
For information on requesting the linker to automatically run CVPACK,
see the section entitled :HDREF refid='xcvpack'.
Alternatively, you can run CVPACK from the command line.
.point NOVELL
.ix 'DEBUG options' 'NOVELL'
(short form "N") specifies a form of global symbol information that
can only be processed by the NetWare debugger.
.endpoint
.remark
Except in rare cases, the most appropriate use of the "DEBUG" directive is
specifying "DEBUG ALL" (short form "D A") prior to any "FILE" or "LIBRARY"
directives. This will cause the &lnkname to emit all available debugging
information in the default format.
.eremark
.np
For the Watcom debugging information format, we can be selective about
the types of debugging information that we include with the executable
file.
We can categorize the types of debugging information as follows:
.ix 'debugging information' 'global symbol'
.ix 'debugging information' 'line numbering'
.ix 'debugging information' 'local symbol'
.ix 'debugging information' 'typing'
.ix 'debugging information' 'NetWare global symbol'
.begbull
.bull
global symbol information
.bull
line numbering information
.bull
local symbol information
.bull
typing information
.bull
NetWare global symbol information
.endbull
.np
The following options can be used with the "DEBUG WATCOM" directive to
control which of the above classes of debugging information is
included in the executable file.
.begpoint $break
.point LINES
.ix 'DEBUG options' 'LINES'
(short form "LI") specifies line numbering and global symbol
information.
.point LOCALS
.ix 'DEBUG options' 'LOCALS'
(short form "LO") specifies local and global symbol information.
.point TYPES
.ix 'DEBUG options' 'TYPES'
(short form "T") specifies typing and global symbol information.
.point ALL
.ix 'DEBUG options' 'ALL'
(short form "A") specifies all of the above debugging information.
.point ONLYEXPORTS
.ix 'DEBUG options' 'ONLYEXPORTS'
(short form "ONL") restricts the generation of global symbol
information to exported symbols.
This option may only be used with Netware executable formats.
.endpoint
.np
The following options can be used with the "DEBUG NOVELL" directive to
control which of the above classes of debugging information is
included in the executable file.
.begpoint $break
.point ONLYEXPORTS
.ix 'DEBUG options' 'ONLYEXPORTS'
(short form "ONL") restricts the generation of global symbol
information to exported symbols.
.point REFERENCED
.ix 'DEBUG options' 'REFERENCED'
(short form "REF") restricts the generation of symbol information to
referenced symbols only.
.endpoint
.remark
The position of the "DEBUG" directive is important.
The level of debugging information specified in a "DEBUG" directive
only applies to object files and libraries that appear in
.us subsequent
"FILE" or "LIBRARY" directives.
For example, if "DEBUG WATCOM ALL" was the only "DEBUG" directive
specified and was also the last linker directive, no debugging
information would appear in the executable file.
.eremark
.np
Only global symbol information is actually produced by the &lnkname;
the other three classes of debugging information are extracted from
object modules and copied to the executable file.
Therefore, at compile time, you must instruct the compiler to generate
local symbol, line numbering and typing information in the object file
so that the information can be transferred to the executable file.
If you have asked the &lnkname to produce a particular class of
debugging information and it appears that none is present, one of the
following conditions may exist.
.autopoint
.point
The debugging information is not present in the object files.
.point
The "DEBUG" directive has been misplaced.
.endpoint
.np
The following sections describe the classes of debugging information.
.*
.beglevel
.*
.section Line Numbering Information - DEBUG WATCOM LINES
.*
.np
.ix 'debugging information' 'line numbering'
The "DEBUG WATCOM LINES" option controls the processing of line
numbering information.
Line numbering information is the line number and address of the
generated code for each line of source code in a particular module.
This allows &dbgname to perform source-level debugging.
When the &lnkname encounters a "DEBUG WATCOM" directive with a "LINES"
or "ALL" option, line number information for each subsequent object
module will be placed in the executable file.
This includes all object modules extracted from object files specified
in subsequent "FILE" directives and object modules extracted from
libraries specified in subsequent "LIBRARY" or "FILE" directives.
.remark
All modules for which line numbering information is requested must
have been compiled with the "d1" or "d2" option.
.eremark
.pc
A subsequent "DEBUG WATCOM" directive without a "LINES" or "ALL"
option terminates the processing of line numbering information.
.*
.section Local Symbol Information - DEBUG WATCOM LOCALS
.*
.np
.ix 'debugging information' 'local symbol'
The "DEBUG WATCOM LOCALS" option controls the processing of local
symbol information.
Local symbol information is the name and address of all symbols local
to a particular module.
This allows &dbgname to locate these symbols so that you can reference
local data and routines by name.
When the &lnkname encounters a "DEBUG WATCOM" directive with a
"LOCALS" or "ALL" option, local symbol information for each subsequent
object module will be placed in the executable file.
This includes all object modules extracted from object files specified
in subsequent "FILE" directives and object modules extracted from
libraries specified in subsequent "LIBRARY" or "FILE" directives.
.remark
All modules for which local symbol information is requested must have
been compiled with the "d2" option.
.eremark
.pc
A subsequent "DEBUG WATCOM" directive without a "LOCALS" or "ALL"
option terminates the processing of local symbol information.
.*
.section Typing Information - DEBUG WATCOM TYPES
.*
.np
.ix 'debugging information' 'typing'
The "DEBUG WATCOM TYPES" option controls the processing of typing
information.
Typing information includes a description of all types, structures and
arrays that are defined in a module.
This allows &dbgname to display variables according to their type.
When the &lnkname encounters a "DEBUG WATCOM" directive with a "TYPES"
or "ALL" option, typing information for each subsequent object module
will be placed in the executable file.
This includes all object modules extracted from object files specified
in subsequent "FILE" directives and object modules extracted from
libraries specified in subsequent "LIBRARY" or "FILE" directives.
.remark
All modules for which typing information is requested must have been
compiled with the "d2" option.
.eremark
.pc
A subsequent "DEBUG WATCOM" directive without a "TYPES" or "ALL"
option terminates the processing of typing information.
.*
.section All Debugging Information - DEBUG WATCOM ALL
.*
.np
.ix 'debugging information' 'all'
The "DEBUG WATCOM ALL" option specifies that "LINES", "LOCALS", and
"TYPES" options are requested.
The "LINES" option controls the processing of line numbering
information.
The "LOCALS" option controls the processing of local symbol
information.
The "TYPES" option controls the processing of typing
information.
Each of these options is described in a previous section.
A subsequent "DEBUG WATCOM " directive without an "ALL" option
discontinues those options which are not specified in the list of
debug options.
.*
.section Global Symbol Information
.*
.np
.ix 'debugging information' 'global symbol'
Global symbol information consists of all the global symbols in your
program and their address.
This allows &dbgname to locate these symbols so that you can reference
global data and routines by name.
When the &lnkname encounters a "DEBUG" directive, global symbol
information for all the global symbols appearing in your program is
placed in the executable file.
.*
.section Global Symbols for the NetWare Debugger - DEBUG NOVELL
.*
.np
.ix 'debugging information' 'for NetWare debugger'
.ix 'NetWare debugger'
The NetWare operating system has a built-in debugger that can be
used to debug programs.
When "DEBUG NOVELL" is specified, the &lnkname will generate global
symbol information that can be used by the NetWare debugger.
Note that any line numbering, local symbol, and typing information
generated in the executable file will not be recognized by the NetWare
debugger.
Also,
.kw &stripcmdup
cannot be used to remove this form of global symbol information from
the executable file.
.*
.section The ONLYEXPORTS Debugging Option
.*
.np
.ix 'DEBUG options' 'ONLYEXPORTS'
The "ONLYEXPORTS" option (short form "ONL") restricts the generation
of global symbol information to exported symbols (symbols appearing in
an "EXPORT" directive).
If "DEBUG WATCOM ONLYEXPORTS" is specified, &dbgname global symbol
information is generated only for exported symbols.
If "DEBUG NOVELL ONLYEXPORTS" is specified, NetWare global symbol
information is generated only for exported symbols.
.*
.section Using DEBUG Directives
.*
.np
Consider the following directive file.
.millust begin
debug watcom all
file module1
debug watcom lines
file module2, module3
debug watcom
library mylib
.millust end
.pc
It specifies that the following debugging information is to be
generated in the executable file.
.autonote
.note
global symbol information for your program
.note
line numbering, typing and local symbol information for the following
object files:
.millust begin
module1.&obj
.millust end
.note
line numbering information for the following object files:
.millust begin
module2.&obj
module3.&obj
.millust end
.endnote
.np
Note that if the "DEBUG WATCOM" directive before the "LIBRARY"
directive is not specified, line numbering information for all object
modules extracted from the library "mylib.lib" would be generated in
the executable file provided the object modules extracted from the
library have line numbering information present.
.remark
A "DEBUG WATCOM" directive with no option suppresses the processing of
line numbering, local symbol and typing information for all subsequent
object modules.
.eremark
.pc
Debugging information can use a significant amount of disk space.
As shown in the above example, you can select only the class of
debugging information you want and for those modules you wish to
debug.
In this way, the amount of debugging information in the executable
file is minimized and hence the amount of disk space used by the
executable file is kept to a minimum.
.np
As you can see from the above example, the position of the
"DEBUG WATCOM" directive is important when describing the debugging
information that is to appear in the executable file.
.remark
If you want all classes of debugging information for all files to
appear in the executable file you must specify "DEBUG WATCOM ALL"
before any "FILE" and "LIBRARY" directives.
.eremark
.*
.section Removing Debugging Information from an Executable File
.*
.np
.ix 'debugging information' 'strip from "EXE" file'
.ix '&stripcmdup command'
A utility called
.kw &stripcmdup
has been provided which takes as input an executable file and removes
the debugging information placed in the executable file by the
&lnkname..
Note that global symbol information generated using "DEBUG NOVELL"
cannot be removed by
.kw &stripcmdup
.ct .li .
.np
For more information on this utility,
see the chapter entitled "The &stripname" in the
.book &company C/C++ Tools User's Guide
or
.book &cmpfname Tools User's Guide.
.*
.endlevel
