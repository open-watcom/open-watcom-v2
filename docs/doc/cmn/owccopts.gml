.sr x86only=
.sr 286only=
.sr 386only=
.sr AXPonly=
.*
.note march=i{,1,2,3}86,axp,mips,ppc
which CPU architecture instruction set is used
.*
.note mtune=i{3,4,5,6}86
which x86 CPU type to optimize for
.*
.note mregparm=1
use register-based argument passing conventions (default)
.*
.note mregparm=0
use stack-based argument passing conventions
.*
.note MMD
generate auto depend makefile fragment
.*
.note MF <file>
change name of makefile style auto depend file. 
Without this option, the filename is the same as the the base name of the
source file, with a suffix of ".d".
.*
.note MT <target>
specify target name generated in makefile style auto depend different than
that of the object file name
.*
.note mconsole
build target is a console application
.*
.note shared
build target is a Dynamic Link Library (DLL)
.*
.note mwindows
build target is a GUI application
.*
.note mthreads
build target is a multi-thread environment
.* 
.note mrtdll
build target uses DLL version of C/C++ run-time libraries
.*
.note mdefault-windowing
build target uses default windowing support
.*
.note g0
(C++ only)
no debugging information
.*
.note g1
line number debugging information
.*
.note g1+
(C only)
line number debugging information plus typing information for global
symbols and local structs and arrays
.*
.note g2
full symbolic debugging information
.*
.note g2i
(C++ only)
d2 and debug inlines; emit inlines as external out-of-line functions
.*
.note g2s
(C++ only)
d2 and debug inlines; emit inlines as static out-of-line functions
.*
.note g2t
(C++ only)
full symbolic debugging information, without type names
.*
.note g3
full symbolic debugging with unreferenced type names
,*
.note g3i
(C++ only)
d3 plus debug inlines; emit inlines as external out-of-line functions
.*
.note g3s
(C++ only)
d3 plus debug inlines; emit inlines as static out-of-line functions
.*
.note g{w,d,c}
set debug output format (&company, Dwarf, Codeview)
.*
.note D<name>[=text]
preprocessor #define name [text]
.*
.note D+
allow extended -D macro definitions
.*
.note fbrowser
generate browsing information
.*
.note Wstop-after-errors=<number>
set error limit number (default is 20)
.*
.note mabi={cdecl,stdcall,fastcall,pascal,fortran,syscall,watcall}
set default calling convention
.*
.note fhook-epilogue
&x86only.
call epilogue hook routine
.*
.note fmessage-full-path
use full path names in error messages
.*
.note fno-short-enum
force enum base type to use at least an int
.*
.note fshort-enum
force enum base type to use minimum
.*
.note femit-names
&x86only.
emit routine name before prologue
.*
.note fhook-prologue[=<number>]
&x86only.
call prologue hook routine with number of stack bytes available
.*
.note include <file_name>
force file_name to be included in front of the source file text
.*
.note fo=<file_name>
set object or preprocessor output file specification
.*
.note msoft-float
&x86only.
generate calls to floating-point library
.*
.note fpmath=287
&x86only.
generate in-line 80x87 instructions
.*
.note fpmath=387
&x86only.
generate in-line 387 instructions
.*
.note fptune=586&optdag.
&x86only.
generate in-line 80x87 instructions optimized for Pentium processor
.*
.note fptune=686&optdag.
&x86only.
generate in-line 80x87 instructions optimized for Pentium Pro processor
.*
.note fr=<file_name>
enable error file creation and specify its name
.*
.note H
(C only)
track include file opens
.*
.note I
add directory to the list of include directories
.*
.note fsigned-char
change char default from unsigned to signed
.*
.note k
(C++ only)
continue processing files (ignore errors)
.*
.note mcmodel={f,s,m,c,l,h}
&x86only.
select a memory model from these choices:
.begnote $compact
.note f;flat 
.note s;small 
.note m;medium 
.note c;compact 
.note l;large 
.note h;huge 
.note t
&286only.
compile code for the small memory model and then use the &lnkname to
generate a "COM" file
.endnote
The default is small for 16-bit and Netware, flat for 32-bit targets.
.*
.note O0
turn off all optimization
.*
.note O1
enable some optimazion
.*
.note O2
enable most of the usual optimizations
.*
.note O3
enable even more optimizations
.*
.note fno-strict-aliasing
relax alias checking
.*
.note fguess-branch-probability
branch prediction
.*
.note fno-optimize-sibling-calls
disable call/ret optimization
.*
.note finline-functions
expand functions inline
.*
.note finline-limit=num
which functions to expand inline
.*
.note fno-omit-frame-pointer
generate traceable stack frames
.*
.note fno-omit-leaf-frame-pointer
generate more stack frames
.*
.note frerun-optimizer
enable repeated optimizations
.*
.note finline-intrinsics[-max]
inline intrinsic functions [-max: more aggressively]
.*
.note fschedule-prologue
control flow entry/exit seq.
.*
.note floop-optimize
perform loop optimizations
.*
.note funroll-loops
perform loop unrolling
.*
.note finline-math
generate inline math functions
.*
.note funsafe-math-optimizations
numerically unstable floating-point
.*
.note ffloat-store
improve floating-point consistency
.*
.note fschedule-insns
re-order instructions to avoid stalls
.*
.note fkeep-duplicates
ensure unique addresses for functions
.*
.note fignore-line-directives
preprocessor ignores #line directives
.*
.note E
preprocess sources, sending output to standard output or filename
selected via -o
.*
.note C
include original comments in -E output
.*
.note P
don't include #line directives in -E output
.*
.note fcpp-wrap=<num>
wrap output lines at <num> columns (zero means no wrap)
.*
.note ftabstop=<num>
(C++ only)
set tab stop multiplier
.*
.note fno-stack-check
remove stack overflow checks
.*
.note fgrow-stack
&x86only.
generate calls to grow the stack
.*
.if '&alpha' eq 'AXP' .do begin
.note si 
&AXPonly.
FIXME: translate this!?
initialize stack frame storage with pattern
.do end
.*
.note fstack-probe
&x86only.
touch stack through SS first
.*
.note U <name>
preprocessor #undef name
.*
.note fwrite-def
output function declarations to .def file (with typedef names)
.*
.note w
turn off all warnings (same as Wlevel0)
.*
.note Wall
turn on most warnings, but not all (same as Wlevel4)
.*
.note Wlevel<number>
set warning level number (default is w1)
.*
.note Wextra
set warning level to maximum setting
.*
.note Wno-n<num>
warning control: disable warning message <num>
.*
.note Wn<num>
warning control: enable warning message <num>
.*
.note Werror
treat all warnings as errors
.*
.note Woverlay
(C only)
&286only.
warn about problems with overlaid code
.*
.note frtti
(C++ only) enable RTTI
.*
.note fno-eh
(C++ only) disable exception handling (default)
.*
.note feh
(C++ only) enable exception handling
.*
.note feh-direct
(C++ only) enable exception handling (direct calls for destruction)
.*
.note feh-table
(C++ only) enable exception handling (table-driven destructors)
.*
.note std={c89,c99,ow}
select language dialect; c89 is (almost) strictly ANSI/ISO standard C89 only,
c99 enables C99 support (may be incomplete),
ow enables all &cmpname extensions.
.*
.note fno-writable-strings
place literal strings in code segment
.*
.note fvoid-ptr-arithmetics&optdag.
(C only, Unix extension) enable arithmetic on void derived types
.*
.note fwrite-def-without-typedefs
output function declarations to .def (without typedef names)
.*
.note fnostdlib
suppress generation of library file names and references in object
file
.*
.note ffunction-sections
place each function in separate segment (near functions not allowed)
.*
.note fpack-struct=[{1,2,4,8,16}]
set minimal structure packing (member alignment)
.*
.if '&alpha' eq 'AXP' .do begin
.note zps
&AXPonly.
FIXME: map this to a long-name option!?
always align structs on qword boundaries
.do end
.*
.note Wpadded
output warning when padding is added in a struct/class
.*
.note finline-fp-rounding
inline floating point rounding code
.*
.note fomit-fp-rounding
omit floating point rounding code
.*
.note fsyntax-only
syntax check only
.*
