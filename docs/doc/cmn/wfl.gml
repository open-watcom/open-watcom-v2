.chap The &product Compile and Link Utility
.*
.np
The &product Compile and Link Utility is designed for generating
applications, simply and quickly, using a single command line.
On the command line, you can list source file names as well as object
file names.
Source files are compiled;
object files and libraries are simply included in the link phase.
Options can be passed on to both the compiler and linker.
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
:cmt. Files with extensions of "ASM" are assumed to be assembler source
:cmt. files and will be assembled by the &asmname..
Files with any other extension, including none at all, are assumed to
be &lang source files and will be compiled.
Pattern matching characters ("*" and "?") may be used in the file
specifications.
If no file extension is specified for a file name then "&langsuffup"
is assumed.
.np
Options are prefixed with a slash (/) or a dash (&minus.) and may be
specified in any order.
Options can include any of the &cmpname compiler options plus some
additional options specific to the &wclname. utility.
Certain options can include a "NO" prefix to disable an option.
A summary of options is displayed on the screen by simply entering the
"&wclcmdup16" or "&wclcmdup32" command with no arguments.
.*
.section &wclname Options Summary
.*
.begnote $compact $setptnt 12
:DTHD.General options:
:DDHD.Description:
.note C
.ix '&wclcmdup16 options' 'C'
.ix '&wclcmdup32 options' 'C'
compile the files only, do not link them
.note Y
.ix '&wclcmdup16 options' 'Y'
.ix '&wclcmdup32 options' 'Y'
ignore the &wclcmdup16/&wclcmdup32 environment variable
.endnote
.*
.dm optref begin
.dm optref end
.*
.im WFCOPTS
.begnote $compact $setptnt 12
:DTHD.Linker options:
:DDHD.Description:
.note FD[=<directive_file>]
.ix '&wclcmdup16 options' 'FD[=<directive_file>]'
.ix '&wclcmdup32 options' 'FD[=<directive_file>]'
keep directive file and, optionally, rename it (default name is
"&wcltemp..LNK").
.note FE=<executable>
.ix '&wclcmdup16 options' 'FE=<executable>'
.ix '&wclcmdup32 options' 'FE=<executable>'
name executable file
.note FI=<fn>
.ix '&wclcmdup16 options' 'FI=<fn>'
.ix '&wclcmdup32 options' 'FI=<fn>'
include additional directive file
.note FM[=<map_file>]
.ix '&wclcmdup16 options' 'FM[=<map_file>]'
.ix '&wclcmdup32 options' 'FM[=<map_file>]'
generate map file and, optionally, name it
.note K=<stack_size>
.ix '&wclcmdup16 options' 'K=<stack_size>'
.ix '&wclcmdup32 options' 'K=<stack_size>'
set stack size
.note LP
.ix '&wclcmdup16 options' 'LP'
(16-bit only)
create an OS/2 protected-mode program
.note LR
.ix '&wclcmdup16 options' 'LR'
(16-bit only)
create a DOS real-mode program
.note L=<system_name>
.ix '&wclcmdup16 options' 'L=<system_name>'
.ix '&wclcmdup32 options' 'L=<system_name>'
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
.note "<linker directives>"
.ix '&wclcmdup16 options' '"<linker directives>"'
.ix '&wclcmdup32 options' '"<linker directives>"'
specify additional linker directives
.endnote
.im optdflts
