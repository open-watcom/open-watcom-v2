:set symbol="chapname" value="The &product Compile and Link Utility".
.*
.chap The &product Compile and Link Utility
.*
.np
The &product Compile and Link Utility is designed for generating
applications, simply and quickly, using a single command line.
On the command line, you can list source file names as well as object
file names.
Source files are either compiled or assembled based on file extension;
object files and libraries are simply included in the link phase.
Options can be passed on to both the compiler and linker.
.*
.*
.section &wclcmdup16/&wclcmdup32 Command Line Format
.*
.np
.ix '&wclcmdup16' 'command line format'
.ix '&wclcmdup32' 'command line format'
.ix 'command line format' '&wclcmdup16'
.ix 'command line format' '&wclcmdup32'
The format of the command line is:
.mbigbox
&wclcmdup16 [files] [options]
&wclcmdup32 [files] [options]
.embigbox
.np
The square brackets [ ] denote items which are optional.
.begnote
.note &wclcmdup16
is the name of the &wclname utility that invokes the 16-bit compiler.
.note &wclcmdup32
is the name of the &wclname utility that invokes the 32-bit compiler.
.endnote
.np
The files and options may be specified in any order.
The &wclname utility uses the extension of the file name to determine
if it is a source file, an object file, or a library file.
Files with extensions of "OBJ" and "LIB" are assumed to be object
files and library files respectively.
Files with extensions of "ASM" are assumed to be assembler source
files and will be assembled by the &asmname..
Files with any other extension, including none at all, are assumed to
be &lang source files and will be compiled.
Pattern matching characters ("*" and "?") may be used in the file
specifications.
.np
If no file extension is specified for a file name then the &wclname
utility will check for a file with one of the following extensions.
.millust begin
Order   Name.Ext        Assumed to be
-----   --------        ---------------
  1.    file.ASM        Assembler source code
  2.    file.CXX        C++ source code
  3.    file.CPP        C++ source code
  4.    file.CC         C++ source code
  5.    file.C          C   source code
.millust end
.pc
It checks for each file in the order listed.
By default, the &asmname will be selected to compile files
with the extension "ASM".
By default, the &cmppname compiler will be selected to compile files
with any of the extensions "CXX", "CPP" or "CC".
By default, the &cmpcname compiler will be selected to compile a file
with a "C" extension.
The default selection of compiler can be overridden by the "cc" and
"cc++" options, described below.
.np
Options are prefixed with a slash (/) or a dash (&minus.) and may be
specified in any order.
Options can include any of the &cmpname compiler options plus some
additional options specific to the &wclname. utility.
A summary of options is displayed on the screen by simply entering the
"&wclcmdup16" or "&wclcmdup32" command with no arguments.
.*
.section &wclname Options Summary
.*
.begnote $compact $setptnt 10
:DTHD.General options:
:DDHD.Description:
.note c
.ix '&wclcmdup16 options' 'c'
.ix '&wclcmdup32 options' 'c'
compile the files only, do not link them
.note cc
.ix '&wclcmdup16 options' 'cc'
.ix '&wclcmdup32 options' 'cc'
treat source files as C code
.note cc++
.ix '&wclcmdup16 options' 'cc++'
.ix '&wclcmdup32 options' 'cc++'
treat source files as C++ code
.note y
.ix '&wclcmdup16 options' 'y'
.ix '&wclcmdup32 options' 'y'
ignore the &wclcmdup16/&wclcmdup32 environment variable
.endnote
.*
.dm optref begin
.dm optref end
.*
.begnote $compact $setptnt 10
:DTHD.Compiler options:
:DDHD.Description:
.ix '&wclcmdup16 options' 'compile'
.ix '&wclcmdup32 options' 'compile'
.im CPOPTSUM
.endnote
.np
See the
.book &product User's Guide
for a full description of compiler options.
.begnote $compact $setptnt 10
:DTHD.Linker options:
:DDHD.Description:
.note bcl=<system name>
.ix '&wclcmdup16 options' 'bcl=<system name>'
.ix '&wclcmdup32 options' 'bcl=<system name>'
Compile and link for the specified system name. See the section for link option 'l=' below and the
linker user guide for available system names.
This is equivalent to specifying -bt=<system name> and -l=<system name>.
.note k<stack_size>
.ix '&wclcmdup16 options' 'k<stack_size>'
.ix '&wclcmdup32 options' 'k<stack_size>'
set stack size
.note fd[=<directive_file>]
.ix '&wclcmdup16 options' 'fd[=<directive_file>]'
.ix '&wclcmdup32 options' 'fd[=<directive_file>]'
keep directive file and, optionally, rename it (default name is
"&wcltemp..LNK").
.note fe=<executable>
.ix '&wclcmdup16 options' 'fe=<executable>'
.ix '&wclcmdup32 options' 'fe=<executable>'
name executable file
.note fm[=<map_file>]
.ix '&wclcmdup16 options' 'fm[=<map_file>]'
.ix '&wclcmdup32 options' 'fm[=<map_file>]'
generate map file and name it (optional)
.note lp
.ix '&wclcmdup16 options' 'lp'
(16-bit only)
create an OS/2 protected-mode program
.note lr
.ix '&wclcmdup16 options' 'lr'
(16-bit only)
create a DOS real-mode program
.note l=<system_name>
.ix '&wclcmdup16 options' 'l=<system_name>'
.ix '&wclcmdup32 options' 'l=<system_name>'
link a program for the specified system.
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
32-bit Tenberry Software DOS Extender executables
.note DOS4GNZ
32-bit Tenberry Software DOS Extender non-zero base executables
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
initialization file, "WLSYSTEM.LNK".
The &lnkname "SYSTEM" directives, found in this file, are used to
specify default link options for particular (operating) systems.
Users can augment the &lnkname initialization file with their own
system definitions and these may be specified as an argument to the
"l=" option.
The "system_name" specified in the "l=" option is used to create a
"SYSTEM system_name" &lnkname directive when linking the application.
.note x
.ix '&wclcmdup16 options' 'x'
.ix '&wclcmdup32 options' 'x'
make names case sensitive
.note @<directive_file>
.ix '&wclcmdup16 options' '@'
.ix '&wclcmdup32 options' '@'
include additional directive file
.note "linker directives"
allows use of any linker directive
.endnote
