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
:cmt.* Description:  MSTOOLS asaxp RISC assembler command line options.
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
:cmt. MS Assembler ASAXP options are probably case-sensitive
:cmt.  I am not able to search any source which confirm it
:cmt.
:cmt. OW ASAXP clone use non case-sensitive options
:cmt.


:argequal. :

:title. Usage: asaxp [options] <filename>
:title. Options:
:title.  .         ( /option is also accepted )
:ntarget. qnx linux osx bsd haiku

:cmt    this is not really an option it is just a storage for filenames
:option. t010101010101
:internal.
:special. parse_t010101010101
:usage. a way to reserve storage space for filenames in OPT_STORAGE

:cmt    /nopp
:option. nopp
:internal.
:usage. do not invoke the C language preprocessor on the input file

:cmt    /D<sym>[=string]
:option. d
:special. parse_D <sym>[=string]
:usage. define constants and macros

:cmt    /Fo
:option. fo
:immediate. handle_Fo
:special. parse_Fo <filename>
:usage. rename object file

:cmt    /o
:option. o
:immediate. handle_Fo
:special. parse_Fo <filename>
:usage. rename object file

:cmt    /I<directory>
:option. i
:special. parse_I <directory>
:usage. specifies path for include files

:cmt    /O0
:option. O0
:internal.
:usage. turn off code scheduling optimization

:cmt    /O1
:option. O1
:internal.
:usage. turn on code scheduling optimization (default)

:cmt    /resumption_safe
:option. resumption_safe
:internal.
:usage. prevent the scheduler from moving floating point operating instructions

:cmt    /symbols_aligned_0mod4
:option. symbols_aligned_0mod4
:internal.
:usage. symbols are longword granular (default)

:cmt    /symbols_not_aligned
:option. symbols_not_aligned
:internal.
:usage. no attempt is made to align symbols

:cmt    /stack_aligned_0mod8
:option. stack_aligned_0mod8
:internal.
:usage. lign stack on a quadword boundary(default)

:cmt    /stack_not_aligned
:option. stack_not_aligned
:internal.
:usage. no attempt is made to align stack

:cmt    /eflag flags
:option. eflag
:number.
:internal.
:usage. encode the exception-related flags

:cmt    /QApdst
:option. QApdst
:internal.
:usage. set the exception-related flags to EXCEPTION_MODE_SILENT(default)

:cmt    /QApdsg
:option. QApdsg
:internal.
:usage. set the exception-related flags to EXCEPTION_MODE_SIGNAL

:cmt    /QApdsa
:option. QApdsa
:internal.
:usage. set the exception-related flags to EXCEPTION_MODE_SIGNAL_ALL

:cmt    /QApdie
:option. QApdie
:internal.
:usage. set the exception-related flags to EXCEPTION_MODE_IEEE

:cmt    /QApdca
:option. QApdca
:internal.
:usage. set the exception-related flags to EXCEPTION_MODE_CALLER

:cmt    /U<symbol>
:option. u
:internal.
:special. parse_U <sym>
:usage. undefine constants and macros

:cmt    /Zd
:option. Zd
:internal.
:usage. emit line numbers and public symbols for debugging

:cmt    /Zi
:option. Zi
:internal.
:usage. emit CodeView(tm) symbolic information for windbg

:cmt    /help
:option. help
:usage. display brief usage message

:cmt    /?
:option. ?
:usage. display brief usage message

:option. noinvoke
:usage. don't invoke the Watcom tool

:cmt    /NOLOGO
:option. nologo
:usage. don't display copyright banner

:option. nowwarn
:immediate. handle_nowwarn
:usage. disable warning messages for ignored options

:option. nowopts
:usage. disable default options

:option. passwopts
:special. parse_passwopts :<options>
:usage. pass <options> directly to the Watcom tools

:option. showwopts
:usage. show translated options
