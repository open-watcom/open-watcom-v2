.*
.*
.* Messages take one of the following forms:
.*
.* .errnote 2002 ** internal ** - %s
.* .errnote 2010,3010 I/O error processing %s : %s
.*
.* For the .H file, we want the last three digits (i.e., 010)
.* to form the context number.
.*
.dm errnote begin
.se *ttl$=&'substr(&*,&'pos(' ',&*)+1)
.se *pos=0
.if &e'&dohelp. eq 0 .do begin
.   .note MSG &*1
.   .bd &*ttl$
.   .ix 'message' '&*1'
.do end
.el .do begin
:ZH2.&*
.   .cntents &*
.   .pu 1 .ixsect &*
:ZI1.&*ttl$
.   .pu 1 .ixline `&*ttl$`
.   .se *ctxn=&'substr(&*,2,&'pos(' ',&*)-2)
.   .se *pos=&'pos(',',&*1)
.   .if &*pos. ne 0 .do begin
.   .   .se *ctxn=&'substr(&*,2,&*pos.-2)
.   .   .se *ttl$=&'substr(&*,&*pos.+1)
:ZI1.&*ttl$
.   .   .pu 1 .ixline `&*ttl$`
.   .do end
.   .if '&dotarget' eq 'os2' .do begin
.   .   .if &'length(&*ctxn.) eq 3 .do begin
.   .   .   .se *ctxn='20&*ctxn.'
.   .   .do end
.   .   .el .do begin
.   .   .   .se *ctxn='2&*ctxn.'
.   .   .do end
.   .do end
.   .ctxstr &*
.   .pu 2 #define HLP_&'upper(&ctx_str.) &*ctxn
.*  .np
.*  .us &*
.do end
.dm errnote end
.*
.* close the header file
.*
.pu 2
.*
.im wlerrfmt
.np
The messages listed contain references to
.id %s,
.id %S,
.id %a,
.id %x,
.id %d,
.id %l,
and
.id %f.
They represent strings that are substituted by the &lnkname to make
the error message more precise.
.autonote
.note
.id %s
represents a string.
This may be a segment or group name, or the name of a linker directive
or option.
.note
.id %S
represents the name of a symbol.
.note
.id %a
represents an address.
The format of the address depends on the format of the executable file
being generated.
.note
.id %x
represents a hexadecimal number.
.note
.id %d
represents integers in the range -32768 and 32767.
.note
.id %l
represents integers in the range -2147483648 and 2147483647.
.note
.id %f
represents an executable file format such as DOS, ZDOS, WINDOWS, PHARLAP,
NOVELL, OS2, QNX or ELF.
.endnote
.np
The following is a list of all warning and error messages produced by
the &lnkname followed by a description of the message.
A message may contain more than one reference to "%s".
In such a case, the description will reference them as "%sn" where n
is the occurrence of "%s" in the message.
.if &e'&dohelp eq 0 .do begin
.begnote
.do end
.*
.errnote 2002 ** internal ** - %s
.np
If this message occurs, you have found a bug in the linker and should
report it.
.*
.errnote 2008 cannot open %s1 : %s2
.np
An error occurred while trying to open the file "%s1".
The reason for the error is given by "%s2".
Generally this error message is issued when the linker cannot open a
file (e.g., an object file or an executable file).
.*
.errnote 3009 dynamic memory exhausted
.np
The linker uses all available memory when linking an application.
.if '&target' ne 'QNX' .do begin
For DOS-hosted versions of the linker, this includes expanded memory
(EMS) and extended memory.
.do end
When all available memory is used, a spill file will be used.
Therefore, unless you are low on disk space, the linker will always
be able to generate the executable file.
.if &e'&optdoc eq 0 .do begin
Dynamic memory is the memory the linker uses to build its internal
data structures and symbol table.
.if '&target' eq 'QNX' .do begin
A spill file is not used for dynamic memory.
.do end
.el .do begin
:CMT. Dynamic memory is the amount of conventional memory (below 1 megabyte)
:CMT. available on your machine;
:CMT. a spill file is not used for dynamic memory.
Dynamic memory is the amount of unallocated memory available on your
machine (including virtual memory for those operating systems that
support it).
A spill file is not used for dynamic memory.
.do end
If the linker issues this message, it cannot link your application.
The following are suggestions that may help you in this situation.
.autonote
.note
Concatenate all your object files into one and specify only the
resulting object file as input to the linker.
.if '&target' eq 'QNX' .do begin
For example, you can issue the following command.
.millust begin
% cat *.obj > all.tmp
% mv all.tmp all.obj
.millust end
.do end
.el .do begin
For example, if you are linking in a (Z)DOS environment, you can issue
the following DOS command.
.millust begin
C>copy/b *.obj all.obj
.millust end
.do end
.pc
This technique only works for OMF-type object files.
This significantly reduces the size of the file list the linker must
maintain.
.note
Object files may contain a record which specifies the module name.
This information is used by &dbgname to locate modules during a
debugging session and usually contains the full path of the source
file.
This can consume a significant amount of memory when many such object
files are being linked.
If your source is being compiled by the &cmpcname or C++ compiler, you
can use the "nm" option to set the module name to just the file name.
This reduces the amount of memory required by the linker.
If your are using &dbgname to debug your application, you may have to
use the "set source" command so that the source corresponding to a
module can be located.
.note
Typically, when you are compiling a program for a large code model,
each module defines a different "text" segment.
If you are compiling your application using the &cmpcname or C++
compiler, you can reduce the number of "text" segments that the linker
has to process by specifying the "nt" option.
The "nt" option allows you to specify the name of the "text" segment
so that a group of object files define the same "text" segment.
.endnote
.do end
.*
.errnote 2010,3010 I/O error processing %s1 : %s2
.np
An error has occurred while processing the file "%s1".
The cause of the error is given by "%s2".
This error is usually detected while reading from object and library
files or writing to the spill file or executable file.
For example, this error would be issued if a "disk full" condition
existed.
.*
.errnote 2011 invalid object file attribute
.np
The linker encountered an object file that was not of the format
required of an object file.
.*
.errnote 2012 invalid library file attribute
.np
The linker encountered a library file that was not of the format
required of a library file.
.*
.errnote 3013 break key detected
.np
The linking process was interrupted by the user from the keyboard.
.*
.errnote 1014 stack segment not found
.np
The linker identifies the stack segment by a segment defined as having
the "STACK" attribute.
This message is issued if no such segment is encountered.
This usually happens if the linker cannot find the run-time libraries
required to link your application.
.*
.errnote 2015 bad relocation type specified
.np
This message is issued if a a relocation is found in an object file
which the linker does not support.
.*
.errnote 2016 %a: absolute target invalid for self-relative relocation
.np
This message is issued, for example, if a near call or jump is made to
an external symbol which is defined using the "EQU" assembler
directive.
"%a" identifies the location of the near call or jump instruction.
.*
.errnote 2017 bad location specified for self-relative relocation at %a
.np
This message is issued if a bad fixup is encountered.
"%a" defines the location of the fixup.
.*
.errnote 2018 relocation offset at %a is out of range
.np
This message is issued when the offset part of a relocation exceeds
64K in a 16-bit executable or an Alpha executable.
"%a" defines the location of the fixup.
The error is most commonly caused by errors in coding assembly
language routines.
Consider a module that references an external symbol that is defined
in a segment different from the one in which the reference occurred.
The module, however, specifies that the segment in which the symbol is
defined is the same segment as the segment that references the symbol.
This error is most commonly caused when the "EXTRN" assembler
directive is placed after the "SEGMENT" assembler directive for the
segment referencing the symbol.
If the segment that references the symbol is allocated far enough away
from the segment that defines the symbol, the linker will issue this
message.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 1019 segment relocation at %a
.np
This message is issued when a 16-bit segment relocation is encountered
and "FORMAT DOS COM", "FORMAT PHARLAP" or "FORMAT NOVELL" has been
specified.
None of the above executable file formats allow segment relocation.
"%a" identifies the location of the segment relocation.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 2020 size of group %s exceeds 64k by %l bytes
.np
The group "%s" has exceeded the maximum size (64K) allowed for a group
in a 16-bit executable by "%l" bytes.
Usually, the group is "DGROUP" (the default data segment) and your
application has placed too much data in this group.
.if &e'&optdoc eq 0 .do begin
One of the following may solve this problem.
.autonote
.note
If you are using the &cmpcname or C++ compiler, you can place some of
your data in a far segment by using the "far" keyword when defining
data.
You can also decrease the value of the data threshold by using the
"zt" compiler option.
Any datum whose size exceeds the value of the data threshold will be
placed in a far segment.
.note
If you are using the &cmpfname compiler,
you can decrease the value of the data threshold by using the "dt"
compiler option.
Any datum whose size exceeds the value of the data threshold will be
placed in a far segment.
.endnote
.do end
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 2021 size of segment %s exceeds 64k by %l bytes
.np
The segment "%s" has exceeded the maximum size (64K) for a segment in
a 16-bit executable.
This usually occurs if you are linking a 16-bit application that has
been compiled for a small code model and the size of the application
has grown in such a way that the size of the code segment ("_TEXT")
has exceeded 64K.
.if '&overlay' eq 'yes' .do begin
You can overlay your application or compile it for a large code model
.do end
.el .do begin
You can compile your application for a large code model
.do end
if you cannot reduce the amount of code in your application.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 2022 cannot have a starting address with an imported symbol
.np
When generating an OS/2 executable file, a symbol imported from a DLL
cannot be a start address.
When generating a NetWare executable file, a symbol imported from
an NLM cannot be a start address.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 1023 no starting address found, using %a
.np
The starting address defines the location where execution is to begin
and must be defined by a special "module end" record in one of the
object files linked into your application.
This message is issued if no such record is encountered in which case
a default starting address, namely "%a", will be used.
This usually happens if the linker cannot find the run-time libraries
required to link your application.
.*
.if '&overlay' eq 'yes' .do begin
.errnote 2024 missing overlay loader
.np
This message is issued when an overlayed 16-bit DOS executable is
being linked and the overlay manager has not been encountered.
This usually happens if the linker cannot find the run-time libraries
required to link your application.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.do end
.*
.if '&overlay' eq 'yes' .do begin
.errnote 2025 short vector %d is out of range
.np
This message is issued when the linker is creating an overlayed 16-bit
DOS executable and "OPTION SMALL" is specified.
Since an overlay vector contains a near call to the overlay loader
followed by a near jump to the routine corresponding to the overlay
vector, all code including the overlay manager and all overlay vectors
must be less than 64K.
This message is issued if the offset of an overlay vector from the
overlay loader or the corresponding routine exceeds 64K.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.do end
.*
.errnote 2026 redefinition of reserved symbol %s
.np
The linker defines certain reserved symbols.
These symbols are "_edata", "_end", "__OVLTAB__", "__OVLSTARTVEC__",
"__OVLENDVEC__", "__LOVLLDR__", "__NOVLLDR__", "__SOVLLDR__",
"__LOVLINIT__", "__NOVLINIT__" and "__SOVLINIT__".
.if &e'&optdoc eq 0 .do begin
.if '&overlay' eq 'yes' .do begin
The symbols "__OVLTAB__", "__OVLSTARTVEC__", "__OVLENDVEC__",
"__LOVLLDR__", "__NOVLLDR__", "__SOVLLDR__", "__LOVLINIT__",
"__NOVLINIT__" and "__SOVLINIT__" are defined only if you are using
overlays in 16-bit DOS executables.
.do end
.do end
The symbols "_edata" and "_end" are defined only if the "DOSSEG"
option is specified.
Your application must not attempt to define these symbols.
"%s" identifies the reserved symbol.
.*
.errnote 1027 redefinition of %S ignored
.np
The symbol "%S" has been defined by more that one module; the first
definition is used.
This is only a warning message.
Note that if a symbol is defined more than once and its address is the
same in both cases, no warning will be issued.
.if &e'&optdoc eq 0 .do begin
This prevents the warning message from being issued when linking
FORTRAN 77 modules that contain common blocks.
.do end
.*
.errnote 1028,2028 %S is an undefined reference
.np
The symbol "%S" has been referenced but not defined.
Check that the spelling of the symbol is consistent.
.if &e'&optdoc eq 0 .do begin
If you wish the linker to ignore undefined references, use the
"UNDEFSOK" option.
.do end
.*
.errnote 2029 premature end of file encountered
.np
This error is issued while processing object files and object modules
from libraries and is caused if the end of the file or module is
reached before the "module end" record is encountered.
The probable cause is a truncated object file.
.*
.errnote 2030 multiple starting addresses found
.np
The starting address defines the location where execution is to begin
and is defined by a "module end" record in a particular object file.
This message is issued if more than one object file contains a "module
end" record that defines a starting address.
.*
.errnote 2031 segment %s is in group %s and group %s
.np
The segment "%s1" has been defined to be in group "%s2" in one module
and in group "%s3" in another module.
A segment can only belong to one group.
.*
.errnote 1032 record (type 0x%x) not processed
.np
An object record type not supported by the linker has been
encountered.
This message is issued when linking object modules created by other
compilers or assemblers that create object files with records that the
linker does not support.
.*
.errnote 2033,3033 directive error near '%s'
.np
A syntax error occurred while the linker was processing directives.
"%s" specifies where the error occurred.
.*
.errnote 2034 %a cannot have an offset with an imported symbol
.np
An imported symbol is one that was specified in an "IMPORT" directive.
Imported symbols are defined in Windows or OS/2 16-bit DLLs and in
Netware NLMs.
References to imported symbols must always have an offset value of 0.
If "DosWrite" is an imported symbol, then referencing "DosWrite+2" is
illegal.
"%a" defines the location of the illegal reference.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 1038 DEBUG directive appears after object files
.np
This message is issued if the first "DEBUG" directive appears after a
"FILE" directive.
A common error is to specify a "DEBUG" directive after the "FILE"
directives in which case no debugging information for those object
files is generated in the executable file.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued when using &powerpp..
If it is issued, please report this problem.
.do end
.*
.errnote 2039 ALIGNMENT value too small
.np
The value specified in the "ALIGNMENT" option refers to the alignment
of segments in the executable file.
For 16-bit Windows or 16-bit OS/2, segments in the executable file are
pointed to by a segment table.
An entry in the segment table contains a 16-bit value which is a
multiple of the alignment value.
Together they form the offset of the segment from the start of the
segment table.
The smaller the alignment, the bigger the value required in the
segment table to point to the segment.
If this value exceeds 64K, then a larger alignment value is required
to decrease the size that goes in the segment table.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 2040 ordinal in IMPORT directive not valid
.np
The specified ordinal in the "IMPORT" directive is incorrect (e.g.,
-1).
An ordinal number must be in the range 0 to 65535.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 2041 ordinal in EXPORT directive not valid
.np
The specified ordinal in the "EXPORT" directive is incorrect (e.g.,
-1).
An ordinal number must be in the range 0 to 65535.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 2042 too many IOPL words in EXPORT directive
.np
The maximum number of IOPL words for a 16-bit executable is 63.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 1043 duplicate exported ordinal
.np
This message is issued for ordinal numbers specified in an "EXPORT"
directive for symbols belonging to DLLs.
This message is issued if an ordinal number is assigned to two
different symbols.
A warning is issued and the linker assigns a non-used ordinal number
to the symbol that caused the warning.
.*
.errnote 1044,2044 exported symbol %s not found
.np
This message is issued when generating a DLL or NetWare NLM.
An attempt has been made to define an entry point into a DLL or NLM
that does not exist.
.*
.errnote 1045 segment attribute defined more than once
.np
A segment appearing in a "SEGMENT" directive has been given conflicting
or duplicate attributes.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 1046 segment name %s not found
.np
The segment name specified in a "SEGMENT" directive has not been
defined.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 1047 class name %s not found
.np
The class name specified in a "SEGMENT" directive has not been defined.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 1048 inconsistent attributes for automatic data segment
.np
This message is issued for Windows or OS/2 16-bit executable files.
Two conflicting attributes were specified for the automatic data
segment.
For example, "LOADONCALL" and "PRELOAD" are conflicting attributes.
Only the first attribute is used.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 2049 invalid STUB file
.np
The stub file is not a valid executable file.
The stub file is only used for OS/2 executable files
and Windows (both Win16 and Win32) executable files.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 1050 invalid DLL specified in OLDLIBRARY option
.np
The DLL specified in an "OLDLIBRARY" option is not a valid dynamic
link library.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 2051 STUB file name same as executable file name
.np
When generating an OS/2 or Windows (Win16, Win32) executable file, the
stub file name must not be same as the executable file name.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 2052 relocation at %a not in the same segment
.np
This message is only issued for Windows (Win16), OS/2, Phar Lap, and
QNX executables.
A relative fixup must relocate to the same segment.
"%a" defines the location of the fixup.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 2053 %a: cannot reach a DLL with a relative relocation
.np
A reference to a symbol in an OS/2 or Windows 16-bit DLL must not be
relative.
"%a" defines the location of the reference.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 1054 debugging information incompatible: using line numbers only
.np
An attempt has been made to link an object file with out-of-date
debugging information.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 2055 %a: frame must be the same as the target in protected mode
.np
Each relocation consists of three components; the location being
relocated, the target (or address being referenced), and the frame
(the segment to which the target is adjusted).
In protected mode, the segment of the target must be the same as the
frame.
"%a" defines the location of the fixup.
This message does not apply to 32-bit OS/2 and Windows (Win32).
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 2056 cannot find library member %s(%s)
.np
Library member "%s2" in library file "%s1" could not be found.
This message is issued if the library file could not be found or the
library file did not contain the specified member.
.*
.errnote 3057 executable format has been established
.np
This message is issued if there is more than one "FORMAT" directive.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 1058 %s option not valid for %s executable
.np
The option "%s1" can only be specified if an executable file whose
format is "%s2" is being generated.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 1059,2059 value for %s too large
.np
The value specified for option "%s" exceeds its limit.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 1060 value for %s incorrect
.np
The value specified for option "%s" is not in the allowable range.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 1061 multiple values specified for REALBREAK
.np
The "REALBREAK" option for Phar Lap executables can only be specified
once.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 1062 export and import records not valid for %f
.np
This message is issued if a reference to a DLL is encountered and the
executable file format is not one that supports DLLs.
The file format is represented by "%f".
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 2063 invalid relocation for flat memory model at %a
.np
A segment relocation in the flat memory model was encountered.
"%a" defines the location of the fixup.
.*
.errnote 2064 cannot combine 32-bit segments (%s1) with 16-bit segments (%s2)
.np
A 32-bit segment "%s1" and a 16-bit segment "%s2" have been encountered.
Mixing object files created by a 286 compiler and object files created
by a 386 compiler is the most probable cause of this error.
.*
.errnote 2065 REALBREAK symbol %s not found
.np
The symbol specified in the "REALBREAK" option for Phar Lap
executables has not been defined.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 2066 invalid relative relocation type for an import at %a
.np
This message is issued only if a NetWare executable file is being
generated.
An imported symbol is one that was specified in an "IMPORT" directive
or an import library.
Any reference to an imported symbol must not refer to the segment of
the imported symbol.
"%a" defines the location of the reference.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 2067 %a: cannot relocate between code and data in Novell formats
.np
This message is issued only if a NetWare executable file is being
generated.
Segment relocation is not permitted.
"%a" defines the location of the fixup.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 2068 absolute segment fixup not valid in protected mode
.np
A reference to an absolute location is not allowed in protected mode.
A protected-mode application is one that is being generated for ZDOS,
OS/2,
CauseWay DOS extender,
Tenberry Software's DOS/4G or DOS/4GW DOS extender,
FlashTek's DOS extender,
Phar Lap's 386|DOS-Extender,
Novell's NetWare operating systems,
Windows NT,
or Windows 95.
An absolute location is most commonly defined by the "EQU" assembler
directive.
.*
.errnote 1069 unload CHECK procedure not found
.np
This message is issued only if a NetWare executable file is being
generated.
The symbol specified in the "CHECK" option has not been defined.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 2070 START procedure not found
.np
This message is issued only if a NetWare executable file is being
generated.
The symbol specified in the "START" option has not been defined.
The default "START" symbol is "_Prelude".
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 2071 EXIT procedure not found
.np
This message is issued only if a NetWare executable file is being
generated.
The symbol specified in the "EXIT" option has not been defined.
The default "STOP" symbol is "_Stop".
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.if '&overlay' eq 'yes' .do begin
.errnote 1072 SECTION directive not allowed in root
.np
When describing 16-bit overlays, "SECTION" directives must appear
between a "BEGIN" directive and its corresponding "END" directive.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.do end
.*
.errnote 2073 bad Novell file format specified
.np
An invalid NetWare executable file format was specified.
Valid formats are NLM, DSK, NAM, LAN, MSL, HAM, CDM or a numerical
module type.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 2074 circular alias found for %s
.np
An attempt was made to circularly define the symbol name specified in
an ALIAS directive.
For example:
.millust begin
ALIAS foo1=foo2, foo2=foo1
.millust end
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.if '&overlay' eq 'yes' .do begin
.errnote 2075 expecting an END directive
.np
A "BEGIN" directive is missing its corresponding "END" directive.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.do end
.*
.errnote 1076 %s option multiply specified
.np
The option "%s" can only be specified once.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 1080 file %s is a %d-bit object file
.np
A 32-bit attribute was encountered while generating a 16-bit
executable file format, or
a 16-bit attribute was encountered while generating a 32-bit
executable file format.
.*
.errnote 2082 invalid record type 0x%x
.np
An object record type not recognized by the linker has been
encountered.
This message is issued when linking object modules created by other
compilers or assemblers that create object files with records that the
linker does not recognize.
.*
.errnote 2083 cannot reference address %a from frame %x
.np
When generating a 16-bit executable, the offset of a referenced symbol
was greater than 64K from the location referencing it.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 2084 target offset exceeds 64K at %a
.np
When generating a 16-bit executable, the computed offset for a symbol
exceeds 64K.
"%a" defines the location of the fixup.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 2086 invalid starting address for .COM file
.np
The value of the segment of the starting address for a 16-bit DOS
"COM" file, as specified in the map file, must be 0.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 1087 stack segment ignored in .COM file
.np
A stack segment must not be defined when generating a 16-bit DOS "COM"
file.
Only a single physical segment is allowed in a DOS "COM" file.
The stack is allocated from the high end of the physical segment.
That is, the initial value of SP is hexadecimal FFFE.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 3088 virtual memory exhausted
.np
This message is similar to the "dynamic memory exhausted" message.
The DOS-hosted version of the linker has run out of memory trying to
keep track of virtual memory blocks.
Virtual memory blocks are allocated from expanded memory, extended
memory and the spill file.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 2089 program too large for a .COM file
.np
The total size of a 16-bit DOS "COM" program must not exceed 64K.
That is, the total amount of code and data must be less than 64K since
only a single physical segment is allowed in a DOS "COM" file.
You must decrease the size of your program or generate a DOS "EXE"
file.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 1090 redefinition of %s by %s ignored
.np
The symbol "%s1" has been redefined by module "%s2".
This message is issued when the size specified in the "NAMELEN" option
has caused two symbols to map to the same symbol.
For example, if the symbols
.sy routine1
and
.sy routine2
are encountered and "OPTION NAMELEN=7" is specified, then this message
will be issued since the first seven characters of the two symbols are
identical.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.if '&overlay' eq 'yes' .do begin
.errnote 2091 group %s is in more than one overlay
.np
A group that spans more than one section in a 16-bit DOS executable
has been detected.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.do end
.*
.errnote 2092 NEWSEGMENT directive appears before object files
.np
The 16-bit "NEWSEGMENT" directive must appear after a "FILE"
directive.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 2093 cannot open %s
.np
This message is issued when the linker is unable to open a file
and is unable to determine the cause.
.*
.errnote 2094 i/o error processing %s
.np
This message is issued when the linker has encountered an i/o error
while processing the file and is unable to determine the cause.
This message may be issued when reading from object and library files,
or writing to the executable and spill file.
.*
.if '&overlay' eq 'yes' .do begin
.errnote 3097 too many library modules
.np
This message is similar to the "dynamic memory exhausted" message.
This message if issued when the "DISTRIBUTE" option for 16-bit DOS
executables is specified.
The linker has run out of memory trying to keep track of the
relationship between object modules extracted from libraries and the
overlays they should be placed in.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.do end
.*
.errnote 1098 Offset option must be a multiple of %dK
.np
The value specified with the "OFFSET" option must be
a multiple of 4K (4096) for Phar Lap and QNX executables and
a multiple of 64K (65536) for OS/2 and Windows 32-bit executables.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 2099 symbol name too long: %s
.np
The maximum size (approximately 2048) of a symbol has been exceeded.
Reduce the size of the symbol to avoid this error.
.*
.errnote 1101 invalid incremental information file
.np
The incremental information file is corrupt or from an older version
of the compiler.
The old information file and the executable will be deleted and new
ones will be generated.
.*
.errnote 1102 object file %s not found for tracing
.np
A "SYMTRACE" or "MODTRACE" directive contained an object file (namely
%s) that could not be found.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 1103 library module %s(%s) not found for tracing
.np
A "SYMTRACE" or "MODTRACE" directive contained an object module
(namely module %s1 in library %s2 ) that could not be found.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.if '&overlay' eq 'yes' .do begin
.errnote 1105 cannot reserve %l bytes of extra overlay space
.np
The value specified with the "AREA" option for 16-bit DOS executables
results in an executable file that requires more than 1 megabyte of
memory to execute.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.do end
.*
.errnote 1107 undefined system name: %s
.np
The name %s was referenced in a "SYSTEM" directive but never defined by
a system block definition.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 1108 system %s defined more than once
.np
The name %s has appeared in a system definition block more than once.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 1109 OFFSET option is less than the stack size
.np
For the QNX operating system, the stack is placed at the front of the
executable image and thus the initial load address must leave enough
room for the stack.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 1110 library members not allowed in libfile
.np
Only object files are allowed in a "LIBFILE" directive.
This message will be issued if a module from a library file is
specified in a "LIBFILE" directive.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 1111 error in default system block
.np
The default system block definition (system name "286" for 16-bit
applications) and (system name "386" for 32-bit applications) contains
a directive error.
The system name "286" or "386" is automatically referenced by the
linker when the format of the executable cannot be determined (i.e. no
"FORMAT" directive has been specified).
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 3114 environment name specified incorrectly
.np
This message is specified if the environment variable is not properly
enclosed between two percent (%) characters.
.*
.errnote 1115 environment name %s not found
.np
The environment variable %s has not been defined in the environment
space.
.*
.if '&overlay' eq 'yes' .do begin
.errnote 1116 overlay area must be at least %l bytes
.np
This message is issued if the size of the largest overlay exceeds the
size of the overlay area specified by the "AREA" option for 16-bit DOS
executables.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.do end
.*
.errnote 1117 segment number too high for a movable entry point
.np
The segment number of a moveable segment must not exceed 255 for
16-bit executables.
Reduce the number of segments or use the "PACKCODE" option.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 1118 heap size too large
.np
This message is issued if the size of the heap, stack and the default
data segment (group DGROUP) exceeds 64K for 16-bit executables.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 2119 wlib import statement incorrect
.np
The "EXPORT" directive allows you to specify a library command file.
This command file is scanned for any librarian commands that create
import library entries.
An invalid command was detected.
.if &e'&optdoc eq 0 .do begin
See the section entitled "The EXPORT Directive" for the correct format
of these commands.
.do end
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 2120 application too large to run under DOS
.np
This message is issued if the size of the 16-bit DOS application
exceeds 1M.
.*
.errnote 1121 '%s' has already been exported
.np
The linker has detected an attempt to export a symbol more than once.
For example, a name appearing in more than one "EXPORT" directive will
cause this message to be issued.
Also, if you have declared a symbol as an export in your source and
have also specified the same symbol in an "EXPORT" directive, this
message will be issued.
This message is only a warning.
.*
.errnote 3122 no FILE directives found
.np
This message is issued if no "FILE" directive has been specified.
In other words, you have specified no object files to link.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 3123 overlays are not supported in this version of the linker
.np
This version of the linker does not support the creation of overlaid
16-bit executables.
.*
.errnote 1124 lazy reference for %S has different default resolutions
.np
A lazy external reference is one which has two resolutions: a preferred
one and a default one which is used if the preferred one is not found.
In this case, the linker has found two lazy references that have the
same preferred resolution but different default resolutions.
.*
.errnote 1125 multiple aliases found for %S
.np
The linker has found a name which has been aliased to two different
symbols.
.*
.errnote 1126 %s has been modified: doing full relink
.np
The linker has determined that the time stamps on the executable file
and symbolic information file (.sym) are different.
An incremental link will not be done.
:CMT. .errnote 1126 INT 15 interrupt may be incorrect
:CMT. .np
:CMT. An error was reported while the linker was trying to access extended
:CMT. memory.
:CMT. The interrupt 15 vector (used to access extended memory) has been
:CMT. corrupted during the linking process.
:CMT. .if &e'&optdoc eq 1 .do begin
:CMT. .np
:CMT. This message should not be issued for a &powerpp Win32 executable.
:CMT. If it is issued, please report this problem.
:CMT. .do end
:CMT. .*
:CMT. .errnote 3126 too many EMS requests queued
:CMT. .np
:CMT. An error was reported while the linker was trying to access expanded
:CMT. memory.
:CMT. The error message is issued if too many applications are
:CMT. simultaneously making extended memory requests.
:CMT. .*
:CMT. .note or
:CMT. .bd EMS physical mapping corrupted
:CMT. .np
:CMT. A serious problem has occurred while trying to access expanded memory.
:CMT. The EMS frame has been corrupted.
:CMT. This can be caused by a TSR that is using the EMS frame for other reasons.
:CMT. .if &e'&optdoc eq 1 .do begin
:CMT. .np
:CMT. These messages should not be issued for a &powerpp Win32 executable.
:CMT. If they are issued, please report this problem.
:CMT. .do end
.*
.errnote 2127 cannot export symbol %S
.np
An attempt was made to export a symbol defined with an absolute
address or to export an imported symbol.
It is not possible to export these symbols with the "EXPORT"
directive.
.*
.errnote 3128 directive error near beginning of input
.np
The linker detected an error at the start of the command line.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 3129 address information too large
.np
The linker has encountered a segment that appears in more than 11000
object files.
An empty segment does not affect this limit.
This can only occur with Watcom debugging information.
If this message appears, switch to DWARF debugging information.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 1130 %s is an invalid shared nlm file
.np
The NLM specified in a "SHAREDNLM" option is not valid.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 3131 cannot open spill file: file already exists
.np
All 26 of the DOS-hosted linker's possible spill file names are in
use.
Spill files can accumulate when linking on a multi-tasking system and
the directory in which the spill file is created is identical for each
invocation of the linker.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 2132 curly brace delimited list incorrect
.np
A list delimited by curly braces is not correct.
The most likely cause is a missing right brace.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 1133 no realbreak specified for 16-bit code
.np
While generating a Phar Lap executable file, both 16-bit and 32-bit
code was linked together and no "REALBREAK" option has been specified.
A warning message is issued since this may be a potential problem.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 1134 %s is an invalid message file
.np
The file specified in a "MESSAGE" option for NetWare executable
files is invalid.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.if '&overlay' eq 'yes' .do begin
.errnote 3135 need exactly 1 overlay area with dynamic overlay manager
.np
Only a single overlay area is supported by the 16-bit dynamic overlay
manager.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.do end
.*
.errnote 1136 segment relocation to a read/write data segment found at %a(%S)
.np
The "RWRELOCCHECK" option for 16-bit Windows (Win16) executables has
been specified and the linker has detected a segment relocation to a
read/write data segment. Where the name of the offending symbol is not
available, "identifier unavailable" is used.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 3137 too many errors encountered
.np
This message is issued when the number of error messages issued by the
linker exceeds the number specified by the "MAXERRORS" option.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 3138 invalid filename '%s'
.np
The linker performs a simple filename validation whenever a filename
is specified to the linker.
For example, a directory specification is not a valid filename.
.*
.errnote 3139 cannot have both 16-bit and 32-bit object files
.np
It is impossible to mix 16-bit code and 32-bit code in the same
executable when generating a QNX executable file.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 1140 invalid message number
.np
An invalid message number has been specified in a "DISABLE" directive.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 1141 virtual function table record for %s mismatched
.np
The linker performs a consistency check to ensure that the C++
compiler has not generated incorrect virtual function information.
If the message is issued, please report this problem.
.*
.errnote 1143 not enough memory to sort map file symbols
.np
There was not enough memory for the linker to sort the symbols in the
"Memory Map" portion of the map file.
This will only occur when the "SORT GLOBAL" option has been specified.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 1145 %S is both pure virtual and non-pure virtual
.np
A function has been declared both as "pure" and "non-pure" virtual.
.*
.errnote 2146 %s is an invalid object file
.np
Something was encountered in the object file that cannot be processed
by the linker.
.*
.errnote 3147 Ambiguous format specified
.np
Not enough of the FORMAT directive attributes were specified to enable
the linker to determine the executable file format.
For example,
.millust begin
FORMAT OS2
.millust end
.pc
will generate this message.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 1148 Invalid segment type specified
.np
The segment type must be one of CODE or DATA.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 1149 Only one debugging format can be specified
.np
The debugging format must be one of Watcom, CodeView, DWARF (default),
or Novell.
You cannot specify multiple debugging formats.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 1150 file %s has code for a different processor
.np
An object file has been encountered which contains code compiled for a
different processor (e.g., an Intel application and an Alpha object
file).
.*
.errnote 2151 big endian code not supported
.np
Big endian code is not supported by the linker.
.*
.errnote 2152 no dictionary found
.np
No symbol search dictionary was found in a library that the linker
attempted to process.
.*
.errnote 2154 cannot execute %s1 : %s2
.np
An attempt by the linker to spawn another application failed.
The application is specified by "%s1" and the reason for the failure is
specified by "%s2".
.*
.errnote 2155 relocation at %a to an improperly aligned target
.np
Some relocations in Alpha executables require that the object be
aligned on a 4 byte boundary.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 2156 OPTION INCREMENTAL must be one of the first directives specified
.np
The option must be specified before any option or directive which
modifies the linker's symbol table (e.g., IMPORT, EXPORT, REFERENCE,
ALIAS).
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 3157 no code or data present
.np
The linker requires that there be at least 1 byte of either code or
data in the executable.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 1158 problem adding resource information
.np
The resource file is invalid or corrupt.
.*
.errnote 3159 incremental linking only supports DWARF debugging information
.np
When OPTION INCREMENTAL is used, you cannot specify non-DWARF
debugging information for the executable.  You must specify DEBUG
DWARF when requesting debugging information.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 3160 incremental linking does not support dead code elimination
.np
When OPTION INCREMENTAL is used, you cannot specify OPTION ELIMINATE.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 1162 relocations on iterated data not supported
.np
An object file was encountered that contained an iterated data record
that requires relocation.
This is most commonly caused by a module coded in assembly language.
.*
.errnote 1163 module has not been compiled with the "zv" option
.np
When OPTION VFREMOVAL is used, all object files must be compiled with
the "zv" option.  The linker has detected an object file that has not
been compiled with this option.
.*
.errnote 3164 incremental linking does not support virtual function removal
.np
When OPTION INCREMENTAL is used, you cannot also specify OPTION
VFREMOVAL.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 1165 resource file %s too big
.np
The resource file specified in OPTION RESOURCE was too big to fit
inside the QNX executable. The maximum size is approximately 32000
bytes.
.if &e'&optdoc eq 1 .do begin
.np
This message should not be issued for a &powerpp Win32 executable.
If it is issued, please report this problem.
.do end
.*
.errnote 2166 both %s1 and %s2 marked as starting symbols
.np
If the linker sees that there is more than one starting address
specified in the program and they have symbol names associated with
them, it will emit this error message. If there is more than one
starting address specified and at least one of them is unnamed, it
will issue message 2030.
.*
.errnote 1167 NLM internal name (%s) truncated
.np
This message is issued when generating a NetWare NLM.
The output file name as specified by the NAME directive has
specified a long file name (exceeds 8.3). The linker will truncate
the generated file name by using the first eight characters of the
specified file name and the first three characters of the file
extension (if supplied), separated by a period.
.*
.errnote 3168 exactly one export must exist for VxD format
.np
The Windows VxD format requires exactly one export to be present, but an
attempt was made to build a VxD module with no exports or more than one export.
.*
.errnote 2169 location counter already beyond fixed segment address %a
.np
When creating an image using the OUTPUT directive, a segment was specified
with an address lower than the current location counter. This would overlay
the segment data with already existing data at the same address, and is not
allowed.
.*
.errnote 1170 directive %s can only occur once
.np
A directive was specified more than once on the &lnkname. command line and
was ignored. Remove the redundant instances of the directive.
.*
.errnote 1171 locally defined symbol %s imported
.np
An imported symbol (intended to be imported from a DLL) was resolved locally.
The linker will ignore the symbol defined in a DLL, if provided, and the local
reference will be used. Ensure that this is the intended behaviour.
.*
.errnote 1172 stack size is less than %d bytes.
.np
The stack size for an executable specified through OPTION STACK is very small.
There is a high probability that the program will not work correctly. Consider
specifying a greater stack size.
.*
.errnote 3173 default data segment exceeds maximum size by %l bytes
.np
The default data segment size in a NE format executable (16-bit OS/2 or
Windows) exceeds the maximum allowed size. The default data segment includes
the data segment plus default stack size plus default heap size. The total
size must be 64K or less for OS/2 executables and 65,533 bytes or less for
Windows executables.
.*
.if &e'&dohelp eq 0 .do begin
.endnote
.do end
