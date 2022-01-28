:cmt.*****************************************************************************
:cmt.*
:cmt.*                            Open Watcom Project
:cmt.*
:cmt.* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
:cmt.*     UTF-8 encoding, ¥
:cmt.*
:cmt.*****************************************************************************
:cmt.
:cmt.
:cmt. GML Macros used:
:cmt.
:cmt.	:chain. <option> <usage text>               options that start with <option>
:cmt.						    	can be chained together i.e.,
:cmt.						    	-oa -ox -ot => -oaxt
:cmt.	:target. <targ1> <targ2> ...                valid for these targets
:cmt.	:ntarget. <targ1> <targ2> ...               not valid for these targets
:cmt.	:usageogrp. <option> <usage text>           group of options that start with <option>
:cmt.                                                	are chained together in usage
:cmt.	:usagegrp. <num> <usage text>               group of options that have group <num>
:cmt.                                                	are chained together in usage
:cmt.	:title. <text>                              English title usage text
:cmt.	:jtitle. <text>                             Japanese title usage text
:cmt.	:titleu. <text>                             English title usage text for QNX resource file
:cmt.	:jtitleu. <text>                            Japanese title usage text for QNX resource file
:cmt.
:cmt.	:option. <option> <synonym> ...             define an option
:cmt.	:immediate. <fn> [<usage argid>]            <fn> is called when option parsed
:cmt.	:code. <source-code>                        <source-code> is executed when option parsed
:cmt.	:enumerate. <name> [<option>]               option is one value in <name> enumeration
:cmt.	:number. [<fn>] [<default>] [<usage argid>] =<num> allowed; call <fn> to check
:cmt.	:id. [<fn>] [<usage argid>]		    =<id> req'd; call <fn> to check
:cmt.	:char. [<fn>] [<usage argid>]		    =<char> req'd; call <fn> to check
:cmt.	:file. [<usage argid>]			    =<file> req'd
:cmt.	:path. [<usage argid>]			    =<path> req'd
:cmt.	:special. <fn> [<usage argid>]		    call <fn> to parse option
:cmt.	:usage. <text>                              English usage text
:cmt.	:jusage. <text>                             Japanese usage text
:cmt.
:cmt.	:optional.                                  value is optional
:cmt.	:internal.                                  option is undocumented
:cmt.	:prefix.                                    prefix of a :special. option
:cmt.	:nochain.                                   option isn't chained with other options
:cmt.	:timestamp.                                 kludge to record "when" an option
:cmt.                                                	is set so that dependencies
:cmt.                                                	between options can be simulated
:cmt.	:negate.                                    negate option value
:cmt.	:group. <num>                               group <num> to which option is included
:cmt.
:cmt. Global macros
:cmt.
:cmt.	:noequal.                                   args can't have option '='
:cmt.	:argequal. <char>                           args use <char> instead of '='
:cmt.
:cmt. where <targ>:
:cmt.		    default - any, dbg
:cmt.		    architecture - i86, 386, x64, axp, ppc, mps, sparc
:cmt.		    host OS - bsd, dos, linux, nt, os2, osx, qnx, haiku, rdos, win
:cmt.		    extra - targ1, targ2
:cmt.
:cmt.	Translations are required for the :jtitle. and :jusage. tags
:cmt.	if there is no text associated with the tag.


:title. Usage: link [options] file [options]
:target. any
:title. Options:
:target. any
:title.  .         ( /option is also accepted )
:target. any
:ntarget. qnx linux osx bsd haiku

:argequal. :


:option. 10x
:target. any
:internal.
:usage. use 10.x options

:option. align
:target. any
:number. check_align
:usage. specify alignment of each section

:option. base
:target. any
:special. parse_base :<arg>
:usage. set program base address

:option. comment
:target. any
:special. parse_comment :<string>
:usage. embed comment string into image

:option. debug
:target. any
:special. parse_debug
:immediate. handle_debug
:usage. link in debugging information

:option. debugtype
:target. any
:special. parse_debugtype
:internal.
:usage. link specified type of debugging information

:option. def
:target. any
:special. parse_def :<file>
:usage. process .def file

:option. defaultlib
:target. any
:special. parse_defaultlib
:usage. add one or more libraries to search

:option. dll
:target. any
:usage. build DLL

:option. entry
:target. any
:special. parse_entry :<symbol>
:usage. set starting address

:option. exetype
:target. any
:special. parse_exetype
:internal.
:usage. specify type of VXD to build

:option. export
:target. any
:special. parse_export :<exportdef>
:usage. export a symbol

:option. fixed
:target. any
:internal.
:usage. prevent program relocation when loading

:option. force:multiple
:target. any
:usage. allow multiply defined symbols

:option. force:undefined
:target. any
:usage. allow undefined symbols

:option. heap
:target. any
:special. parse_heap :<reserve>[,<commit>]
:usage. set heap size in bytes

:option. implib
:target. any
:special. parse_implib :<file>
:usage. override default import library name

:option. include
:target. any
:special. parse_include :<symbol>
:usage. force reference to a symbol

:option. incremental
:target. any
:special. parse_incremental :yes|no
:usage. enable or disable incremental linking

:option. internaldllname
:target. any
:special. parse_internaldllname
:internal.
:usage. enable or disable incremental linking

:option. machine
:target. any
:special. parse_machine
:internal.
:usage. specify target platform

:option. map
:target. any
:special. parse_map [:<file>]
:usage. generate map file

:option. nodefaultlib
:target. any
:usage. ignore default libraries

:option. noentry
:target. any
:internal.
:usage. create a DLL with no entry point

:option. nofuzzy
:target. any
:usage. disable fuzzy linking

:option. noinvoke
:target. any
:usage. don't invoke the Watcom tool

:option. nologo
:target. any
:usage. don't display copyright banner

:option. opt:ref
:target. any
:enumerate. opt_level
:usage. enable dead code elimination

:option. opt:noref
:target. any
:enumerate. opt_level
:usage. disable dead code elimination

:option. order
:target. any
:special. parse_order
:internal.
:usage. specify order of functions

:option. out
:target. any
:special. parse_out :<file>
:usage. specify output file name

:option. pdb
:target. any
:special. parse_pdb
:internal.
:usage. set PDB file name

:option. profile
:target. any
:internal.
:usage. enable profiling support in output file

:option. release
:target. any
:usage. set the checksum in the executable header

:option. section
:target. any
:special. parse_section
:internal.
:usage. set a section's attributes

:option. showwopts
:target. any
:usage. show translated options

:option. passwopts
:target. any
:special. parse_passwopts :<options>
:usage. pass <options> directly to the Watcom tools

:option. nowopts
:target. any
:usage. disable default options

:option. nowref
:target. any
:usage. disable default startup module references

:option. nowwarn
:target. any
:immediate. handle_nowwarn
:usage. disable warning messages for ignored options

:option. stack
:target. any
:special. parse_stack :<reserve>[,<commit>]
:usage. set stack size in bytes

:option. stub
:target. any
:special. parse_stub :<file>
:usage. attach DOS stub to program

:option. subsystem
:target. any
:special. parse_subsystem :<subsystem>
:usage. tell OS how to run the executable

:option. verbose
:target. any
:internal.
:usage. enable verbose messages

:option. version
:target. any
:special. parse_version :<major>[.<minor>]
:usage. embed version number into output file

:option. vxd
:target. any
:internal.
:usage. create a VXD

:option. warn
:target. any
:number.
:internal.
:usage. set linker warning level
