.chap *refid=cpopts &product Compiler Options
.*
.np
.ix 'options'
.if '&target' eq 'QNX' .do begin
This chapter describes all the compiler options that are available.
.do end
.el .do begin
Source files can be compiled using either the IDE or command-line
compilers.
This chapter describes all the compiler options that are available.
.np
For information about compiling applications from the &vip,
see the
.book &company Graphical Tools User's Guide.
.np
For information about compiling applications from the command line,
see the chapter entitled :HDREF refid='cpwcc'..
.np
The &product compiler command names (
.ct .us compiler_name
.ct ) are:
.ix 'command name' 'compiler'
.if '&target' eq 'QNX' .do begin
.begnote $compact
.note &ccmd16
the &cmpcname compiler for 16-bit Intel platforms.
.note &pcmd16
the &cmppname compiler for 16-bit Intel platforms.
.note &ccmd32
the &cmpcname compiler for 32-bit Intel platforms.
.note &pcmd32
the &cmppname compiler for 32-bit Intel platforms.
.if '&alpha' eq 'AXP' .do begin
.note &ccmdAX
the &cmpcname compiler for DEC Alpha AXP platforms.
.note &pcmdAX
the &cmppname compiler for DEC Alpha AXP platforms.
.do end
.endnote
.do end
.el .do begin
.begnote $compact
.note &ccmdup16
the &cmpcname compiler for 16-bit Intel platforms.
.note &pcmdup16
the &cmppname compiler for 16-bit Intel platforms.
.note &ccmdup32
the &cmpcname compiler for 32-bit Intel platforms.
.note &pcmdup32
the &cmppname compiler for 32-bit Intel platforms.
.if '&alpha' eq 'AXP' .do begin
.note &ccmdupAX
the &cmpcname compiler for DEC Alpha AXP platforms.
.note &pcmdupAX
the &cmppname compiler for DEC Alpha AXP platforms.
.do end
.endnote
.do end
.*
.if &e'&dohelp eq 1 .do begin
.*
.section Compiler Options - Indexed
.*
.np
.ix 'compiling options'
In this section, we present an index of all compiler options.
.* .begnote $compact $setptnt 10
.* .notehd1 Option:
.* .notehd2 See description:
.im CPOPTLST
.* .endnote
.do end
.*
.section Compiler Options - Summarized Alphabetically
.*
.np
.ix 'compiling options'
In this section, we present a terse summary of compiler options.
This summary is displayed on the screen by simply entering the
compiler command name with no arguments.
.begnote $compact $setptnt 10
.notehd1 Option:
.notehd2 Description:
.im CPOPTSUM
.endnote
.*
.section Compiler Options - Summarized By Category
.*
.np
.ix 'compiling options'
In the following sections, we present a terse summary of compiler
options organized into categories.
.*
.beglevel
.*
.helppref Summary:
.*
.section Target Specific
.*
.begnote $compact $setptnt 10
.ix 'options' 'target specific'
.notehd1 Option:
.notehd2 Description:
.if &e'&$SWbc eq 1 .do begin
.note bc
build target is a console application
:optref refid='SWbc'.
.do end
.if &e'&$SWbd eq 1 .do begin
.note bd
build target is a Dynamic Link Library (DLL)
:optref refid='SWbd'.
.do end
.if &e'&$SWbg eq 1 .do begin
.note bg
build target is a GUI application
:optref refid='SWbg'.
.do end
.if &e'&$SWbm eq 1 .do begin
.note bm
build target is a multi-threaded environment
:optref refid='SWbm'.
.do end
.if &e'&$SWbr eq 1 .do begin
.note br
build target uses DLL version of C/C++ run-time library
:optref refid='SWbr'.
.do end
.if &e'&$SWbt eq 1 .do begin
.note bt[=<os>]
build target for operating system <os>
:optref refid='SWbt'.
.do end
.if &e'&$SWbw eq 1 .do begin
.note bw
build target uses default windowing support
:optref refid='SWbw'.
.do end
.if &e'&$SWof eq 1 .do begin
.note of
generate traceable stack frames as needed
:optref refid='SWof'.
.do end
.if &e'&$SWofpls eq 1 .do begin
.note of+
always generate traceable stack frames
:optref refid='SWofpls'.
.do end
.if &e'&$SWsg eq 1 .do begin
.note sg
generate calls to grow the stack
:optref refid='SWsg'.
.do end
.if &e'&$SWst eq 1 .do begin
.note st
touch stack through SS first
:optref refid='SWst'.
.do end
.if &e'&$SWzw eq 1 .do begin
.note zw
generate code for Microsoft Windows
:optref refid='SWzw'.
.do end
.if &e'&$SWzW86 eq 1 .do begin
.note zW
&286only.
Microsoft Windows optimized prologue/epilogue code sequences
:optref refid='SWzW86'.
.do end
.if &e'&$SWzWs eq 1 .do begin
.note zWs
&286only.
Microsoft Windows smart callback sequences
:optref refid='SWzWs'.
.do end
.endnote
.*
.section Debugging/Profiling
.*
.begnote $compact $setptnt 10
.ix 'options' 'debugging/profiling'
.notehd1 Option:
.notehd2 Description:
.if &e'&$SWd0 eq 1 .do begin
.note d0
(C++ only)
no debugging information
:optref refid='SWd0'.
.do end
.if &e'&$SWd1 eq 1 .do begin
.note d1
line number debugging information
:optref refid='SWd1'.
.do end
.if &e'&$SWd1pls eq 1 .do begin
.note d1+
(C only)
line number debugging information plus typing information for global
symbols and local structs and arrays
:optref refid='SWd1pls'.
.do end
.if &e'&$SWd2 eq 1 .do begin
.note d2
full symbolic debugging information
:optref refid='SWd2'.
.do end
.if &e'&$SWd2i eq 1 .do begin
.note d2i
(C++ only)
d2 and debug inlines; emit inlines as external out-of-line functions
:optref refid='SWd2i'.
.do end
.if &e'&$SWd2s eq 1 .do begin
.note d2s
(C++ only)
d2 and debug inlines; emit inlines as static out-of-line functions
:optref refid='SWd2s'.
.do end
.if &e'&$SWd2t eq 1 .do begin
.note d2t
(C++ only)
d2 but without type names
:optref refid='SWd2t'.
.do end
.if &e'&$SWd3 eq 1 .do begin
.note d3
full symbolic debugging with unreferenced type names
:optref refid='SWd3'.
.do end
.if &e'&$SWd3i eq 1 .do begin
.note d3i
(C++ only)
d3 plus debug inlines; emit inlines as external out-of-line functions
:optref refid='SWd3i'.
.do end
.if &e'&$SWd3s eq 1 .do begin
.note d3s
(C++ only)
d3 plus debug inlines; emit inlines as static out-of-line functions
:optref refid='SWd3s'.
.do end
.if &e'&$SWee eq 1 .do begin
.note ee
call epilogue hook routine
:optref refid='SWee'.
.do end
.if &e'&$SWen eq 1 .do begin
.note en
emit routine names in the code segment
:optref refid='SWen'.
.do end
.if &e'&$SWep eq 1 .do begin
.note ep[<number>]
call prologue hook routine with number stack bytes available
:optref refid='SWep'.
.do end
.if &e'&$SWet eq 1 .do begin
.note et
Pentium profiling
:optref refid='SWet'.
.do end
.if &e'&$SWhwdc eq 1 .do begin
.note h{w,d,c}
set debug output format (&company, DWARF, Codeview)
:optref refid='SWhwdc'
.do end
.if &e'&$SWs eq 1 .do begin
.note s
remove stack overflow checks
:optref refid='SWs'.
.do end
.if &e'&$SWsi eq 1 .do begin
.note si
&AXPonly.
initialize stack frame storage with pattern
:optref refid='SWsi'.
.do end
.endnote
.*
.section Preprocessor
.*
.begnote $compact $setptnt 10
.ix 'options' 'preprocessor'
.notehd1 Option:
.notehd2 Description:
.if &e'&$SWd eq 1 .do begin
.note d<name>[=text]
precompilation #define name [text]
:optref refid='SWd'.
.do end
.if &e'&$SWdpls eq 1 .do begin
.note d+
allow extended "d" macro definitions on command line
:optref refid='SWdpls'.
.do end
.if &e'&$SWfo eq 1 .do begin
.note fo[=<file_name>]
set preprocessor output file name
:optref refid='SWfo'.
.do end
.if &e'&$SWpil eq 1 .do begin
.note pil
preprocessor ignores #line directives
:optref refid='SWpil'.
.do end
.if &e'&$SWp eq 1 .do begin
.note p{e,l,c,w=<num>}
preprocess file
.begnote $compact
.note c
preserve comments
.note e
encrypt identifiers (C++ only)
.note l
insert #line directives
.note w=<num>
wrap output lines at <num> columns. Zero means no wrap.
.endnote
.np
:optref refid='SWp'.
.do end
.if &e'&$SWu eq 1 .do begin
.note u<name>
undefine macro name
:optref refid='SWu'.
.do end
.endnote
.*
.section Diagnostics
.*
.begnote $compact $setptnt 10
.ix 'options' 'diagnostics'
.notehd1 Option:
.notehd2 Description:
.if &e'&$SWe eq 1 .do begin
.note e<number>
set error limit number
:optref refid='SWe'.
.do end
.if &e'&$SWef eq 1 .do begin
.note ef
use full path names in error messages
:optref refid='SWef'.
.do end
.if &e'&$SWeq eq 1 .do begin
.note eq
do not display error messages (they are still written to a file)
:optref refid='SWeq'.
.do end
.if &e'&$SWer eq 1 .do begin
.note er
(C++ only)
do not recover from undefined symbol errors
:optref refid='SWer'.
.do end
.if &e'&$SWew eq 1 .do begin
.note ew
(C++ only)
alternate error message formatting
:optref refid='SWew'.
.do end
.if &e'&$SWq eq 1 .do begin
.note q
operate quietly
:optref refid='SWq'.
.do end
.if &e'&$SWt eq 1 .do begin
.note t=<num>
set tab stop multiplier
:optref refid='SWt'.
.do end
.if &e'&$SWw eq 1 .do begin
.note w<number>
set warning level number
:optref refid='SWw'.
.do end
.if &e'&$SWwcd eq 1 .do begin
.note wcd=<num>
warning control: disable warning message <num>
:optref refid='SWwcd'.
.do end
.if &e'&$SWwce eq 1 .do begin
.note wce=<num>
warning control: enable warning message <num>
:optref refid='SWwce'.
.do end
.if &e'&$SWwe eq 1 .do begin
.note we
treat all warnings as errors
:optref refid='SWwe'.
.do end
.if &e'&$SWwx eq 1 .do begin
.note wx
set warning level to maximum setting
:optref refid='SWwx'.
.do end
.if &e'&$SWza eq 1 .do begin
.note z{a,e}
disable/enable language extensions
:optref refid='SWza'.
:optref refid='SWze'.
.do end
.if &e'&$SWzq eq 1 .do begin
.note zq
operate quietly
:optref refid='SWzq'.
.do end
.if &e'&$SWzs eq 1 .do begin
.note zs
syntax check only
:optref refid='SWzs'.
.do end
.endnote
.*
.section Source/Output Control
.*
.begnote $compact $setptnt 10
.ix 'options' 'source/output control'
.notehd1 Option:
.notehd2 Description:
.if &e'&$SWad eq 1 .do begin
.note ad[=<file_name>]
generate make style auto-dependency file
:optref refid='SWad'.
.do end
.if &e'&$SWadbs eq 1 .do begin
.note adbs
force path separators generated in make style auto-dependency file to
backslashes
:optref refid='SWadbs'.
.do end
.if &e'&$SWadd eq 1 .do begin
.note add[=<file_name>]
set source name (the first dependency) for make style auto-dependency file
:optref refid='SWadd'.
.do end
.if &e'&$SWadhp eq 1 .do begin
.note adhp[=<path prefix>]
set default path for header dependencies which result in filename only
:optref refid='SWadhp'.
.do end
.if &e'&$SWadfs eq 1 .do begin
.note adfs
force path separators generated in make style auto-dependency file to
forward slashes
:optref refid='SWadfs'.
.do end
.if &e'&$SWadt eq 1 .do begin
.note adt[=<target_name>]
specify target name generated in make style auto-dependency file
:optref refid='SWadt'.
.do end
.if &e'&$SWdb eq 1 .do begin
.note db
generate browsing information
:optref refid='SWdb'.
.do end
.if &e'&$SWez eq 1 .do begin
.note ez
generate PharLap EZ-OMF object files
:optref refid='SWez'.
.do end
.if &e'&$SWfc eq 1 .do begin
.note fc=<file_name>
(C++ only)
specify file of command lines to be batch processed
:optref refid='SWfc'.
.do end
.if &e'&$SWfh eq 1 .do begin
.note fh[q][=<file_name>]
use precompiled headers
:optref refid='SWfh'.
.do end
.if &e'&$SWfhd eq 1 .do begin
.note fhd
store debug info for pre-compiled header once (DWARF only)
:optref refid='SWfhd'.
.do end
.if &e'&$SWfhr eq 1 .do begin
.note fhr
(C++ only)
force compiler to read pre-compiled header (will never write)
:optref refid='SWfhr'.
.do end
.if &e'&$SWfhw eq 1 .do begin
.note fhw
(C++ only)
force compiler to write pre-compiled header (will never read)
:optref refid='SWfhw'.
.do end
.if &e'&$SWfhwe eq 1 .do begin
.note fhwe
(C++ only)
don't include pre-compiled header warnings when "we" is used
:optref refid='SWfhwe'.
.do end
.if &e'&$SWfi eq 1 .do begin
.note fi=<file_name>
force file_name to be included
:optref refid='SWfi'.
.do end
.if &e'&$SWfo eq 1 .do begin
.note fo[=<file_name>]
set object or preprocessor output file name
:optref refid='SWfo2'.
.do end
.if &e'&$SWfr eq 1 .do begin
.note fr[=<file_name>]
set error file name
:optref refid='SWfr'.
.do end
.if &e'&$SWft eq 1 .do begin
.note ft
(C++ only)
try truncated (8.3) header file specification
:optref refid='SWft'.
.do end
.if &e'&$SWfti eq 1 .do begin
.note fti
(C only)
track include file opens
:optref refid='SWfti'.
.do end
.if &e'&$SWfx eq 1 .do begin
.note fx
(C++ only)
do not try truncated (8.3) header file specification
:optref refid='SWfx'.
.do end
.if &e'&$SWfzh eq 1 .do begin
.note fzh
(C++ only)
do not automatically append extensions for include files
:optref refid='SWfzh'.
.do end
.if &e'&$SWfzs eq 1 .do begin
.note fzs
(C++ only)
do not automatically append extensions for source files
:optref refid='SWfzs'.
.do end
.if &e'&$SWi eq 1 .do begin
.note i=<directory>
another include directory
:optref refid='SWi'.
.do end
.if &e'&$SWk eq 1 .do begin
.note k
continue processing files (ignore errors)
:optref refid='SWk'.
.do end
.if &e'&$SWv eq 1 .do begin
.note v
output function declarations to .def
:optref refid='SWv'.
.do end
.if &e'&$SWx eq 1 .do begin
.note x
ignore environment variables when searching for include files
:optref refid='SWx'.
.do end
.if &e'&$SWzat eq 1 .do begin
.note zat
(C++ only) disable alternative tokens
:optref refid='SWzat'.
.do end
.if &e'&$SWzf eq 1 .do begin
.note zf
(C++ only) let scope of for loop initialization extend beyond loop
:optref refid='SWzf'.
.do end
.if &e'&$SWzg eq 1 .do begin
.note zg
generate function prototypes using base types
:optref refid='SWzg'.
.do end
.if &e'&$SWzl eq 1 .do begin
.note zl
remove default library information
:optref refid='SWzl'.
.do end
.if &e'&$SWzld eq 1 .do begin
.note zld
remove file dependency information
:optref refid='SWzld'.
.do end
.if &e'&$SWzlf eq 1 .do begin
.note zlf
add default library information to object files
:optref refid='SWzlf'.
.do end
.if &e'&$SWzls eq 1 .do begin
.note zls
remove automatically generated symbols references
:optref refid='SWzls'.
.do end
.endnote
.section Code Generation
.*
.begnote $compact $setptnt 10
.ix 'options' 'code generation'
.notehd1 Option:
.notehd2 Description:
.if &e'&$SWas eq 1 .do begin
.note as
&AXPonly.
assume short integers are aligned
:optref refid='SWas'.
.do end
.if &e'&$SWecc eq 1 .do begin
.note ecc
set default calling convention to __cdecl
:optref refid='SWecc'.
.do end
.if &e'&$SWecd eq 1 .do begin
.note ecd
set default calling convention to __stdcall
:optref refid='SWecd'.
.do end
.if &e'&$SWecf eq 1 .do begin
.note ecf
set default calling convention to __fastcall
:optref refid='SWecf'.
.do end
:CMT. .if &e'&$SWeco eq 1 .do begin
:CMT. .note eco
:CMT. set default calling convention to _Optlink
:CMT. :optref refid='SWeco'.
:CMT. .do end
.if &e'&$SWecp eq 1 .do begin
.note ecp
set default calling convention to __pascal
:optref refid='SWecp'.
.do end
.if &e'&$SWecr eq 1 .do begin
.note ecr
set default calling convention to __fortran
:optref refid='SWecr'.
.do end
.if &e'&$SWecs eq 1 .do begin
.note ecs
set default calling convention to __syscall
:optref refid='SWecs'.
.do end
.if &e'&$SWecw eq 1 .do begin
.note ecw
set default calling convention to __watcall (default)
:optref refid='SWecw'.
.do end
.if &e'&$SWei eq 1 .do begin
.note ei
force enum base type to use at least an int
:optref refid='SWei'.
.do end
.if &e'&$SWem eq 1 .do begin
.note em
force enum base type to use minimum
:optref refid='SWem'.
.do end
.if &e'&$SWj eq 1 .do begin
.note j
change char default from unsigned to signed
:optref refid='SWj'.
.do end
.if &e'&$SWri eq 1 .do begin
.note ri
return chars and shorts as ints
:optref refid='SWri'.
.do end
.if &e'&$SWxr eq 1 .do begin
.note xr
(C++ only)
enable RTTI
:optref refid='SWxr'.
.do end
.if &e'&$SWzc eq 1 .do begin
.note zc
place literal strings in the code segment
:optref refid='SWzc'.
.do end
.if &e'&$SWzp eq 1 .do begin
.note zp{1,2,4,8,16}
pack structure members
:optref refid='SWzp'.
.do end
.if &e'&$SWzps eq 1 .do begin
.note zps
&AXPonly.
always align structs on qword boundaries
:optref refid='SWzps'.
.do end
.if &e'&$SWzpw eq 1 .do begin
.note zpw
output warning when padding is added in a struct/class
:optref refid='SWzpw'.
.do end
.if &e'&$SWzt eq 1 .do begin
.note zt<number>
set data threshold
:optref refid='SWzt'.
.do end
.if &e'&$SWzv eq 1 .do begin
.note zv
(C++ only)
enable virtual function removal optimization
:optref refid='SWzv'.
.do end
.endnote
.*
.section 80x86 Floating Point
.*
.begnote $compact $setptnt 10
.ix 'options' 'floating point'
.notehd1 Option:
.notehd2 Description:
.if &e'&$SWfpc eq 1 .do begin
.note fpc
calls to floating-point library
:optref refid='SWfpc'.
.do end
.if &e'&$SWfpi eq 1 .do begin
.note fpi
in-line 80x87 instructions with emulation
:optref refid='SWfpi'.
.do end
.if &e'&$SWfpi87 eq 1 .do begin
.note fpi87
in-line 80x87 instructions
:optref refid='SWfpi87'.
.do end
.if &e'&$SWfp2 eq 1 .do begin
.note fp2
generate floating-point for 80x87
:optref refid='SWfp2'.
.do end
.if &e'&$SWfp3 eq 1 .do begin
.note fp3
generate floating-point for 387
:optref refid='SWfp3'.
.do end
.if &e'&$SWfp5 eq 1 .do begin
.note fp5
optimize floating-point for Pentium
:optref refid='SWfp5'.
.do end
.if &e'&$SWfp6 eq 1 .do begin
.note fp6
optimize floating-point for Pentium Pro
:optref refid='SWfp6'.
.do end
.if &e'&$SWfpd eq 1 .do begin
.note fpd
enable generation of Pentium FDIV bug check code
:optref refid='SWfpd'.
.do end
.if &e'&$SW7 eq 1 .do begin
.note 7
in-line 80x87 instructions
:optref refid='SW7'.
.do end
.endnote
.*
.section Segments/Modules
.*
.begnote $compact $setptnt 10
.ix 'options' 'segments/modules'
.notehd1 Option:
.notehd2 Description:
.if &e'&$SWg eq 1 .do begin
.note g=<codegroup>
set code group name
:optref refid='SWg'.
.do end
.if &e'&$SWnc eq 1 .do begin
.note nc=<name>
set code class name
:optref refid='SWnc'.
.do end
.if &e'&$SWnd eq 1 .do begin
.note nd=<name>
set data segment name
:optref refid='SWnd'.
.do end
.if &e'&$SWnm eq 1 .do begin
.note nm=<name>
set module name
:optref refid='SWnm'.
.do end
.if &e'&$SWnt eq 1 .do begin
.note nt=<name>
set name of text segment
:optref refid='SWnt'.
.do end
.if &e'&$SWzm eq 1 .do begin
.note zm
place each function in separate segment (near functions not allowed)
:optref refid='SWzm'.
.do end
.if &e'&$SWzmf eq 1 .do begin
.note zmf
(C++ only)
place each function in separate segment (near functions allowed)
:optref refid='SWzmf'.
.do end
.endnote
.*
.section 80x86 Run-time Conventions
.*
.begnote $compact $setptnt 10
.ix 'options' 'run-time conventions'
.notehd1 Option:
.notehd2 Description:
.if &e'&$SW0 eq 1 .do begin
.note 0
&286only.
8088 and 8086 instructions
:optref refid='SW0'.
.do end
.if &e'&$SW1 eq 1 .do begin
.note 1
&286only.
188 and 186 instructions
:optref refid='SW1'.
.do end
.if &e'&$SW2 eq 1 .do begin
.note 2
&286only.
286 instructions
:optref refid='SW2'.
.do end
.if &e'&$SW3 eq 1 .do begin
.note 3
&286only.
386 instructions
:optref refid='SW3'.
.do end
.if &e'&$SW4 eq 1 .do begin
.note 4
&286only.
486 instructions
:optref refid='SW4'.
.do end
.if &e'&$SW5 eq 1 .do begin
.note 5
&286only.
Pentium instructions
:optref refid='SW5'.
.do end
.if &e'&$SW6 eq 1 .do begin
.note 6
&286only.
Pentium Pro instructions
:optref refid='SW6'.
.do end
.if &e'&$SW3RS eq 1 .do begin
.note 3r
&386only.
386 register calling conventions
:optref refid='SW3RS'.
.note 3s
&386only.
386 stack calling conventions
:optref refid='SW3RS'.
.do end
.if &e'&$SW4RS eq 1 .do begin
.note 4r
&386only.
486 register calling conventions
:optref refid='SW4RS'.
.note 4s
&386only.
486 stack calling conventions
:optref refid='SW4RS'.
.do end
.if &e'&$SW5RS eq 1 .do begin
.note 5r
&386only.
Pentium register calling conventions
:optref refid='SW5RS'.
.note 5s
&386only.
Pentium stack calling conventions
:optref refid='SW5RS'.
.do end
.if &e'&$SW6RS eq 1 .do begin
.note 6r
&386only.
Pentium Pro register calling conventions
:optref refid='SW6RS'.
.note 6s
&386only.
Pentium Pro stack calling conventions
:optref refid='SW6RS'.
.do end
.if &e'&$SWmf eq 1 .do begin
.note m{f,s,m,c,l,h}
memory model (Flat,Small,Medium,Compact,Large,Huge)
:optref refid='SWmf'.
.do end
.if &e'&$SWzdfp eq 1 .do begin
.note zdf
DS floats i.e. not fixed to DGROUP
:optref refid='SWzdfp'.
.do end
.if &e'&$SWzdfp eq 1 .do begin
.note zdp
DS is pegged to DGROUP
:optref refid='SWzdfp'.
.do end
.if &e'&$SWzdl eq 1 .do begin
.note zdl
Load DS directly from DGROUP
:optref refid='SWzdl'.
.do end
.if &e'&$SWzffp eq 1 .do begin
.note zff
FS floats i.e. not fixed to a segment
:optref refid='SWzffp'.
.note zfp
FS is pegged to a segment
:optref refid='SWzffp'.
.do end
.if &e'&$SWzfp eq 1 .do begin
.note zfp
Generate FWAIT instructions on 386 and later
:optref refid='SWzfp'.
.do end
.if &e'&$SWzgfp eq 1 .do begin
.note zgf
GS floats i.e. not fixed to a segment
:optref refid='SWzgfp'.
.note zgp
GS is pegged to a segment
:optref refid='SWzgfp'.
.do end
.if &e'&$SWzri eq 1 .do begin
.note zri
Inline floating point rounding code
:optref refid='SWzri'.
.do end
.if &e'&$SWzro eq 1 .do begin
.note zro
Omit floating point rounding code
:optref refid='SWzro'.
.do end
.if &e'&$SWzu eq 1 .do begin
.note zu
SS != DGROUP
:optref refid='SWzu'.
.do end
.endnote
.*
.section Optimizations
.*
.begnote $compact $setptnt 10
.ix 'options' 'optimizations'
.notehd1 Option:
.notehd2 Description:
.if &e'&$SWoa eq 1 .do begin
.note oa
relax aliasing constraints
:optref refid='SWoa'.
.do end
.if &e'&$SWob eq 1 .do begin
.note ob
enable branch prediction
:optref refid='SWob'.
.do end
.if &e'&$SWoc eq 1 .do begin
.note oc
disable <call followed by return> to <jump> optimization
:optref refid='SWoc'.
.do end
.if &e'&$SWod eq 1 .do begin
.note od
disable all optimizations
:optref refid='SWod'.
.do end
.if &e'&$SWoe eq 1 .do begin
.note oe[=<num>]
expand user functions in-line. <num> controls max size
:optref refid='SWoe'.
.do end
.if &e'&$SWoh eq 1 .do begin
.note oh
enable repeated optimizations (longer compiles)
:optref refid='SWoh'.
.do end
.if &e'&$SWoi eq 1 .do begin
.note oi
expand intrinsic functions in-line
:optref refid='SWoi'.
.do end
.if &e'&$SWoipls eq 1 .do begin
.note oi+
(C++ only)
expand intrinsic functions in-line and set inline_depth to maximum
:optref refid='SWoipls'.
.do end
.if &e'&$SWok eq 1 .do begin
.note ok
enable control flow prologues and epilogues
:optref refid='SWok'.
.do end
.if &e'&$SWol eq 1 .do begin
.note ol
enable loop optimizations
:optref refid='SWol'.
.do end
.if &e'&$SWolpls eq 1 .do begin
.note ol+
enable loop optimizations with loop unrolling
:optref refid='SWolpls'.
.do end
.if &e'&$SWom eq 1 .do begin
.note om
generate in-line 80x87 code for math functions
:optref refid='SWom'.
.do end
.if &e'&$SWon eq 1 .do begin
.note on
allow numerically unstable optimizations
:optref refid='SWon'.
.do end
.if &e'&$SWoo eq 1 .do begin
.note oo
continue compilation if low on memory
:optref refid='SWoo'.
.do end
.if &e'&$SWop eq 1 .do begin
.note op
generate consistent floating-point results
:optref refid='SWop'.
.do end
.if &e'&$SWor eq 1 .do begin
.note or
reorder instructions for best pipeline usage
:optref refid='SWor'.
.do end
.if &e'&$SWos eq 1 .do begin
.note os
favor code size over execution time in optimizations
:optref refid='SWos'.
.do end
.if &e'&$SWot eq 1 .do begin
.note ot
favor execution time over code size in optimizations
:optref refid='SWot'.
.do end
.if &e'&$SWou eq 1 .do begin
.note ou
all functions must have unique addresses
:optref refid='SWou'.
.do end
.if &e'&$SWox eq 1 .do begin
.note ox
equivalent to -obmiler -s
:optref refid='SWox'.
.do end
.if &e'&$SWoz eq 1 .do begin
.note oz
NULL points to valid memory in the target environment
:optref refid='SWoz'.
.do end
.endnote
.*
.section C++ Exception Handling
.*
.begnote $compact $setptnt 10
.ix 'options' 'C++ exception handling'
.ix 'exception handling'
.notehd1 Option:
.notehd2 Description:
.*
.if &e'&$SWxd eq 1 .do begin
.note xd
disable exception handling (default)
:optref refid='SWxd'.
.do end
.*
.if &e'&$SWxdt eq 1 .do begin
.note xdt
disable exception handling (same as "xd")
:optref refid='SWxdt'.
.do end
.*
.if &e'&$SWxds eq 1 .do begin
.note xds
disable exception handling (table-driven destructors)
:optref refid='SWxds'.
.do end
.*
.if &e'&$SWxs eq 1 .do begin
.note xs
enable exception handling
:optref refid='SWxs'.
.do end
.*
.if &e'&$SWxst eq 1 .do begin
.note xst
enable exception handling (direct calls for destruction)
:optref refid='SWxst'.
.do end
.*
.if &e'&$SWxss eq 1 .do begin
.note xss
enable exception handling (table-driven destructors)
:optref refid='SWxss'.
.do end
.*
.*
.endnote
.*
.section Double-Byte/Unicode Characters
.*
.begnote $compact $setptnt 10
.ix 'options' 'double-byte characters'
.notehd1 Option:
.notehd2 Description:
.if &e'&$SWzk eq 1 .do begin
.note zk{0,1,2,l}
double-byte char support: 0=Kanji,1=Chinese/Taiwanese,2=Korean,l=local
:optref refid='SWzk'.
.do end
.if &e'&$SWzk0u eq 1 .do begin
.note zk0u
translate double-byte Kanji to UNICODE
:optref refid='SWzk0u'.
.do end
.if &e'&$SWzku eq 1 .do begin
.note zku=<codepage>
load UNICODE translate table for specified code page
:optref refid='SWzku'.
.do end
.endnote
.*
.if &e'&$SWvc eq 1 .do begin
.*
.section Compatibility with Microsoft Visual C++
.*
.begnote $compact $setptnt 10
.ix 'options' 'compatibility with Visual C++'
.notehd1 Option:
.notehd2 Description:
.note vc...
VC++ compatibility options
:optref refid='SWvc'.
.note vcap
allow alloca() or _alloca() in a parameter list
.endnote
.do end
.*
.section Compatibility with Older Versions of the 80x86 Compilers
.*
.begnote $compact $setptnt 10
.ix 'options' 'compatibility with older versions'
.notehd1 Option:
.notehd2 Description:
.if &e'&$SWr eq 1 .do begin
.note r
save/restore segment registers across calls
:optref refid='SWr'.
.do end
.if &e'&$SWfpr eq 1 .do begin
.note fpr
generate backward compatible 80x87 code
:optref refid='SWfpr'.
.do end
.if &e'&$SWzz eq 1 .do begin
.note zz
generate backward compatible __stdcall conventions by removing the
"@size" from __stdcall function names (10.0 compatible)
:optref refid='SWzz'.
.do end
.endnote
.*
.helppref
.*
.endlevel
.*
.section Compiler Options - Full Description
.*
.np
.ix 'compiling options'
In the following sections, we present complete descriptions of
compiler options organized into categories.
.*
.beglevel
.*
.section Target Specific
.*
.np
This group of options deals with characteristics of the target
application;
for example, simple executables versus Dynamic Link Libraries,
character-mode versus graphical user interface,
single-threaded versus multi-threaded,
and so on.
.*
:OPTLIST.
.*
.if &e'&$SWbc eq 1 .do begin
:OPT refid='SWbc' name='bc'.
.ix 'options' 'bc'
.ix 'console application'
(OS/2, Win16/32 only)
This option causes the compiler to emit into the object file
references to the appropriate startup code for a character-mode
console application.
The presence of
.id LibMain/DLLMain
or
.id WinMain/wWinMain
in the source code does not influence the selection of startup code.
Only
.id main
and
.id wmain
are significant.
.np
If none of "bc", "bd", "bg" or "bw" are specified then the order of
priority in determining which combination of startup code and
libraries to use are as follows.
.autonote
.note
The presence of one of
.id LibMain
or
.id DLLMain
implies that the DLL startup code and libraries should be used.
.note
The presence of
.id WinMain
or
.id wWinMain
implies that the GUI startup code and libraries should be used.
.note
The presence of
.id main
or
.id wmain
implies that the default startup code and libraries should be used.
.endnote
.np
If both a wide and non-wide version of an entry point are specified,
the "wide" entry point will be used.
Thus
.id wWinMain
is called when both
.id WinMain
and
.id wWinMain
are present.
Similarly,
.id wmain
is called when both
.id main
and
.id wmain
are present (and
.id WinMain/wWinMain
are not present).
By default, if both
.id wmain
and
.id WinMain
are included in the source code, then the startup code will attempt
to call
.id wWinMain
(since both "wide" and "windowed" entry points were included).
.if '&target' eq 'QNX' .do begin
This option does not apply to QNX.
.do end
.do end
.*
.if &e'&$SWbd eq 1 .do begin
:OPT refid='SWbd' name='bd'.
.ix 'options' 'bd'
.ix 'dynamic link library'
.ix 'DLL'
.ix '__DLLstart_'
(OS/2, Win16/32 only)
This option causes the compiler to emit into the object file
references to the run-time DLL startup code (reference to the
.id __DLLstart_
symbol) and, if required, special
versions of the run-time libraries that support DLLs. The presence of
.id main/wmain
or
.id WinMain/wWinMain
in the source code does not influence the selection of startup code.
Only
.id LibMain
and
.id DLLMain
are significant
:optref refid='SWbc'..
If you are building a DLL with statically linked C runtime (the default),
it is recommended that you compile at least one of its object files with
the "bd" switch to ensure that the DLL's C runtime is properly initialized.
.ix 'predefined macros' 'see macros'
.ix 'macros' '__SW_BD'
The macro
.kwm __SW_BD
will be predefined if "bd" is selected.
.if '&target' eq 'QNX' .do begin
This option does not apply to QNX.
.do end
.do end
.*
.if &e'&$SWbg eq 1 .do begin
:OPT refid='SWbg' name='bg'.
.ix 'options' 'bg'
.ix 'console application'
(OS/2, Win16/32 only)
This option causes the compiler to emit into the object file
references to the appropriate startup code for a windowed (GUI)
application.
The presence of
.id LibMain/DLLMain
or
.id main/wmain
in the source code does not influence the selection of startup code.
Only
.id WinMain
and
.id wWinMain
are significant
:optref refid='SWbc'..
.if '&target' eq 'QNX' .do begin
This option does not apply to QNX.
.do end
.do end
.*
.if &e'&$SWbm eq 1 .do begin
:OPT refid='SWbm' name='bm'.
.ix 'options' 'bm'
(Netware, OS/2, Win32 only)
This option causes the compiler to emit into the object file
references to the appropriate multi-threaded library name(s).
.ix 'macros' '_MT'
.ix 'macros' '__SW_BM'
The macros
.kwm _MT
and
.kwm __SW_BM
will be predefined if "bm" is selected.
.if '&target' eq 'QNX' .do begin
This option does not apply to QNX.
.do end
.do end
.*
.if &e'&$SWbr eq 1 .do begin
:OPT refid='SWbr' name='br'.
.ix 'options' 'br'
(OS/2, Win32 only)
This option causes the compiler to emit into the object file
references to the run-time DLL library name(s).
The run-time DLL libraries are special subsets of the &product
run-time libraries that are available as DLLs.
When you use this option with an OS/2 application, you must also
specify the "CASEEXACT" option to the &lnkname..
.ix 'macros' '_DLL'
.ix 'macros' '__SW_BR'
The macros
.kwm _DLL
and
.kwm __SW_BR
will be predefined if "br" is selected.
.if '&target' eq 'QNX' .do begin
This option does not apply to QNX.
.do end
.do end
.*
.if &e'&$SWbt eq 1 .do begin
:OPT refid='SWbt' name='bt'.[=<os>]
.ix 'options' 'bt'
This option causes the compiler to define the "build" target.
This option is used for cross-development work.
It prevents the compiler from defining the default build target (which
is based on the host system the compiler is running on).
The default build targets are:
.begnote
.note DOS
when the host operating system is DOS,
.note OS2
when the host operating system is OS/2,
.note NT
when the host operating system is Windows NT (including Windows 95),
.note QNX
when the host operating system is QNX.
or
.note LINUX
when the host operating system is Linux.
.endnote
.np
It also prevents the compiler from defining the default target macro.
Instead the compiler defines a macro consisting of the string "<os>"
converted to uppercase and prefixed and suffixed with two underscores.
The default target macros are described in the
section entitled :HDREF refid='wccmacs'..
.np
For example, specifying the option:
.millust begin
bt=foo
.millust end
.pc
would cause the compiler to define the macro
.millust begin
__FOO__
.millust end
.pc
and prevent it from defining
.kwm MSDOS
.ct ,
.kwm _DOS
and
.kwm __DOS__
if the compiler was being run under DOS,
.kwm __OS2__
if using the OS/2 hosted compiler,
.kwm __NT__
if using the Windows NT or Windows 95 hosted compiler,
.kwm __QNX__
and
.kwm __UNIX__
if using the QNX hosted version.
or
.kwm __LINUX__
and
.kwm __UNIX__
if using the Linux hosted version.
Any string consisting of letters, digits, and the underscore character
may be used for the target name.
.np
The compiler will also construct an environment variable called
.ev <os>_INCLUDE
and see if it has been defined.
If the environment variable is defined then each directory listed in
it is searched (in the order that they were specified).
For example, the environment variable
.ev WINDOWS_INCLUDE
will be searched if
.sy bt=WINDOWS
option was specified.
.exam begin 1
&setcmd. windows_include=&pc.watcom&pc.h&pc.win
.exam end
.np
Include file processing is described in the
section entitled :HDREF refid='wccinc'..
.np
Several target names are recognized by the compiler and perform
additional operations.
.begnote $setptnt 10
.notehd1 Target name
.notehd2 Additional operation
.note DOS
Defines the macros
.kwm _DOS
and
.kwm MSDOS
.ct .li .
.note WINDOWS
Same as specifying one of the "zw" options.
Defines the macros
.kwm _WINDOWS
(16-bit only)
and
.kwm __WINDOWS_386__
(32-bit only).
.note NETWARE
(32-bit only)
Causes the compiler to use stack-based calling conventions.
Also defines the macro
.kwm __NETWARE_386__
.ct .li .
.note QNX
Defines the macro
.kwm __UNIX__
.ct .li .
.note LINUX
Defines the macro
.kwm __UNIX__
.ct .li .
.endnote
.np
Specifying "bt" with no target name following restores the default
target name.
.do end
.*
.if &e'&$SWbw eq 1 .do begin
:OPT refid='SWbw' name='bw'.
.ix 'options' 'bw'
(Win16 only)
This option causes the compiler to import a special symbol so that the
default windowing library code is linked into your application.
The presence of
.id LibMain/DLLMain
in the source code does not influence the selection of startup code.
Only
.id main,
.id wmain,
.id WinMain
and
.id wWinMain
are significant
:optref refid='SWbc'..
.ix 'macros' '__SW_BW'
The macro
.kwm __SW_BW
will be predefined if "bw" is selected.
.if '&target' eq 'QNX' .do begin
This option does not apply to QNX.
.do end
.do end
.*


.if &e'&$SWof eq 1 .do begin
:OPT refid='SWof' name='of'.
.ix 'options' 'of'
This option selects the generation of traceable stack frames for those
functions that contain calls or require stack frame setup.
.np
&286only.
To use &company's "Dynamic Overlay Manager" (DOS only), you must
compile all modules using one of the "of" or "of+" options ("of" is
sufficient).
.np
For near functions, the following function prologue sequence is
generated.
.millust begin
&286only.
    push BP
    mov  BP,SP

&386only.
    push EBP
    mov  EBP,ESP
.millust end
.pc
For far functions, the following function prologue sequence is
generated.
.millust begin
&286only.
    inc  BP
    push BP
    mov  BP,SP

&386only.
    inc  EBP
    push EBP
    mov  EBP,ESP
.millust end
.pc
The BP/EBP value on the stack will be even or odd depending on the
code model.
.np
For 16-bit DOS systems, the Dynamic Overlay Manager uses this
information to determine if the return address on the stack is a short
address (16-bit offset) or long address (32-bit segment:offset).
.np
Do not use this option for 16-bit Windows applications.
.ix '_export functions'
It will alter the code sequence generated for "_export" functions.
.exam begin 1
&prompt.:SF font=1.compiler_name:eSF. toaster &sw.of
.exam end
.np
.ix 'macros' '__SW_OF'
The macro
.kwm __SW_OF
will be predefined if "of" is selected.
.do end
.*
.if &e'&$SWofpls eq 1 .do begin
:OPT refid='SWofpls' name='of+'.
.ix 'options' 'of+'
This option selects the generation of traceable stack frames for all
functions regardless of whether they contain calls or require stack
frame setup.
This option is intended for developers of embedded systems (ROM-based
applications).
.np
To use &company's "Dynamic Overlay Manager" (16-bit DOS only), you
must compile all modules using one of the "of" or "of+" options ("of"
is sufficient).
.np
For near functions, the following function prologue sequence is
generated.
.millust begin
&286only.
    push BP
    mov  BP,SP

&386only.
    push EBP
    mov  EBP,ESP
.millust end
.pc
For far functions, the following function prologue sequence is
generated.
.millust begin
&286only.
    inc  BP
    push BP
    mov  BP,SP

&386only.
    inc  EBP
    push EBP
    mov  EBP,ESP
.millust end
.pc
The BP/EBP value on the stack will be even or odd depending on the
code model.
.np
For 16-bit DOS systems, the Dynamic Overlay Manager uses this
information to determine if the return address on the stack is a short
address (16-bit offset) or long address (32-bit segment:offset).
.np
Do not use this option for 16-bit Windows applications.
.ix '_export functions'
It will alter the code sequence generated for "_export" functions.
.exam begin 1
&prompt.:SF font=1.compiler_name:eSF. toaster &sw.of+
.exam end
.do end
.*
.if &e'&$SWsg eq 1 .do begin
:OPT refid='SWsg' name='sg'.
.ix 'stack growing'
.ix 'options' 'sg'
This option is useful for 32-bit OS/2 and Win32 multi-threaded
applications.
It requests the code generator to emit a run-time call at the start of
any function that has more than 4K bytes of automatic variables
(variables located on the stack).
.np
.ix 'guard page'
.ix 'primary thread'
.ix 'threads' 'growing the stack'
.ix 'guard page'
Under 32-bit OS/2, the stack is grown automatically in 4K pages for
any threads,
other than the primary thread, using the stack "guard page" mechanism.
The stack consists of in-use committed pages topped off with a special
guard page.
A memory reference into the 4K guard page causes the operating system
to grow the stack by one 4K page and to add a new 4K guard page.
This works fine when there is less than 4K of automatic variables in a
function.
When there is more than 4K of automatic data, the stack must be grown
in an orderly fashion, 4K bytes at a time, until the stack has grown
sufficiently to accommodate all the automatic variable storage
requirements.
Hence the requirement for a stack-growing run-time routine.
.ix '__GRO' 'stack growing'
The stack-growing run-time routine is called
.id __GRO.
.np
The "stack=" linker option specifies how much stack is available and
committed for the primary thread when an executable starts.
The stack size parameter to
.mono _beginthread()
specifies how much stack is available for a child thread.
The child thread starts with just 4k of stack committed.
The stack will not grow to be bigger than the size specified by the
stack size parameter.
.np
Under 32-bit Windows (Win32), the stack is grown automatically in 4K
pages for all threads using a similar stack "guard page" mechanism.
The stack consists of in-use committed pages topped off with a special
guard page.
The techniques for growing the stack in an orderly fashion are the
same as that described above for OS/2.
.np
The "stack=" linker option specifies how much stack is available for
the primary thread when an executable starts.
The "commit stack=" linker directive specifies how much of that stack
is committed when the executable starts.
If no "commit stack=" directive is used, it defaults to the same value
as the stack size.
The stack size parameter to
.mono _beginthread()
specifies how much stack is committed for a child thread.
If the size is set to zero, the size of the primary thread stack is
used for the child thread stack.
When the child thread executes, the stack space is not otherwise
restricted.
.np
.ix 'macros' '__SW_SG'
The macro
.kwm __SW_SG
will be predefined if "sg" is selected.
.do end
.*
.if &e'&$SWst eq 1 .do begin
:OPT refid='SWst' name='st'.
.ix 'stack touching'
.ix 'options' 'st'
This option causes the code generator to ensure that the first reference
to the stack in a function is to the stack "bottom" using the SS register.
If the memory for this part of the stack is not mapped to the task, a
memory fault will occur involving the SS register.
This permits an operating system
to allocate additional stack space to the faulting task.
.np
Suppose that a function requires 100 bytes of stack space.
The code generator usually emits an instruction sequence to reduce the
stack pointer by the required number of bytes of stack space, thereby
establishing a new stack bottom.
When the "st" option is specified, the code generator will ensure
that the first reference to the stack is to a memory location
with the lowest address.
If a memory fault occurs, the operating system can determine that it
was a stack reference (since the SS register is involved) and also
how much additional stack space is required.
.np
See the description of the "sg" option for a more general solution to
the stack allocation problem.
.ix 'macros' '__SW_ST'
The macro
.kwm __SW_ST
will be predefined if "st" is selected.
.do end
.*
.if &e'&$SWzw eq 1 .do begin
:OPT refid='SWzw' name='zw'.
.ix 'options' 'zw'
&286only.
This option causes the compiler to generate the prologue/epilogue code
sequences required for Microsoft Windows applications.
.ix '_export functions'
.ix 'pascal functions'
The following "fat" prologue/epilogue sequence is generated for any
functions declared to be "far _export" or "far pascal".
.millust begin
far pascal func(...)
far _export func(...)
far _export pascal func(...)

    push DS
    pop  AX
    nop
    inc  BP
    push BP
    mov  BP,SP
    push DS
    mov  DS,AX
    .
    .
    .
    pop DS
    pop BP
    dec BP
    retf n
.millust end
.np
.ix 'macros' '__WINDOWS__'
.ix '__WINDOWS__'
The macro
.kwm __WINDOWS__
will be predefined if "zw" is selected.
.np
&386only.
This option causes the compiler to generate any special code sequences
required for 32-bit Microsoft Windows applications.
.ix 'macros' '__WINDOWS__'
.ix 'macros' '__WINDOWS_386__'
The macro
.kwm __WINDOWS__
and
.kwm __WINDOWS_386__
will be predefined if "zw" is selected.
.if '&target' eq 'QNX' .do begin
.np
This option does not apply to QNX.
.do end
.do end
.*
.if &e'&$SWzW86 eq 1 .do begin
:OPT refid='SWzW86' name='zW (optimized)'.&optdag.
.ix 'options' 'zW'
&286only.
This option is similar to "zw" but causes the compiler to generate
more efficient prologue/epilogue code sequences in some cases.
This option may be used for Microsoft Windows applications code other
than user callback functions.
.ix '_export functions'
Any functions declared as "far _export" will be compiled with the
"fat" prologue/epilogue code sequence described under the "zw"
option.
.millust begin
far _export func(...)
far _export pascal func(...)
.millust end
.np
The following "skinny" prologue/epilogue sequence is generated for
functions that are not declared to be "far _export".
.ix 'pascal functions'
.millust begin
far pascal func(...)
far func(...)

    inc  BP
    push BP
    mov  BP,SP
    .
    .
    .
    pop BP
    dec BP
    retf n
.millust end
.np
.ix 'macros' '__WINDOWS__'
The macro
.kwm __WINDOWS__
will be predefined if "zW" is selected.
.if '&target' eq 'QNX' .do begin
.np
This option does not apply to QNX.
.do end
.do end
.*
.if &e'&$SWzWs eq 1 .do begin
:OPT refid='SWzWs' name='zWs'.&optdag
.ix 'options' 'zWs'
&286only.
This option is similar to "zW" but causes the compiler to generate
"smart callbacks".
This option may be used for Microsoft Windows user callback functions
in executables only.
It is not permitted for DLLs.
Normally, a callback function cannot be called directly.
You must use MakeProcInstance to obtain a function pointer with which
to call the callback function.
.np
If you specify "zWs" then you do not need to use MakeProcInstance in
order to call your own callback functions.
.ix '_export functions'
Any functions declared as "far _export" will be compiled with the
"smart" prologue code sequence described here.
.np
The following example shows the usual prologue code sequence that is
generated when the "zWs" option is NOT used.
.exam begin 14
:SF font=1.compiler_name:eSF. winapp &sw.mc &sw.bt=windows &sw.d1

short FAR PASCAL __export Function1( short var1,
                                     long varlong,
                                     short var2 )
{
 0000  1e            FUNCTION1       push    ds
 0001  58                            pop     ax
 0002  90                            nop
 0003  45                            inc     bp
 0004  55                            push    bp
 0005  89 e5                         mov     bp,sp
 0007  1e                            push    ds
 0008  8e d8                         mov     ds,ax
.exam end
.np
The following example shows the "smart" prologue code sequence that is
generated when the "zWs" option is used.
The assumption here is that the SS register contains the address of
DGROUP.
.exam begin 12
:SF font=1.compiler_name:eSF. winapp &sw.mc &sw.bt=windows &sw.d1 &sw.zWs

short FAR PASCAL __export Function1( short var1,
                                     long varlong,
                                     short var2 )
{
 0000  8c d0         FUNCTION1       mov     ax,ss
 0002  45                            inc     bp
 0003  55                            push    bp
 0004  89 e5                         mov     bp,sp
 0006  1e                            push    ds
 0007  8e d8                         mov     ds,ax
.exam end
.do end
.*
:eOPTLIST.
.*
.section Debugging/Profiling
.*
.np
This group of options deals with all the forms of debugging
information that can be included in an object file.
Support for profiling of Pentium code is also described.
.*
:OPTLIST.
.*
.if &e'&$SWd0 eq 1 .do begin
:OPT refid='SWd0' name='d0'.
.ix 'options' 'd0'
(C++ only)
No debugging information is included in the object file.
.do end
.*
.if &e'&$SWd1 eq 1 .do begin
:OPT refid='SWd1' name='d1'.
.ix 'options' 'd1'
Line number debugging information is included in the object file.
This option provides additional information to the &dbgname (at the
expense of larger object files and executable files).
Line numbers are handy when debugging your application with the
&dbgname at the source code level.
Code speed is not affected by this option.
To avoid recompiling, the &stripname can be used to remove debugging
information from the executable image.
.do end
.*
.if &e'&$SWd1pls eq 1 .do begin
:OPT refid='SWd1pls' name='d1+'.
.ix 'options' 'd1+'
(C only)
Line number debugging information plus typing information for global
symbols and local structs and arrays is included in the object file.
Although global symbol information can be made available to the
&dbgname through a &lnkname option, typing information for global
symbols and local structs and arrays must be requested when the source
file is compiled.
This option provides additional information to the &dbgname (at the
expense of larger object files and executable files).
Code speed is not affected by this option.
To avoid recompiling, the &stripname can be used to remove debugging
information from the executable image.
.do end
.*
.if &e'&$SWd2 eq 1 .do begin
:OPT refid='SWd2' name='d2'.
.ix 'options' 'd2'
In addition to line number information, local symbol and data
type information is included in the object file.
Although global symbol information can be made available to the
&dbgname through a &lnkname option, local symbol and typing
information must be requested when the source file is compiled.
This option provides additional information to the &dbgname (at the
expense of larger object files and executable files).
.np
By default, the compiler will select the "od" level of optimization if
"d2" is specified (see the description of the "od" option).
Starting with version 11.0, the compiler now expands functions in-line
where appropriate.
This means that symbolic information for the in-lined function will
not be available.
.np
The use of this option will make the debugging chore somewhat easier
at the expense of code speed and size.
To create production code, you should recompile without this option.
.do end
.*
.if &e'&$SWd2i eq 1 .do begin
:OPT refid='SWd2i' name='d2i'.
.ix 'options' 'd2i'
(C++ only)
This option is identical to "d2" but does not permit in-lining of
functions.
Functions are emitted as external out-of-line functions.
This option can result in larger object and/or executable files than
with "d2" (we are discussing both "code" and "file" size here).
.do end
.*
.if &e'&$SWd2s eq 1 .do begin
:OPT refid='SWd2s' name='d2s'.
.ix 'options' 'd2s'
(C++ only)
This option is identical to "d2" but does not permit in-lining of
functions.
Functions are emitted as static out-of-line functions.
This option can result in larger object and/or executable files than
with "d2" or "d2i" (we are discussing both "code" and "file" size
here).
Link times are faster than "d2i" (fewer segment relocations) but
executables are slightly larger.
.do end
.*
.if &e'&$SWd2t eq 1 .do begin
:OPT refid='SWd2t' name='d2t'.
.ix 'options' 'd2t'
(C++ only)
This option is identical to "d2" but does not include type name
debugging information.
This option can result in smaller object and/or executable files (we
are discussing "file" size here).
.do end
.*
.if &e'&$SWd3 eq 1 .do begin
:OPT refid='SWd3' name='d3'.
.ix 'options' 'd3'
This option is identical to "d2" but also includes symbolic debugging
information for unreferenced type names.
Note that this can result in very large object and/or executable files
when header files like
.fi WINDOWS.H
or
.fi OS2.H
are included.
.do end
.*
.if &e'&$SWd3i eq 1 .do begin
:OPT refid='SWd3i' name='d3i'.
.ix 'options' 'd3i'
(C++ only)
This option is identical to "d3" but does not permit in-lining of
functions.
Functions are emitted as external out-of-line functions.
This option can result in larger object and/or executable files than
with "d3" (we are discussing both "code" and "file" size here).
.do end
.*
.if &e'&$SWd3s eq 1 .do begin
:OPT refid='SWd3s' name='d3s'.
.ix 'options' 'd3s'
(C++ only)
This option is identical to "d3" but does not permit in-lining of
functions.
Functions are emitted as static out-of-line functions.
This option can result in larger object and/or executable files than
with "d3" or "d3i" (we are discussing both "code" and "file" size
here).
Link times are faster than "d3i" (fewer segment relocations) but
executables are slightly larger.
.do end
.*
.if &e'&$SWee eq 1 .do begin
:OPT refid='SWee' name='ee'.
.ix 'options' 'ee'
&x86only.
This option causes the compiler to generate a call to
.kwm __EPI
in the epilogue sequence at the end of every function.
This user-written routine can be used to collect/record profiling
information.
.if &e'&dohelp eq 0 .do begin
Other related options are :HDREF refid='SWep'. and
:HDREF refid='SWen'..
.do end
.ix 'macros' '__SW_EE'
The macro
.kwm __SW_EE
will be predefined if "ee" is selected.
.do end
.*
.if &e'&$SWen eq 1 .do begin
:OPT refid='SWen' name='en'.
.ix 'options' 'en'
&x86only.
The compiler will emit the function name into the object code as a
string of characters just before the function prologue sequence is
generated.
The string is terminated by a byte count of the number of characters
in the string.
.millust begin
    ; void Toaster( int arg )

            db      "Toaster", 7
    public  Toaster
    Toaster label   byte
            .
            .
            .
            ret
.millust end
.pc
This option is intended for developers of embedded systems (ROM-based
applications).
It may also be used in conjunction with the "ep" option
for special user-written profiling applications.
.ix 'macros' '__SW_EN'
The macro
.kwm __SW_EN
will be predefined if "en" is selected.
.do end
.*
.if &e'&$SWep eq 1 .do begin
:OPT refid='SWep' name='ep'.[<number>]
.ix 'options' 'ep'
&x86only.
This option causes the compiler to generate a call to a user-written
.kwm __PRO
routine in the prologue sequence at the start of every function.
This routine can be used to collect/record profiling information.
The optional argument
.sy <number>
can be used to cause the compiler to allocate that many bytes on the
stack as a place for
.kwm __PRO
to store information.
.if &e'&dohelp eq 0 .do begin
Other related options are :HDREF refid='SWee'. and
:HDREF refid='SWen'..
.do end
.ix 'macros' '__SW_EP'
The macro
.kwm __SW_EP
will be predefined if "ep" is selected.
.do end
.*
.if &e'&$SWet eq 1 .do begin
:OPT refid='SWet' name='et'.
.ix 'options' 'et'
(Pentium only)
This option causes the compiler to generate code into the prolog of
each function to count exactly how much time is spent within that
function, in clock ticks.
This option is valid only for Pentium compatible processors (i.e., the
instructions inserted into the code do not work on 486 or earlier
architectures).
The Pentium "rdtsc" opcode is used to obtain the instruction cycle
count.
.np
At the end of the execution of the program, a file will be written to
the same location as the executable, except with a "&prf" extension.
The contents of the file will look like this:
.exam begin
          1903894223          1  main
          1785232334    1376153  StageA
          1882249150      13293  StageB
          1830895850       2380  StageC
           225730118         99  StageD
.exam end
.np
The first column is the total number of clock ticks spent inside of
the function during the execution of the program,
the second column is the number of times it was called
and the third column is the individual function name.
The total number of clock ticks includes time spent within functions
called from this function.
.np
The overhead of the profiling can be somewhat intrusive, especially
for small leaf functions (i.e., it may skew your results somewhat).
.if '&target' eq 'QNX' .do begin
.np
Some versions of QNX emulate the "rdtsc" opcode on earlier processors
by catching the invalid opcode fault and fishing the timer.
Thus, the "et" option may work on other processors but will be a lot
less accurate on non-Pentium processors.
.do end
.do end
.*
.if &e'&$SWhwdc eq 1 .do begin
:OPT refid='SWhwdc' name='h'.{w,d,c}
.ix 'debugging information format'
.ix 'options' 'hw'
.ix 'options' 'hd'
.ix 'options' 'hc'
The type of debugging information that is to be included in the object
file is one of "&company", "DWARF" or "Codeview".
The default is "DWARF".
.np
If you wish to use the Microsoft Codeview debugger, then choose the
"hc" option (this option causes Codeview Level 4 information to be
generated).
.ix 'CVPACK'
.ix 'Debugging Information Compactor'
It will be necessary to run the Microsoft Debugging Information
Compactor, CVPACK, on the executable once the linker has created it.
For information on requesting the linker to automatically run CVPACK,
see the section entitled "OPTION CVPACK" in the
.if '&target' eq 'QNX' .do begin
part of this guide that describes the &lnkname..
.do end
.el .do begin
.book &lnkname User's Guide.
.do end
Alternatively, you can run CVPACK from the command line.
.np
When linking the application, you must also choose the appropriate
&lnkname DEBUG directive.
.if '&target' eq 'QNX' .do begin
See the part of this guide that describes the &lnkname
for more information.
.do end
.el .do begin
See the
.book &lnkname User's Guide
for more information.
.do end
.do end
.*
.if &e'&$SWs eq 1 .do begin
:OPT refid='SWs' name='s'.
.ix 'stack overflow'
.ix 'options' 's'
Stack overflow checking is omitted from the generated code.
By default, the compiler will emit code at the beginning of every
function that checks for the "stack overflow" condition.
This option can be used to disable this feature.
.ix 'macros' '__SW_S'
The macro
.kwm __SW_S
will be predefined if "s" is selected.
.do end
.*
.if &e'&$SWsi eq 1 .do begin
:OPT refid='SWsi' name='si'.
.ix 'stack growing'
.ix 'options' 'si'
&AXPonly.
This option can be used to initialize stack frame storage with a
pattern.
This option is useful for debugging an application since it sets up
the stack frame with a predictable pattern.
.np
.ix 'macros' '__SW_SI'
The macro
.kwm __SW_SI
will be predefined if "si" is selected.
.do end
.*
:eOPTLIST.
.*
.section Preprocessor
.*
.np
This group of options deals with the compiler preprocessor.
.*
:OPTLIST.
.*
.if &e'&$SWd eq 1 .do begin
:OPT refid='SWd' name='d'.<name>[=text]
.ix 'options' 'd'
This option can be used to define a preprocessor macro from the
command line.
If
.us =text
is not specified, then 1 is assumed.
In other words, specifying
.mono &sw.dDBGON
is equivalent to specifying
.mono &sw.dDBGON=1
on the command line.
.np
If
.us =text
is specified, then this option is equivalent to including the
following line in your source code.
.millust begin
#define name text
.millust end
.np
Consider the following example.
.exam begin 1
d_MODDATE="87.05.04"
.exam end
.pc
The above example is equivalent to a line in the source file
containing:
.millust begin
#define _MODDATE "87.05.04"
.millust end
.do end
.*
.if &e'&$SWdpls eq 1 .do begin
:OPT refid='SWdpls' name='d+'.
.ix 'options' 'd+'
The syntax of any "d" option which follows on the command line is
extended to include &lang tokens as part of "text".
The token string is terminated by a space character.
This permits more complex syntax than is normally allowed.
.exam begin 1
&sw.d+ &sw.d_radx=x*3.1415926/180
.exam end
.np
This is equivalent to specifying the following in the source code.
.exam begin 1
#define _radx x*3.1415926/180
.exam end
.np
&cmppname extends this feature by allowing parameterized macros.
When a parameter list is specified, the "=" character must not be
specified.
It also permits immediate definition of the macro as shown in the
second line of the example.
.exam begin 2
&sw.d+ &sw.d_rad(x)x*3.1415926/180
&sw.d+_rad(x)x*3.1415926/180
.exam end
.np
This is equivalent to specifying the following in the source code.
.exam begin 1
#define _rad(x) x*3.1415926/180
.exam end
.do end
.*
.if &e'&$SWfo eq 1 .do begin
:OPT refid='SWfo' name='fo'.[=<file_name>] (preprocessor)
.ix 'options' 'fo'
.ix 'preprocessor'
The "fo" option is used with any form of the "p" (preprocessor) option
to name the output file &drive, path, file name and extension.
If the output file name is not specified, it is constructed from the
source file name.
If the output file extension is not specified, it is "&pxt" by
default.
.exam begin 1
&prompt.:SF font=1.compiler_name:eSF. report &sw.p &sw.fo=&dr4.&pc.proj&pc.prep&pc
.exam end
.pc
A trailing "&pc" must be specified for directory names.
If, for example, the option was specified as
.mono fo=&dr4&pc.proj&pc.prep
then the output file would be called
.fi &dr4.&pc.proj&pc.prep.i
.ct ~.
A default filename extension must be preceded by a period (".").
.exam begin 1
&prompt.:SF font=1.compiler_name:eSF. report &sw.p &sw.fo=&dr4.&pc.proj&pc.prep&pc..cpr
.exam end
.do end
.if &e'&$SWpil eq 1 .do begin
:OPT refid='SWpil' name='pil'.
.ix 'options' 'pil'
By default, #line directives embedded in source files are processed and
will be used as a basis for file name and line number information in error
messages, __FILE__ and __LINE__ symbols, etc. The "pil" option causes the
preprocessor to ignore #line directives and refer to actual file names and
line numbers.
.do end
.*
.if &e'&$SWp eq 1 .do begin
:OPT refid='SWp' name='p'.{e,l,c,w=<num>}
.ix 'options' 'p'
.ix 'options' 'pc'
.ix 'options' 'pe'
.ix 'options' 'pl'
.ix 'options' 'pw'
.ix 'preprocessor'
The input file is preprocessed and, by default, is written to the
standard output file.
The "fo" option may be used to redirect the output to a file with
default extension "&pxt".
.np
.ix 'preprocessor' 'source comments'
Specify "pc" if you wish to include the original source comments in
the &product preprocessor output file.
.np
.ix 'preprocessor' 'encryption'
(C++ Only) Specify "pe" if you wish to encrypt the original
identifiers when they are written to the &product preprocessor output
file.
.np
.ix 'line directive'
.ix '#line'
.ix 'preprocessor' '#line directives'
Specify "pl" if you wish to include
.id #line
directives.
.np
Specify "pcl" or "plc" if you wish both source comments and #line
directives.
.np
Use the "w=<num>" suffix if you wish to wish output lines to wrap at
<num> columns.
Zero means no wrap.
.exam begin 1
&prompt.:SF font=1.compiler_name:eSF. report &sw.pcelw=80
.exam end
.np
The input file is preprocessed only.
When only "p" is specified, source comments and #line directives are
not included.
You must request these using the "c" and "l" suffixes.
When the output of the preprocessor is fed into the compiler,
the
.id #line
directive enables the compiler to issue diagnostics in terms of line
numbers of the original source file.
.np
The options which are supported when the &product preprocessor is
requested are: "d", "fi", "fo", "i", "m?", and "u".
.do end
.*
.if &e'&$SWu eq 1 .do begin
:OPT refid='SWu' name='u'.<name>
.ix 'options' 'u'
The "u" option may be used to turn off the definition of a predefined
macro.
If no name is specified then all predefined macros
:CMT. with the exception of the memory model macros
are undefined.
:CMT. ????????????????????????
:CMT. ??                 ?????
:CMT. ?? This needs work ?????
:CMT. ??                 ?????
:CMT. ????????????????????????
.exam begin 1
&prompt.:SF font=1.compiler_name:eSF. report &sw.uM_I386
.exam end
.do end
.*
:eOPTLIST.
.*
.section Diagnostics
.*
.np
This group of options deals with the control of compiler diagnostics.
.*
:OPTLIST.
.*
.if &e'&$SWe eq 1 .do begin
:OPT refid='SWe' name='e'.<number>
.ix 'options' 'e'
The compiler will stop compilation after reaching
.sy <number>
errors.
By default, the compiler will stop compilation after 20 errors.
.do end
.*
.if &e'&$SWef eq 1 .do begin
:OPT refid='SWef' name='ef'.
.ix 'options' 'ef'
This option causes the compiler to display full path names for files
in error messages.
.do end
.*
.if &e'&$SWeq eq 1 .do begin
:OPT refid='SWeq' name='eq'.
.ix 'options' 'eq'
This option causes the compiler to not display error messages on the
console; however, they are still written to a file
:optref refid='SWfr'..
.do end
.*
.if &e'&$SWer eq 1 .do begin
:OPT refid='SWer' name='er'.
.ix 'options' 'er'
(C++ only)
This option causes the C++ compiler to not recover from undefined
symbol errors.
By default, the compiler recovers from "undefined symbol" errors by
injecting a special entry into the symbol table that prevents further
issuance of diagnostics relating to the use of the same name.
Specify the "er" option if you want all uses of the symbol to be
diagnosed.
.exam begin
struct S {
};

void foo( S *p ) {
    p->m = 1; // member 'm' has not been declared in 'S'

}
void bar( S *p ) {
    p->m = 2; // no error unless "er" is specified
}
.exam end
.do end
.*
.*
.if &e'&$SWew eq 1 .do begin
:OPT refid='SWew' name='ew'.
.ix 'options' 'ew'
(C++ only)
This option causes the C++ compiler to generate equivalent but less
verbose diagnostic messages.
.do end
.*
.if &e'&$SWq eq 1 .do begin
:OPT refid='SWq' name='q'.
.ix 'options' 'q'
This option is equivalent to the "zq" option :optref refid='SWzq'..
.do end
.*
.if &e'&$SWt eq 1 .do begin
:OPT refid='SWt' name='t'.=<num>
.ix 'options' 't'
.ix 'preprocessor'
(C++ only)
The "t" option is used to set the tab stop interval.
By default, the compiler assumes a tab stop occurs at multiples
of 8 (1+n x 8 = 1, 9, 17, ... for n=0, 1, 2, ...).
When the compiler reports a line number and column number in a
diagnostic message, the column number has been adjusted for
intervening tabs.
If the default tab stop setting for your text editor is not a multiple
of 8, then you should use this option.
.exam begin 1
&prompt.:SF font=1.compiler_name:eSF. report &sw.t=4
.exam end
.do end
.*
.if &e'&$SWw eq 1 .do begin
:OPT refid='SWw' name='w'.<number>
.ix 'options' 'w'
The compiler will issue only warning type messages of severity
.sy <number>
or below.
Type 1 warning messages are the most severe while type 3 warning
messages are the least severe.
Specify "w0" to prevent warning messages from being issued.
Specify "wx" to obtain all warning messages.
.do end
.*
.if &e'&$SWwcd eq 1 .do begin
:OPT refid='SWwcd' name='wcd'.=<number>
.ix 'options' 'wcd'
The compiler will not issue the warning message indicated by
.sy <number>.
.do end
.*
.if &e'&$SWwce eq 1 .do begin
:OPT refid='SWwce' name='wce'.=<number>
.ix 'options' 'wce'
The compiler will issue the warning message indicated by
.sy <number>
despite any pragmas that may have disabled it.
.do end
.*
.if &e'&$SWwe eq 1 .do begin
:OPT refid='SWwe' name='we'.
.ix 'options' 'we'
By default, the compiler will continue to create an object file when
there are warnings produced.
This option can be used to treat all warnings as errors, thereby
preventing the compiler from creating an object file if there are
warnings found within a module.
.do end
.*
.if &e'&$SWwo eq 1 .do begin
:OPT refid='SWwo' name='wo'.
.ix 'options' 'wo'
(C only)
&286only.
This option tells the compiler to emit warnings for things that will
cause problems when compiling code for use in overlays.
.do end
.*
.if &e'&$SWwx eq 1 .do begin
:OPT refid='SWwx' name='wx'.
.ix 'options' 'wx'
This option sets the warning level to its maximum setting.
.do end
.*
.if &e'&$SWza eq 1 .do begin
:OPT refid='SWza' name='za'.
.ix 'options' 'za'
.ix 'ISO/ANSI compatibility'
This option helps to ensure that the module to be compiled conforms to
the ISO/ANSI C or C++ programming language specification (depending on the
compiler which is selected).
.ix 'macros' 'NO_EXT_KEYS'
The macro
.kwm NO_EXT_KEYS
(no extended keywords)
will be predefined if "za" is selected.
The "ou" option will be enabled :optref refid='SWou'..
See also the description of the "ze" option.
.np
When using the C compiler, there is an exception to the enforcement of
the ISO C standard programming language specification.
The use of C++ style comments (// comment) are not diagnosed.
.do end
.*
.if &e'&$SWze eq 1 .do begin
:OPT refid='SWze' name='ze'.
.ix 'options' 'ze'
The "ze" option (default) enables the use of the following compiler
extensions:
.autonote
.note
The requirement for at least one external definition per module is
relaxed.
.if '&target' eq 'QNX' .do begin
.note
The escape sequence "\l" is interpreted as a synonym for "\n" in
strings and character constants.
.do end
.note
When using the C compiler, some forgiveable pointer type mismatches
become warnings instead of errors.
.note
In-line math functions are allowed (note that
.us errno
will not be set by in-line functions).
.note
When using the C compiler, anonymous structs/unions are allowed
(this is always permitted in C++).
.exam begin 8
struct {
    int a;
    union {
        int   b;
        float alt_b;
    };
    int c;
} x;
.exam end
.pc
In the above example, "x.b" is a valid reference to the "b" field.
.note
For C only, ISO function prototype scope rules are relaxed to allow
the following program to compile without any errors.
.exam begin 12
void foo( struct a *__p );

struct a {
    int b;
    int c;
};

void bar( void )
{
    struct a x;
    foo( &x );
}
.exam end
.pc
According to a strict interpretation of the ISO C standard, the
function prototype introduces a new scope which is terminated at the
semicolon (;).
The effect of this is that the structure tag "a" in the function "foo"
is not the same structure tag "a" defined after the prototype.
A diagnostic must be issued for a conforming ISO C implementation.
.note
A trailing comma (,) is allowed after the last constant in an enum
declaration.
.exam begin 1
enum colour { RED, GREEN, BLUE, };
.exam end
.note
The ISO requirement that all enums have a base type of
.us int
is relaxed.
The motivation for this extension is conservation of storage.
Many enums can be represented by integral types that are smaller
in size than an
.us int.
.exam begin 8
enum colour { RED, GREEN, BLUE, };

void foo( void )
{
    enum colour x;

    x = RED;
}
.exam end
.pc
In the example, "x" can be stored in an
.us unsigned char
because its values span the range 0 to 2.
.note
The ISO requirement that the base type of a bitfield be
.us int
or
.us unsigned
is relaxed.
This allows a programmer to allocate bitfields from smaller units of
storage than an
.us int
(e.g.,
.us unsigned char
.ct ).
.exam begin 11
struct {
    unsigned char a : 1;
    unsigned char b : 1;
    unsigned char c : 1;
} x;

struct {
    unsigned a : 1;
    unsigned b : 1;
    unsigned c : 1;
} y;
.exam end
.pc
In the above example, the size of "x" is the same size as an
.us unsigned char
whereas the size of "y" is the same size as an
.us unsigned int.
.note
The following macros are defined.
.ix '_near macro'
.ix 'near macro'
.ix '_far macro'
.ix 'far macro'
.ix 'SOMDLINK macro'
.ix '_huge macro'
.ix 'huge macro'
.ix '_based macro'
.ix '_segment macro'
.ix '_self macro'
.ix '_cdecl macro'
.ix 'cdecl macro'
.ix 'SOMLINK macro'
.ix '_pascal macro'
.ix 'pascal macro'
.ix '_fastcall macro'
.ix '_fortran macro'
.ix 'fortran macro'
.ix '_inline macro'
.ix '_interrupt macro'
.ix 'interrupt macro'
.ix '_export macro'
.ix '_loadds macro'
.ix '_saveregs macro'
.ix '_stdcall macro'
.ix '_syscall macro'
.ix '_far16 macro'
.ix 'macros' '_near'
.ix 'macros' 'near'
.ix 'macros' '_far'
.ix 'macros' 'far'
.ix 'macros' 'SOMDLINK'
.ix 'macros' '_huge'
.ix 'macros' 'huge'
.ix 'macros' '_based'
.ix 'macros' '_segment'
.ix 'macros' '_self'
.ix 'macros' '_cdecl'
.ix 'macros' 'cdecl'
.ix 'macros' 'SOMLINK'
.ix 'macros' '_pascal'
.ix 'macros' 'pascal'
.ix 'macros' '_fastcall'
.ix 'macros' '_fortran'
.ix 'macros' 'fortran'
.ix 'macros' '_inline'
.ix 'macros' '_interrupt'
.ix 'macros' 'interrupt'
.ix 'macros' '_export'
.ix 'macros' '_loadds'
.ix 'macros' '_saveregs'
.ix 'macros' '_stdcall'
.ix 'macros' '_syscall'
.ix 'macros' '_far16'
.millust begin
_near, near
_far, far, SOMDLINK (16-bit)
_huge, huge
_based
_segment
_segname
_self
_cdecl, cdecl, SOMLINK (16-bit)
_pascal, pascal
_fastcall
_fortran, fortran
_inline
_interrupt, interrupt
_export
_loadds
_saveregs
_stdcall
_syscall, SOMLINK (32-bit), SOMDLINK (32-bit)
_far16
.millust end
.endnote
.np
See also the description of the "za" option.
.*
:OPT refid='SWzq' name='zq'.
.ix 'options' 'zq'
The "quiet mode" option causes the informational messages displayed by
the compiler to be suppressed.
Normally, messages are displayed identifying the compiler and
summarizing the number of lines compiled.
As well, a dot is displayed every few seconds while the code
generator is active, to indicate that the compiler is still working.
These messages are all suppressed by the "quiet mode" option.
Error and warning messages are not suppressed.
.do end
.*
.if &e'&$SWzs eq 1 .do begin
:OPT refid='SWzs' name='zs'.
.ix 'options' 'zs'
The compiler will check the source code only and omit the generation
of object code.
Syntax checking, type checking, and so on are performed as
usual.
.do end
.*
:eOPTLIST.
.*
.section Source/Output Control
.*
.np
This group of options deals with control over the input files and
output files that the compiler processes and/or creates.
.*
:OPTLIST.
.*
.if &e'&$SWad eq 1 .do begin
:OPT refid='SWad' name='ad[=<file_name>]'.
.ix 'options' 'ad'
This option enables generation of automatic dependency infomation in
makefile format, as a list of targets and their dependents.
If the name of the automatic dependency file is not specified, it is
constructed from the source file name.  If the dependency
extension is not specified, it is ".d" by default.
.exam begin 1
&prompt.:SF font=1.compiler_name:eSF. report &sw.ad=&dr4.&pc.proj&pc.obj&pc
.exam end
A trailing "&pc" must be specified for directory names.
If, for example, the option was specified as
.mono fo=&dr4.&pc.proj&pc.obj
then the dependency file will be called
.fi &dr4.&pc.proj&pc.obj.d
.ct ~.
.np
A default filename extension must be preceded by a period (".").
.exam begin 1
&prompt.:SF font=1.compiler_name:eSF. report &sw.ad=&dr4.&pc.proj&pc.obj&pc..dep
.exam end
The content of the generated file has the following format:
.exam begin 1
<targetname>:<input source file> <included header files...>
.exam end
Note that the header files listed in the dependency file normally do not
include the standard library headers.
.do end
.*
.if &e'&$SWadbs eq 1 .do begin
:OPT refid='SWadbs' name='adbs'.
.ix 'options' 'adbs'
When generating make style automatic dependency files, this option forces all
path separators ("/" or "\") to be a backslash ("\").  Certain usage may
result in mixed path separators; this options helps generating
automatic dependency information in a format appropriate for the
make tool used.
.do end
.*
.if &e'&$SWadd eq 1 .do begin
:OPT refid='SWadd' name='add[=<file_name>]'.
.ix 'options' 'add'
Set the first dependency name in a make style automatic dependency file.
By default, the name of the source file to be compiled is used.
:optref refid='SWad'.
.do end
.*
.if &e'&$SWadhp eq 1 .do begin
:OPT refid='SWadhp' name='adhp[=<path_name>]'.
.ix 'options' 'adhp'
When including a file with "" delimiters, the resulting filename in make
style automatic dependency files will have no path.  This option allows such
files to be given a path; said path may be relative.  The path_name
argument is directly prepended to the filename, therefore a trailing
path separator must be specified.
.np
This issue only affects headers found in the current directory, that is,
the directory current at the time of compilation.
If a header is located in an including file's directory, it will
automatically receive a path.
.np
This option is useful in situations where the current directory at the time
when the automatic dependency information is evaluated is not the same as
the current directory at the time of compilation (i.e., when the automatic
dependency information was generated).
.do end
.*
.if &e'&$SWadfs eq 1 .do begin
:OPT refid='SWadfs' name='adfs'.
.ix 'options' 'adfs'
When generating make style automatic dependency files, this option forces all
path separators ("/" or "\") to be a forward slash ("/"). Certain usage may
result in mixed path separators; this options helps generating
automatic dependency information in a format appropriate for the
make tool used.
.do end
.*
.if &e'&$SWadt eq 1 .do begin
:OPT refid='SWadt' name='adt[=<target_name>]'.
.ix 'options' 'adt'
This option enables generation of automatic dependency infomation in the
form of a makefile.
The target in the makefile can be specified.  If the automatic dependency
target is not specified through this option, it is constructed from the
source file name.
If the target extension is not specified, it is "&obj" by default.
.exam begin 1
&prompt.:SF font=1.compiler_name:eSF. report &sw.adt=&dr4.&pc.proj&pc.obj&pc
.exam end
A trailing "&pc" must be specified for directory names.
If, for example, the option was specified as
.mono fo=&dr4.&pc.proj&pc.obj
then the dependency file would be called
.fi &dr4.&pc.proj&pc.obj&obj.
.ct ~.
.np
A default filename extension must be preceded by a period (".").
.exam begin 1
&prompt.:SF font=1.compiler_name:eSF. report &sw.adt=&dr4.&pc.proj&pc.obj&pc..dep
.exam end
The generated file has the following contents:
.exam begin 1
<targetname>:<input source file> <included header files...>
.exam end
Note that the header files listed in the dependency file normally do not
include the standard library headers.
.do end
.*
.if &e'&$SWdb eq 1 .do begin
:OPT refid='SWdb' name='db'.
.ix 'options' 'db'
Use this option to generate browsing information.
The browsing information is recorded in a file whose name is
constructed from the source file name and the extension ".mbr".
.do end
.*
.if &e'&$SWez eq 1 .do begin
:OPT refid='SWez' name='ez'.&optdag.
.ix 'options' 'ez'
&386only.
The compiler will generate an object file in Phar Lap Easy OMF-386
(object module format) instead of the default Microsoft OMF.
.ix 'macros' '__SW_EZ'
The macro
.kwm __SW_EZ
will be predefined if "ez" is selected.
.do end
.*
.if &e'&$SWfc eq 1 .do begin
:OPT refid='SWfc' name='fc'.=<file_name>
.ix 'options' 'fc'
(C++ only)
The specified "batch" file contains a list of command lines to be
processed.
This enables the processing of a number of source files together with
options for each file with one single invocation of the compiler.
Only one "fc" option is allowed and no source file names are permitted
on the command line.
.exam begin 6
[batch.txt]
main        &sw.onatx &sw.zp4
part1 part2 &sw.onatx &sw.zp4 &sw.d1
part3       &sw.onatx &sw.zp4 &sw.d2

&prompt.:SF font=1.compiler_name:eSF. &sw.fc=&pathnam.&hdrdir.&pc.batch.txt
.exam end
.np
Each line in the file is treated stand-alone.
In other words, the options from one line do not carry over to another
line.
However, any options specified on the command line or the associated
compiler environment variable will carry over to the individual
command lines in the batch file.
When the compiler diagnoses errors in a source file, processing of
subsequent command lines is halted unless the "k" option was specified
:optref refid='SWk'..
.do end
.*
.if &e'&$SWfh eq 1 .do begin
:OPT refid='SWfh' name='fh'.[q][=<file_name>]
.ix 'options' 'fh'
.ix 'options' 'fhq'
The compiler will generate/use a precompiled header for the first
header file referenced by
.id #include
in the source file.
See the chapter entitled :HDREF refid='cphdr'. for more information.
.do end
.*
.if &e'&$SWfhd eq 1 .do begin
:OPT refid='SWfhd' name='fhd'.
.ix 'options' 'fhd'
The compiler will store debug info for the pre-compiled header once
(DWARF only).
See the chapter entitled :HDREF refid='cphdr'. for more information.
.do end
.*
.if &e'&$SWfhr eq 1 .do begin
:OPT refid='SWfhr' name='fhr'.
.ix 'options' 'fhr'
(C++ only)
This option will force the compiler to read the pre-compiled header
if it appears to be up-to-date; otherwise, it will read the header
files included by the source code.
It will never write the pre-compiled header (even when it is
out-of-date).
See the chapter entitled :HDREF refid='cphdr'. for more information.
.do end
.*
.if &e'&$SWfhw eq 1 .do begin
:OPT refid='SWfhw' name='fhw'.
.ix 'options' 'fhw'
(C++ only)
This option will force the compiler to write the pre-compiled header
(even when it appears to be up-to-date).
See the chapter entitled :HDREF refid='cphdr'. for more information.
.do end
.*
.if &e'&$SWfhwe eq 1 .do begin
:OPT refid='SWfhwe' name='fhwe'.
.ix 'options' 'fhwe'
(C++ only)
This option will ensure that pre-compiled header warnings are not
counted as errors when the "we" (treat warnings as errors) option is
specified.
.do end
.*
.if &e'&$SWfi eq 1 .do begin
:OPT refid='SWfi' name='fi'.=<file_name>
.ix 'options' 'fi'
The specified file is included as if a
.millust begin
#include "<file_name>"
.millust end
.pc
directive were placed at the start of the source file.
.exam begin 1
&prompt.:SF font=1.compiler_name:eSF. report &sw.fi=&pathnam.&hdrdir.&pc.stdarg.h
.exam end
.do end
.*
.if &e'&$SWfo2 eq 1 .do begin
:OPT refid='SWfo2' name='fo'.[=<file_name>]
.ix 'options' 'fo'
When generating an object file, the "fo" option may be used to name
the object file &drive, path, file name and extension.
If the object file name is not specified, it is constructed from the
source file name.
If the object file extension is not specified, it is "&obj" by
default.
.exam begin 1
&prompt.:SF font=1.compiler_name:eSF. report &sw.fo=&dr4.&pc.proj&pc.obj&pc
.exam end
.pc
A trailing "&pc" must be specified for directory names.
If, for example, the option was specified as
.mono fo=&dr4.&pc.proj&pc.obj
then the object file would be called
.fi &dr4.&pc.proj&pc.obj&obj.
.ct ~.
.np
A default filename extension must be preceded by a period (".").
.exam begin 1
&prompt.:SF font=1.compiler_name:eSF. report &sw.fo=&dr4.&pc.proj&pc.obj&pc..dbo
.exam end
.do end
.*
.if &e'&$SWfr eq 1 .do begin
:OPT refid='SWfr' name='fr'.[=<file_name>]
.ix 'options' 'fr'
.ix 'error file'
The "fr" option is used to name the error file &drive, path, file name
and extension.
If the error file name is not specified, it is constructed from the
source file name.
If the output file extension is not specified, it is "&err" by
default.
.ix 'error file' 'disabling'
.ix 'disabling error file'
If no part of the name is specified, then no error file is produced
(i.e., &sw.fr disables production of an error file).
.exam begin 1
&prompt.:SF font=1.compiler_name:eSF. report &sw.fr=&dr4.&pc.proj&pc.errs&pc
.exam end
.pc
A trailing "&pc" must be specified for directory names.
If, for example, the option was specified as
.mono fr=&dr4&pc.proj&pc.errs
then the output file would be called
.fi &dr4.&pc.proj&pc.errs&err
.ct ~.
A default filename extension must be preceded by a period (".").
.exam begin 1
&prompt.:SF font=1.compiler_name:eSF. report &sw.fr=&dr4.&pc.proj&pc.errs&pc..erf
.exam end
.do end
.*
.if &e'&$SWft eq 1 .do begin
:OPT refid='SWft' name='ft'.
.ix 'options' 'ft'
(C++ only)
If the compiler cannot open a header file whose file name is longer
than 8 letters or whose file extension is longer than 3 letters,
it will truncate the name at 8 letters and the extension at 3 letters
and try to open a file with the shortened name.
This is the default behaviour for the compiler.
.np
For example, if the compiler cannot open the header file called
.fi strstream.h,
it will attempt to open a header file called
.fi strstrea.h.
.do end
.*
.if &e'&$SWfti eq 1 .do begin
:OPT refid='SWfti' name='fti'.
.ix 'options' 'fti'
(C only)
Whenever a file is open as a result of 
.id #include
directive processing, an informational message is printed. The message
contains the file name and line number identifying where the
.id #include
directive was located.
.do end
.*
.if &e'&$SWfx eq 1 .do begin
:OPT refid='SWfx' name='fx'.
.ix 'options' 'fx'
(C++ only)
This option can be used to disable the truncated header filename
processing that the compiler does by default (see "ft" above).
.do end
.*
.if &e'&$SWfzh eq 1 .do begin
:OPT refid='SWfzh' name='fzh'.
.ix 'options' 'fzh'
(C++ only)
This option can be used to stop the compiler from automatically adding
extensions to include files. The default behaviour of the compiler is to
search for the specified file, then to try known extensions if the file
specifier does not have an extension. Thus, #include <string> could be
matched by 'string', 'string.h' or 'string.hpp' (see "fzs" below). The
macro
.kwm __SW_FZH
will be defined when this switch is used.
.do end
.*
.if &e'&$SWfzs eq 1 .do begin
:OPT refid='SWfzs' name='fzs'.
.ix 'options' 'fzs'
(C++ only)
This option can be used to stop the compiler from automatically adding
extensions to source files. The default behaviour of the compiler is to
search for the specified file, then to try known extensions if the file
specifier does not have an extension. Thus, 'src_file' could be matched
by 'src_file', 'src_file.cpp' or 'src_file.cc' (see "fzh" above). The
macro
.kwm __SW_FZS
will be defined when this switch is used.
.do end
.*
.if &e'&$SWi eq 1 .do begin
:OPT refid='SWi' name='i'.=<directory>
.ix 'options' 'i'
where "<directory>" takes the form
.if '&target' eq 'QNX' .do begin
.millust begin
path&ps.path...
.millust end
.do end
.el .do begin
.millust begin
[d:]path;[d:]path...
.millust end
.do end
.pc
The specified paths are added to the list of directories in which the
compiler will search for "include" files.
See the section entitled :HDREF refid='wccinc'. for information on
directory searching.
.do end
.*
.if &e'&$SWk eq 1 .do begin
:OPT refid='SWk' name='k'.
.ix 'options' 'k'
(C++ only)
This option instructs the compiler to continue processing subsequent
source files after an error has been diagnosed in the current source
file.
See the option :HDREF refid='SWfc'. for information on compiling
multiple source files.
.do end
.*
.if &e'&$SWv eq 1 .do begin
:OPT refid='SWv' name='v'.
.ix 'options' 'v'
&cmpcname will output function declarations to a file with the same
filename as the C source file but with extension "&def".
The "definitions" file may be used as an "include" file when compiling
other modules in order to take advantage of the compiler's function
and argument type checking.
.do end
.*
.if &e'&$SWx eq 1 .do begin
:OPT refid='SWx' name='x'.
.ix 'options' 'x'
The compiler ignores the
.ev INCLUDE
and
.ev <os>_INCLUDE
environment variables, if they exist, when searching for include files.
See the section entitled :HDREF refid='wccinc'. for information on
directory searching.
.do end
.*
.if &e'&$SWzat eq 1 .do begin
:OPT refid='SWzat' name='zat'.
.ix 'options' 'zat'
ISO C++ defines a number of alternative tokens that can be used instead
of certain traditional tokens. For example "and" instead of "&&", "or"
instead of "||", etc. See section 2.5 of the ISO C++ 98 standard for the
complete list of such tokens. The "zat" option disables support for
these tokens so that the names "and", "or", etc are no longer reserved.
.do end
.*
.if &e'&$SWzf eq 1 .do begin
:OPT refid='SWzf' name='zf'.
.ix 'options' 'zf'
Starting with Open Watcom 1.3, the scope of a variable declared in the
initialization expression of a for loop header is by default limited to
the body of the loop. This is in accordance with the ISO C++ standard.
The "zf" option causes the compiler to revert to the behavior it had
before Open Watcom 1.3. In particular, it causes the scope of variables
declared in the initialization expression of a for loop header to extend
beyond the loop.
.np
.exam begin 9
#include <iostream>

void f()
{
  for( int i = 0; i < 10; ++i ) {
    std::cout << i << "\n";
  }
  std::cout << "Value of i at loop termination: " << i << "\n";
}
.exam end
.np
The above code will not compile with Open Watcom 1.3 or later because
the variable "i" is out of scope in the last output statement. The "zf"
option will allow such code to compile by extending the scope of "i"
beyond the loop.
.do end
.*
.if &e'&$SWzg eq 1 .do begin
:OPT refid='SWzg' name='zg'.
.ix 'options' 'zg'
The "zg" option is similar to the "v" option except that
function declarations will be output to the "DEF" file using
base types (i.e., typedefs are reduced to their base type).
.exam begin 5
typedef unsigned int UINT;
UINT f( UINT x )
 {
    return( x + 1 );
 }
.exam end
.np
If you use the "v" option, the output will be:
.millust begin
extern UINT f(UINT );
.millust end
.pc
If you use the "zg" option, the output will be:
.millust begin
extern unsigned int f(unsigned int );
.millust end
.do end
.*
.if &e'&$SWzl eq 1 .do begin
:OPT refid='SWzl' name='zl'.
.ix 'options' 'zl'
By default, the compiler places in the object file the names of the C
libraries that correspond to the memory model and floating-point
options that were selected.
The &lnkname uses these library names to select the libraries required
to link the application.
If you use the "zl" option, the library names will not be included in
the generated object file.
.np
The compiler may generate external references for library code that
conveniently cause the linker to link in different code.
One such case is: if you have any functions that pass or return
floating-point values (i.e., float or double), the compiler will
insert an external reference that will cause the floating-point
formatting routines to be included in the executable.
The "zl" option will disable these external references.
.np
Use this option when you wish to create a library of object modules
which do not contain &product library name references.
.do end
.*
.if &e'&$SWzld eq 1 .do begin
:OPT refid='SWzld' name='zld'.
.ix 'options' 'zld'
By default, the compiler places in the object file the names and time
stamps of all the files referenced by the source file.
This file dependency information can then be used by
.if '&target' eq 'QNX' .do begin
a make utility program
.do end
.el .do begin
WMAKE
.do end
to determine that this file needs to be recompiled if any of the
referenced files has been modified since the object file was created.
This option causes the compiler to not emit this information into the
object file.
.do end
.*
.if &e'&$SWzlf eq 1 .do begin
:OPT refid='SWzlf' name='zlf'.&optdag.
.ix 'options' 'zlf'
The "zlf" option tells the compilers to emit references for all default
library information into the compiled object file. See also the options
"zl", "zld" and "zls".
.do end
.*
.if &e'&$SWzls eq 1 .do begin
:OPT refid='SWzls' name='zls'.&optdag.
.ix 'options' 'zls'
The "zls" option tells the compilers to remove automatically inserted
symbols. These symbols are usually used to force symbol references
to be fixed up from the run-time libraries. An example would be
the symbol __DLLstart_, that is inserted into any object file that has
a DllMain() function defined within its source file.
.do end
.*
:eOPTLIST.
.*
.section Code Generation
.*
.np
This group of options deals with controlling some aspects of the code
that is generated by the compiler.
.*
:OPTLIST.
.*
.if &e'&$SWas eq 1 .do begin
:OPT refid='SWas' name='as'.
.ix 'options' 'as'
&AXPonly.
The "as" option informs the code generator that all short integers are
aligned on even boundaries.
This permits the code generator to emit a shorter sequence of
instructions to load short integers.
.ix 'macros' '__SW_AS'
The macro
.kwm __SW_AS
will be predefined if "as" is selected.
.do end
.*
.if &e'&$SWecc eq 1 .do begin
:OPT refid='SWecc' name='ecc'.
.ix 'options' 'ecc'
set default calling convention to __cdecl
.do end
.*
.if &e'&$SWecd eq 1 .do begin
:OPT refid='SWecd' name='ecd'.
.ix 'options' 'ecd'
set default calling convention to __stdcall
.do end
.*
.if &e'&$SWecf eq 1 .do begin
:OPT refid='SWecf' name='ecf'.
.ix 'options' 'ecf'
set default calling convention to __fastcall
.do end
.*
:CMT. .if &e'&$SWeco eq 1 .do begin
:CMT. :OPT refid='SWeco' name='eco'.
:CMT. .ix 'options' 'eco'
:CMT. set default calling convention to _Optlink
:CMT. .do end
.*
.if &e'&$SWecp eq 1 .do begin
:OPT refid='SWecp' name='ecp'.
.ix 'options' 'ecp'
set default calling convention to __pascal
.do end
.*
.if &e'&$SWecr eq 1 .do begin
:OPT refid='SWecr' name='ecr'.
.ix 'options' 'ecr'
set default calling convention to __fortran
.do end
.*
.if &e'&$SWecs eq 1 .do begin
:OPT refid='SWecs' name='ecs'.
.ix 'options' 'ecs'
set default calling convention to __syscall
.do end
.*
.if &e'&$SWecw eq 1 .do begin
:OPT refid='SWecw' name='ecw'.
.ix 'options' 'ecw'
set default calling convention to __watcall (default)
.do end
.*
.if &e'&$SWei eq 1 .do begin
:OPT refid='SWei' name='ei'.
.ix 'options' 'ei'
This option can be used to force the compiler to allocate at least an
"int" for all enumerated types.
.if '&alpha' eq 'AXP' .do begin
This option is the default for the AXP architecture.
.do end
.ix 'macros' '__SW_EI'
The macro
.kwm __SW_EI
will be predefined if "ei" is selected.
.do end
.*
.if &e'&$SWem eq 1 .do begin
:OPT refid='SWem' name='em'.
.ix 'options' 'em'
This option can be used to force the compiler to allocate the smallest
storage unit required to hold all possible values given for an
enumerated list.
This option is the default for the x86 architecture.
.ix 'macros' '__SW_EM'
The macro
.kwm __SW_EM
will be predefined if "em" is selected.
.do end
.*
.if &e'&$SWj eq 1 .do begin
:OPT refid='SWj' name='j'.
.ix 'options' 'j'
The default
.kwm char
type is changed from an unsigned to a signed quantity.
.ix 'macros' '__CHAR_SIGNED__'
.ix 'macros' '__SW_J'
The macros
.kwm __CHAR_SIGNED__
and
.kwm __SW_J
will be predefined if "j" is selected.
.do end
.*
.if &e'&$SWri eq 1 .do begin
:OPT refid='SWri' name='ri'.&optdag.
.ix 'options' 'ri'
Functions declared to return integral types such as chars and shorts
are promoted to returning ints.
This allows non-ISO-conforming source code which does not properly
declare the return types of functions to work properly.
The use of this option should be avoided.
.do end
.*
.if &e'&$SWxr eq 1 .do begin
:OPT refid='SWxr' name='xr'.&optdag.
.ix 'options' 'xr'
.ix 'options' 'RTTI'
.ix 'RTTI'
The "xr" option is used to to enable the use of the C++ feature called
Run-Time Type Information (RTTI).
RTTI can only be used with classes that have virtual functions
declared.
This restriction implies that if you enable RTTI, the amount of
storage used for a class in memory does not change.
The RTTI information is added to the virtual function information
block so there will be an increase in the executable size if you
choose to enable RTTI.
There is no execution penalty at all unless you use the dynamic_cast<>
feature in which case, you should be aware that the operation requires
a lookup operation in order to perform the conversion properly.
You can mix and match modules compiled with and without "xr", with the
caveat that dynamic_cast<> and typeid() may not function (return NULL
or throw an exception) if used on a class instance that was not
compiled with the "xr" option.
.do end
.*
.if &e'&$SWzc eq 1 .do begin
:OPT refid='SWzc' name='zc'.
.ix 'options' 'zc'
.ix 'code segment'
The "zc" option causes the code generator to place literal strings and
.us const
items in the code segment.
.exam begin 4
extern const int cvar = 1;
int var = 2;
const int ctable[ 5 ] = { 1, 2, 3, 4, 5 };
char *birds[ 3 ] = { "robin", "finch", "wren" };
.exam end
.pc
In the above example,
.id cvar
and
.id ctable
and the strings
.id "robin",
.id "finch",
etc. are placed in the code segment.
This option is supported in large data or flat memory models only, or
if the item is explicitly "far".
.ix 'macros' '__SW_ZC'
The macro
.kwm __SW_ZC
will be predefined if "zc" is selected.
.do end
.*
.if &e'&$SWzp eq 1 .do begin
:OPT refid='SWzp' name='zp'.[{1,2,4,8,16}]
.ix 'options' 'zp'
The "zp" option allows you to specify the alignment of members in
a structure.
The default is "zp2" for the 16-bit compiler and "zp8" for 32-bit
compiler.
The alignment of structure members is described in the following
table.
If the size of the member is 1, 2, 4, 8 or 16, the alignment is given
for each of the "zp" options.
If the member of the structure is an array or structure, the alignment
is described by the row "x".
.millust begin
                    zp1     zp2     zp4     zp8     zp16
sizeof(member)  \---------------------------------------
        1       |   0       0       0       0       0
        2       |   0       2       2       2       2
        4       |   0       2       4       4       4
        8       |   0       2       4       8       8
        16      |   0       2       4       8       16
        x       |   aligned to largest member
.millust end
.pc
An alignment of 0 means no alignment, 2 means word boundary, 4 means
doubleword boundary, etc.
.np
Note that packed structures are padded to ensure that consecutive
occurrences of the same structure in memory are aligned appropriately.
This is illustrated when the following example is compiled with "zp4".
The amount of padding is determined as follows.
If the largest member of structure "s" is 1 byte then "s" is not
aligned.
If the largest member of structure "s" is 2 bytes then "s" is aligned
according to row 2.
If the largest member of structure "s" is 4 bytes then "s" is aligned
according to row 4.
If the largest member of structure "s" is 8 bytes then "s" is aligned
according to row 8.
At present, there are no scalar objects that can have a size of 16
bytes.
If the largest member of structure "s" is an array or structure then
"s" is aligned according to row "x".
Padding is the inclusion of slack bytes at the end of a structure in
order to guarantee the alignment of consecutive occurrences of the
same structure in memory.
.np
To understand why structure member alignment may be important,
consider the following example.
.exam begin 10
#include <stdio.h>
#include <stddef.h>

typedef struct memo_el {
    char           date[9];
    struct memo_el *prev,*next;
    int            ref_number;
    char           sex;
} memo;

.exam break
void main( )
{
    printf( "Offset of %s is %d\n",
            "date", offsetof( memo, date ) );
    printf( "Offset of %s is %d\n",
            "prev", offsetof( memo, prev ) );
    printf( "Offset of %s is %d\n",
            "next", offsetof( memo, next ) );
    printf( "Offset of %s is %d\n",
            "ref_number", offsetof( memo, ref_number ) );
    printf( "Offset of %s is %d\n",
            "sex", offsetof( memo, sex ) );
    printf( "Size of %s is %d\n",
            "memo", sizeof( memo ) );
    printf( "Number of padding bytes is %d\n",
            sizeof( memo )
            - (offsetof( memo, sex ) + sizeof( char )) );
}
.exam end
.np
In the above example, the default alignment "zp8" will cause the
pointer and integer items to be aligned on even addresses
although the array "date" is 9 bytes in length.
The items are 2-byte aligned when sizeof(item) is 2 and 4-byte
aligned when sizeof(item) is 4.
.np
On computer systems that have a 16-bit (or 32-bit) bus, improved
performance can be obtained when pointer, integer and floating-point
items are aligned on an even boundary.
This could be done by careful rearrangement of the fields of the
structure or it can be forced by use of the "zp" option.
.millust begin
16-bit output when compiled zp1:
Offset of date is 0
Offset of prev is 9
Offset of next is 11
Offset of ref_number is 13
Offset of sex is 15
Size of memo is 16
Number of padding bytes is 0

.millust break
16-bit output when compiled zp4:
Offset of date is 0
Offset of prev is 10
Offset of next is 12
Offset of ref_number is 14
Offset of sex is 16
Size of memo is 18
Number of padding bytes is 1

.millust break
32-bit output when compiled zp1:
Offset of date is 0
Offset of prev is 9
Offset of next is 13
Offset of ref_number is 17
Offset of sex is 21
Size of memo is 22
Number of padding bytes is 0

.millust break
32-bit output when compiled zp4:
Offset of date is 0
Offset of prev is 12
Offset of next is 16
Offset of ref_number is 20
Offset of sex is 24
Size of memo is 28
Number of padding bytes is 3
.millust end
.do end
.*
.if &e'&$SWzps eq 1 .do begin
:OPT refid='SWzps' name='zps'.
.ix 'options' 'zps'
&AXPonly.
The "zps" option requests the code generator to align all structures
on qword boundaries.
This includes structs within structs.
Although this will improve execution performance, it could result in
increased memory requirements.
.do end
.*
.if &e'&$SWzpw eq 1 .do begin
:OPT refid='SWzpw' name='zpw'.
.ix 'options' 'zpw'
The compiler will output a warning message whenever padding is added
to a struct/class for alignment purposes.
.do end
.*
.if &e'&$SWzt eq 1 .do begin
:OPT refid='SWzt' name='zt'.<number>
.ix 'options' 'zt'
The "data threshold" option is used to set the maximum size for data
objects to be included in the default data segment.
This option can be used with the compact, large, and huge (16-bit)
memory models only.
These are memory models where there can be more than one data segment.
Normally, all data objects whose size is less than or equal to the
threshold value are placed in the default data segment "_DATA" unless
they are specifically declared to be
.kwm far
items.
When there is a large amount of static data, it is often useful to
set the data threshold size so that all objects larger than this size
are placed in another (far) data segment.
For example, the option "zt100" causes all data objects larger than
100 bytes in size to be implicitly declared as
.kwm far
and grouped in other data segments.
.np
The default data threshold value is 32767.
Thus, by default, all objects greater than 32767 bytes in size are
implicitly declared as
.kwm far
and will be placed in other data segments.
If the "zt" option is specified without a size, the data threshold
value is 256.
The largest value that can be specified is 32767 (a larger value will
result in 256 being selected).
.np
If the "zt" option is used to compile any module in a program, then
you must compile all the other modules in the program with the same
option (and value).
.np
Care must be exercised when declaring the size of objects in different
modules.
Consider the following declarations in two different C files.
Suppose we define an array in one module as follows:
.millust begin
extern int Array[100] = { 0 };
.millust end
and, suppose we reference the same array in another module as follows:
.millust begin
extern int Array[10];
.millust end
.pc
Assuming that these modules were compiled with the option "zt100",
we would have a problem.
In the first module, the array would be placed in another segment
since
.id Array[100]
is bigger than the data threshold.
In the second module, the array would be placed in the default data
segment since
.id Array[10]
is smaller than the data threshold.
The extra code required to reference the object in another data
segment would not be generated.
.np
Note that this problem can also occur even when the "zt" option
is not used (i.e., for objects greater than 32767 bytes in size).
There are two solutions to this problem: (1) be consistent when
declaring an object's size, or, (2) do not specify the size in data
reference declarations.
.do end
.*
.if &e'&$SWzv eq 1 .do begin
:OPT refid='SWzv' name='zv'.
.ix 'options' 'zv'
(C++ only)
Enable virtual function removal optimization.
.do end
.*
:eOPTLIST.
.*
.section 80x86 Floating Point
.*
.np
This group of options deals with control over the type of
floating-point instructions that the compiler generates.
There are two basic types &mdash. floating-point calls (FPC) or
floating-point instructions (FPI).
They are selectable through the use of one of the compiler options
described below.
You may wish to use the following list when deciding which option best
suits your requirements.
Here is a summary of advantages/disadvantages to both.
.begnote
.note FPC
.autopoint
.point
not IEEE floating-point
.point
not tailorable to processor
.point
uses coprocessor if present; simulates otherwise
.point
32-bit/64-bit accuracy
.point
runs somewhat faster if coprocessor present
.point
faster emulation (fewer bits of accuracy)
.point
leaner "math" library
.point
fatter application code (calls to library rather than in-line instructions)
.point
application cannot trap floating-point exceptions
.point
ideal for ROM applications
.endpoint
.note FPI, FPI87
.autopoint
.point
IEEE floating-point
.point
tailorable to processor (see fp2, fp3, fp5, fp6)
.point
uses coprocessor if present; emulates IEEE otherwise
.point
up to 80-bit accuracy
.point
runs "full-tilt" if coprocessor present
.point
slower emulation (more bits of accuracy)
.point
fatter "math" library
.point
leaner application code (in-line instructions)
.point
application can trap floating-point exceptions
.point
ideal for general-purpose applications
.endpoint
.endnote
.np
To see the difference in the type of code generated, consider the
following small example.
.exam begin
#include <stdio.h>
#include <time.h>

void main()
{
    clock_t  cstart, cend;
    cstart = clock();
    /*  .
        .
        .
    */
    cend = clock();
    printf( "%4.2f seconds to calculate\n",
            ((float)cend - cstart) / CLOCKS_PER_SEC );
}
.exam end
.np
The following 32-bit code is generated by the &cmpcname compiler
(&ccmd32) using the "fpc" option.
.code begin
main_   push    ebx
        push    edx
        call    clock_
        mov     edx,eax
        call    clock_
        call    __U4FS  ; unsigned 4 to floating single
        mov     ebx,eax
        mov     eax,edx
        call    __U4FS  ; unsigned 4 to floating single
        mov     edx,eax
        mov     eax,ebx
        call    __FSS   ; floating single subtract
        mov     edx,3c23d70aH
        call    __FSM   ; floating single multiply
        call    __FSFD  ; floating single to floating double
        push    edx
        push    eax
        push    offset L1
        call    printf_
        add     esp,0000000cH
        pop     edx
        pop     ebx
        ret
.code end
.np
The following 32-bit code is generated by the &cmpcname compiler
(&ccmd32) using the "fpi" option.
.code begin
main_   push    ebx
        push    edx
        sub     esp,00000010H
        call    clock_
        mov     edx,eax
        call    clock_
        xor     ebx,ebx
        mov     [esp],eax
        mov     +4H[esp],ebx
        mov     +8H[esp],edx
        mov     +0cH[esp],ebx
        fild    qword ptr [esp]     ; integer to double
        fild    qword ptr +8H[esp]  ; integer to double
        fsubp   st(1),st            ; subtract
        fmul    dword ptr L2        ; multiply
        sub     esp,00000008H
        fstp    qword ptr [esp]     ; store into memory
        push    offset L1
        call    printf_
        add     esp,0000000cH
        add     esp,00000010H
        pop     edx
        pop     ebx
        ret
.code end
.*
:OPTLIST.
.*
.if &e'&$SWfpc eq 1 .do begin
:OPT refid='SWfpc' name='fpc'.
.ix 'options' 'fpc'
All floating-point arithmetic is done with calls to a floating-point
emulation library.
If a numeric data processor is present in the system, it will be used
by the library; otherwise floating-point operations are simulated in
software.
This option should be used for any of the following reasons:
.autopoint
.point
Speed of floating-point emulation is favoured over code size.
.point
An application containing floating-point operations is to be stored in
ROM and an 80x87 will not be present in the system.
.endpoint
.np
.ix 'macros' '__SW_FPC'
The macro
.kwm __SW_FPC
will be predefined if "fpc" is selected.
.cp 10
.remark
.ix 'floating-point' 'consistency of options'
When any module in an application is compiled with the "fpc" option,
then all modules must be compiled with the "fpc" option.
.np
Different math libraries are provided for applications which have been
compiled with a particular floating-point option.
See the section entitled :HDREF refid='libmath'..
.eremark
.if '&target' ne 'QNX' .do begin
.np
See the section entitled :HDREF refid='libno87'. for information on
testing the floating-point simulation code on personal computers
equipped with a coprocessor.
.do end
.do end
.*
.if &e'&$SWfpi eq 1 .do begin
:OPT refid='SWfpi' name='fpi'.&optdag.
.ix 'options' 'fpi'
&286only.
The compiler will generate in-line 80x87 numeric data processor
instructions into the object code for floating-point operations.
Depending on which library the code is linked against, these
instructions will be left as is or they will be replaced by special
interrupt instructions.
In the latter case, floating-point will be emulated if an 80x87 is not
present.
This is the default floating-point option if none is specified.
.np
&386only.
The compiler will generate in-line 387-compatible numeric data
processor instructions into the object code for floating-point
operations.
When any module containing floating-point operations is compiled with
the "fpi" option, coprocessor emulation software will be included in
the application when it is linked.
.if '&target' ne 'QNX' .do begin
.np
For 32-bit &company Windows-extender applications or 32-bit
applications run in Windows 3.1 DOS boxes, you must also include the
.fi WEMU387.386
file in the
.mono [386enh]
section of the
.fi SYSTEM.INI
file.
.exam begin
device=C:&pathnamup.\binw\wemu387.386
.exam end
Note that the
.fi WDEBUG.386
file which is installed by the &company Installation software contains
the emulation support found in the
.fi WEMU387.386
file.
.do end
.np
Thus, a math coprocessor need not be present at run-time.
This is the default floating-point option if none is specified.
.ix 'macros' '__FPI__'
.ix 'macros' '__SW_FPI'
The macros
.kwm __FPI__
and
.kwm __SW_FPI
will be predefined if "fpi" is selected.
.ix 'numeric data processor' 'option'
.ix 'math coprocessor' 'option'
.ix 'floating-point' 'option'
.cp 13
.remark
.ix 'floating-point' 'consistency of options'
When any module in an application is compiled with a particular
"floating-point" option, then all modules must be compiled with
the same option.
.np
If you wish to have floating-point emulation software included in
the application, you should select the "fpi" option.
A math coprocessor need not be present at run-time.
.np
Different math libraries are provided for applications which have been
compiled with a particular floating-point option.
See the section entitled :HDREF refid='libmath'..
.eremark
.do end
.if '&target' ne 'QNX' .do begin
.np
See the section entitled :HDREF refid='libno87'. for information on
testing the math coprocessor emulation code on personal computers
equipped with a coprocessor.
.do end
.*
.if &e'&$SWfpi87 eq 1 .do begin
:OPT refid='SWfpi87' name='fpi87'.&optdag.
.ix 'options' 'fpi87'
&286only.
The compiler will generate in-line 80x87 numeric data processor
instructions into the object code for floating-point operations.
An 8087 or compatible math coprocessor must be present at run-time.
If the "2" option is used in conjunction with this option, the
compiler will generate 287 and upwards compatible instructions;
otherwise, the compiler will generate 8087 compatible instructions.
.np
&386only.
The compiler will generate in-line 387-compatible numeric data
processor instructions into the object code for floating-point
operations.
When the "fpi87" option is used exclusively, coprocessor emulation
software is not included in the application when it is linked.
A 387 or compatible math coprocessor must be present at run-time.
.np
.ix 'macros' '__FPI__'
.ix 'macros' '__SW_FPI87'
The macros
.kwm __FPI__
and
.kwm __SW_FPI87
will be predefined if "fpi87" is selected.
See Note with description of "fpi" option.
.do end
.*
.if &e'&$SWfp2 eq 1 .do begin
:OPT refid='SWfp2' name='fp2'.&optdag.
.ix 'options' 'fp2'
The compiler will generate in-line 80x87 numeric data processor
instructions into the object code for floating-point operations.
For &company compilers generating 16-bit code, this option is the
default.
For 32-bit applications, use this option if you wish to support those
few 386 systems that are equipped with a 287 numeric data processor
("fp3" is the default for &company compilers generating 32-bit code).
However, for 32-bit applications, the use of this option will reduce
execution performance.
Use this option in conjunction with the "fpi" or "fpi87" options.
.ix 'macros' '__SW_FP2'
.ix '__SW_FP2'
The macro
.kwm __SW_FP2
will be predefined if "fp2" is selected.
.do end
.*
.if &e'&$SWfp3 eq 1 .do begin
:OPT refid='SWfp3' name='fp3'.
.ix 'options' 'fp3'
The compiler will generate in-line 387-compatible numeric data
processor instructions into the object code for floating-point
operations.
For 16-bit applications, the use of this option will limit the range
of systems on which the application will run but there are execution
performance improvements.
For &company compilers generating 32-bit code, this option is the
default.
Use this option in conjunction with the "fpi" or "fpi87" options.
.ix 'macros' '__SW_FP3'
The macro
.kwm __SW_FP3
will be predefined if "fp3" is selected.
.do end
.*
.if &e'&$SWfp5 eq 1 .do begin
:OPT refid='SWfp5' name='fp5'.
.ix 'options' 'fp5'
The compiler will generate in-line 80x87 numeric data processor
instructions into the object code for floating-point operations.
The sequence of floating-point instructions will be optimized for
greatest possible performance on the Intel Pentium processor.
For 16-bit applications, the use of this option will limit the range
of systems on which the application will run but there are execution
performance improvements.
Use this option in conjunction with the "fpi" or "fpi87" options.
.ix 'macros' '__SW_FP5'
The macro
.kwm __SW_FP5
will be predefined if "fp5" is selected.
.do end
.*
.if &e'&$SWfp6 eq 1 .do begin
:OPT refid='SWfp6' name='fp6'.
.ix 'options' 'fp6'
The compiler will generate in-line 80x87 numeric data processor
instructions into the object code for floating-point operations.
The sequence of floating-point instructions will be optimized for
greatest possible performance on the Intel Pentium Pro processor.
For 16-bit applications, the use of this option will limit the range
of systems on which the application will run but there are execution
performance improvements.
Use this option in conjunction with the "fpi" or "fpi87" options.
.ix 'macros' '__SW_FP6'
The macro
.kwm __SW_FP6
will be predefined if "fp6" is selected.
.do end
.*
.if &e'&$SWfpd eq 1 .do begin
:OPT refid='SWfpd' name='fpd'.
.ix 'options' 'fpd'
.ix 'FDIV bug'
.ix 'Pentium bug'
A subtle problem was detected in the FDIV instruction of Intel's
original Pentium CPU.
In certain rare cases, the result of a floating-point divide could
have less precision than it should.
Contact Intel directly for more information on the issue.
.np
As a result, the run-time system startup code has been modified to
test for a faulty Pentium.
If the FDIV instruction is found to be flawed, the low order bit of
the run-time system variable
.mono __chipbug
will be set.
.millust begin
extern unsigned __near __chipbug;
.millust end
.np
If the FDIV instruction does not show the problem, the low order bit
will be clear.
If the Pentium FDIV flaw is a concern for your application, there are
two approaches that you could take:
.autonote
.note
You may test the
.mono __chipbug
variable in your code in all floating-point and memory models and take
appropriate action (such as display a warning message or discontinue
the application).
.note
Alternately, you can use the "fpd" option when compiling your code.
This option directs the compiler to generate additional code whenever
an FDIV instruction is generated which tests the low order bit of
.mono __chipbug
and, if on, calls the software workaround code in the math libraries.
If the bit is off, an in-line FDIV instruction will be performed as
before.
.endnote
.np
If you know that your application will never run on a defective
Pentium CPU, or your analysis shows that the FDIV problem will not
affect your results, you need not use the "fpd" option.
.ix 'macros' '__SW_FPD'
The macro
.kwm __SW_FPD
will be predefined if "fpd" is selected.
.do end
.*
.if &e'&$SW7 eq 1 .do begin
:OPT refid='SW7' name='7'.&optdag.
.ix 'options' '7'
&286only.
The compiler will generate in-line 80x87 numeric data processor
instructions into the object code for floating-point operations.
.np
&386only.
The compiler will generate in-line 387-compatible numeric data
processor instructions into the object code for floating-point
operations.
.np
This option is equivalent to "fpi87".
.ix 'macros' '__FPI__'
.ix 'macros' '__SW_FPI87'
The macros
.kwm __FPI__
and
.kwm __SW_FPI87
will be predefined if "7" is selected.
.do end
.*
:eOPTLIST.
.*
.section Segments/Modules
.*
.np
This group of options deals with object file data structures that are
generated by the compiler.
.*
:OPTLIST.
.*
.if &e'&$SWg eq 1 .do begin
:OPT refid='SWg' name='g'.=<codegroup>
.ix 'options' 'g'
The generated code is placed in the group called "<codegroup>".
The default "text" segment name will be "<codegroup>_TEXT" but this
can be overridden by the "nt" option.
.exam begin 1
&prompt.:SF font=1.compiler_name:eSF. report &sw.g=RPTGROUP &sw.s
.exam end
.np
&286only. <<
.np
This is useful when compiling applications for small code models where
the total application will contain more than 64 kilobytes of code.
Each group can contain up to 64 kilobytes of code.
The application follows a "mixed" code model since it contains a mix
of small and large code (intra-segment and inter-segment calls).
Memory models are described in the
chapter entitled :HDREF refid='mdl86'..
The
.kwm far
keyword is used to describe routines that are referenced from one
group/segment but are defined in another group/segment.
.np
.ix '__STK' 'stack overflow'
.ix 'stack overflow'
For small code models, the "s" option should be used in conjunction
with the "g" option to prevent the generation of calls to the C
run-time "stack overflow check" routine (
.ct
.id __STK
.ct
).
You must also avoid calls to other "small code" C run-time library
routines since inter-segment "near" calls to C library routines are
not possible.
.np
>> &286only.
.do end
.*
.if &e'&$SWnc eq 1 .do begin
:OPT refid='SWnc' name='nc'.=<name>
.ix 'options' 'nc'
.ix 'CODE class'
.ix 'class' 'CODE'
.ix '_TEXT segment'
.ix 'segment' '_TEXT'
The default "code" class name is "CODE".
The small code model "_TEXT" segment and the large code model
"module_name_TEXT" segments belong to the "CODE" class.
This option allows you to select a different class name for these code
segments.
The name of the "code" class is explicitly set to "<name>".
.np
.ix 'DATA class'
.ix 'class' 'DATA'
.ix 'CONST segment'
.ix 'segment' 'CONST'
.ix 'CONST2 segment'
.ix 'segment' 'CONST2'
.ix '_DATA segment'
.ix 'segment' '_DATA'
.ix 'BSS class'
.ix 'class' 'BSS'
.ix '_BSS segment'
.ix 'segment' '_BSS'
Note that the default "data" class names are "DATA" (for the "CONST",
"CONST2" and "_DATA" segments) and "BSS" (for the "_BSS" segment).
There is no provision for changing the data class names.
.do end
.*
.if &e'&$SWnd eq 1 .do begin
:OPT refid='SWnd' name='nd'.=<name>
.ix 'options' 'nt'
.ix 'CONST segment'
.ix 'segment' 'CONST'
.ix 'CONST2 segment'
.ix 'segment' 'CONST2'
.ix '_DATA segment'
.ix 'segment' '_DATA'
.ix '_BSS segment'
.ix 'segment' '_BSS'
This option permits you to define a special prefix for the "CONST",
"CONST2", "_DATA", and "_BSS" segment names.
.ix 'DGROUP group'
.ix 'group' 'DGROUP'
The name of the group to which these segments belong is also changed
from "DGROUP" to "<name>_GROUP".
.if '&target' ne 'QNX' .do begin
.ix 'dynamic link library'
.ix 'DLL'
This option is especially useful in the creation of 16-bit Dynamic
Link Library (DLL) routines.
.do end
.exam begin 1
&prompt.:SF font=1.compiler_name:eSF. report &sw.nd=spec
.exam end
.pc
In the above example, the segment names become "specCONST",
"specCONST2", "spec_DATA", and "spec_BSS" and the group name becomes
"spec_GROUP".
.np
By default, the data group "DGROUP" consists of the "CONST", "CONST2",
"_DATA", and "_BSS" segments.
The compiler places certain types of data in each segment.
The "CONST" segment contains constant literals that appear in your
source code.
.exam begin 3
char *birds[ 3 ] = { "robin", "finch", "wren" };

printf( "Hello world\n" );
.exam end
.pc
In the above example, the strings "Hello world\n", "robin", "finch",
etc. appear in the "CONST" segment.
.np
The "CONST2" segment contains initialized read-only data.
.exam begin 4
extern const int cvar = 1;
int var = 2;
int table[ 5 ] = { 1, 2, 3, 4, 5 };
char *birds[ 3 ] = { "robin", "finch", "wren" };
.exam end
.pc
In the above example, the constant variable
.id cvar
is placed in the "CONST2" segment.
.np
The "_BSS" segment contains uninitialized data such as scalars,
structures, or arrays.
.exam begin 2
int var1;
int array1[ 400 ];
.exam end
.np
Other data segments containing data, specifically declared to be
.kwm far
or exceeding the data threshold (see "zt" option), are named
either
"module_nameN_DATA" when using the C compiler or
"module_name_DATAN" when using the C++ compiler
where "N" is some integral number.
.exam begin 1
int far array2[400];
.exam end
.pc
In the above example,
.id array2
is placed in the segment "report11_DATA" (C) or "report_DATA11" (C++)
provided that the module name is "report".
.np
.ix 'macros' '__SW_ND'
The macro
.kwm __SW_ND
will be predefined if "nd" is selected.
.do end
.*
.if &e'&$SWnm eq 1 .do begin
:OPT refid='SWnm' name='nm'.=<name>
.ix 'options' 'nm'
.ix '_TEXT segment'
.ix 'segment' '_TEXT'
By default, the object file name and the module name that is placed
within it are constructed from the source file name.
When the "nm" option is used, the module name that is placed into the
object file is "<name>".
For large code models, the "text" segment name is "<name>_TEXT"
unless the "nt" option is used.
.np
In the following example, the preprocessed output from
.fi report.c
is stored
.if '&target' eq 'QNX' .do begin
on another node under the name
.fi &dr4.&pc.temp.c.
.do end
.el .do begin
on drive "D" under the name
.fi temp.c.
.do end
The file is compiled with the "nm" option so that the module name
imbedded into the object file is "REPORT" rather than "TEMP".
.exam begin 2
&prompt.:SF font=1.compiler_name:eSF. report &sw.pl &sw.fo=&dr4.&pc.temp.c
&prompt.:SF font=1.compiler_name:eSF. &dr4.&pc.temp &sw.nm=report &sw.fo=report
.exam end
.pc
Since the "fo" option is also used, the resultant object file is
called
.fi report&obj..
.do end
.*
.if &e'&$SWnt eq 1 .do begin
:OPT refid='SWnt' name='nt'.=<name>
.ix 'options' 'nt'
The name of the "text" segment is explicitly set to "<name>".
By default, the "text" segment name is "_TEXT" for small code models
and "module_name_TEXT" for large code models.
.millust begin
Application Memory      Code
Type        Model       Segment
----------- -------     -------------------
.if '&target' ne 'QNX' .do begin
16-bit      tiny        _TEXT
.do end
32-bit      flat        _TEXT
16/32-bit   small       _TEXT
16/32-bit   medium      module_name_TEXT
16/32-bit   compact     _TEXT
16/32-bit   large       module_name_TEXT
16-bit      huge        module_name_TEXT
.millust end
.do end
.*
.if &e'&$SWzm eq 1 .do begin
:OPT refid='SWzm' name='zm'.
.ix 'options' 'zm'
The "zm" option instructs the code generator to place each function
into a separate segment.
.np
In small code models, the segment name is "_TEXT" by default.
.np
(C only)
In large code models, the segment name is composed of the function name
concatenated with the string "_TEXT".
.np
(C++ only)
In large code models, the segment name is composed of the module name
concatenated with the string "_TEXT" and a unique integral number.
.if &e'&$SWnt eq 1 .do begin
.np
The default string "_TEXT" can be altered using the "nt" option
:optref refid='SWnt'..
.do end
.np
The advantages to this option are:
.autopoint
.point
.ix 'ELIMINATE linker option'
Since each function is placed in its own segment, functions that are
not required by an application are omitted from the executable by the
linker (when "OPTION ELIMINATE" is specified).
.point
This can result in smaller executables.
.point
This benefit applies to both small and large code models.
.point
This option allows you to create granular libraries without resorting
to placing each function in a separate file.
.endpoint
.exam begin
static int foo( int x )
{
    return x - 1;
}
.exam break
static int near foo1( int x )
{
    return x + 1;
}
.exam break
int foo2( int y )
{
    return foo(y) * foo1(y-1);
}
.exam break
int foo3( int x, int y )
{
    return x + y * x;
}
.exam end
.np
The disadvantages to this option are:
.autopoint
.point
The "near call" optimization for static functions in large code models
is disabled (e.g., the function
.id foo
in the example above will never be "near called".
Static functions will always be "far called" in large code models.
.point
Near static functions will still be "near called"
(e.g., the function
.id foo1
is "near called" in the example above).
However, this can lead to problems at link time if the caller function
ends up in a different segment from the called function (the linker
will issue a message if this is the case).
.point
The "common epilogue" optimization is lost.
.point
.ix 'ELIMINATE linker option'
The linker "OPTION ELIMINATE" must be specified when linking an
application to take advantage of the granularity inherent in object
files/libraries compiled with this option.
.point
Any assumptions about relative position of functions are
invalid. Consider the following code which attempts to determine
the size of a code region by subtracting function addresses:
.exam begin
region_size = (unsigned long)&function2 - (unsigned long)function1;
.exam end
When "zm" is in effect,
.id region_size
may be extremely large or even
a negative value. For the above code to work as intended, both
.id function1
and
.id function2
(and every function intended to be located between them) must reside
 in a single code segment.
.endpoint
.np
This option can be used in paging environments where special segment
ordering may be employed.
.ix 'alloc_text'
.ix 'pragma' 'alloc_text'
The "alloc_text" pragma is often used in conjunction with this option
to place functions into specific segments.
.np
.ix 'macros' '__SW_ZM'
The macro
.kwm __SW_ZM
will be predefined if "zm" is selected.
.do end
.*
.if &e'&$SWzmf eq 1 .do begin
:OPT refid='SWzmf' name='zmf'.
.ix 'options' 'zmf'
(C++ only)
This option is identical to the "zm" option :optref refid='SWzm'.
except for the following large code model consideration.
.np
Instead of placing each function in a segment with a different name,
the code generator will place each function in a segment with the same
name (the name of the module suffixed by "_TEXT").
.np
The advantages to this option are:
.autopoint
.point
All functions in a module will reside in the same physical segment in
an executable.
.point
The "near call" optimization for static functions in large code models
is not disabled (e.g., the function
.id foo
in the example above will be "near called".
Static functions will always be "near called" in large code models.
.point
The problem associated with calling "near" functions goes away since
all functions in a module will reside in the same physical segment
(e.g., the function
.id foo1
is "near" in the example above).
.endpoint
.np
The disadvantages to this option are:
.autopoint
.point
The size of a physical segment is restricted to 64K in 16-bit
applications.
Although this may limit the number of functions that can be placed in
the segment, the restriction is only on a "per module" basis.
.point
Although less constricting, the size of a physical segment is
restricted to 4G in a 32-bit application.
.endpoint
.do end
.*
:eOPTLIST.
.*
.section 80x86 Run-time Conventions
.*
.np
This group of options deals with the 80x86 run-time environment.
.*
:OPTLIST.
.*
.if &e'&$SW0 eq 1 .do begin
:OPT refid='SW0' name='0'.
.ix 'options' '0'
&286only.
The compiler will make use of only 8086 instructions in the generated
object code.
This is the default.
The resulting code will run on 8086 and all upward compatible
processors.
The macro
.kwm __SW_0
will be predefined if "0" is selected.
.do end
.*
.if &e'&$SW1 eq 1 .do begin
:OPT refid='SW1' name='1'.
.ix 'options' '1'
&286only.
The compiler will make use of 186 instructions in the generated object
code whenever possible.
The resulting code probably will not run on 8086 compatible processors
but it will run on 186 and upward compatible processors.
The macro
.kwm __SW_1
will be predefined if "1" is selected.
.do end
.*
.if &e'&$SW2 eq 1 .do begin
:OPT refid='SW2' name='2'.
.ix 'options' '2'
&286only.
The compiler will make use of 286 instructions in the generated object
code whenever possible.
The resulting code probably will not run on 8086 or 186 compatible
processors but it will run on 286 and upward compatible processors.
The macro
.kwm __SW_2
will be predefined if "2" is selected.
.do end
.*
.if &e'&$SW3 eq 1 .do begin
:OPT refid='SW3' name='3'.
.ix 'options' '3'
&286only.
The compiler will make use of some 386 instructions and FS or GS (if
"zff" or "zgf" options are used) in the generated object code whenever
possible.
The code will be optimized for 386 processors.
The resulting code probably will not run on 8086, 186 or 286
compatible processors but it will run on 386 and upward compatible
processors.
The macro
.kwm __SW_3
will be predefined if "3" is selected.
.do end
.*
.if &e'&$SW4 eq 1 .do begin
:OPT refid='SW4' name='4'.
.ix 'options' '4'
&286only.
The compiler will make use of some 386 instructions and FS or GS (if
"zff" or "zgf" options are used) in the generated object code whenever
possible.
The code will be optimized for 486 processors.
The resulting code probably will not run on 8086, 186 or 286
compatible processors but it will run on 386 and upward compatible
processors.
The macro
.kwm __SW_4
will be predefined if "4" is selected.
.do end
.*
.if &e'&$SW5 eq 1 .do begin
:OPT refid='SW5' name='5'.
.ix 'options' '5'
&286only.
The compiler will make use of some 386 instructions and FS or GS (if
"zff" or "zgf" options are used) in the generated object code whenever
possible.
The code will be optimized for the Intel Pentium processor.
The resulting code probably will not run on 8086, 186 or 286
compatible processors but it will run on 386 and upward compatible
processors.
The macro
.kwm __SW_5
will be predefined if "5" is selected.
.do end
.*
.if &e'&$SW6 eq 1 .do begin
:OPT refid='SW6' name='6'.
.ix 'options' '6'
&286only.
The compiler will make use of some 386 instructions and FS or GS (if
"zff" or "zgf" options are used) in the generated object code whenever
possible.
The code will be optimized for the Intel Pentium Pro processor.
The resulting code probably will not run on 8086, 186 or 286
compatible processors but it will run on 386 and upward compatible
processors.
.ix 'macros' '__SW_6'
The macro
.kwm __SW_6
will be predefined if "6" is selected.
.do end
.*
.if &e'&$SW3RS eq 1 .do begin
:OPT refid='SW3RS' name='3'.{r|s}&optdag.
.ix 'options' '3r, 3s'
&386only.
The compiler will generate 386 instructions based on 386 instruction
timings (see "4", "5" and "6" below).
.np
If the "r" suffix is specified, the following machine-level code
strategy is employed.
.begbull
.bull
The compiler will pass arguments in registers whenever possible. This
is the default method used to pass arguments (unless the "bt=netware"
option is specified).
.bull
All registers except EAX are preserved across function calls.
.bull
When any form of the "fpi" option is used, the result of functions of
type "float" and "double" is returned in ST(0).
.bull
When the "fpc" option is used, the result of a function of type
"float" is returned in EAX and the result of a function of type
"double" is returned in EDX:EAX.
.bull
The resulting code will be smaller than that which is generated for
the stack-based method of passing arguments (see "3s" below).
.bull
The default naming convention for all global functions is such that an
underscore character ("_") is suffixed to the symbol name.
The default naming convention for all global variables is such that an
underscore character ("_") is prefixed to the symbol name.
.endbull
.np
If the "s" suffix is specified, the following machine-level code
strategy is employed.
.begbull
.bull
The compiler will pass all arguments on the stack.
.bull
The EAX, ECX and EDX registers are not preserved across function calls.
.bull
The FS and GS registers are not preserved across function calls.
.bull
The result of a function of type "float" is returned in EAX.
The result of a function of type "double" is returned in EDX:EAX.
.bull
The resulting code will be larger than that which is generated for the
register method of passing arguments (see "3r" above).
.bull
The naming convention for all global functions and variables is
modified such that no underscore characters ("_") are prefixed or
suffixed to the symbol name.
.endbull
.np
.ix 'MetaWare' 'High C calling convention'
The "s" conventions are similar to those used by the MetaWare High C
386 compiler.
.np
By default, "r" is selected if neither "r" nor "s" is specified.
.np
The macro
.kwm __SW_3
will be predefined if "3" is selected.
The macro
.kwm __SW_3R
will be predefined if "r" is selected (or defaulted).
The macro
.kwm __SW_3S
will be predefined if "s" is selected.
.do end
.*
.if &e'&$SW4RS eq 1 .do begin
:OPT refid='SW4RS' name='4'.{r|s}&optdag.
.ix 'options' '4r, 4s'
&386only.
This option is identical to "3{r|s}" except that the compiler will
generate 386 instructions based on 486 instruction timings.
The code is optimized for 486 processors rather than 386 processors.
By default, "r" is selected if neither "r" nor "s" is specified.
The macro
.kwm __SW_4
will be predefined if "4" is selected.
The macro
.kwm __SW_3R
will be predefined if "r" is selected (or defaulted).
The macro
.kwm __SW_3S
will be predefined if "s" is selected.
.do end
.*
.if &e'&$SW5RS eq 1 .do begin
:OPT refid='SW5RS' name='5'.{r|s}&optdag.
.ix 'options' '5r, 5s'
&386only.
This option is identical to "3{r|s}" except that the compiler will
generate 386 instructions based on Intel Pentium instruction timings.
This is the default.
The code is optimized for Intel Pentium processors rather than 386
processors.
By default, "r" is selected if neither "r" nor "s" is specified.
The macro
.kwm __SW_5
will be predefined if "5" is selected.
The macro
.kwm __SW_3R
will be predefined if "r" is selected (or defaulted).
The macro
.kwm __SW_3S
will be predefined if "s" is selected.
.do end
.*
.if &e'&$SW6RS eq 1 .do begin
:OPT refid='SW6RS' name='6'.{r|s}&optdag.
.ix 'options' '6r, 6s'
&386only.
This option is identical to "3{r|s}" except that the compiler will
generate 386 instructions based on Intel Pentium Pro instruction
timings.
The code is optimized for Intel Pentium Pro processors rather than 386
processors.
By default, "r" is selected if neither "r" nor "s" is specified.
.ix 'macros' '__SW_6'
The macro
.kwm __SW_6
will be predefined if "6" is selected.
.ix 'macros' '__SW_3R'
The macro
.kwm __SW_3R
will be predefined if "r" is selected (or defaulted).
The macro
.kwm __SW_3S
will be predefined if "s" is selected.
.do end
.*
.im CPOPTMDL
.*
.if &e'&$SWzdfp eq 1 .do begin
:OPT refid='SWzdfp' name='zd'.{f,p}&optdag.
.ix 'options' 'zdf'
.ix 'options' 'zdp'
&x86only.
The "zdf" option allows the code generator to use the DS register to
point to other segments besides "DGROUP"
This is the default in the 16-bit compact, large, and huge memory
models (except for 16-bit Windows applications).
.np
The "zdp" option informs the code generator that the DS register must
always point to "DGROUP"
This is the default in the 16-bit small and medium memory models, all
of the 16-bit Windows memory models, and the 32-bit small and flat
memory models.
The macro
.kwm __SW_ZDF
will be predefined if "zdf" is selected.
The macro
.kwm __SW_ZDP
will be predefined if "zdp" is selected.
.do end
.*
.if &e'&$SWzdl eq 1 .do begin
:OPT refid='SWzdl' name='zdl'.&optdag.
.ix 'options' 'zdl'
&386only.
The "zdl" option causes generation of code to load the DS register
directly from DGROUP (rather than the default run-time call).
This option causes the generation of a segment relocation.
This option is used with the "zdp" option but not the "zdf" option.
.do end
.*
.if &e'&$SWzev eq 1 .do begin
:OPT refid='SWzev' name='zev'.&optdag.
.ix 'options' 'zev'
The "zev" option is an extension to the Watcom C compiler to allow
arithmetic operations on void derived types. This option has been added
for compatibility with some Unix compilers and is not ISO compliant.
The use of this option should be avoided.
.do end
.*
.if &e'&$SWzffp eq 1 .do begin
:OPT refid='SWzffp' name='zf'.{f,p}&optdag.
.ix 'options' 'zff'
.ix 'options' 'zfp'
The "zff" option allows the code generator to use the FS register
(default for all but flat memory model).
The "zfp" option informs the code generator that the FS register must
not be used (default in flat memory model).
The macro
.kwm __SW_ZFF
will be predefined if "zff" is selected.
The macro
.kwm __SW_ZFP
will be predefined if "zfp" is selected.
.do end
.*
.if &e'&$SWzfw eq 1 .do begin
:OPT refid='SWzfw' name='zfw'.&optdag.
.ix 'options' 'zfw'
The "zfw" option turns on generation of FWAIT instructions on 386 and later
CPUs. Note that when targeting 286 and earlier, this option has no effect
because FWAITs are always required for synchronization between CPU and FPU.
.np
This option generates larger and slower code and should only be used when
restartable floating-point exceptions are required.
.np
The macro
.kwm __SW_ZFW
will be predefined if "zfw" is selected.
.do end
.*
.if &e'&$SWzgfp eq 1 .do begin
:OPT refid='SWzgfp' name='zg'.{f,p}&optdag.
.ix 'options' 'zgf'
.ix 'options' 'zgp'
The "zgf" option allows the code generator to use the GS register
(default for all memory models).
The "zgp" option informs the code generator that the GS register must
not be used.
The macro
.kwm __SW_ZGF
will be predefined if "zgf" is selected.
The macro
.kwm __SW_ZGP
will be predefined if "zgp" is selected.
.do end
.*
.if &e'&$SWzri eq 1 .do begin
:OPT refid='SWzri' name='zri'.&optdag.
.ix 'options' 'zri'
&386only.
The "zri" option inlines the code for floating point rounding.
Normally a function call is generated for each float to int conversion
 which may not be desirable.
.np
The macro
.kwm __SW_ZRI
will be predefined if "zri" is selected.
.do end
.*
.if &e'&$SWzro eq 1 .do begin
:OPT refid='SWzro' name='zro'.&optdag.
.ix 'options' 'zro'
The "zro" option omits the code for floating point rounding.
This results in non-conformant code - the rounding mode is not ISO/ANSI C
compliant - but the code generated is very fast.
.np
The macro
.kwm __SW_ZRO
will be predefined if "zro" is selected.
.do end
.*
.if &e'&$SWzu eq 1 .do begin
:OPT refid='SWzu' name='zu'.&optdag.
.ix 'options' 'zu'
.ix 'SS segment register'
The "zu" option relaxes the restriction that the SS register
contains the base address of the default data segment, "DGROUP".
Normally, all data items are placed into the group "DGROUP" and the SS
register contains the base address of this group.
When the "zu" option is selected, the SS register is volatile (assumed
to point to another segment) and any global data references require
loading a segment register such as DS with the base address of
"DGROUP".
.np
.if '&target' eq 'QNX' .do begin
.ix 'shared library'
This option is useful when compiling routines that are to be placed in
a "shared library" since the SS register points to the stack segment
of the calling application upon entry to the function.
.do end
.el .do begin
.ix 'dynamic link library'
.ix 'DLL'
&286only.
This option is useful when compiling routines that are to be placed in
a Dynamic Link Library (DLL) since the SS register points to the stack
segment of the calling application upon entry to the function.
.do end
.np
The macro
.kwm __SW_ZU
will be predefined if "zu" is selected.
.do end
.*
:eOPTLIST.
.*
.section Optimizations
.*
.np
When specified on the command line, optimization options may be
specified individually (oa, oi) or the letters may be strung together
(oailt).
.*
:OPTLIST.
.*
.if &e'&$SWoa eq 1 .do begin
:OPT refid='SWoa' name='oa'.
.ix 'options' 'oa'
.ix 'aliasing'
Alias checking is relaxed.
When this option is specified, the code optimizer will assume that
global variables are not indirectly referenced through pointers.
This assumption may reduce the size of the code that is generated.
The following example helps to illustrate this point.
.exam begin 10
extern int i;

void rtn( int *pi )
{
    int k;
    for( k = 0; k < 10; ++k ) {
        (*pi)++;
        i++;
    }
}
.exam end
.pc
In the above example, if "i" and "*pi" referenced the same integer
object then "i" would be incremented by 2 each time through the
"for" loop and we would call the pointer reference "*pi" an alias
for the variable "i".
In this situation, the compiler could not bind the variable "i" to
a register without making sure that the "in-memory" copy of "i" was
kept up-to-date.
In most cases, the above situation does not arise.
Rarely would we reference the same variable directly by name and
indirectly through a pointer in the same routine.
The "oa" option instructs the code generator that such cases do not
arise in the module to be compiled.
The code generator will be able to produce more efficient code when
it does not have to worry about the alias "problem".
.np
.ix 'macros' '__SW_OA'
The macro
.kwm __SW_OA
will be predefined if "oa" is selected.
.do end
.*
.if &e'&$SWob eq 1 .do begin
:OPT refid='SWob' name='ob'.
.ix 'options' 'ob'
When the "ob" option is specified, the code generator will try to
order the blocks of code emitted such that the "expected" execution
path (as determined by a set of simple heuristics) will be straight
through, with other cases being handled by jumps to separate blocks of
code "out of line".
This will result in better cache utilization on the Pentium.
If the heuristics do not apply to your code, it could result in a
performance decrease.
.do end
.*
.if &e'&$SWoc eq 1 .do begin
:OPT refid='SWoc' name='oc'.
.ix 'options' 'oc'
This option may be used to disable the optimization where a "CALL"
followed by a "RET" (return) is changed into a "JMP" (jump)
instruction.
.np
&286only.
This option is required if you wish to link an overlayed program using
the Microsoft DOS Overlay Linker.
The Microsoft DOS Overlay Linker will create overlay calls for a
"CALL" instruction only.
This option is not required when using the &lnkname..
.np
.ix 'macros' '__SW_OC'
The macro
.kwm __SW_OC
will be predefined if "oc" is selected.
.do end
.*
.if &e'&$SWod eq 1 .do begin
:OPT refid='SWod' name='od'.
.ix 'options' 'od'
Non-optimized code sequences are generated.
The resulting code will be much easier to debug when using the
&dbgname..
By default, the compiler will select "od" if "d2" is specified.
If "d2" is followed by one of the other "o?" options then "od" is
overridden.
.exam begin 1
&prompt.:SF font=1.compiler_name:eSF. report &sw.d2 &sw.os
.exam end
.np
.ix 'macros' '__SW_OD'
The macro
.kwm __SW_OD
will be predefined if "od" is selected.
.do end
.*
.if &e'&$SWoe eq 1 .do begin
:OPT refid='SWoe' name='oe'.=<num>
.ix 'options' 'oe'
Certain user functions are expanded in-line.
The criteria for which functions are selected for in-line expansion is
based on the "size" of the function in terms of the number of "tree nodes"
generated by the function.
Functions are internally represented as tree structures, where each
operand and each operator is a node of the tree.
For example, the statement
.mono a = -b * (c + d)
can be represented as a tree with 8 nodes, one for each operand and
operator.
.np
The number of "nodes" generated corresponds closely with the number
of operators used in an expression.
Functions which require more than "<num>" nodes are not expanded
in-line.
The default number is 20. With larger "<num>" values, more (larger)
functions will be expanded in-line.
This optimization is especially useful when locally-referenced
functions are small in size.
.exam begin 1
&prompt.:SF font=1.compiler_name:eSF. dhrystone &sw.oe
.exam end
.do end
.*
.if &e'&$SWoh eq 1 .do begin
:OPT refid='SWoh' name='oh'.
.ix 'options' 'oh'
This option enables repeated optimizations (which can result in longer
compiles).
.do end
.*
.if &e'&$SWoi eq 1 .do begin
:OPT refid='SWoi' name='oi'.
.ix 'options' 'oi'
Certain library functions are generated in-line.
You must include the appropriate header file containing the prototype
for the desired function so that it will be generated in-line.
The functions that can be generated in-line are:
.keep 8
.contents
abs~b~b~b~b~b~b~b
_disable~b~b
div~b~b~b~b~b~b~b
_enable~b~b~b
fabs~b~b~b~b~b~b
_fmemchr~b~b
_fmemcmp~b~b
_fmemcpy~b~b
_fmemset~b~b
_fstrcat~b~b
_fstrcmp~b~b
_fstrcpy~b~b
_fstrlen~b~b
inpd~b(2)~b~b
inpw~b~b~b~b~b~b
inp~b~b~b~b~b~b~b
labs~b~b~b~b~b~b
ldiv~b(2)~b~b
_lrotl~b(2)
_lrotr~b(2)
memchr~b~b~b~b
memcmp~b~b~b~b
memcpy~b~b~b~b
memset~b(1)
movedata~b~b
outpd~b(2)~b
outpw~b~b~b~b~b
outp~b~b~b~b~b~b
_rotl~b~b~b~b~b
_rotr~b~b~b~b~b
strcat~b~b~b~b
strchr~b~b~b~b
strcmp~b(1)
strcpy~b~b~b~b
strlen~b~b~b~b
.econtents
.illust begin
*1 16-bit only
*2 32-bit only
.illust end
.np
.ix 'macros' '__INLINE_FUNCTIONS__'
.ix 'macros' '__SW_OI'
The macros
.kwm __INLINE_FUNCTIONS__
and
.kwm __SW_OI
will be predefined if "oi" is selected.
.do end
.*
.if &e'&$SWoipls eq 1 .do begin
:OPT refid='SWoipls' name='oi+'.
.ix 'options' 'oi+'
(C++ only)
This option encompasses "oi" but also sets
.us inline_depth
to its maximum (255).
By default,
.us inline_depth
is 3.
The
.us inline_depth
can also be changed by using the C++
.mono inline_depth
pragma.
.*
.if &e'&$SWok eq 1 .do begin
:OPT refid='SWok' name='ok'.
.ix 'options' 'ok'
This option enables flowing of register save (from prologue) down into
the function's flow graph.
This means that register save/restores will not be executed when it is
not necessary (as can be the case when a function consists of an
if-else construct with a simple part that does little and a more
complex part that does a lot).
.do end
.*
.if &e'&$SWol eq 1 .do begin
:OPT refid='SWol' name='ol'.
.ix 'options' 'ol'
Loop optimizations are performed.
This includes moving loop-invariant expressions outside the loops.
.ix 'macros' '__SW_OL'
The macro
.kwm __SW_OL
will be predefined if "ol" is selected.
.do end
.*
.if &e'&$SWolpls eq 1 .do begin
:OPT refid='SWolpls' name='ol+'.
.ix 'options' 'ol+'
Loop optimizations are performed including loop unrolling.
This includes moving loop-invariant expressions outside the loops
and turning some loops into straight-line code.
.ix 'macros' '__SW_OL'
The macro
.kwm __SW_OL
will be predefined if "ol+" is selected.
.do end
.*
.if &e'&$SWom eq 1 .do begin
:OPT refid='SWom' name='om'.
Generate in-line 80x87 code for math functions like sin, cos, tan, etc.
If this option is selected, it is the programmer's responsibility to
make sure that arguments to these functions are within the range
accepted by the
.id fsin, fcos,
etc. instructions since no run-time check is made.
For 16-bit, you must also include the "fp3" option to get in-line
80x87 code (except for fabs).
The functions that can be generated in-line are:
.contents
atan~b
cos~b~b
exp~b~b
fabs~b
log10
log~b~b
sin~b~b
sqrt~b
tan~b~b
.econtents
.np
.ix 'macros' '__SW_OM'
The macro
.kwm __SW_OM
will be predefined if "om" is selected.
.do end
.*
.if &e'&$SWon eq 1 .do begin
:OPT refid='SWon' name='on'.
.ix 'options' 'on'
This option allows the compiler to replace floating-point divisions
with multiplications by the reciprocal.
This generates faster code, but the result may not be the same because
the reciprocal may not be exactly representable.
.ix 'macros' '__SW_ON'
The macro
.kwm __SW_ON
will be predefined if "on" is selected.
.do end
.*
.if &e'&$SWoo eq 1 .do begin
:OPT refid='SWoo' name='oo'.
.ix 'options' 'oo'
By default, the compiler will abort compilation if it runs low on memory.
This option forces the compiler to continue compilation even when low
on memory, however, this can result in very poor code being generated.
.ix 'macros' '__SW_OO'
The macro
.kwm __SW_OO
will be predefined if "oo" is selected.
.do end
.*
.if &e'&$SWop eq 1 .do begin
:OPT refid='SWop' name='op'.
.ix 'options' 'op'
This option causes the compiler to store intermediate floating-point
results into memory in order to generate consistent floating-point
results rather than keeping values in the 80x87 registers where they
have more precision.
.ix 'macros' '__SW_OP'
The macro
.kwm __SW_OP
will be predefined if "op" is selected.
.do end
.*
.if &e'&$SWor eq 1 .do begin
:OPT refid='SWor' name='or'.
.ix 'options' 'or'
This option enables reordering of instructions (instruction scheduling)
to achieve better performance on pipelined architectures such as the
Intel 486 and Pentium processors.
This option is essential for generating fast code for the Intel
Pentium processor.
Selecting this option will make it slightly more difficult to debug
because the assembly language instructions generated for a source
statement may be intermixed with instructions generated for surrounding
statements.
.ix 'macros' '__SW_OR'
The macro
.kwm __SW_OR
will be predefined if "or" is selected.
.do end
.*
.if &e'&$SWos eq 1 .do begin
:OPT refid='SWos' name='os'.
.ix 'options' 'os'
Space is favoured over time when generating code (smaller code
but possibly slower execution).
By default, the compiler selects a balance between "space" and
"time".
.ix 'macros' '__SW_OS'
The macro
.kwm __SW_OS
will be predefined if "os" is selected.
.do end
.*
.if &e'&$SWot eq 1 .do begin
:OPT refid='SWot' name='ot'.
.ix 'options' 'ot'
Time is favoured over space when generating code (faster
execution but possibly larger code).
By default, the compiler selects a balance between "space" and
"time".
.ix 'macros' '__SW_OT'
The macro
.kwm __SW_OT
will be predefined if "ot" is selected.
.do end
.*
.if &e'&$SWou eq 1 .do begin
:OPT refid='SWou' name='ou'.
.ix 'options' 'ou'
This option forces the compiler to make sure that all function labels
are unique.
Thus the compiler will not place two function labels at the same
address even if the code for the two functions are identical.
This option is automatically selected if the "za" option is specified.
.ix 'macros' '__SW_OU'
The macro
.kwm __SW_OU
will be predefined if "ou" is selected.
.do end
.*
.if &e'&$SWox eq 1 .do begin
:OPT refid='SWox' name='ox'.
.ix 'options' 'ox'
.ix 'fastest code'
.ix 'execution' 'fastest'
&x86only.
The "obmiler" and "s" (no stack overflow checking) options are
selected.
.if '&alpha' eq 'AXP' .do begin
.np
&AXPonly.
The "obiler" and "s" (no stack overflow checking) options are selected.
.do end
.do end
.*
.if &e'&$SWoz eq 1 .do begin
:OPT refid='SWoz' name='oz'.
.ix 'options' 'oz'
This option prevents the compiler from omitting NULL pointer checks on
pointer conversions.
By default, the compiler omits NULL pointer checks on pointer
conversions when it is safe to do so.
Consider the following example.
.millust begin
struct B1 {
    int b1;
};
struct B2 {
    int b2;
};
struct D : B1, B2 {
    int d;
};

void clear_D( D *p )
{
        p->d = 0;
        B1 *p1 = p;
        p1->b1 = 0;
        B2 *p2 = p;
        p2->b2 = 0;
}
.millust end
.pc
In this example, the C++ compiler must ensure that
.id p1
and
.id p2
become NULL if
.id p
is NULL (since no offset adjustment is allowed for a NULL pointer).
However, the first executable statement implies that
.id p
is not NULL since, in most operating environments, the executing
program would crash at the first executable statement if
.id p
was NULL.
The "oz" option will prevent the compiler from omitting the check for
a NULL pointer.
.np
.ix 'macros' '__SW_OZ'
The macro
.kwm __SW_OZ
will be predefined if "oz" is selected.
.do end
.*
:eOPTLIST.
.np
When "ox" is combined with the "on", "oa" and "ot" options
("onatx") and the "zp4" option, the code generator will attempt to
give you the fastest executing code possible irrespective of
architecture.
Other options can give you architecture specific optimizations to
further improve the speed of your code.
Note that specifying "onatx" is equivalent to specifying "onatblimer"
and "s".
See the section entitled :HDREF refid='wccfast'. for more information
on generating fast code.
.*
.section C++ Exception Handling
.*
.np
.ix 'exception handling'
The "xd..." options disable exception handling.
Consequently, it is not possible to use
.kw throw
.ct ,
.kw try
.ct , or
.kw catch
statements, or to specify a function exception specification.
If your program (or a library which it includes) throws exceptions,
then one of the "xs..." options should be used to compile all the
modules in your program; otherwise, any active objects created within
the module will not be destructed during exception processing.
.np
Multiple schemes are possible, allowing experimentation to determine
the optimal scheme for particular circumstances.
You can mix and match schemes on a module basis, with the proviso
that exceptions should be enabled wherever it is possible that a
created object should be destructed by the exception mechanism.
.*
:OPTLIST.
.*
.if &e'&$SWxd eq 1 .do begin
:OPT refid='SWxd' name='xd'.
.ix 'options' 'xd'
This option disables exception handling.
It is the default option if no exception handling option is specified.
When this option is specified (or defaulted):
.begbull
.bull
Destruction of objects is caused by direct calls to the appropriate
destructors
.bull
Destructor functions are implemented with direct calls to appropriate
destructors to destruct base classes and class members.
.endbull
.do end
.*
.if &e'&$SWxdt eq 1 .do begin
:OPT refid='SWxdt' name='xdt'.
.ix 'options' 'xdt'
This option is the same as "xd" :optref refid='SWxd'..
.do end
.*
.if &e'&$SWxds eq 1 .do begin
:OPT refid='SWxds' name='xds'.
.ix 'options' 'xds'
This option disables exception handling.
When this option is specified:
.begbull
.bull
Destruction of objects is caused by direct calls to the appropriate
destructors.
.bull
Destruction of base classes and class members is accomplished by
interpreting tables.
.bull
This option, in general, generates smaller code, with increased
execution time and with more run-time system routines included by
the linker.
.endbull
.do end
.*
.if &e'&$SWxs eq 1 .do begin
:OPT refid='SWxs' name='xs'.
.ix 'options' 'xs'
This option enables exception handling using a balanced scheme.
When this option is specified:
.begbull
.bull
Tables are interpreted to effect destruction of temporaries and
automatic objects; destructor functions are implemented with direct
calls to appropriate destructors to destruct base classes and class
members.
.endbull
.do end
.*
.if &e'&$SWxst eq 1 .do begin
:OPT refid='SWxst' name='xst'.
.ix 'options' 'xst'
This option enables exception handling using a time-saving scheme.
When this option is specified:
.begbull
.bull
Destruction of temporaries and automatic objects is accomplished with
direct calls to appropriate destructors; destructor functions are
implemented with direct calls to appropriate destructors to destruct
base classes and class members.
.bull
This scheme will execute faster, but will use more space in general.
.endbull
.do end
.*
.if &e'&$SWxss eq 1 .do begin
:OPT refid='SWxss' name='xss'.
.ix 'options' 'xss'
This option enables exception handling using a space-saving scheme.
When this option is specified:
.begbull
.bull
Tables are interpreted to effect destruction of temporaries and
automatic objects; destruction of base classes and class members is
accomplished by interpreting tables.
.bull
This option, in general, generates smaller code, with increased
execution time.
.endbull
.do end
.*
:eOPTLIST.
.*
.section Double-Byte/Unicode Characters
.*
.np
This group of options deals with compile-time aspects of character
sets used in the source code.
.*
:OPTLIST.
.*
.if &e'&$SWzk eq 1 .do begin
:OPT refid='SWzk' name='zk'.{0,1,2,l}
.ix 'options' 'zk'
This option causes the compiler to recognize double-byte characters
in strings.
When the compiler scans a text string enclosed in quotes ("),
it will recognize the first byte of a double-byte character and
suppress lexical analysis of the second byte.
This will prevent the compiler from misinterpreting the second byte as
a "\" or quote (") character.
.begnote
.note zk, zk0
These options cause the compiler to process strings for Japanese
double-byte characters (range 0x81 - 0x9F and 0xE0 - 0xFC).
The characters in the range A0 - DF are single-byte Katakana.
.note zk1
This option causes the compiler to process strings for Traditional
Chinese and Taiwanese double-byte characters (range 0x81 - 0xFC).
.note zk2
This option causes the compiler to process strings for Korean Hangeul
double-byte characters (range 0x81 - 0xFD).
.note zkl
This option causes the compiler to process strings using the current
code page.
If the local character set includes double-byte characters then string
processing will check for lead bytes.
.endnote
.np
.ix 'macros' '__SW_ZK'
The macro
.kwm __SW_ZK
will be predefined if any "zk" option is selected.
.do end
.*
.if &e'&$SWzk0u eq 1 .do begin
:OPT refid='SWzk0u' name='zk0u'.
.ix 'options' 'zk0u'
This option causes the compiler to process strings for Japanese
double-byte characters (range 0x81 - 0x9F and 0xE0 - 0xFC).
The characters in the range A0 - DF are single-byte Katakana.
All characters, including Kanji, in wide characters (L'c') and wide
strings (L"string") are translated to UNICODE.
.np
When the compiler scans a text string enclosed in quotes ("),
it will recognize the first byte of a double-byte character and
suppress lexical analysis of the second byte.
This will prevent the compiler from misinterpreting the second byte as
a "\" or quote (") character.
.do end
.*
.if &e'&$SWzku eq 1 .do begin
:OPT refid='SWzku' name='zku'.=<codepage>
.ix 'options' 'zku'
Characters in wide characters (L'c') and wide strings (L"string") are
translated to UNICODE.
The UNICODE translate table for the specified code page is loaded
from a file.
The compiler locates this file by searching the paths listed in the
.if '&target' eq 'QNX' .do begin
.ev ETC_PATH
.do end
.el .do begin
.ev PATH
.do end
environment variable.
.if '&target' eq 'QNX' .do begin
If the file cannot be located then the
.fi /usr/watcom
directory is searched.
.do end
The following table lists the supported code pages.
.millust begin
 codepage | character set   | file name
-------------------------------------------
    437   | US-ASCII (DOS)  | unicode.437
    850   | Latin-1 (DOS)   | unicode.850
    852   | Latin-2 (DOS)   | unicode.852
   1250   | Latin-2 (Win32) | unicode1.250
   1252   | Latin-1 (Win32) | unicode1.252
.millust end
.do end
.*
:eOPTLIST.
.*
.if &e'&$SWvc eq 1 .do begin
.*
.section Compatibility with Microsoft Visual C++
.*
.np
.ix 'options' 'compatibility with Visual C++'
This group of options deals with compatibility with Microsoft's Visual
C++ compiler.
.*
:OPTLIST.
.*
:OPT refid='SWvc' name='vc...'.&optdag.
.ix 'options' 'vc...'
The "vc" option prefix is used to introduce a set of Microsoft Visual
C++ compatibility options.
At present, there is only one: vcap.
.*
:OPT refid='SWvcap' name='vcap'.
.ix 'options' 'vcap'
This options tells the compiler to allow
.kw _alloca()
to be used in a parameter list.
The optimizer has to do extra work to allow this but since it is rare
(and easily worked around if you can), you have to ask the optimizer
to handle this case.
You also may get less efficient code in some cases.
.*
:eOPTLIST.
.*
.section Compatibility with Older Versions of the 80x86 Compilers
.*
.np
This group of options deals with compatibility with older versions of
&company's 80x86 compilers.
.*
:OPTLIST.
.*
.if &e'&$SWr eq 1 .do begin
:OPT refid='SWr' name='r'.&optdag.
.ix 'save/restore segment registers'
.ix 'options' 'r'
This option instructs the compiler to generate function prologue and
epilogue sequences that save and restore any segment registers that
are modified by the function.
Caution should be exercised when using this option.
If the value of the segment register being restored matches the value
of a segment that was freed within the function, a general protection
fault will occur in protected-mode environments.
When this option is used, the compiler also assumes that called functions
save and restore segment registers.
By default, the compiler does not generate code to save and restore
segment registers.
This option is provided for compatibility with the version 8.0 release.
.ix 'macros' '__SW_R'
.ix '__SW_R'
The macro
.kwm __SW_R
will be predefined if "r" is selected.
.do end
.*
.if &e'&$SWfpr eq 1 .do begin
:OPT refid='SWfpr' name='fpr'.
.ix 'options' 'fpr'
Use this option if you want to generate floating-point instructions
that will be compatible with version 9.0 or earlier of the compilers.
For more information on floating-point conventions see the sections
entitled :HDREF refid='prg87'. and :HDREF refid='prg387'..
.do end
.*
.if &e'&$SWzz eq 1 .do begin
:OPT refid='SWzz' name='zz'.
.ix 'options' 'zz'
Use this option if you want to generate __stdcall function names that
will be compatible with version 10.0 of the compilers.
When this option is omitted,
all C symbols (extern "C" symbols in C++) are suffixed by "@nnn" where
"nnn" is the sum of the argument sizes (each size is rounded up to a
multiple of 4 bytes so that char and short are size 4).
When the argument list contains "...", the "@nnn" suffix is omitted.
This convention is compatible with Microsoft.
For more information on the __stdcall convention see the section
entitled :HDREF refid='wccext'..
.do end
.*
:eOPTLIST.
.*
.endlevel
