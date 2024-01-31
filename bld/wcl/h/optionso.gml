:cmt.*****************************************************************************
:cmt.*
:cmt.*                            Open Watcom Project
:cmt.*
:cmt.* Copyright (c) 2002-2024 The Open Watcom Contributors. All Rights Reserved.
:cmt.*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
:cmt.*
:cmt.*  ========================================================================
:cmt.*
:cmt.*    This file contains Original Code and/or Modifications of Original
:cmt.*    Code as defined in and that are subject to the Sybase Open Watcom
:cmt.*    Public License version 1.0 (the 'License'). You may not use this file
:cmt.*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
:cmt.*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
:cmt.*    provided with the Original Code and Modifications, and is also
:cmt.*    available at www.sybase.com/developer/opensource.
:cmt.*
:cmt.*    The Original Code and all software distributed under the License are
:cmt.*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
:cmt.*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
:cmt.*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
:cmt.*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
:cmt.*    NON-INFRINGEMENT. Please see the License for the specific language
:cmt.*    governing rights and limitations under the License.
:cmt.*
:cmt.*  ========================================================================
:cmt.*
:cmt.* Description:  owcc command line options.
:cmt.*
:cmt.*****************************************************************************
:cmt.
:cmt. Source file uses UTF-8 encoding, ¥
:cmt.
:cmt. Definition of command line options to use by optencod utility to generate
:cmt.  	appropriate command line parser and usage text.
:cmt.
:cmt.
:cmt. GML Macros used:
:cmt.
:cmt.	:chain. <option> <option> ...	options that start with <option>
:cmt.					    can be chained together i.e.,
:cmt.					    -oa -ox -ot => -oaxt
:cmt.	:option. <option> <synonym> ... define an option
:cmt.	:title. <text>			English title usage text
:cmt.	:jtitle. <text>			Japanese title usage text
:cmt.	:titleu. <text>			English title usage text for QNX resource file
:cmt.	:jtitleu. <text>		Japanese title usage text for QNX resource file
:cmt.	:usagegroup. <group_id>		define group of options that have group
:cmt.					    <group_id> are grouped together in
:cmt.					    usage text
:cmt.	:usagechain. <group_id> <option>    
:cmt.                                   group of options that start with <option>
:cmt.					    are chained together in usage text for
:cmt.					    defined group <group_id>
:cmt.
:cmt.	:target. <targ1> <targ2> ...	valid for these targets (default is 'any')
:cmt.	:ntarget. <targ1> <targ2> ...	not valid for these targets
:cmt.	:immediate. <fn> [<usage argid>]
:cmt.					<fn> is called when option parsed
:cmt.	:code. <source-code>		<source-code> is executed when option parsed
:cmt.	:enumerate. <name> [<option>]	option is one value in <name> enumeration
:cmt.   :number. [<fn>] [<default>] [<usage argid>]
:cmt.					=<num> allowed; call <fn> to check
:cmt.	:id. [<fn>] [<usage argid>]	=<id> req'd; call <fn> to check
:cmt.	:char. [<fn>] [<usage argid>]	=<char> req'd; call <fn> to check
:cmt.	:file. [<usage argid>]		=<file> req'd
:cmt.	:path. [<usage argid>]		=<path> req'd
:cmt.	:special. <fn> [<usage argid>]	call <fn> to parse option
:cmt.
:cmt.	:optional.			value is optional
:cmt.	:internal.			option is undocumented
:cmt.	:prefix.			prefix of a :special. option
:cmt.	:nochain.			option isn't chained with other options
:cmt.					    in parser code
:cmt.	:timestamp.			kludge to record "when" an option
:cmt.					    is set so that dependencies
:cmt.					    between options can be simulated
:cmt.	:negate.			negate option value
:cmt.
:cmt.	:usage. <text>			English usage text
:cmt.	:jusage. <text>			Japanese usage text
:cmt.
:cmt.	:usagenochain.			option isn't chained with other options
:cmt.					    in usage text
:cmt.	:group. <group_id>		group <group_id> to which option is
:cmt.					    included
:cmt.
:cmt. Global macros
:cmt.
:cmt.	:noequal.			args can't have option '='
:cmt.	:argequal. <char>		args use <char> instead of '='
:cmt.
:cmt. where <targ>:
:cmt.   default - any, dbg, unused
:cmt.   architecture - i86, 386, x64, axp, ppc, mps, sparc
:cmt.   host OS - bsd, dos, linux, nov, nt, os2, osx, pls, qnx, rsi, haiku, rdos, win
:cmt.   extra - targ1, targ2
:cmt.
:cmt. The :jtitle. or :jusage. tag is required if no text is associated with the tag.
:cmt. Otherwise, English text defined with :title. or :use. tag will be used instead.
:cmt.
:cmt. If any required argument is blank then use '.' (dot) character as placeholder.
:cmt.


:title. Usage:  owcc [options] file ...

:title.  ..

:title.  Options:
:jtitle. オプション:

:title.  ..


:cmt.*************************************************************************
:cmt.
:cmt.Description:  Usage help for owcc.
:cmt.
:cmt.*************************************************************************

:option. b
:usage. compile and link for <target>
:argequal. ..
:id. . <target>

:option. c
:usage. compile only, no link

:option. o
:usage. set output file name
:argequal. ..
:file.

:option. v
:usage. show sub program invocations

:option. x
:usage. treat source files as C or C++ code
:argequal. ..
:id. . {c,c++}

:cmt
:cmt Processor options
:cmt
:usagegroup. 1
:usage. [Processor options]

:option. march
:usage. generate code for this architecture
:group. 1
:id. . {i86,i186,i286,i386,axp,mips,ppc}

:option. mregparm
:usage. calling convention
:number.
:group. 1

:option. mtune
:usage. optimize for this family
:group. 1
:id. . {i386,i486,i586,i686}

:cmt
:cmt x86 Floating-point processor options
:cmt
:usagegroup. 2
:usage. [x86 Floating-point processor options]

:option. fpmath
:usage. hardware floating-point code
:group. 2
:id. . {87,287,387}

:option. fptune
:usage. optimize f-p for CPU type
:group. 2
:id. . {586,686}

:option. mhard-emu-float
:usage. inline 80x87 instructions with emulation
:group. 2

:option. msoft-float
:usage. calls to floating-point library
:group. 2

:cmt
:cmt Compiler options
:cmt
:usagegroup. 3
:usage. [Compiler options]

:option. fbrowser
:usage. generate browsing information
:group. 3

:option. femit-names
:usage. emit routine names in the code
:group. 3

:option. ffunction-sections
:usage. functions in separate segments
:group. 3

:option. fgrow-stack
:usage. generate calls to grow the stack
:group. 3

:option. fhook-epilogue
:usage. call epilogue hook routine
:group. 3

:option. fhook-prologue
:usage. call prologue hook routine
:group. 3
:optional.
:number.

:option. fmessage-full-path
:usage. full paths in messages
:group. 3

:option. fno-rtti
:usage. (C++) disable RTTI
:group. 3

:option. fno-short-enum
:usage. force enums to be type int
:group. 3

:option. fno-stack-check
:usage. remove stack overflow checks
:group. 3

:option. fno-writable-strings
:usage. don't place strings in CODE segment
:group. 3

:option. fnonconst-initializers
:usage. allow non-constant initializers
:group. 3

:option. fnostdlib
:usage. remove default library information
:group. 3

:option. fo
:usage. set object file name
:group. 3
:file.

:option. fpack-struct
:usage. default struct member alignment
:group. 3
:id. . {1,2,4,8,16}

:option. fr
:usage. output errors to a file name
:group. 3
:file.

:option. fshort-enum
:usage. use minimum base type for enum
:group. 3

:option. fsigned-char
:usage. change char default to signed
:group. 3

:option. fstack-probe
:usage. touch stack through SS first
:group. 3

:option. fsyntax-only
:usage. check syntax only
:group. 3

:option. fvoid-ptr-arithmetic
:usage. allow arithmetic operations on (void *)
:group. 3

:option. fwrite-def
:usage. output func declarations to .def
:group. 3

:option. fwrite-def-without-typedefs
:usage. same, but skip typedef names
:group. 3

:usagechain. 3 mabi=
:usage. set calling convention

:option. mabi=cdecl
:usage. set calling conv. to __cdecl
:enumerate. mabi
:group. 3

:option. mabi=fastcall
:usage. set calling conv. to __fastcall
:enumerate. mabi
:group. 3

:option. mabi=fortran
:usage. set calling conv. to __fortran
:enumerate. mabi
:group. 3

:cmt :: _Optlink is undocumented
:cmt :option. mabi=optlink
:cmt :usage. set calling conv. to _Optlink
:cmt :enumerate. mabi
:cmt :group. 3

:option. mabi=pascal
:usage. set calling conv. to __pascal
:enumerate. mabi
:group. 3

:option. mabi=stdcall
:usage. set calling conv. to __stdcall
:enumerate. mabi
:group. 3

:option. mabi=system
:usage. set calling conv. to _System
:enumerate. mabi
:group. 3

:option. mabi=watcall
:usage. set calling conv. to __watcall
:enumerate. mabi
:group. 3

:usagechain. 3 mcmodel=
:usage. memory model

:option. mcmodel=c
:usage. memory model compact
:enumerate. mcmodel
:group. 3

:option. mcmodel=f
:usage. memory model flat
:enumerate. mcmodel
:group. 3

:option. mcmodel=h
:usage. memory model huge
:enumerate. mcmodel
:group. 3

:option. mcmodel=l
:usage. memory model large
:enumerate. mcmodel
:group. 3

:option. mcmodel=m
:usage. memory model medium
:enumerate. mcmodel
:group. 3

:option. mcmodel=s
:usage. memory model small
:enumerate. mcmodel
:group. 3

:option. mcmodel=t
:usage. memory model tiny
:enumerate. mcmodel
:group. 3

:option. mwindows
:usage. generate code for MS Windows
:group. 3

:option. shared
:usage. generate code for a DLL
:group. 3

:option. std
:usage. disable/enable extensions
:group. 3
:id. . {c89,c99,ow}

:option. Wall
:usage. set warning level to 4
:group. 3

:option. Wc,
:usage. pass any option to WCC
:group. 3
:special. . <option>

:option. Werror
:usage. treat all warnings as errors
:group. 3

:option. Wextra
:usage. enable maximal warnings
:group. 3

:option. Wlevel
:usage. set warning level number
:group. 3
:number.

:option. Wn
:usage. enable warning message <num>
:group. 3
:number.

:option. Wno-n
:usage. disable warning message <num>
:group. 3
:number.

:option. Woverlay
:usage. warn about problems in overlaying
:group. 3

:option. Wpadded
:usage. warn when padding a struct
:group. 3

:option. Wstop-after-errors
:usage. stop compiler after <num> errors
:group. 3
:number.

:cmt
:cmt Debugging options
:cmt
:usagegroup. 4
:usage. [Debugging options]

:option. g0
:usage. no debugging information
:group. 4

:option. g1{+}
:usage. line number debugging info
:group. 4

:option. g2
:usage. full symbolic debugging info
:group. 4

:option. g2i
:usage. (C++) + inlines as COMDATs
:group. 4

:option. g2s
:usage. (C++) + inlines as statics
:group. 4

:option. g2t
:usage. (C++) without type names
:group. 4

:option. g3
:usage. add info for unreferenced type names
:group. 4

:option. g3i
:usage. (C++) + inlines as COMDATs
:group. 4

:option. g3s
:usage. (C++) + inlines as statics
:group. 4

:option. gcodeview
:usage. codeview debug format
:group. 4

:option. gdwarf
:usage. dwarf debug format
:group. 4

:option. gwatcom
:usage. watcom debug format
:group. 4

:cmt
:cmt Optimization options
:cmt
:usagegroup. 5
:usage. [Optimization options]

:option. feh{,=s,=t}
:usage. exception handling: balanced, space, time
:group. 5

:option. ffloat-store
:usage. improve floating-point consistency
:group. 5

:option. fguess-branch-probability
:usage. branch prediction
:group. 5

:option. finline-functions
:usage. expand functions inline
:group. 5

:option. finline-intrinsics
:usage. inline intrinsic functions
:group. 5

:option. finline-intrinsics-max
:usage. inline intrinsic functions
:group. 5

:option. finline-limit
:usage. which functions to expand inline
:group. 5
:number.

:option. finline-math
:usage. generate inline math functions
:group. 5

:option. fkeep-duplicates
:usage. ensure unique addresses for functions
:group. 5

:option. floop-optimize
:usage. perform loop optimizations
:group. 5

:option. fno-omit-frame-pointer
:usage. generate traceable stack frames
:group. 5

:option. fno-omit-leaf-frame-pointer
:usage. generate more stack frames
:group. 5

:option. fno-optimize-sibling-calls
:usage. disable call/ret optimization
:group. 5

:option. fno-strict-aliasing
:usage. relax alias checking
:group. 5

:option. frerun-optimizer
:usage. enable repeated optimizations
:group. 5

:option. fschedule-insns
:usage. re-order instructions to avoid stalls
:group. 5

:option. fschedule-prologue
:usage. control flow entry/exit seq.
:group. 5

:option. funroll-loops
:usage. perform loop unrolling
:group. 5

:option. funsafe-math-optimizations
:usage. numerically unstable floating-point
:group. 5

:option. O0
:usage. disable optimizations
:group. 5

:option. Os
:usage. optimize for space
:group. 5

:option. Ot
:usage. optimize for time
:group. 5

:option. O{1,2,3}
:usage. optimization presets
:group. 5

:cmt
:cmt Preprocessor options
:cmt
:usagegroup. 6
:usage. [Preprocessor options]

:option. C
:usage. preserve comments
:group. 6

:option. D
:usage. define a macro
:group. 6
:special. . <name>[=text]

:option. D+
:usage. extend syntax of -D option
:group. 6

:option. E
:usage. preprocess source file
:group. 6

:option. fcpp-wrap
:usage. wrap output at column <num>
:group. 6
:number.

:option. fmangle-cpp
:usage. encrypt C++ names
:group. 6


:option. ftabstop
:usage. # of spaces in tab stop
:group. 6
:number.

:option. I
:usage. include directory
:group. 6
:path.

:option. include
:usage. force pre-include of file
:group. 6
:file.

:option. MD
:usage. output autodepend make rule
:group. 6

:option. MF
:usage. autodepend output file name
:group. 6
:file.

:option. MMD
:usage. output autodepend make rule
:group. 6

:option. MT
:usage. autodepend target name
:group. 6
:special. . <target>

:option. P
:usage. don't insert #line directives
:group. 6

:option. pil
:usage. ignore #line directives
:group. 6

:option. tp
:usage. (C) set #pragma on <name>
:group. 6
:special. . <name>

:option. U
:usage. undefine macro name
:group. 6
:special. . <name>

:cmt
:cmt Linker options
:cmt
:usagegroup. 7
:usage. [Linker options]

:option. fd
:usage. save WLINK directives to file
:group. 7
:file.
:optional.

:option. fm
:usage. generate map file
:group. 7
:file.
:optional.

:option. mrtdll
:usage. build with dll run-time library
:group. 7

:option. mstack-size
:usage. set stack size
:group. 7
:number. . . =<stack_size>

:option. mthreads
:usage. build Multi-thread application
:group. 7

:option. s
:usage. strip debug info from executable
:group. 7

:option. Wl,
:usage. pass any directive to WLINK
:group. 7
:special. . <option>
