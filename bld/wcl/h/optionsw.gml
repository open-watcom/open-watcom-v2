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
:cmt.* Description:  wcl command line options.
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


:title. Usage:  wclaxp [options] file(s)
:target. axp
:title. Usage:  wclppc [options] file(s)
:target. ppc
:title. Usage:  wclmps [options] file(s)
:target. mps
:title. Usage:  wcl386 [options] file(s)
:target. 386
:title. Usage:  wcl [options] file(s)
:target. i86

:title.  ..

:title.  Options:
:jtitle. オプション:
:title.  .         ( /option is also accepted )
:jtitle. .         ( /ｵﾌﾟｼｮﾝ でも指定できます )
:ntarget. bsd linux osx qnx haiku


:chain. p

:option. c
:usage. compile only, no link

:option. cc
:usage. treat source files as C code

:option. cc++
:usage. treat source files as C++ code

:option. y
:usage. ignore the WCLAXP environment variable
:target. axp
:option. y
:usage. ignore the WCLPPC environment variable
:target. ppc
:option. y
:usage. ignore the WCLMPS environment variable
:target. mps
:option. y
:usage. ignore the WCL386 environment variable
:target. 386
:option. y
:usage. ignore the WCL environment variable
:target. i86

:usagegroup. 1
:usage. [Processor options]

:option. 3r
:usage. 386 register calling conventions
:target. 386
:group. 1
:option. 3s
:usage. 386 stack calling conventions
:target. 386
:group. 1
:option. 4r
:usage. 486 register calling conventions
:target. 386
:group. 1
:option. 4s
:usage. 486 stack calling conventions
:target. 386
:group. 1
:option. 5r
:usage. Pentium register calling conv.
:target. 386
:group. 1
:option. 5s
:usage. Pentium stack calling conventions
:target. 386
:group. 1
:option. 6r
:usage. Pentium Pro register call conven.
:target. 386
:group. 1
:option. 6s
:usage. Pentium Pro stack call conven.
:target. 386
:group. 1

:option. 0
:usage. generate code for 8086 or higher
:target. i86
:group. 1
:option. 1
:usage. generate code for 186 or higher
:target. i86
:group. 1
:option. 2
:usage. generate code for 286 or higher
:target. i86
:group. 1
:option. 3
:usage. generate code for 386 or higher
:target. i86
:group. 1
:option. 4
:usage. generate code optimized for 486
:target. i86
:group. 1
:option. 5
:usage. generate code optimized for Pentium
:target. i86
:group. 1
:option. 6
:usage. generate code opt. for Pentium Pro
:target. i86
:group. 1

:usagegroup. 2
:usage. [Floating-point processor options]

:option. fpc
:usage. calls to floating-point library
:target. i86 386
:group. 2
:option. fpd
:usage. enable Pentium FDIV check
:target. i86 386
:group. 2
:option. fpi
:usage. inline 80x87 with emulation
:target. i86 386
:group. 2
:option. fpi87
:usage. inline 80x87
:target. i86 386
:group. 2
:option. fpr
:usage. use old floating-point conventions
:target. i86 386
:group. 2
:option. fp2
:usage. generate 287 floating-point code
:target. i86 386
:group. 2
:option. fp3
:usage. generate 387 floating-point code
:target. i86 386
:group. 2
:option. fp5
:usage. optimize f-p for Pentium
:target. i86 386
:group. 2
:option. fp6
:usage. optimize f-p for Pentium Pro
:target. i86 386
:group. 2

:usagegroup. 3
:usage. [Compiler options]

:option. bcl
:usage. compile and link for <os>
:id. . <os>
:group. 3
:option. bt
:usage. compile for target <os>
:id. . <os>
:group. 3
:option. db
:usage. generate browsing information
:group. 3
:option. e
:usage. set error limit number <num>
:number.
:group. 3
:option. ecc
:usage. set calling conv. to __cdecl
:target. i86 386
:group. 3
:option. ecd
:usage. set calling conv. to __stdcall
:target. i86 386
:group. 3
:option. ecf
:usage. set calling conv. to __fastcall
:target. i86 386
:group. 3
:cmt. :cmt. not officially supported
:cmt. :option. eco
:cmt. :usage. set calling conv. to _Optlink
:cmt. :target. i86 386
:cmt. :group. 3
:option. ecp
:usage. set calling conv. to __pascal
:target. i86 386
:group. 3
:option. ecr
:usage. set calling conv. to __fortran
:target. i86 386
:group. 3
:option. ecs
:usage. set calling conv. to __syscall
:target. i86 386
:group. 3
:option. ecw
:usage. set calling conv. to __watcall
:target. i86 386
:group. 3
:option. ee
:usage. call epilogue hook routine
:group. 3
:option. ef
:usage. full paths in messages
:group. 3
:option. ei
:usage. force enums to be type int
:group. 3
:option. em
:usage. minimum base type for enum is int
:group. 3
:option. en
:usage. emit routine names in the code
:group. 3
:option. ep
:usage. call prologue hook routine <num>
:number.
:optional.
:group. 3
:option. eq
:usage. do not display error messages
:group. 3
:option. et
:usage. P5 profiling
:target. 386
:group. 3
:option. ez
:usage. generate PharLap EZ-OMF object
:target. 386
:group. 3
:option. fh
:usage. pre-compiled headers
:file.
:group. 3
:option. fhq
:usage. -fh without warnings
:file.
:optional.
:group. 3
:option. fhr
:usage. (C++) only read PCH
:group. 3
:option. fhw
:usage. (C++) only write PCH
:group. 3
:option. fhwe
:usage. (C++) don't count PCH warnings
:group. 3
:option. fi
:usage. force include of file
:file.
:group. 3
:option. fo
:usage. set object file name
:file.
:group. 3
:option. fr
:usage. set error file name
:file.
:group. 3
:option. ft
:usage. (C++) check for 8.3 file names
:group. 3
:option. fx
:usage. (C++) no check for 8.3 file names
:group. 3
:option. g
:usage. set code group name
:id. . <name>
:target. i86 386
:group. 3
:option. hc
:usage. codeview debug format
:group. 3
:option. hd
:usage. dwarf debug format
:group. 3
:option. hw
:usage. watcom debug format
:group. 3
:option. j
:usage. change char default to signed
:group. 3
:option. m{f,s,m,c,l}
:usage. memory model
:target. 386
:group. 3
:option. m{t,s,m,c,l,h}
:usage. memory model
:target. i86
:group. 3
:option. nc
:usage. set CODE class name
:id. . <name>
:target. i86 386
:group. 3
:option. nd
:usage. set data segment name
:id. . <name>
:target. i86 386
:group. 3
:option. nm
:usage. set module name
:id. . <name>
:target. i86 386
:group. 3
:option. nt
:usage. set text segment name
:id. . <name>
:target. i86 386
:group. 3
:option. q
:usage. operate quietly
:target. i86 386
:group. 3
:option. r
:usage. save/restore segregs across calls
:target. i86 386
:group. 3
:option. ri
:usage. promote function args/rets to int
:group. 3
:option. s
:usage. remove stack overflow checks
:group. 3
:option. sg
:usage. generate calls to grow the stack
:target. i86 386
:group. 3
:option. st
:usage. touch stack through SS first
:target. i86 386
:group. 3
:option. v
:usage. output func declarations to .def
:group. 3
:option. vcap
:usage. VC++ compat: alloca in arg lists
:group. 3
:option. w
:usage. set warning level number <num>
:number.
:group. 3
:option. wcd
:usage. disable warning message <num>
:number.
:group. 3
:option. wce
:usage. enable warning message <num>
:number.
:group. 3
:option. we
:usage. treat all warnings as errors
:group. 3
:option. wx
:usage. set warning level to max
:group. 3
:option. wo
:usage. diagnose problems in overlaid code
:target. i86
:group. 3
:option. x
:usage. preprocessor ignores env.variables
:group. 3
:option. xr
:usage. (C++) enable RTTI
:group. 3
:option. z{a,e}
:usage. disable/enable extensions
:group. 3
:option. zc
:usage. place strings in CODE segment
:group. 3
:option. zd{f,p}
:usage. DS floats vs DS pegged to DGROUP
:target. i86 386
:group. 3
:option. zdl
:usage. load DS directly from DGROUP
:target. 386
:group. 3
:option. zf{f,p}
:usage. FS floats vs FS pegged to seg
:target. i86 386
:group. 3
:option. zg{f,p}
:usage. GS floats vs GS pegged to seg
:target. i86 386
:group. 3
:option. zg
:usage. function prototype using base type
:group. 3
:option. zk{0,0u,1,2,3,l}
:usage. double-byte support
:group. 3
:option. zku=<codepage>
:usage. UNICODE support
:group. 3
:option. zl
:usage. remove default library information
:group. 3
:option. zld
:usage. remove file dependency information
:group. 3
:option. zm
:usage. place functions in separate segments
:group. 3
:option. zmf
:usage. (C++) -zm with near calls allowed
:group. 3
:option. zp
:usage. struct packing align {1,2,4,8,16}
:number.
:group. 3
:option. zpw
:usage. warning when padding a struct
:group. 3
:option. zq
:usage. operate quietly
:group. 3
:option. zs
:usage. check syntax only
:group. 3
:option. zt
:usage. set data threshold to <num>
:number.
:group. 3
:option. zu
:usage. SS != DGROUP
:target. i86 386
:group. 3
:option. zv
:usage. (C++) enable virt. fun. removal opt
:group. 3
:option. zw
:usage. generate code for MS Windows
:target. 386
:group. 3
:option. zW
:usage. -zw with efficient entry sequences
:target. i86
:group. 3
:option. zws
:usage. -zw with smart callbacks
:target. i86
:group. 3
:option. zWs
:usage. -zW with smart callbacks
:target. i86
:group. 3
:option. zz
:usage. remove @size from __stdcall func.
:target. 386
:group. 3

:usagegroup. 4
:usage. [Debugging options]

:option. d0
:usage. no debugging information
:group. 4
:option. d1
:usage. line number debugging info
:group. 4
:option. d1+
:usage. (C) line number and types debugging info
:group. 4
:option. d2
:usage. full symbolic debugging info
:group. 4
:option. d2i
:usage. (C++) -d2 and inlines as COMDATs
:group. 4
:option. d2s
:usage. (C++) -d2 and inlines as statics
:group. 4
:option. d2t
:usage. (C++) -d2 but without type names
:group. 4
:option. d3
:usage. debug info with unref'd type names
:group. 4
:option. d3i
:usage. (C++) -d3 and inlines as COMDATs
:group. 4
:option. d3s
:usage. (C++) -d3 and inlines as statics
:group. 4

:usagegroup. 5
:usage. [Optimization options]

:option. oa
:usage. relax alias checking
:group. 5
:option. ob
:usage. branch prediction
:target. i86 386
:group. 5
:option. oc
:usage. disable call/ret optimization
:target. i86 386
:group. 5
:option. od
:usage. disable optimizations
:group. 5
:option. oe
:usage. expand functions inline (<num> max size)
:number.
:optional.
:group. 5
:option. of[+]
:usage. generate traceable stack frames
:target. i86 386
:group. 5
:option. oh
:usage. enable repeated optimizations
:group. 5
:option. oi
:usage. inline intrinsic functions
:group. 5
:option. oi+
:usage. (C++) -oi with max inlining depth
:group. 5
:option. ok
:usage. control flow entry/exit seq.
:group. 5
:option. ol
:usage. perform loop optimizations
:group. 5
:option. ol+
:usage. -ol with loop unrolling
:group. 5
:option. om
:usage. generate inline math functions
:target. i86 386
:group. 5
:option. on
:usage. numerically unstable floating-point
:group. 5
:option. oo
:usage. continue compile when low on memory
:group. 5
:option. op
:usage. improve floating-point consistency
:target. i86 386
:group. 5
:option. or
:usage. re-order instructions to avoid stalls
:group. 5
:option. os
:usage. optimize for space
:group. 5
:option. ot
:usage. optimize for time
:group. 5
:option. ou
:usage. ensure unique addresses for functions
:group. 5
:option. ox
:usage. maximum optimization (-oilr -s)
:target. axp ppc
:group. 5
:option. ox
:usage. maximum optimization (-obmiler -s)
:target. i86 386
:group. 5

:usagegroup. 6
:usage. [C++ exception handling options]

:option. xd
:usage. no exception handling
:group. 6
:option. xds
:usage. no exception handling: space
:group. 6
:option. xdt
:usage. no exception handling
:group. 6
:option. xs
:usage. exception handling: balanced
:group. 6
:option. xss
:usage. exception handling: space
:group. 6
:option. xst
:usage. exception handling: time
:group. 6

:usagegroup. 7
:usage. [Preprocessor options]

:usagechain. 7 p
:usage. Preprocess source file
:jusage. p ソースファイルを前処理します

:option. d<name>[=text]
:usage. define a macro
:group. 7
:option. d+
:usage. extend syntax of -d option
:group. 7
:option. fo
:usage. set object file name
:file.
:group. 7
:option. i
:usage. include directory
:path.
:group. 7
:option. t
:usage. (C++) <num> of spaces in tab stop
:number.
:group. 7
:option. tp
:usage. (C) set #pragma on( <name> )
:jusage. (C) #pragma on( <name> )を設定します
:id. . <name>
:group. 7
:option. u<name>
:usage. undefine macro name
:group. 7
:option. pil
:usage. ignore #line directives
:nochain.
:usagenochain.
:group. 7
:option. pc
:usage. preserve comments
:group. 7
:option. pl
:usage. insert #line directives
:group. 7
:option. pw
:usage. wrap output at column <num>
:number.
:group. 7

:usagegroup. 8
:usage. [Linker options]

:option. bd
:usage. build Dynamic link library
:group. 8
:option. bm
:usage. build Multi-thread application
:group. 8
:option. br
:usage. build with dll run-time library
:group. 8
:option. bw
:usage. build default Windowing app.
:group. 8
:option. bcl
:usage. compile and link for <os>
:id. . <os>
:group. 8
:option. fd
:usage. write directives
:file.
:optional.
:group. 8
:option. fe
:usage. name executable file
:file.
:group. 8
:option. fm
:usage. generate map file
:file.
:optional.
:group. 8
:option. k<stack_size>
:usage. set stack size
:group. 8
:option. lp
:usage. create an OS/2 protected-mode pgm
:target. i86
:group. 8
:option. lr
:usage. create a DOS real-mode program
:target. i86
:group. 8
:option. l
:usage. link for the specified <target>
:id. . <target>
:group. 8
:option. @
:usage. additional directive file
:file.
:group. 8
:option. "linker_directives"
:usage. additional linker directives
:group. 8
