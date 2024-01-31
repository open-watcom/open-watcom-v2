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
:cmt.* Description:  MSTOOLS cl command line options.
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

:cmt.
:cmt. MS Compiler CL options are case-sensitive except /HELP
:cmt.


:noequal.

:title. Usage: cl [options] file [options]
:title. Options:
:title.  .         ( /option is also accepted )
:ntarget. qnx linux osx bsd haiku


:option. 10x
:internal.
:usage. use 10.x options

:option. \C
:usage. preserve comments

:option. \c
:usage. compile only

:option. \D
:special. parse_D <macro>[=<value>]
:usage. same as #define <macro>[=<value>] before compilation

:option. \E
:usage. preprocess and insert #line directives to stdout

:option. \E\H\a
:internal.
:usage. specify exception handling

:option. \E\H\a\c
:internal.
:usage. specify exception handling

:option. \E\H\c
:internal.
:usage. specify exception handling

:option. \E\H\c\a
:internal.
:usage. specify exception handling

:option. \E\H\c\s
:internal.
:usage. specify exception handling

:option. \E\H\s
:internal.
:usage. specify exception handling

:option \E\H\s\c
:internal.
:usage. specify exception handling

:option. \E\P
:usage. preprocess without #line directives to stdout

:usagechain. . \F
:usage. file options

:option. \F
:special. parse_F <size>
:immediate. handle_F
:usage. set stack size

:option. \F\a
:path.
:internal.
:usage. specify listing output file

:option. \F\A
:internal.
:usage. generate assembly listing

:option. \F\A\c
:internal.
:usage. generate assembly and machine code listing

:option. \F\A\c\s
:internal.
:usage. generate assembly, source, and machine code listing

:option. \F\A\s
:internal.
:usage. generate assembly and source listing

:option. \F\d
:file.
:internal.
:usage. specify PDB filename

:option. \F\e
:immediate. handle_Fe
:file.
:usage. set executable or DLL file name

:option. \F\I
:special. parse_FI <file>
:usage. force <file> to be included

:option. \F\m
:special. parse_Fm [<file>]
:usage. set map file name

:option. \F\o
:file.
:usage. set object output file name

:option. \F\p
:immediate. handle_Fp
:file.
:usage. set precompiled header data file name

:option. \F\R
:immediate. handle_FR
:file.
:usage. generate browsing information

:option. \F\r
:internal.
:usage. generate SBR file without local variables

:usagechain. . \G
:usage. code generation options

:option. \G3
:target. 386
:immediate. handle_arch_i86
:enumerate. arch_i86
:usage. 386 instructions

:option. \G4
:target. 386
:immediate. handle_arch_i86
:enumerate. arch_i86
:usage. 386 instructions, optimize for 486

:option. \G5
:target. 386
:immediate. handle_arch_i86
:enumerate. arch_i86
:usage. 386 instructions, optimize for Pentium

:option. \G\B
:target. 386
:immediate. handle_arch_i86
:enumerate. arch_i86
:usage. 386 instructions, optimize for 486

:option. \G\d
:enumerate. calling_convention
:internal.
:usage. use __cdecl (stack-based) calling convention

:option. \G\e
:enumerate. stack_probes
:immediate. handle_stack_probes
:usage. activate stack probes for all functions

:option. \G\f
:usage. merge duplicate strings

:option. \G\F
:usage. merge duplicate read-only strings

:option. \G\h
:usage. call __penter at the start of each function

:option. \G\r
:enumerate. calling_convention
:internal.
:usage. use __fastcall (register-based) calling convention

:option. \G\s
:enumerate. stack_probes
:immediate. handle_stack_probes
:special. parse_Gs <distance>
:usage. set stack probe distance

:option. \G\X
:immediate. handle_GX
:usage. destruct static objects during stack unwinding

:option. \G\y
:usage. store each function in its own COMDAT

:option. \G\z
:enumerate. calling_convention
:internal.
:usage. use __stdcall (register-based) calling convention

:option. \H
:number.
:internal.
:usage. set maximum identifier length

:option. help
:usage. get help

:option. ?
:usage. get help

:option. \I
:special. parse_I <path>
:usage. add another include path

:option. \J
:usage. change char default from signed to unsigned

:option. \l\i\n\k
:special. parse_link
:usage. specify linker options

:option. \L\D
:usage. create DLL

:option. \M\D
:immediate. handle_threads_linking
:enumerate. threads_linking
:usage. use multithreaded DLL version of C library

:option. \M\D\d
:immediate. handle_threads_linking
:enumerate. threads_linking
:usage. use multithreaded debug DLL version of C library

:option. \M\L
:immediate. handle_threads_linking
:enumerate. threads_linking
:usage. use single-thread statically linked version of C library

:option. \M\L\d
:immediate. handle_threads_linking
:enumerate. threads_linking
:usage. use single-thread debug static link version of C library

:option. \M\T
:immediate. handle_threads_linking
:enumerate. threads_linking
:usage. use multithreaded static version of C library

:option. \M\T\d
:immediate. handle_threads_linking
:enumerate. threads_linking
:usage. use multithreaded debug static version of C library

:option. \n\o\l\o\g\o
:usage. operate quietly

:usagechain. . \O
:usage. optimization options

:option. \O1
:immediate. handle_opt_level
:enumerate. opt_level
:usage. minimize size

:option. \O2
:immediate. handle_opt_level
:enumerate. opt_level
:usage. maximize speed

:option. \O\a
:usage. assume no aliasing

:option. \O\b
:immediate. handle_inlining_level
:number. check_inlining_level
:usage. control function inlining

:option. \O\d
:immediate. handle_opt_level
:enumerate. opt_level
:usage. disable all optimizations

:option. \O\g
:usage. enable global optimizations

:option. \O\i
:usage. expand intrinsic functions inline

:option. \O\p
:immediate. handle_Op
:timestamp.
:usage. generate consistent floating-point results

:option. \O\s
:immediate. handle_opt_size_time
:enumerate. opt_size_time
:usage. favor code size over execution time in optimizations

:option. \O\t
:immediate. handle_opt_size_time
:enumerate. opt_size_time
:usage. favor execution time over code size in optimizations

:option. \O\w
:internal.
:usage. assume aliasing across function calls

:option. \O\x
:immediate. handle_opt_level
:enumerate. opt_level
:usage. equivalent to /Ob1 /Og /Oi /Ot /Oy /Gs

:option. \O\y
:immediate. handle_Oy
:usage. disable stack frames

:option. \o
:special. parse_o <file>
:usage. set executable or DLL file name

:option. \P
:usage. preprocess to a file

:option. \Q\I\f\d\i\v
:target. 386
:immediate. handle_QIfdiv
:usage. enable Pentium FDIV fix

:option. \s\h\o\w\w\o\p\t\s
:usage. show translated options

:option. \p\a\s\s\w\o\p\t\s
:special. parse_passwopts :<options>
:usage. pass <options> directly to the Watcom tools

:option. \n\o\i\n\v\o\k\e
:usage. don't invoke the Watcom tool

:option. \n\o\w\o\p\t\s
:usage. disable default options

:option. \n\o\w\w\a\r\n
:immediate. handle_nowwarn
:usage. disable warning messages for ignored options

:option. \l\e\s\s\w\d
:internal.
:usage. change debug info from -d2 to -d1

:option. \T\C
:immediate. handle_TC
:usage. force compilation of all files as C

:option. \T\c
:special. parse_Tc <file>
:usage. force compilation of <file> as C

:option. \T\P
:immediate. handle_TP
:usage. force compilation of all files as C++

:option. \T\p
:special. parse_Tp <file>
:usage. force compilation of <file> as C++

:option. \U
:special. parse_U <macro>
:usage. undefine macro name

:option. \u
:internal.
:usage. undefine all predefined macros

:option. \v\d0
:internal.
:usage. disable constructor/destructor displacements

:option. \v\d1
:internal.
:usage. enable vtordisp constructor/destructor displacements

:option. \v\m\b
:internal.
:usage. use best case always pointer representation

:option. \v\m\g
:internal.
:usage. use general purpose always pointer representation

:option. \v\m\m
:internal.
:usage. general-purpose pointers to single- and multiple-inheritance classes

:option. \v\m\s
:internal.
:usage. general-purpose pointers to single-inheritance classes

:option. \v\m\v
:internal.
:usage. general-purpose pointers to any classes

:option. \V
:special. parse_V
:internal.
:usage. embed string in object file

:option. \W
:immediate. handle_warn_level
:enumerate. warn_level
:number. check_warn_level
:usage. set warning level number

:option. \w
:immediate. handle_warn_level
:enumerate. warn_level
:usage. disable all warning messages

:option. \W\X
:usage. treat all warnings as errors

:option. \X
:internal.
:usage. ignore standard include paths

:option. \Y\c
:enumerate. precomp_headers
:immediate. handle_precomp_headers
:file.
:optional.
:usage. create pre-compiled header file

:option. \Y\d
:usage. full debug info from pre-compiled headers

:option. \Y\u
:enumerate. precomp_headers
:immediate. handle_precomp_headers
:file.
:optional.
:usage. use pre-compiled header file

:option. \Y\X
:enumerate. precomp_headers
:immediate. handle_precomp_headers
:file.
:optional.
:usage. use pre-compiled header file

:usagechain. . \Z
:usage. language options

:option. \Z7
:immediate. handle_debug_info
:enumerate. debug_info
:usage. generate Codeview debugging information

:option. \Z\a
:enumerate. iso
:timestamp.
:usage. disable extensions (i.e., accept only ISO/ANSI C++)

:option. \Z\d
:immediate. handle_debug_info
:enumerate. debug_info
:usage. line number debugging information

:option. \Z\e
:enumerate. iso
:usage. enable extensions (e.g., near, far, export, etc.)

:option. \Z\g
:usage. output function declarations to stdout

:option. \Z\i
:immediate. handle_debug_info
:enumerate. debug_info
:internal.
:usage. full symbolic debugging information

:option. \Z\l
:usage. remove default library information

:option. \Z\m
:number. check_maxmem
:usage. maximum memory allocation in % of default (ignored)

:option. \Z\n
:internal.
:usage. disable SBR file packing

:option. \Z\p
:number. check_packing 1
:usage. pack structure members with alignment {1,2,4,8,16}

:option. \Z\s
:usage. syntax check only
