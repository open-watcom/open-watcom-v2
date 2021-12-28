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
:cmt.* Description:  MSTOOLS nmake command line options.
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


:title. Usage: nmake [options] [macros] [targets] [@commandfile]
:target. any
:title. Options:
:target. any
:title.  .         ( /option is also accepted )
:target. any
:ntarget. qnx linux osx bsd haiku


:argequal. :


:cmt    this is not really an option it is just a targets and macros storage
:option. t010101010101
:target. any
:internal.
:special. parse_t010101010101
:usage. a way to reserve storage space for tagets and macros in OPT_STORAGE

:cmt     A
:option. A
:target. any
:immediate. parse_combining
:usage. build all evaluated targets

:cmt     B
:option. B
:target. any
:immediate. parse_combining
:internal.
:usage. forces build even if timestamps are equal

:cmt     C
:option. C
:target. any
:immediate. parse_combining
:internal.
:usage. suppress output messages

:cmt     D
:option. D
:target. any
:immediate. parse_combining
:usage. display build information

:cmt     E
:option. E
:target. any
:immediate. parse_combining
:internal.
:usage. override env-var macros

:cmt     F
:option. F
:target. any
:special. parse_F <filename>
:usage. specifies filename as a makefile

:cmt     HELP
:option. HELP
:target. any
:usage. display brief usage message

:cmt     \?
:option. ?
:target. any
:usage. display brief usage message

:cmt     I
:option. I
:target. any
:immediate. parse_combining
:usage. ignore exit codes from commands

:cmt     K
:option. K
:target. any
:immediate. parse_combining
:usage. build unrelated targets on error

:cmt     L
:option. L
:target. any
:immediate. parse_combining
:internal.
:usage. don't display copyright banner

:cmt     N
:option. N
:target. any
:immediate. parse_combining
:usage. display commands but do not execute

:option. noinvoke
:target. any
:usage. don't invoke the Watcom tool

:cmt     NOLOGO
:option. NOLOGO
:target. any
:usage. don't display copyright banner

:cmt     P
:option. P
:target. any
:immediate. parse_combining
:usage. display MAKE information

:cmt     Q
:option. Q
:target. any
:immediate. parse_combining
:usage. check time stamps but do not build

:cmt     R
:option. R
:target. any
:immediate. parse_combining
:internal.
:usage. ignore predefined rules/macros

:cmt     S
:option. S
:target. any
:immediate. parse_combining
:usage. suppress executed-commands display

:cmt     T
:option. T
:target. any
:immediate. parse_combining
:usage. change time stamps but do not build

:cmt     U
:option. U
:target. any
:immediate. parse_combining
:internal.
:usage. dump inline files

:cmt     Y
:option. Y
:target. any
:immediate. parse_combining
:internal.
:usage. disable batch-mode

:cmt     X
:option. X
:target. any
:special. parse_X <filename>
:internal.
:usage. sends NMAKE error output to filename

:option. nowwarn
:target. any
:immediate. handle_nowwarn
:usage. disable warning messages for ignored options

:option. nowopts
:target. any
:usage. disable default options

:option. passwopts
:target. any
:special. parse_passwopts :<options>
:usage. pass <options> directly to the Watcom tools

:option. showwopts
:target. any
:usage. show translated options

:option. verbose
:target. any
:usage. show contents of inline temporary files
