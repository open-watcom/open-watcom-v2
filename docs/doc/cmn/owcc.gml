:set symbol="chapname" value="The &product POSIX-like Compiler Driver".
.*
.chap The &product POSIX-like Compiler Driver
.*
.np
The &product POSIX-like Compiler Driver is designed for generating
applications, simply and quickly, using a single command line.
On the command line, you can list source file names as well as object
file names.
Source files are either compiled or assembled based on file extension;
object files and libraries are simply included in the link phase.
Options can be passed on to both the compiler and linker.
.*
.*
.section owcc Command Line Format
.*
.np
.ix 'owcc' 'command line format'
.ix 'command line format' 'owcc'
The format of the command line is:
.mbigbox
owcc [options] [files]
.embigbox
.np
The square brackets [ ] denote items which are optional.
.np
The files and options may be specified in any order.
The owcc utility uses the extension of the file name to determine
if it is a source file, an object file, or a library file.
Files with extensions of "o" and "lib" are assumed to be object
files and library files respectively.
Files with extensions of "asm" are assumed to be assembler source
files and will be assembled by the &asmname..
Files with any other extension, including none at all, are assumed to
be &lang source files and will be compiled.
Pattern matching characters ("*" and "?") may be used in the file
specifications.
.np
If no file extension is specified for a file name then the owcc
utility will check for a file with one of the following extensions.
.millust begin
Order   Name.Ext        Assumed to be
-----   --------        ---------------
  1.    file.asm        Assembler source code
  2.    file.cxx        C++ source code
  3.    file.cpp        C++ source code
  4.    file.cc         C++ source code
  5.    file.c          C   source code
.millust end
.pc
It checks for each file in the order listed.
By default, the &asmname will be selected to compile files
with the extension "asm".
By default, the &cmppname compiler will be selected to compile files
with any of the extensions "cxx", "cpp" or "cc".
By default, the &cmpcname compiler will be selected to compile a file
with a "c" extension.
The default selection of compiler can be overridden by the "-x" option, described below.
.np
Options are prefixed with a dash (&minus.) and may be
specified in any order.
Option names were chosen to resemble those of the GNU Compiler
Collection (a.k.a. GCC).  They are translated into &cmpname options,
or to directives for the &product wlink utility, accordingly.
A summary of options is displayed on the screen by running the compiler
driver like this: "owcc -?".  If run without any arguments the compiler driver just
displays its name and hints towards the "-?" option.

.*
.section owcc Options Summary
.*
.begnote $compact $setptnt 10
:DTHD.General options:
:DDHD.Description:
.note c
.ix 'owcc options' 'c'
compile the files only, do not link them
.note S
.ix 'owcc options' 'S'
compile the source file(s), then run the &product disassembler on the
generated object file(s) instead of linking them.
Please note that this leaves you with both an object file and an assembly
source file.
Unix compilers traditionally compile by generating asm source and
pass that to the assembler, so there, the "-S" option is done by stopping
short of assembling the file.  &product compiles directly to object files,
so we need the disassembler to achieve a similar effect.
.note x {c,c++}
.ix 'owcc options' 'x'
treat all source files as written in the specified programming language, regardless
of filename suffix.
.* This option doesn't actually exist:
.*.note y
.*.ix 'owcc options' 'y'
.*ignore the OWCC environment variable
.note o <filename>
.ix 'owcc options' 'o'
Change the name of the generated file.
If only the preprocessor is run, this sends the preprocessed output to a file instead 
of the standard output stream.  
If only compilation is done, this allows to change the name of the object file.  
If compilation and disassembly is done, this changes the name of the assembly source file.
If owcc runs the linker, this changes the name of the generated executable or DLL.
.note v
operate verbosely, displaying the actual command lines used to invoke the compiler and
linker, and passing flags to them to operate verbosely, too.
.note zq
operate quietly (default).  This is the opposite of the "-v" option.
.endnote
.*
.dm optref begin
.dm optref end
.*
.begnote $compact $setptnt 10
:DTHD.Compiler options:
:DDHD.Description:
.ix 'owcc options' 'compile'
.im owccopts
.endnote
.np
See the
.book &product User's Guide
for a full description of compiler options.
.begnote $compact $setptnt 10
:DTHD.Linker options:
:DDHD.Description:
.note b <target name>
.ix 'owcc options' 'b <system name>'
Compile and link for the specified target system name. 
See the section linker user guide for available system names.
The linker will effectively receive a -l=<target name> option.
owcc looks up <system name> in a specification table "specs.owc" to
find out which of the &cmpcname utilities to run.  One those options will
be -bt=<os>, where <os> is the generic target platform name, and usually less
specific than the linker <system name>.
Among the supported systems are:
.begnote $compact
.note 286
16-bit DOS executables (synonym for "DOS") under DOS and
NT hosted platforms;
16-bit OS/2 executables (synonym for "OS2") under 32-bit OS/2
hosted OS/2 session.
.note 386
32-bit DOS executables (synonym for "DOS4G") under DOS;
32-bit NT character-mode executables (synonym for "NT") under
Windows NT;
32-bit OS/2 executables (synonym for "OS2V2") under 32-bit OS/2
hosted OS/2 session.
.note COM
16-bit DOS "COM" files
.note DOS
16-bit DOS executables
.note DOS4G
32-bit Tenberry Software DOS/4G DOS Extender executables
.note DOS4GNZ
32-bit Tenberry Software DOS/4G DOS Extender non-zero base executables
.note NETWARE
32-bit Novell NetWare 386 NLMs
.note NOVELL
32-bit Novell NetWare 386 NLMs (synonym for NETWARE)
.note NT
32-bit Windows NT character-mode executables
.note NT_DLL
32-bit Windows NT DLLs
.note NT_WIN
32-bit Windows NT windowed executables
.note OS2
16-bit OS/2 V1.x executables
.note OS2_DLL
16-bit OS/2 DLLs
.note OS2_PM
16-bit OS/2 PM executables
.note OS2V2
32-bit OS/2 executables
.note OS2V2_DLL
32-bit OS/2 DLLs
.note OS2V2_PM
32-bit OS/2 PM executables
.note PHARLAP
32-bit PharLap DOS Extender executables
.note QNX
16-bit QNX executables
.note QNX386
32-bit QNX executables
.note TNT
32-bit Phar Lap TNT DOS-style executable
.note WIN386
32-bit extended Windows 3.x executables/DLLs
.note WIN95
32-bit Windows 9x executables/DLLs
.note WINDOWS
16-bit Windows executables
.note WINDOWS_DLL
16-bit Windows Dynamic Link Libraries
.note X32R
32-bit FlashTek (register calling convention) executables
.note X32RV
32-bit FlashTek Virtual Memory (register calling convention) executables
.note X32S
32-bit FlashTek (stack calling convention) executables
.note X32SV
32-bit FlashTek Virtual Memory (stack calling convention) executables
.endnote
.np
These names are among the systems identified in the &lnkname
initialization file, "wlsystem.lnk".
The &lnkname "SYSTEM" directives, found in this file, are used to
specify default link options for particular (operating) systems.
Users can augment the &lnkname initialization file with their own
system definitions and these may be specified as an argument to the
"l=" option.
The "system_name" specified in the "l=" option is used to create a
"SYSTEM system_name" &lnkname directive when linking the application.
.note mstack-size=<size>
.ix 'owcc options' 'mstack-size=<size>'
set stack size
.note fd[=<directive_file>]
.ix 'owcc options' 'fd[=<directive_file>]'
keep linker directive file generated by this tool and, optionally, 
rename it (default name is "__owcc__.lnk").
.note fm[=<map_file>]
.ix 'owcc options' 'fm[=<map_file>]'
generate map file, optionally specify its name.
.note s
.ix 'owcc options' 's'
strip symbolic information not strictly required to run from executable.
.note Wl,"directives"
.ix 'owcc options' 'Wl'
send any supplementary directives directly to the linker
.note Wl,@<file>
include additional linker directives from <file>.
This is actually just a special case of -Wl used to pass the
linker's @ directive to pull in directives from <file>
.endnote
.*
.section owcc Command Line Examples
.*
.np
For most small applications, the
.kw owcc
command will suffice.
We have only scratched the surface in describing the capabilities
of the
.kw owcc
command.
The following examples describe the
.kw owcc
commands in more detail.
.np
Suppose that your application is contained in three files called
.fi apdemo.c
.ct ,
.fi aputils.c
.ct , and
.fi apdata.c.
We can compile and link all three files with one command.
.exam begin 2 ~b1
&prompt.owcc -g apdemo.c aputils.c apdata.c
.exam end
.pc
The executable program will be stored in
.fi a.out.
Each of the three files is compiled with the "g" debug option.
Debugging information is included in the executable file.
.np
We can issue a simpler command if the current directory contains only
our three &lang source files.
.exam begin 2 ~b2
&prompt.owcc -g *.c
.exam end
.pc
.kw owcc
will locate all files with the "&cxt" filename extension and compile
each of them.
The default name of the executable file will be
.fi a.out.
Since it is only possible to have one executable with the name
.fi a.out
in a directory,
.kw owcc
has an option, "o", which will allow you to specify the name to
be used.
.exam begin 2 ~b3
&prompt.owcc -g -o apdemo *.c
.exam end
.pc
By using the "o" option, the executable file will always be called
.fi apdemo.
.np
If the directory contains other &lang source files which are not
part of the application then other tricks may be used to identify a
subset of the files to be compiled and linked.
.exam begin 2 ~b4
&prompt.owcc -g -o apdemo ap*.c
.exam end
.pc
Here we compile only those &lang source files that begin with the
letters "ap".
.np
In our examples, we have recompiled all the source files each time.
In general, we will only compile one of them and include the object
code for the others.
.exam begin 2 ~b5
&prompt.owcc -g -o apdemo aputils.c ap*&obj
.exam end
.pc
The source file
.fi aputils.c
is recompiled and
.fi apdemo&obj
and
.fi apdata&obj
are included when linking the application.
The "&obj" filename extension indicates that this file need not be
compiled.
.exam begin 2 ~b6
&prompt.owcc -o demo *.c utility&obj
.exam end
.pc
All of the &lang source files in the current directory are compiled
and then linked with
.fi utility&obj
to generate
.fi demo.
The temporary linker directive file that is created by
.kw owcc
will be kept and renamed to
.fi grdemo.lnk.
.pc
For more complex applications, you should use a "Make" utility.

