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
:cmt.* Description:  MSTOOLS link command line options.
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
:cmt. MS Linker LINK options are not case-sensitive
:cmt.


:argequal. :

:title. Usage: link [options] file [options]
:title. Options:
:title.  .         ( /option is also accepted )
:ntarget. qnx linux osx bsd haiku


:option. 10x
:internal.
:usage. use 10.x options

:option. align
:number. check_align
:usage. specify alignment of each section

:option. base
:special. parse_base :<arg>
:usage. set program base address

:option. comment
:special. parse_comment :<string>
:usage. embed comment string into image

:option. debug
:special. parse_debug
:immediate. handle_debug
:usage. link in debugging information

:option. debugtype
:special. parse_debugtype
:internal.
:usage. link specified type of debugging information

:option. def
:special. parse_def :<file>
:usage. process .def file

:option. defaultlib
:special. parse_defaultlib
:usage. add one or more libraries to search

:option. dll
:usage. build DLL

:option. entry
:special. parse_entry :<symbol>
:usage. set starting address

:option. exetype
:special. parse_exetype
:internal.
:usage. specify type of VXD to build

:option. export
:special. parse_export :<exportdef>
:usage. export a symbol

:option. fixed
:internal.
:usage. prevent program relocation when loading

:option. force:multiple
:usage. allow multiply defined symbols

:option. force:undefined
:usage. allow undefined symbols

:option. heap
:special. parse_heap :<reserve>[,<commit>]
:usage. set heap size in bytes

:option. implib
:special. parse_implib :<file>
:usage. override default import library name

:option. include
:special. parse_include :<symbol>
:usage. force reference to a symbol

:option. incremental
:special. parse_incremental :yes|no
:usage. enable or disable incremental linking

:option. internaldllname
:special. parse_internaldllname
:internal.
:usage. enable or disable incremental linking

:option. machine
:special. parse_machine
:internal.
:usage. specify target platform

:option. map
:special. parse_map [:<file>]
:usage. generate map file

:option. nodefaultlib
:usage. ignore default libraries

:option. noentry
:internal.
:usage. create a DLL with no entry point

:option. nofuzzy
:usage. disable fuzzy linking

:option. noinvoke
:usage. don't invoke the Watcom tool

:option. nologo
:usage. don't display copyright banner

:option. opt:ref
:enumerate. opt_level
:usage. enable dead code elimination

:option. opt:noref
:enumerate. opt_level
:usage. disable dead code elimination

:option. order
:special. parse_order
:internal.
:usage. specify order of functions

:option. out
:special. parse_out :<file>
:usage. specify output file name

:option. pdb
:special. parse_pdb
:internal.
:usage. set PDB file name

:option. profile
:internal.
:usage. enable profiling support in output file

:option. release
:usage. set the checksum in the executable header

:option. section
:special. parse_section
:internal.
:usage. set a section's attributes

:option. showwopts
:usage. show translated options

:option. passwopts
:special. parse_passwopts :<options>
:usage. pass <options> directly to the Watcom tools

:option. nowopts
:usage. disable default options

:option. nowref
:usage. disable default startup module references

:option. nowwarn
:immediate. handle_nowwarn
:usage. disable warning messages for ignored options

:option. stack
:special. parse_stack :<reserve>[,<commit>]
:usage. set stack size in bytes

:option. stub
:special. parse_stub :<file>
:usage. attach DOS stub to program

:option. subsystem
:special. parse_subsystem :<subsystem>
:usage. tell OS how to run the executable

:option. verbose
:internal.
:usage. enable verbose messages

:option. version
:special. parse_version :<major>[.<minor>]
:usage. embed version number into output file

:option. vxd
:internal.
:usage. create a VXD

:option. warn
:number.
:internal.
:usage. set linker warning level
