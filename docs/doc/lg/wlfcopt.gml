.*
.*
.chap *refid=wlfcopt Optimization of Far Calls
.*
.np
.ix 'far call optimizations'
Optimization of far calls can result in smaller executable files and
improved performance.
It is most useful when the automatic grouping of logical segments into
physical segments takes place.
Note that, by default, automatic grouping is performed by the &lnkname..
.np
The &company C, C++ and FORTRAN 77 compilers automatically enable the
far call optimization.
The &lnkname will optimize far calls to procedures that reside in the
same physical segment as the caller.
For example, a large code model program will probably contain many far
calls to procedures in the same physical segment.
Since the segment address of the caller is the same as the segment
address of the called procedure, only a near call is necessary.
A near call does not require a relocation entry in the relocation
table of the executable file whereas a far call does.
Thus, the far call optimization will result in smaller executable
files that will load faster.
Furthermore, a near call will generally execute faster than a far
call, particularly on 286 and 386-based machines where, for
applications running in protected mode, segment switching is fairly
expensive.
.np
The following describes the far call optimization.
The
.bd call far label
instruction is converted to one of the following sequences
of code.
.millust begin
push   cs                seg    ss
call   near label        push   cs
nop                      call   near label
.millust end
.autonote Notes:
.note
The
.bd nop
or
.bd seg ss
instruction is present since a
.bd call far label
instruction is five bytes.
The
.bd push cs
instruction is one byte and the
.bd call near label
instruction is three bytes.
The
.bd seg ss
instruction is used because it is faster than the
.bd nop
instruction.
.note
The called procedure will still use a
.bd retf
instruction but since the code segment and the near address are pushed
on the stack, the far return will execute correctly.
.note
The position of the padding instruction is chosen so that the return
address is word aligned.
A word aligned return address improves performance.
.note
When two consecutive
.bd call far label
instructions are optimized and the first
.bd call far label
instruction is word aligned, the following sequence replaces both
.bd call far label
instructions.
.millust begin
push    cs
call    near label1
seg     ss
push    cs
seg     cs
call    near label2
.millust end
.note
If your program contains only near calls, this optimization will have
no effect.
.endnote
.np
.ix 'far jump optimization'
A far jump optimization is also performed by the &lnkname..
This has the same benefits as the far call optimization.
A
.bd jmp far label
instruction to a location in the same segment will be replaced by the
following sequence of code.
.millust begin
jmp    near label
mov    ax,ax
.millust end
.pc
Note that for 32-bit segments, this instruction becomes
.mono mov eax,eax.
.*
.if '&target' ne 'QNX' .do begin
.*
.section Far Call Optimizations for Non-&company Object Modules
.*
.np
.ix 'far call optimization' 'enabling'
The far call optimization is automatically enabled when object modules
created by the &company C, C++, or FORTRAN 77 compilers are linked.
These compilers mark those segments in which this optimization can be
performed.
The following utility can be used to enable this optimization for
object modules that have been created by other compilers or
assemblers.
.*
.beglevel
.*
.section The &fcename
.*
.np
.ix '&fcename'
Only DOS, OS/2 and Windows-hosted versions of the &fcename are
available.
A QNX-hosted version is not necessary since QNX-hosted development
tools that generate object files, generate the necessary information
that enables the far call optimization.
.np
The format of the &fcename is as follows.
Items enclosed in square brackets are optional; items enclosed in
braces may be repeated zero or more times.
.mbigbox
&fcecmdup { [option] [file] }
.embigbox
.synote
.mnote option
is an option and must be preceded by a dash ('-') or slash ('/').
.mnote file
is a file specification for an object file or library file.
If no file extension is specified, a file extension of "obj" is
assumed.
Wild card specifiers may be used.
.esynote
.np
The following describes the command line options.
.ix '&fcecmdup options' 'b'
.begnote $break
.note b
Do not create a backup file.
By default, a backup file will be created.
The backup file name will have the same file name as the input file
and a file extension of "bob" for object files and "bak" for library
files.
.ix '&fcecmdup options' 'c'
.note c
Specify a list of class names, each separated by a comma.
This enables the far call optimization for all segments belonging to
the specified classes.
.ix '&fcecmdup options' 's'
.note s
Specify a list of segment names, each separated by a comma.
This enables the far call optimization for all specified segments.
.ix '&fcecmdup options' 'x'
.note x
Specify a list of ranges, each separated by a comma, for which no far
call optimizations are to be made.
A range has the following format.
.millust begin
seg_name start-end
        or
seg_name start:length
.millust end
.pc
.sy seg_name
is the name of a segment.
.sy start
is an offset into the specified segment defining the start of the
range.
.sy end
is an offset into the specified segment defining the end of the range.
.sy length
is the number of bytes from
.sy start
to be included in the range.
All values are assumed to be hexadecimal.
.endnote
.autonote Notes:
.note
If more than one class list or segment list is specified, only the
last one is used.
A class or segment list applies to all object and library files
regardless of their position relative to the class or segment list.
.note
A range list applies only to the first object file following the range
specification.
If the object file contains more than one module, the range list will
only apply to the first module in the object file.
.endnote
.np
The following examples illustrate the use of the &fcename..
.exam begin
&fcecmd &sw.c code *.obj
.exam end
.pc
In the above example, the far call optimization will be enabled for
all segments belonging to the "code" class.
.exam begin
&fcecmd &sw.s _text *.obj
.exam end
.pc
In the above example, the far call optimization will be enabled for
all segments with name "_text".
.exam begin
&fcecmd &sw.x special 0:400 asmfile.obj
.exam end
.pc
In the above example, the far call optimization will be disabled for
the first 1k bytes of the segment named "special" in the object file
"asmfile".
.exam begin
&fcecmd &sw.x special 0-ffffffff asmfile.obj
.exam end
.pc
In the above example, the far call optimization will be disabled for
the entire segment named "special" in the object file "asmfile".
.*
.endlevel
.do end
