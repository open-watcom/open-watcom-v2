.chap The Object Module Processor
.*
.section Converting &company Object Files To Other Formats
.*
.np
.ix 'debugging information'
.ix '&company object files'
.ix 'Microsoft object files'
.ix 'Phar Lap Easy OMF-386 object files'
.ix 'object files' '&company'
.ix 'object files' 'Microsoft'
.ix 'object files' 'Phar Lap Easy OMF-386'
When one of the "d1" or "d2" compiler options is used, a &company
compiler will include debugging information in object files.
This debugging information can be utilized by the &lnkname and the
&dbgname..
Unfortunately, since there is no standard for the representation of
debugging information in executable applications, &company's debugging
information cannot be processed by other linkers or debuggers.
A utility program is provided that may be used to convert the
debugging information present in object files created by a &company
compiler to various formats acceptable by other linkers and debuggers.
.ix '&company debugging information'
.ix 'Microsoft CodeView debugging information'
.ix 'MetaWare debugging information'
.ix 'Turbo debugging information'
.ix 'debugging information' '&company'
.ix 'debugging information' 'Microsoft CodeView'
.ix 'debugging information' 'MetaWare'
.ix 'debugging information' 'Turbo'
.ix 'WOMP'
This utility program is called "WOMP"
(&company Object Module Processor).
.np
The format of the WOMP command line is:
.ix 'WOMP' 'command line format'
.ix 'command line format' 'WOMP'
.mbigbox
WOMP [options]* file [options|file]*
.embigbox
.pc
The square brackets [ ] denote items which are optional.
.begpoint
.mnote options
is a list of valid WOMP options, each preceded by a slash ("/") or a
dash ("&minus.").
Options may be specified in any order.
Options are described below.
.mnote file
is a file specification which can include wild cards in the file name
and extension parts.
A file specification takes the form:
.begpoint
.mnote d:
is an optional drive specification such as "A:", "B:", etc.
If not specified, the default drive is assumed.
.mnote path
is an optional path specification such as "\PROGRAMS\EXE\".
If not specified, the current directory is assumed.
.mnote file
is the file name of the object or library file.
The file name may contain an "*" wild card.
.mnote ext
is the file extension of the object or library file.
The file extension may contain an "*" wild card.
.endpoint
.np
The output object or library file will have the same name as the input
object or library file.
The object or library file extension will default to the input object
or library file extension.
.endpoint
.np
Temporary files are used by WOMP to contain the intermediate object
or library file during processing.
The temporary files used follow the naming convention "_Wn.TMP" where
.id n
is a decimal number starting at zero (maximum is 65535).
.*
.section WOMP Options Summary
.*
.np
Options must precede the file(s) you wish them to affect.
The options marked "*" are the default ones.
.*
.begnote $break $compact
.*
:opt name='&sw.o'.=spec
Output filename/directory specification
:opt name='&sw.fm'.
*Generate Microsoft 16- and 32-bit object files
:opt name='&sw.fm2'.
*Generate Microsoft object files for OS/2 2.0.
:opt name='&sw.fp'.
Generate Phar Lap Easy OMF-386 32-bit object files
:opt name='&sw.f-'.
Do not generate an object file (useful with /dx)
:opt name='&sw.dm'.
*Generate Microsoft CodeView debugging information
:opt name='&sw.dp'.
Generate Phar Lap/MetaWare debugging information
:opt name='&sw.dt'.
Generate Turbo debugging information
:opt name='&sw.dx'.
Generate human-readable text to console
:opt name='&sw.d-'.
Do not generate any debugging information
:opt name='&sw.pw'.
*Parse &company debugging information
:opt name='&sw.p7'.
Parse &company C 7.0 debugging information
:opt name='&sw.p-'.
Do not attempt to parse any debugging information
:opt name='&sw.q'.
Operate quietly
:opt name='&sw.b'.
Leave temporary files and output batch file commands
.endnote
.np
The following options apply to .WMP files.
.*
.begnote $break $compact
.*
:opt name='@'.file
Read file[.WMP] for options
:opt name='#'.
Ignore from # to end of line (comment)
.endnote
.*
.cp 10
.section Description of WOMP Options
.*
.begnote $break
.*
:opt name='&sw.o'.=spec
Output filename/directory specification
.np
The output directory, filename, and/or default extension may be
specified.
If this option is not specified, then WOMP will overwrite the original
object file.
.exam begin
C>rem specify a new file name
C>womp /o=new myobj
C>rem specify a new extension
C>womp /o=.mbj myobj
C>rem specify a default directory and extension
C>womp /o=\tmp\.mbj myobj
.exam end
.np
A '#' may be used in place of '=' (for use with the operating system
shell's "SET" command and BAT files).
:opt name='&sw.fm'.
Generate Microsoft 16- and 32-bit object files
.np
The default action is to convert 16- and 32-bit &company object file
formats to Microsoft 16- and 32-bit object file formats.
If the input object file is a 16-bit file, then the output is
guaranteed to be a 16-bit object file.
.np
The "dx" option may be used in conjunction with this option.
:opt name='&sw.fm2'.
Generate Microsoft 16- and 32-bit object files for OS/2 2.0
.np
Optionally, WOMP can convert 16- and 32-bit &company object file
formats to OS/2 2.0 16- and 32-bit object file formats.
If the input object file is a 16-bit file, then the output is
guaranteed to be a 16-bit object file.
.np
The "dx" option may be used in conjunction with this option.
:opt name='&sw.fp'.
Generate Phar Lap Easy OMF-386 32-bit object files
.np
Optionally, WOMP can convert 16- and 32-bit &company object file
formats to Phar Lap Easy OMF-386 32-bit object file formats.
If the input object file is a 16-bit file, WOMP does its best to
convert it to a Phar Lap object file.
This results in USE16 segments, possibly with improper access rights.
See Phar Lap documentation for more information.
.np
The "dx" option may be used in conjunction with this option.
:opt name='&sw.f-'.
Do not generate an object file (useful with &sw.dx)
.np
The object file is not converted to any other format.
:opt name='&sw.dm'.
Generate Microsoft CodeView debugging information
.np
The default action is to convert &company debugging information
to Microsoft CodeView debugging information.
:opt name='&sw.dp'.
Generate Phar Lap/MetaWare debugging information
.np
Optionally, WOMP can convert &company debugging information to
Phar Lap/MetaWare debugging information.
This debugging format is a small variation on Microsoft format.
Some programs that accept output from a MetaWare compiler will require
a Phar Lap object file (specify &sw.fp).
:opt name='&sw.dt'.
Generate Turbo debugging information
.np
Optionally, WOMP can convert &company debugging information to Borland's
Turbo debugging information.
:opt name='&sw.dx'.
Generate human-readable text to console
.np
Optionally, WOMP can display &company debugging information in a
human-readable representation (see subsequent section for example).
:opt name='&sw.d-'.
Do not generate any debugging information
.np
The symbolic debugging information in the object file is omitted from
the output object file.
If the input file contained symbolic debugging information it will be
lost.
Line numbering information will remain.
:opt name='&sw.pw'.
Parse &company debugging information
.np
By default, WOMP will process the debugging information in an object
file created by &company C 8.0 or later.
:opt name='&sw.p7'.
Parse &company C 7.0 debugging information
.np
Optionally, WOMP can process the debugging information in an object
file created by &company C 7.0.
:opt name='&sw.p-'.
Do not attempt to parse any debugging information
.np
If this option is specified, then no attempt is made to interpret the
object file's contents.
However, WOMP's object file rearrangements still occur.
For example, to convert a &company 32-bit object file from Phar Lap
format into a Microsoft 32-bit object file, one could issue the
command:
.exam begin
WOMP &sw.d- &sw.p- file.obj
.exam end
.pc
If the &sw.d- is not specified, then artifacts of another debugging
generator could result (e.g., the Microsoft generator would output
some extra COMENT records).
:opt name='&sw.q'.
Operate quietly
.np
No informational messages are displayed.
:opt name='&sw.b'.
Leave temporary files and output batch file commands
.np
The "b" option is useful when the size of a library is too large
for &libcmdup to handle when it is executed by WOMP.
The "b" option will leave temporary files and output
the appropriate batch file commands to accomplish the
necessary file manipulations.
.exam begin
C>WOMP &sw.b large.lib >doit.bat
C>doit
.exam end
.endnote
.np
The following options apply to .WMP files.
.*
.begnote $break
.*
:opt name='@'.file
Read file[.WMP] for options
.np
WOMP will attempt to access the environment variable "file"; if that
does not exist it will open the file named "file".
The default file extension used is ".WMP".
The contents of the file/environment variable will be inserted at the
point of the @file command.
:opt name='#'.
Ignore from # to end of line (comment)
.np
This can be used for comments in ".WMP" files.
WOMP ignores all text following a "#" up to the end of the line.
.endnote
.*
.section WOMP Environment Variable
.*
.np
The
.ev WOMP
environment variable can be used to specify commonly used WOMP
options.
These options are processed before options specified on the command
line.
.ix '&setcmdup' 'WOMP environment variable'
.exam begin
&prompt.&setcmd &setdelim.WOMP=&sw.fp &sw.dp&setdelim
.exam end
.pc
The above example defines the default WOMP options to be "fp" and
"dp".
.np
.ix 'environment string' '= substitute'
.ix 'environment string' '#'
Whenever you wish to specify an option that requires the use of an
"=" character, you can use the "#" character in its place.
This is required by the syntax of the "&setcmdup" command.
.np
Once the
.ev WOMP
environment variable has been defined, those options listed become the
default each time the
.kw WOMP
command is used.
The WOMP command line can be used to override any options specified in
the environment string.
.hint
.if '&target' eq 'QNX' .do begin
.ix 'user initialization file'
If you use the same compiler options all the time, you may find it
handy to place the "&setcmd WOMP" command in your user
initialization file.
.do end
.el .do begin
.ix 'system initialization file' 'AUTOEXEC.BAT'
.ix 'AUTOEXEC.BAT' 'system initialization file'
If you are running DOS and you use the same WOMP options all the time,
you may find it handy to place the "&setcmdup WOMP" command in your
DOS system initialization file,
.fi AUTOEXEC.BAT.
.np
.ix 'system initialization' 'Windows NT'
.ix 'Windows NT' 'system initialization'
If you are running Windows NT, use the "System" icon in the
.bd Control Panel
to define environment variables.
.np
.ix 'system initialization file' 'CONFIG.SYS'
.ix 'CONFIG.SYS' 'system initialization file'
If you are running OS/2 and you use the same WOMP options all the
time, you may find it handy to place the "&setcmdup WOMP" command
in your OS/2 system initialization file,
.fi CONFIG.SYS.
.do end
.ehint
.*
.section Input Object File Processing
.*
.np
WOMP can process most object files generated by MS/DOS compilers and
assemblers.
Specifically, WOMP handles:
.begbull
.bull
Microsoft's variant on Intel 16-bit object files.
.bull
Microsoft style 32-bit object files (Xenix object format).
.bull
Phar Lap Easy OMF-386 object files.
.endbull
.np
There is no need to tell WOMP the style of an input object file since
the formats are distinguishable from one another.
.*
.section Output Object File Processing
.*
.np
To properly do some of the debugging conversions, WOMP must rearrange
some records within the object file.
WOMP places all PUBDEFs before the first LEDATA/LIDATA record.
Additionally, all EXTDEFs and COMDEFs are placed before the first
PUBDEF.
These changes are benign for the most part.
However, some compilers write COMENT records that are supposed to
refer to the next EXTDEF/COMDEF/PUBDEF.
In this case the COMENT records will be out of place, and the object
file may not be useable.
.*
.section Using WOMP with &company Compilers
.*
.np
Specify "&sw.d2" on the compiler command line (after any optimization
options) to generate &company symbolic information.
Then use WOMP on the resulting object file.
.np
The default external naming convention for &company C functions is
"name_" where "name" is the name of a function.
Some debuggers (e.g., CodeView) will expect "_name" and will not
recognize a command referencing the symbol "name".
To work around this problem you must use "name_" whenever referring to
a function (e.g., "go main_")
.np
The default external naming convention for &company FORTRAN functions is
.bd ALL UPPERCASE
.ct .li .
So, you can use "go FMAIN" to begin a FORTRAN program.
.np
&company parameter passing is described in the "User's Guide".
There is no support for registerized parameters in Microsoft/MetaWare
debugging information.
Debuggers working with a &company program might get confused as to the
location of parameters and display random values.
This should not occur after the function prolog has been executed
(i.e., the variables should be available on the first line of code in
any function).
In particular, CodeView's "Calls" menu will not show valid parameters.
.*
.section Using Microsoft CodeView with WOMP
.*
.np
Note that CodeView supports debugging of 16-bit applications only.
It does not support 32-bit applications.
.np
Unfortunately, CodeView does not understand the entire Microsoft
Debugging format.
The most noticeable area is typedefs.
WOMP makes an attempt to resolve any problems CodeView will have with
typedefs.
The result is that types will be displayed in their most basic form
while in CodeView (all typedefs are stripped).
.np
The Microsoft Debugging format does not have a "pointer to void" type
in it.
Pointers to void are treated as pointers to char for debugging
purposes.
.np
Some FORTRAN types (notably CHARACTER *(*) SAM, and REAL A(1:M))
cannot be converted to Microsoft format because of code generation
differences.
A warning will be issued in this case.
.*
.section Using the Borland Turbo Debugger with WOMP
.*
.np
Note that the Turbo Debugger supports debugging of 16-bit applications
only.
It does not support 32-bit applications.
.np
The Turbo debugging format has good support for parameterized
functions.
Turbo Debug should have no problem finding parameters when asked for
them.
.np
Turbo Debug does not have a FORTRAN mode, so FORTRAN programs will appear
in C mode.
The FORTRAN COMPLEX data type will appear as a C struct with two
fields named "real" and "imag".
.np
Any other unconvertible type will appear as an array of unsigned
characters (bytes) of the appropriate size.
.*
.section Linking Turbo Debugger Executables
.*
.np
.ix 'Turbo Linker'
.ix 'TLINK'
Once all object files required by the application are converted
to the appropriate format, the application must be linked by the
vendor's linker.
The following is an example using the Borland linker, "TLINK".
.syntax
tlink /m /n /v /s [obj],[exe],[map],\watcom\lib286\clibs.lib
.esyntax
.pc
where
.sy [obj]
is a list of object file names,
.sy [exe]
is the name of the executable image file,
and
.sy [map]
is the name of the map file.
.* .remark
.* A special startup file must be included when linking the application
.* with the Turbo Linker.
.* The file "TSTART_?" (where "?" is one of S, M, C, L, H corresponding
.* to the memory model)
.* .us must
.* be the first file that is included when linking the application.
.* This file will guarantee that segments are included in the executable
.* image in the correct order.
.* .eremark
.np
.ix 'Turbo Debugger'
The application is now ready to be debugged by Borland's Turbo
Debugger.
.*
.section Dumping Debugging Information - The "dx" Option
.*
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
:set symbol="ccmd16"   value="wcc".
:set symbol="ccmd32"   value="wcc386".
:cmt. .if '&machine' eq '8086' .do begin
.np
The following are 16-bit examples.
.tinyexam begin
&prompt.&ccmd16 calendar
&prompt.womp &sw.f- &sw.dx calendar
&company Object Module Processor Version 1.1
.cpyrit 1990
.trdmrk
Converting hello.OBJ
                            Miscellaneous Information
                            -------------------------
Processor       : 8086
Memory Model    : Small
Floating-point  : Inline Emulation
Optimized       : Yes
Lines Present   : No
Types Present   : No
Symbs Present   : No
Source Language : C
.tinyexam end
.tinyexam begin
&prompt.&ccmd16 &sw.d1 calendar
&prompt.womp &sw.f- &sw.dx calendar
&company Object Module Processor Version 1.1
.cpyrit 1990
.trdmrk
Converting hello.OBJ
                            Miscellaneous Information
                            -------------------------
Processor       : 8086
Memory Model    : Small
Floating-point  : Inline Emulation
Optimized       : Yes
Lines Present   : Yes
Types Present   : No
Symbs Present   : No
Source Language : C
.tinyexam end
.tinyexam begin
&prompt.&ccmd16 &sw.d2 calendar
&prompt.womp &sw.f- &sw.dx calendar
&company Object Module Processor Version 1.1
.cpyrit 1990
.trdmrk
Converting hello.OBJ
                            Miscellaneous Information
                            -------------------------
Processor       : 8086
Memory Model    : Small
Floating-point  : Inline Emulation
Optimized       : Yes
Lines Present   : Yes
Types Present   : Yes
Symbs Present   : Yes
Source Language : C
.code break
                                Type Information
                                ----------------
0x0000: RESERVED
.code break
0x0001: RESERVED
.code break
0x0002: INTEGER size 0x00000008 bits SIGNED
.code break
0x0003: INTEGER size 0x00000008 bits UNSIGNED
.code break
0x0004: INTEGER size 0x00000010 bits SIGNED
.code break
0x0005: INTEGER size 0x00000010 bits UNSIGNED
.code break
0x0006: INTEGER size 0x00000020 bits SIGNED
.code break
0x0007: INTEGER size 0x00000020 bits UNSIGNED
.code break
0x0008: REAL size 0x00000020 bits
.code break
0x0009: REAL size 0x00000040 bits
.code break
0x000a: VOID
.code break
0x000b: POINTER size 0x00000010 bits base_type 0x0003 NEAR
.code break
0x000c: TYPEDEF size 0x00000008 bits
    scope none type 0x0003 name "char"
.code break
0x000d: TYPEDEF size 0x00000008 bits
    scope none type 0x0003 name "char"
.code break
0x000e: POINTER size 0x00000010 bits base_type 0x0002 NEAR
.code break
0x000f: PROCEDURE
    NEAR ret_type 0x000e num_parms 00
.code break
0x0010: POINTER size 0x00000010 bits base_type 0x000c NEAR
.code break
0x0011: ARRAY_ZERO size 0x00000010 bits
    base_type 0x0010 high_bound 0x00000000
.code break
0x0012: STRUCT size 0x00000070 bits num_entries 0x0008
    bit_offset 0x00000000 type 0x0010 name "_ptr"
    bit_offset 0x00000010 type 0x0004 name "_cnt"
    bit_offset 0x00000020 type 0x0010 name "_base"
    bit_offset 0x00000030 type 0x0005 name "_flag"
    bit_offset 0x00000040 type 0x0004 name "_handle"
    bit_offset 0x00000050 type 0x0005 name "_bufsize"
    bit_offset 0x00000060 type 0x0003 name "_ungotten"
    bit_offset 0x00000068 type 0x0003 name "_tmpfchar"
.code break
0x0013: TYPEDEF size 0x00000070 bits
    scope struct type 0x0012 name "__iobuf"
.code break
0x0014: ARRAY_ZERO size 0x000008c0 bits
    base_type 0x0013 high_bound 0x00000013
.code break
0x0015: ARRAY_ZERO size 0x00000140 bits
    base_type 0x0005 high_bound 0x00000013
.code break
0x0016: POINTER size 0x00000010 bits base_type 0x0013 NEAR
.code break
0x0017: POINTER size 0x00000010 bits base_type 0x000d NEAR
.code break
0x0018: POINTER size 0x00000010 bits base_type 0x0006 NEAR
.code break
0x0019: POINTER size 0x00000010 bits base_type 0x000a NEAR
.code break
0x001a: POINTER size 0x00000010 bits base_type 0x0006 NEAR
.code break
0x001b: POINTER size 0x00000010 bits base_type 0x000a NEAR
.code break
0x001c: POINTER size 0x00000010 bits base_type 0x0010 NEAR
.code break
0x001d: ARRAY_ZERO size 0x00000070 bits
    base_type 0x0003 high_bound 0x0000000d
.code break
0x001e: TYPEDEF size 0x00000020 bits
    scope none type 0x0006 name "fpos_t"
.code break
0x001f: TYPEDEF size 0x00000010 bits
    scope none type 0x0011 name "__va_list"
.code break
0x0020: TYPEDEF size 0x00000010 bits
    scope none type 0x0005 name "size_t"
.code break
0x0021: TYPEDEF size 0x00000070 bits
    scope none type 0x0013 name "FILE"
.code break
0x0022: PROCEDURE
    NEAR ret_type 0x000a num_parms 00
.code break
                               Symbol Information
                               ------------------
CHANGE_SEG
    data 00 00 00 00
    fixup SEG_REL  POINTER  Frame: TARGET  Target: SI(0x0001)
BLOCK name "main" type 0x0022 start_offset 0x00000000 size 0x00000017
    PROLOG size 09 ret_addr_offset 0x00000002
    EPILOG size 04
BLOCK_END
.tinyexam end
:cmt. .do end
:cmt. .if '&machine' eq '80386' .do begin
.np
The following are 32-bit examples.
.tinyexam begin
&prompt.&ccmd32 calendar
&prompt.womp &sw.f- &sw.dx calendar
&company Object Module Processor Version 1.1
.cpyrit 1990
.trdmrk
Converting calendar.OBJ
                            Miscellaneous Information
                            -------------------------
Processor       : 80386
Memory Model    : Flat
Floating-point  : 80x87 Inline Code
Optimized       : Yes
Lines Present   : No
Types Present   : No
Symbs Present   : No
Source Language : C
.tinyexam end
.tinyexam begin
&prompt.&ccmd32 &sw.d1 calendar
&prompt.womp &sw.f- &sw.dx calendar
&company Object Module Processor Version 1.1
.cpyrit 1990
.trdmrk
Converting calendar.OBJ
                            Miscellaneous Information
                            -------------------------
Processor       : 80386
Memory Model    : Flat
Floating-point  : 80x87 Inline Code
Optimized       : Yes
Lines Present   : Yes
Types Present   : No
Symbs Present   : No
Source Language : C
.tinyexam end
.tinyexam begin
&prompt.&ccmd32 &sw.d2 calendar
&prompt.womp &sw.f- &sw.dx calendar
&company Object Module Processor Version 1.1
.cpyrit 1990
.trdmrk
Converting calendar.OBJ
                            Miscellaneous Information
                            -------------------------
Processor       : 80386
Memory Model    : Flat
Floating-point  : 80x87 Inline Code
Optimized       : Yes
Lines Present   : Yes
Types Present   : Yes
Symbs Present   : Yes
Source Language : C
.code break
                                Type Information
                                ----------------
0x0000: RESERVED
.code break
0x0001: RESERVED
.code break
0x0002: INTEGER size 0x00000008 bits SIGNED
.code break
0x0003: INTEGER size 0x00000008 bits UNSIGNED
.code break
0x0004: INTEGER size 0x00000010 bits SIGNED
.code break
0x0005: INTEGER size 0x00000010 bits UNSIGNED
.code break
0x0006: INTEGER size 0x00000020 bits SIGNED
.code break
0x0007: INTEGER size 0x00000020 bits UNSIGNED
.code break
0x0008: TYPEDEF size 0x00000008 bits
    scope none type 0x0003 name "char"
.code break
0x0009: TYPEDEF size 0x00000008 bits
    scope none type 0x0003 name "char"
.code break
0x000a: TYPEDEF size 0x00000020 bits
    scope none type 0x0007 name "size_t"
.code break
0x000b: POINTER size 0x00000020 bits base_type 0x0008 NEAR 386
.code break
0x000c: ARRAY_ZERO size 0x00000020 bits
    base_type 0x000b high_bound 0x00000000
.code break
0x000d: TYPEDEF size 0x00000020 bits
    scope none type 0x000c name "__va_list"
.code break
0x000e: STRUCT size 0x000000d0 bits num_entries 0x0008
    bit_offset 0x00000000 type 0x000b name "_ptr"
    bit_offset 0x00000020 type 0x0006 name "_cnt"
    bit_offset 0x00000040 type 0x000b name "_base"
    bit_offset 0x00000060 type 0x0007 name "_flag"
    bit_offset 0x00000080 type 0x0006 name "_handle"
    bit_offset 0x000000a0 type 0x0007 name "_bufsize"
    bit_offset 0x000000c0 type 0x0003 name "_ungotten"
    bit_offset 0x000000c8 type 0x0003 name "_tmpfchar"
.code break
0x000f: TYPEDEF size 0x000000d0 bits
    scope struct type 0x000e name "__iobuf"
.code break
0x0010: TYPEDEF size 0x000000d0 bits
    scope none type 0x000f name "FILE"
.code break
0x0011: TYPEDEF size 0x00000020 bits
    scope none type 0x0006 name "fpos_t"
.code break
0x0012: TYPEDEF size 0x00000020 bits
    scope none type 0x0006 name "fpos_t"
.code break
0x0013: TYPEDEF size 0x00000010 bits
    scope none type 0x0005 name "wchar_t"
.code break
0x0014: STRUCT size 0x00000040 bits num_entries 0x0002
    bit_offset 0x00000000 type 0x0006 name "quot"
    bit_offset 0x00000020 type 0x0006 name "rem"
.code break
0x0015: TYPEDEF size 0x00000040 bits
    scope none type 0x0014 name "div_t"
.code break
0x0016: STRUCT size 0x00000040 bits num_entries 0x0002
    bit_offset 0x00000000 type 0x0006 name "quot"
    bit_offset 0x00000020 type 0x0006 name "rem"
.code break
0x0017: TYPEDEF size 0x00000040 bits
    scope none type 0x0016 name "ldiv_t"
.code break
0x0018: TYPEDEF size 0x00000010 bits
    scope none type 0x0005 name "wchar_t"
.code break
0x0019: VOID
.code break
0x001a: PROCEDURE
    NEAR 386 ret_type 0x0019 num_parms 00
.code break
0x001b: POINTER size 0x00000020 bits base_type 0x001a NEAR 386
.code break
0x001c: TYPEDEF size 0x00000020 bits
    scope none type 0x001b name "onexit_t"
.code break
0x001d: TYPEDEF size 0x00000020 bits
    scope none type 0x0006 name "ptrdiff_t"
.code break
0x001e: TYPEDEF size 0x00000020 bits
    scope none type 0x0007 name "time_t"
.code break
0x001f: TYPEDEF size 0x00000020 bits
    scope none type 0x0007 name "clock_t"
.code break
0x0020: STRUCT size 0x00000120 bits num_entries 0x0009
    bit_offset 0x00000000 type 0x0006 name "tm_sec"
    bit_offset 0x00000020 type 0x0006 name "tm_min"
    bit_offset 0x00000040 type 0x0006 name "tm_hour"
    bit_offset 0x00000060 type 0x0006 name "tm_mday"
    bit_offset 0x00000080 type 0x0006 name "tm_mon"
    bit_offset 0x000000a0 type 0x0006 name "tm_year"
    bit_offset 0x000000c0 type 0x0006 name "tm_wday"
    bit_offset 0x000000e0 type 0x0006 name "tm_yday"
    bit_offset 0x00000100 type 0x0006 name "tm_isdst"
.code break
0x0021: TYPEDEF size 0x00000120 bits
    scope struct type 0x0020 name "tm"
.code break
0x0022: STRUCT size 0x00000120 bits num_entries 0x0009
    bit_offset 0x00000000 type 0x0006 name "tm_sec"
    bit_offset 0x00000020 type 0x0006 name "tm_min"
    bit_offset 0x00000040 type 0x0006 name "tm_hour"
    bit_offset 0x00000060 type 0x0006 name "tm_mday"
    bit_offset 0x00000080 type 0x0006 name "tm_mon"
    bit_offset 0x000000a0 type 0x0006 name "tm_year"
    bit_offset 0x000000c0 type 0x0006 name "tm_wday"
    bit_offset 0x000000e0 type 0x0006 name "tm_yday"
    bit_offset 0x00000100 type 0x0006 name "tm_isdst"
.code break
0x0023: TYPEDEF size 0x00000120 bits
    scope struct type 0x0022 name "tm"
.code break
0x0024: TYPEDEF size 0x00000020 bits
    scope none type 0x0007 name "time_t"
.code break
0x0025: STRUCT size 0x000000e0 bits num_entries 0x0007
    bit_offset 0x00000000 type 0x0007 name "eax"
    bit_offset 0x00000020 type 0x0007 name "ebx"
    bit_offset 0x00000040 type 0x0007 name "ecx"
    bit_offset 0x00000060 type 0x0007 name "edx"
    bit_offset 0x00000080 type 0x0007 name "esi"
    bit_offset 0x000000a0 type 0x0007 name "edi"
    bit_offset 0x000000c0 type 0x0007 name "cflag"
.code break
0x0026: TYPEDEF size 0x000000e0 bits
    scope struct type 0x0025 name "DWORDREGS"
.code break
0x0027: STRUCT size 0x000000e0 bits num_entries 0x0007
    bit_offset 0x00000000 type 0x0005 name "ax"
    bit_offset 0x00000020 type 0x0005 name "bx"
    bit_offset 0x00000040 type 0x0005 name "cx"
    bit_offset 0x00000060 type 0x0005 name "dx"
    bit_offset 0x00000080 type 0x0005 name "si"
    bit_offset 0x000000a0 type 0x0005 name "di"
    bit_offset 0x000000c0 type 0x0007 name "cflag"
.code break
0x0028: TYPEDEF size 0x000000e0 bits
    scope struct type 0x0027 name "WORDREGS"
.code break
0x0029: STRUCT size 0x00000070 bits num_entries 0x0008
    bit_offset 0x00000000 type 0x0003 name "al"
    bit_offset 0x00000008 type 0x0003 name "ah"
    bit_offset 0x00000020 type 0x0003 name "bl"
    bit_offset 0x00000028 type 0x0003 name "bh"
    bit_offset 0x00000040 type 0x0003 name "cl"
    bit_offset 0x00000048 type 0x0003 name "ch"
    bit_offset 0x00000060 type 0x0003 name "dl"
    bit_offset 0x00000068 type 0x0003 name "dh"
.code break
0x002a: TYPEDEF size 0x00000070 bits
    scope struct type 0x0029 name "BYTEREGS"
.code break
0x002b: STRUCT size 0x000000e0 bits num_entries 0x0003
    bit_offset 0x00000000 type 0x002a name "h"
    bit_offset 0x00000000 type 0x0026 name "x"
    bit_offset 0x00000000 type 0x0028 name "w"
.code break
0x002c: TYPEDEF size 0x000000e0 bits
    scope union type 0x002b name "REGS"
.code break
0x002d: STRUCT size 0x00000060 bits num_entries 0x0006
    bit_offset 0x00000000 type 0x0005 name "es"
    bit_offset 0x00000010 type 0x0005 name "cs"
    bit_offset 0x00000020 type 0x0005 name "ss"
    bit_offset 0x00000030 type 0x0005 name "ds"
    bit_offset 0x00000040 type 0x0005 name "fs"
    bit_offset 0x00000050 type 0x0005 name "gs"
.code break
0x002e: TYPEDEF size 0x00000060 bits
    scope struct type 0x002d name "SREGS"
.code break
0x002f: STRUCT size 0x00000070 bits num_entries 0x0008
    bit_offset 0x00000000 type 0x0003 name "al"
    bit_offset 0x00000008 type 0x0003 name "ah"
    bit_offset 0x00000020 type 0x0003 name "bl"
    bit_offset 0x00000028 type 0x0003 name "bh"
    bit_offset 0x00000040 type 0x0003 name "cl"
    bit_offset 0x00000048 type 0x0003 name "ch"
    bit_offset 0x00000060 type 0x0003 name "dl"
    bit_offset 0x00000068 type 0x0003 name "dh"
.code break
0x0030: TYPEDEF size 0x00000070 bits
    scope struct type 0x002f name "REGPACKB"
.code break
0x0031: STRUCT size 0x00000140 bits num_entries 0x000c
    bit_offset 0x00000000 type 0x0005 name "ax"
    bit_offset 0x00000020 type 0x0005 name "bx"
    bit_offset 0x00000040 type 0x0005 name "cx"
    bit_offset 0x00000060 type 0x0005 name "dx"
    bit_offset 0x00000080 type 0x0005 name "bp"
    bit_offset 0x000000a0 type 0x0005 name "si"
    bit_offset 0x000000c0 type 0x0005 name "di"
    bit_offset 0x000000e0 type 0x0005 name "ds"
    bit_offset 0x000000f0 type 0x0005 name "es"
    bit_offset 0x00000100 type 0x0005 name "fs"
    bit_offset 0x00000110 type 0x0005 name "gs"
    bit_offset 0x00000120 type 0x0007 name "flags"
.code break
0x0032: TYPEDEF size 0x00000140 bits
    scope struct type 0x0031 name "REGPACKW"
.code break
0x0033: STRUCT size 0x00000140 bits num_entries 0x000c
    bit_offset 0x00000000 type 0x0007 name "eax"
    bit_offset 0x00000020 type 0x0007 name "ebx"
    bit_offset 0x00000040 type 0x0007 name "ecx"
    bit_offset 0x00000060 type 0x0007 name "edx"
    bit_offset 0x00000080 type 0x0007 name "ebp"
    bit_offset 0x000000a0 type 0x0007 name "esi"
    bit_offset 0x000000c0 type 0x0007 name "edi"
    bit_offset 0x000000e0 type 0x0005 name "ds"
    bit_offset 0x000000f0 type 0x0005 name "es"
    bit_offset 0x00000100 type 0x0005 name "fs"
    bit_offset 0x00000110 type 0x0005 name "gs"
    bit_offset 0x00000120 type 0x0007 name "flags"
.code break
0x0034: TYPEDEF size 0x00000140 bits
    scope struct type 0x0033 name "REGPACKX"
.code break
0x0035: STRUCT size 0x00000140 bits num_entries 0x0003
    bit_offset 0x00000000 type 0x0030 name "h"
    bit_offset 0x00000000 type 0x0034 name "x"
    bit_offset 0x00000000 type 0x0032 name "w"
.code break
0x0036: TYPEDEF size 0x00000140 bits
    scope union type 0x0035 name "REGPACK"
.code break
0x0037: STRUCT size 0x000001e0 bits num_entries 0x000f
    bit_offset 0x00000000 type 0x0007 name "gs"
    bit_offset 0x00000020 type 0x0007 name "fs"
    bit_offset 0x00000040 type 0x0007 name "es"
    bit_offset 0x00000060 type 0x0007 name "ds"
    bit_offset 0x00000080 type 0x0007 name "edi"
    bit_offset 0x000000a0 type 0x0007 name "esi"
    bit_offset 0x000000c0 type 0x0007 name "ebp"
    bit_offset 0x000000e0 type 0x0007 name "esp"
    bit_offset 0x00000100 type 0x0007 name "ebx"
    bit_offset 0x00000120 type 0x0007 name "edx"
    bit_offset 0x00000140 type 0x0007 name "ecx"
    bit_offset 0x00000160 type 0x0007 name "eax"
    bit_offset 0x00000180 type 0x0007 name "eip"
    bit_offset 0x000001a0 type 0x0007 name "cs"
    bit_offset 0x000001c0 type 0x0007 name "flags"
.code break
0x0038: TYPEDEF size 0x000001e0 bits
    scope struct type 0x0037 name "INTPACKX"
.code break
0x0039: STRUCT size 0x000001e0 bits num_entries 0x000f
    bit_offset 0x00000000 type 0x0005 name "gs"
    bit_offset 0x00000020 type 0x0005 name "fs"
    bit_offset 0x00000040 type 0x0005 name "es"
    bit_offset 0x00000060 type 0x0005 name "ds"
    bit_offset 0x00000080 type 0x0005 name "di"
    bit_offset 0x000000a0 type 0x0005 name "si"
    bit_offset 0x000000c0 type 0x0005 name "bp"
    bit_offset 0x000000e0 type 0x0005 name "sp"
    bit_offset 0x00000100 type 0x0005 name "bx"
    bit_offset 0x00000120 type 0x0005 name "dx"
    bit_offset 0x00000140 type 0x0005 name "cx"
    bit_offset 0x00000160 type 0x0005 name "ax"
    bit_offset 0x00000180 type 0x0005 name "ip"
    bit_offset 0x000001a0 type 0x0005 name "cs"
    bit_offset 0x000001c0 type 0x0007 name "flags"
.code break
0x003a: TYPEDEF size 0x000001e0 bits
    scope struct type 0x0039 name "INTPACKW"
.code break
0x003b: STRUCT size 0x00000170 bits num_entries 0x0008
    bit_offset 0x00000100 type 0x0003 name "bl"
    bit_offset 0x00000108 type 0x0003 name "bh"
    bit_offset 0x00000120 type 0x0003 name "dl"
    bit_offset 0x00000128 type 0x0003 name "dh"
    bit_offset 0x00000140 type 0x0003 name "cl"
    bit_offset 0x00000148 type 0x0003 name "ch"
    bit_offset 0x00000160 type 0x0003 name "al"
    bit_offset 0x00000168 type 0x0003 name "ah"
.code break
0x003c: TYPEDEF size 0x00000170 bits
    scope struct type 0x003b name "INTPACKB"
.code break
0x003d: STRUCT size 0x000001e0 bits num_entries 0x0003
    bit_offset 0x00000000 type 0x003c name "h"
    bit_offset 0x00000000 type 0x0038 name "x"
    bit_offset 0x00000000 type 0x003a name "w"
.code break
0x003e: TYPEDEF size 0x000001e0 bits
    scope union type 0x003d name "INTPACK"
.code break
0x003f: STRUCT size 0x00000038 bits num_entries 0x0004
    bit_offset 0x00000000 type 0x0006 name "exterror"
    bit_offset 0x00000020 type 0x0008 name "class"
    bit_offset 0x00000028 type 0x0008 name "action"
    bit_offset 0x00000030 type 0x0008 name "locus"
.code break
0x0040: TYPEDEF size 0x00000038 bits
    scope struct type 0x003f name "DOSERROR"
.code break
0x0041: STRUCT size 0x00000028 bits num_entries 0x0004
    bit_offset 0x00000000 type 0x0003 name "day"
    bit_offset 0x00000008 type 0x0003 name "month"
    bit_offset 0x00000010 type 0x0005 name "year"
    bit_offset 0x00000020 type 0x0003 name "dayofweek"
.code break
0x0042: TYPEDEF size 0x00000028 bits
    scope struct type 0x0041 name "dosdate_t"
.code break
0x0043: STRUCT size 0x00000020 bits num_entries 0x0004
    bit_offset 0x00000000 type 0x0003 name "hour"
    bit_offset 0x00000008 type 0x0003 name "minute"
    bit_offset 0x00000010 type 0x0003 name "second"
    bit_offset 0x00000018 type 0x0003 name "hsecond"
.code break
0x0044: TYPEDEF size 0x00000020 bits
    scope struct type 0x0043 name "dostime_t"
.code break
0x0045: ARRAY_ZERO size 0x000000a8 bits
    base_type 0x0008 high_bound 0x00000014
.code break
0x0046: ARRAY_ZERO size 0x00000068 bits
    base_type 0x0008 high_bound 0x0000000c
.code break
0x0047: STRUCT size 0x00000158 bits num_entries 0x0006
    bit_offset 0x00000000 type 0x0045 name "reserved"
    bit_offset 0x000000a8 type 0x0008 name "attrib"
    bit_offset 0x000000b0 type 0x0005 name "wr_time"
    bit_offset 0x000000c0 type 0x0005 name "wr_date"
    bit_offset 0x000000d0 type 0x0007 name "size"
    bit_offset 0x000000f0 type 0x0046 name "name"
.code break
0x0048: TYPEDEF size 0x00000158 bits
    scope struct type 0x0047 name "find_t"
.code break
0x0049: STRUCT size 0x00000040 bits num_entries 0x0004
    bit_offset 0x00000000 type 0x0005 name "total_clusters"
    bit_offset 0x00000010 type 0x0005 name "avail_clusters"
    bit_offset 0x00000020 type 0x0005 name "sectors_per_cluster"
    bit_offset 0x00000030 type 0x0005 name "bytes_per_sector"
.code break
0x004a: TYPEDEF size 0x00000040 bits
    scope struct type 0x0049 name "diskfree_t"
.code break
0x004b: ARRAY_ZERO size 0x00000180 bits
    base_type 0x0006 high_bound 0x0000000b
.code break
0x004c: ARRAY_ZERO size 0x00000180 bits
    base_type 0x0006 high_bound 0x0000000b
.code break
0x004d: ARRAY_ZERO size 0x00000180 bits
    base_type 0x000b high_bound 0x0000000b
.code break
0x004e: PROCEDURE
    NEAR 386 ret_type 0x0006 num_parms 00
.code break
0x004f: POINTER size 0x00000020 bits base_type 0x0021 NEAR 386
.code break
0x0050: PROCEDURE
    NEAR 386 ret_type 0x0019 num_parms 06
    parm_num 00 parm_type 0x0006
    parm_num 01 parm_type 0x0006
    parm_num 02 parm_type 0x0006
    parm_num 03 parm_type 0x0006
    parm_num 04 parm_type 0x0006
    parm_num 05 parm_type 0x000b
.code break
0x0051: PROCEDURE
    NEAR 386 ret_type 0x0019 num_parms 04
    parm_num 00 parm_type 0x0006
    parm_num 01 parm_type 0x0006
    parm_num 02 parm_type 0x0006
    parm_num 03 parm_type 0x0006
.code break
0x0052: PROCEDURE
    NEAR 386 ret_type 0x0019 num_parms 06
    parm_num 00 parm_type 0x0006
    parm_num 01 parm_type 0x0006
    parm_num 02 parm_type 0x0006
    parm_num 03 parm_type 0x0008
    parm_num 04 parm_type 0x0008
    parm_num 05 parm_type 0x0008
.code break
0x0053: ARRAY_ZERO size 0x00000280 bits
    base_type 0x0008 high_bound 0x0000004f
.code break
0x0054: PROCEDURE
    NEAR 386 ret_type 0x0019 num_parms 02
    parm_num 00 parm_type 0x0006
    parm_num 01 parm_type 0x0006
.code break
                               Symbol Information
                               ------------------
MEM_LOC (static) name "NarrowTitle" type 0x000b
    data 94 00 00 00 00 00
    fixup SEG_REL  POINTER386  Frame: GI(0x0002)  Target: SI(0x0004)
.code break
MEM_LOC (static) name "WideTitle" type 0x000b
    data 90 00 00 00 00 00
    fixup SEG_REL  POINTER386  Frame: GI(0x0002)  Target: SI(0x0004)
.code break
MEM_LOC (static) name "Jump" type 0x004b
    data 00 00 00 00 00 00
    fixup SEG_REL  POINTER386  Frame: GI(0x0002)  Target: SI(0x0004)
.code break
MEM_LOC (static) name "MonthDays" type 0x004c
    data 30 00 00 00 00 00
    fixup SEG_REL  POINTER386  Frame: GI(0x0002)  Target: SI(0x0004)
.code break
MEM_LOC (static) name "MonthName" type 0x004d
    data 60 00 00 00 00 00
    fixup SEG_REL  POINTER386  Frame: GI(0x0002)  Target: SI(0x0004)
.code break
CHANGE_SEG
    data 00 00 00 00 00 00
    fixup SEG_REL  POINTER386  Frame: GI(0x0002)  Target: SI(0x0001)
.code break
BLOCK name "main" type 0x004e start_offset 0x00000000 size 0x000000e7
    PROLOG size 12 ret_addr_offset 0x00000010
    Return Value:
        REGISTER name "" type 0x0000: EAX
    BP_OFFSET 0xfffffffc name "curr_time" type 0x001e
    BP_OFFSET 0xfffffff8 name "tyme" type 0x004f
    EPILOG size 05
BLOCK_END
.code break
BLOCK name "Calendar" type 0x0050 start_offset 0x000000e7 size 0x00000199
    Parms:
        REGISTER name "month" type 0x0006: EAX
        REGISTER name "year" type 0x0006: EDX
        REGISTER name "row" type 0x0006: EBX
        REGISTER name "col" type 0x0006: ECX
        BP_OFFSET 0x00000008 name "width" type 0x0006
        BP_OFFSET 0x0000000c name "title" type 0x000b
    PROLOG size 16 ret_addr_offset 0x00000004
    BP_OFFSET 0xfffffffc name "start" type 0x0006
    BP_OFFSET 0xfffffff8 name "days" type 0x0006
    BP_OFFSET 0xfffffff4 name "box_width" type 0x0006
    BP_OFFSET 0xfffffff0 name "str" type 0x000b
    BP_OFFSET 0xffffffec name "i" type 0x0006
    BP_OFFSET 0x0000000c name "title" type 0x000b
    BP_OFFSET 0x00000008 name "width" type 0x0006
    BP_OFFSET 0xffffffdc name "col" type 0x0006
    BP_OFFSET 0xffffffe0 name "row" type 0x0006
    BP_OFFSET 0xffffffe4 name "year" type 0x0006
    BP_OFFSET 0xffffffe8 name "month" type 0x0006
    EPILOG size 04
BLOCK_END
.code break
BLOCK name "Box" type 0x0051 start_offset 0x00000280 size 0x000000c4
    Parms:
        REGISTER name "row" type 0x0006: EAX
        REGISTER name "col" type 0x0006: EDX
        REGISTER name "width" type 0x0006: EBX
        REGISTER name "height" type 0x0006: ECX
    PROLOG size 14 ret_addr_offset 0x00000004
    BP_OFFSET 0xfffffffc name "i" type 0x0006
    BP_OFFSET 0xffffffec name "height" type 0x0006
    BP_OFFSET 0xfffffff0 name "width" type 0x0006
    BP_OFFSET 0xfffffff4 name "col" type 0x0006
    BP_OFFSET 0xfffffff8 name "row" type 0x0006
    EPILOG size 02
BLOCK_END
.code break
BLOCK name "Line" type 0x0052 start_offset 0x00000344 size 0x00000064
    Parms:
        REGISTER name "row" type 0x0006: EAX
        REGISTER name "col" type 0x0006: EDX
        REGISTER name "width" type 0x0006: EBX
        REGISTER name "left" type 0x0008: ECX
        BP_OFFSET 0x00000008 name "centre" type 0x0008
        BP_OFFSET 0x0000000c name "right" type 0x0008
    PROLOG size 15 ret_addr_offset 0x00000004
    BP_OFFSET 0xffffffac name "buffer" type 0x0053
    BP_OFFSET 0x0000000c name "right" type 0x0008
    BP_OFFSET 0x00000008 name "centre" type 0x0008
    BP_OFFSET 0xfffffffc name "left" type 0x0008
    BP_OFFSET 0xffffffa0 name "width" type 0x0006
    BP_OFFSET 0xffffffa4 name "col" type 0x0006
    BP_OFFSET 0xffffffa8 name "row" type 0x0006
    EPILOG size 04
BLOCK_END
.code break
BLOCK name "PosCursor" type 0x0054 start_offset 0x000003a8 size 0x00000045
    Parms:
        REGISTER name "row" type 0x0006: EAX
        REGISTER name "col" type 0x0006: EDX
    PROLOG size 15 ret_addr_offset 0x00000008
    BP_OFFSET 0xffffffe4 name "regs" type 0x002c
    BP_OFFSET 0xffffffdc name "col" type 0x0006
    BP_OFFSET 0xffffffe0 name "row" type 0x0006
    EPILOG size 03
BLOCK_END
.code break
BLOCK name "ClearScreen" type 0x001a start_offset 0x000003ed size 0x0000003c
    PROLOG size 12 ret_addr_offset 0x0000000c
    BP_OFFSET 0xffffffe4 name "regs" type 0x002c
    EPILOG size 04
BLOCK_END
.tinyexam end
:cmt. .do end
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
:set symbol="ccmd16"   value="wfc".
:set symbol="ccmd32"   value="wfc386".
:cmt. .if '&machine' eq '8086' .do begin
.np
The following are 16-bit examples.
.tinyexam begin
&prompt.&ccmd16 sieve
&prompt.womp &sw.f- &sw.dx sieve
&company Object Module Processor Version 1.1
.cpyrit 1990
.trdmrk
Converting sieve.OBJ
                            Miscellaneous Information
                            -------------------------
Processor       : 8086
Memory Model    : Large
Floating-point  : Inline Emulation
Optimized       : Yes
Lines Present   : No
Types Present   : No
Symbs Present   : No
Source Language : Fortran
.tinyexam end
.tinyexam begin
&prompt.&ccmd16 &sw.d1 sieve
&prompt.womp &sw.f- &sw.dx sieve
&company Object Module Processor Version 1.1
.cpyrit 1990
.trdmrk
Converting sieve.OBJ
                            Miscellaneous Information
                            -------------------------
Processor       : 8086
Memory Model    : Large
Floating-point  : Inline Emulation
Optimized       : Yes
Lines Present   : Yes
Types Present   : No
Symbs Present   : No
Source Language : Fortran
.tinyexam end
.tinyexam begin
&prompt.&ccmd16 &sw.d2 sieve
&prompt.womp &sw.f- &sw.dx sieve
&company Object Module Processor Version 1.1
.cpyrit 1990
.trdmrk
Converting sieve.OBJ
                            Miscellaneous Information
                            -------------------------
Processor       : 8086
Memory Model    : Large
Floating-point  : Inline Emulation
Optimized       : Yes
Lines Present   : Yes
Types Present   : Yes
Symbs Present   : Yes
Source Language : Fortran
.code break
                                Type Information
                                ----------------
0x0000: RESERVED
.code break
0x0001: RESERVED
.code break
0x0002: INTEGER size 0x00000008 bits UNSIGNED
.code break
0x0003: INTEGER size 0x00000020 bits UNSIGNED
.code break
0x0004: INTEGER size 0x00000008 bits SIGNED
.code break
0x0005: INTEGER size 0x00000010 bits SIGNED
.code break
0x0006: INTEGER size 0x00000020 bits SIGNED
.code break
0x0007: REAL size 0x00000020 bits
.code break
0x0008: REAL size 0x00000040 bits
.code break
0x0009: COMPLEX size 0x00000040 bits
.code break
0x000a: COMPLEX size 0x00000080 bits
.code break
0x000b: PROCEDURE
    FAR ret_type 0x000c num_parms 00
.code break
0x000c: VOID
.code break
0x000d: SUBRANGE size 0x00000020 bits
    base_type 0x0006(SIGNED) low 0x00000002 high 0x000003e8
.code break
0x000e: ARRAY size 0x00001f38 bits
    base_type 0x0002 index_type 0x000d
.code break
                               Symbol Information
                               ------------------
CHANGE_SEG
    data 00 00 00 00
    fixup SEG_REL  POINTER  Frame: TARGET  Target: SI(0x0007)
.code break
BLOCK name "FMAIN" type 0x000b start_offset 0x00000000 size 0x00000146
    PROLOG size 0b ret_addr_offset 0x0000000c
    MEM_LOC (static) name "NUMBERS" type 0x000e
        data 00 00 00 00
        fixup SEG_REL  POINTER  Frame: TARGET  Target: SI(0x0004)
    BP_OFFSET 0xfffffff6 name "PRIMES" type 0x0006
    BP_OFFSET 0xfffffff2 name "K" type 0x0006
    BP_OFFSET 0xffffffee name "I" type 0x0006
    EPILOG size 09
BLOCK_END
.tinyexam end
:cmt. .do end
:cmt. .if '&machine' eq '80386' .do begin
.np
The following are 32-bit examples.
.tinyexam begin
&prompt.&ccmd32 sieve
&prompt.womp &sw.f- &sw.dx sieve
&company Object Module Processor Version 1.1
.cpyrit 1990
.trdmrk
Converting sieve.OBJ
                            Miscellaneous Information
                            -------------------------
Processor       : 80386
Memory Model    : Flat
Floating-point  : 80x87 Inline Code
Optimized       : Yes
Lines Present   : No
Types Present   : No
Symbs Present   : No
Source Language : Fortran
.tinyexam end
.tinyexam begin
&prompt.&ccmd32 &sw.d1 sieve
&prompt.womp &sw.f- &sw.dx sieve
&company Object Module Processor Version 1.1
.cpyrit 1990
.trdmrk
Converting sieve.OBJ
                            Miscellaneous Information
                            -------------------------
Processor       : 80386
Memory Model    : Flat
Floating-point  : 80x87 Inline Code
Optimized       : Yes
Lines Present   : Yes
Types Present   : No
Symbs Present   : No
Source Language : Fortran
.tinyexam end
.tinyexam begin
&prompt.&ccmd32 &sw.d2 sieve
&prompt.womp &sw.f- &sw.dx sieve
&company Object Module Processor Version 1.1
.cpyrit 1990
.trdmrk
Converting sieve.OBJ
                            Miscellaneous Information
                            -------------------------
Processor       : 80386
Memory Model    : Flat
Floating-point  : 80x87 Inline Code
Optimized       : Yes
Lines Present   : Yes
Types Present   : Yes
Symbs Present   : Yes
Source Language : Fortran
.code break
                                Type Information
                                ----------------
0x0000: RESERVED
.code break
0x0001: RESERVED
.code break
0x0002: INTEGER size 0x00000008 bits UNSIGNED
.code break
0x0003: INTEGER size 0x00000020 bits UNSIGNED
.code break
0x0004: INTEGER size 0x00000008 bits SIGNED
.code break
0x0005: INTEGER size 0x00000010 bits SIGNED
.code break
0x0006: INTEGER size 0x00000020 bits SIGNED
.code break
0x0007: REAL size 0x00000020 bits
.code break
0x0008: REAL size 0x00000040 bits
.code break
0x0009: COMPLEX size 0x00000040 bits
.code break
0x000a: COMPLEX size 0x00000080 bits
.code break
0x000b: PROCEDURE
    NEAR 386 ret_type 0x000c num_parms 00
.code break
0x000c: VOID
.code break
0x000d: SUBRANGE size 0x00000020 bits
    base_type 0x0006(SIGNED) low 0x00000002 high 0x000003e8
.code break
0x000e: ARRAY size 0x00001f38 bits
    base_type 0x0002 index_type 0x000d
.code break
                               Symbol Information
                               ------------------
CHANGE_SEG
    data 00 00 00 00 00 00
    fixup SEG_REL  POINTER386  Frame: GI(0x0002)  Target: SI(0x0007)
.code break
BLOCK name "FMAIN" type 0x000b start_offset 0x00000000 size 0x000000c3
    PROLOG size 09 ret_addr_offset 0x00000010
    MEM_LOC (static) name "NUMBERS" type 0x000e
        data 00 00 00 00 00 00
        fixup SEG_REL  POINTER386  Frame: GI(0x0002)  Target: SI(0x0004)
    BP_OFFSET 0xfffffff8 name "PRIMES" type 0x0006
    BP_OFFSET 0xfffffff4 name "K" type 0x0006
    BP_OFFSET 0xfffffff0 name "I" type 0x0006
    EPILOG size 05
BLOCK_END
.tinyexam end
:cmt. .do end
.do end
.*
.section WOMP Messages
.*
.np
Many of the error messages that are listed below are unlikely to
appear.
The object module processor is thorough at checking for cases that
fall outside of the range of known object module formats (OMF).
It is prepared to issue a message if such a case appears.
.autonote
.note
.us @Include file is too large (max 64k)
.np
This fatal error message is issued when any WOMP directive file
exceeds more than 64K bytes of text.
The solution is to split the file into smaller files.
.note
.us Access classes (RW, EO, RO, ER) not supported in MS386 OMF
.np
This warning message may be issued when converting Phar Lap Easy OMF
to Microsoft OMF.
Microsoft OMF does not support Read-Write, Execute-Only, Read-Only, or
Execute-Read access classes.
.note
.us ADD_PREV_SEG no longer supported; probable cause: old version of
.us &company C used
.np
This fatal error message is issued when an obsolete debug record
(supported by &company Express C) is encountered.
.note
.us Array too large (%X elements of size %X)
.np
This fatal error message may be issued when converting a debug record
from one format to another.
This message is unlikely to occur since the array must exceed 4
gigabytes in size.
.note
.us ARRAY_DESC types not supported
.np
This warning message may be issued when converting to Microsoft or
Turbo debug records.
This type is used by the &company FORTRAN 77 compiler when the array
bounds are not known at compile time.
.note
.us CHARBLOCK_IND types not supported
.np
This warning message may be issued when converting to Microsoft or
Turbo debug records.
This type is used by the &company FORTRAN 77 compiler when the character
string size is not known at compile time (e.g., CHARACTER *(*) STR).
.note
.us Disk error during %s: %Z
.np
This fatal error message is issued when an error occurs attempting to
read, write, seek, rename or erase a file or when an attempt to spawn
the &libname, &libcmdup, fails.
The file name and a message corresponding to the C "errno" value is
printed.
.note
.us Enumerated type too large to represent
.np
This fatal error message may be issued when converting to Turbo debug
records.
Turbo debug format does not support 32-bit enumerated types.
.note
.us Impossible to translate register variable
.np
This fatal error message may be issued when converting to Turbo debug
records.
Turbo debug format does not support as many register combinations
as &company does.
However, this message is unlikely to be issued since &company presently
does not use any combination that is not supported by Borland.
.note
.us Incompatible version of the debugging information
.np
This fatal error message may be issued when converting a &company 7.0
debug record format and the "p7" option was not specified on the
command line.
It may also be issued when attempting to convert a &company debug record
format that is newer than the version of WOMP being used.
In this case, the solution is to use a more recent version of WOMP.
.note
.us IND_REG not supported; probable cause: function returning a
.us union/struct
.np
This warning message may be issued when converting to Microsoft or
Turbo debug records.
Neither supports a debug record describing a union/struct return
type.
.note
.us Invalid fixup
.np
This fatal error message is issued when an invalid object file record
is encountered.
The probable cause is that file is not an object file.
.note
.us Invalid processor/model COMENT record
.np
This warning message is issued when an invalid processor/model COMENT
record is encountered.
WOMP will assume small model for 16-bit object files and flat model
for 32-bit object files.
.note
.us Maximum object record size (%u byte) exceeded
.np
This fatal error message is issued when an object file record exceeds
the 4 kilobyte limit.
.note
.us More data in LEDATAs than defined in SEGDEF for seg %x
.np
This fatal error message is issued when an invalid object file record
is encountered.
.note
.us Name (%s) too long, truncated.
.np
This warning message is issued when converting an object file record
to Microsoft OMF.
A maximum of 256 bytes (minus the amount of data already in the
record) is allowed.
The long name is simply truncated.
The long name is printed as part of the message.
.note
.us No 4k Page-aligned segments in MS386 OMF
.np
This warning message may be issued when converting Phar Lap Easy OMF
to Microsoft OMF.
WOMP will change the page alignment to 1K.
.note
.us No LIDATAs allowed for &company debug info
.np
This fatal error message is issued when an LIDATA (for a special debug
segment) is encountered.
Neither WOMP nor the &lnkname supports it.
.note
.us No loader resolved fixups allowed in Easy OMF-386
.np
This warning message may be issued when converting Microsoft OMF
to Phar Lap Easy OMF.
WOMP will convert it to a linker resolved fixup.
.note
.us No support for BIG 32-bit SEGDEFs
.np
This fatal error message is issued when a segment that is exactly 4
gigabytes in size is encountered.
.note
.us No support for LTL segments
.np
This fatal error message is issued when a load time locatable segment
is encountered.
Intel OMF describes this type of segment but generally it is not used
in DOS object files.
.note
.us No support for multi-register symbols
.np
This warning message may be issued when converting to Microsoft OMF.
Microsoft OMF does not support symbols in multiple registers.
This type of debug record will not be produced by existing &company
compilers so its appearance should be rare.
.note
.us Object file contains invalid data
.np
This fatal error message is issued when no definitions (SEGDEF,
PUBDEF, GRPDEF, LNAMES, EXTDEF, etc.) are encountered in the object
file.
The probable cause is that the file is not an object file or that it
was incompletely written.
.note
.us Out of memory
.np
This fatal error message is issued when WOMP cannot allocate any more
memory for its data structures.
.note
.us Premature end of file encountered
.np
This fatal error message is issued when WOMP encounters the end of an
object file before the expected MODEND record.
The probable cause is that the object file was incompletely written.
.note
.us Subrange too large to represent
.np
This fatal error message is issued when more than 32 bits is required
for a subrange type.
This message cannot arise from any object file produced by a &company
compiler.
.note
.us THEADR record missing
.np
This fatal error message is issued when the THEADR (translator header)
record is not present as the first record in an object file.
.note
.us Too many registers in aggregate
.np
This fatal error message is issued when more than 4 registers are
required for an aggregate (e.g., AX:BC:CX:DX is a 4-register aggregate
that could be used for 64-bit floating-point items).
This message cannot arise from any object file produced by a &company
compiler.
.note
.us Too many types to continue conversion
.np
This fatal error message is issued when more than 65535 types appear
in an object file.
This situation is unlikely to occur.
.note
.us Unable to CHANGE_SEG inside code block
.np
This fatal error message is issued when a change of code segment
appears inside of a nested scope.
This is invalid in any object file.
.note
.us Unable to open file %s: %Z
.np
This fatal error message is issued when WOMP is unable to open an
object file or a directive file.
The file name and a message corresponding to the C "errno" value is
printed.
.note
.us Unable to process 32-bit repeat count in LIDATA
.np
This fatal error message may be issued when converting from Microsoft
OMF to Phar Lap Easy OMF.
Microsoft OMF allows a 32-bit repeat count while Phar Lap Easy OMF
allows a 16-bit repeat count.
In this case, the repeat count could not be represented in 16 bits.
.note
.us Unknown COMDEF LEAF type (%t)
.np
This fatal error message is issued when WOMP encounters a COMMON block
definition of an invalid type in a Microsoft OMF file.
The type number is printed as a 2-digit hexadecimal value.
.note
.us Unknown register type, assuming AX
.np
This fatal error message is issued when an unsupported register type
is encountered in a debug record.
All register types for the current Intel 80x86 family architecture are
supported so it is unlikely that this message will appear.
The probable cause would be an invalid object file record.
.note
.us Unknown SCOPE (%s)
.np
This fatal error message is issued when a scope other than STRUCT,
UNION or ENUM is encountered in an object file.
It is possible that, in the future, a new compiler might generate
other types of scope.
The scope name is printed as part of the message.
.note
.us Unknown SEGDEF align value (%t)
.np
This fatal error message is issued when the alignment type is not one
of "ABSOLUTE", "BYTE", "WORD", "PARA", "PAGE", "DWORD" or "4KPAGE".
The type number is printed as a 2-digit hexadecimal value.
.note
.us Unsupported floating-point option
.np
This warning message is issued when the floating-point option in the
THEADR (translator header) record is not one of "emulated-inline" (FPI
option), "emulated-calls" (FPC option), or "80x87-inline" (FPI87
option).
.note
.us Unsupported memory model
.np
This warning message is issued when the memory model option in the
THEADR (translator header) record is not one of "small", "medium",
"compact", "large", "huge", "tiny" or "flat".
.note
.us Unsupported scalar size
.np
This warning message is issued when converting to Turbo debug format
and an integer item encountered in a debug record is greater than 32
bits and not 64 bits in size.
WOMP will create an equivalent array type of an appropriate size.
.note
.us Debugging segment too large (max is 64k)
.np
This fatal error message is issued when a debugging segment is greater
than 64K bytes.
Neither the linker nor debugger will handle a debugging segment that
is larger than this.
.endnote
