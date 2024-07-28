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
:cmt.* Description:  resource compiler wrc command line options.
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


:title.  Usage: wrc {options} input-filename {options} [output-filename] {options}
:jtitle. 使用方法: wrc {options} input-filename {options} [output-filename] {options}

:title.  Options:
:jtitle. オプション:
:title.  .         ( /option is also accepted )
:jtitle. .         ( /ｵﾌﾟｼｮﾝ でも指定できます )
:ntarget. bsd linux osx qnx haiku


:chain. v zk

:option. ? h
:usagenochain.
:usage.  print this message
:jusage. このメッセージを表示します

:option. q
:usage.  operate quietly

:option. 10
:enumerate. win16_ver
:usage.  stamp file as requiring Windows 1.0

:option. 20
:enumerate. win16_ver
:usage.  stamp file as requiring Windows 2.0

:option. 30
:enumerate. win16_ver
:usage.  stamp file as requiring Windows 3.0

:option. 31
:enumerate. win16_ver
:usage.  (*) stamp file as requiring Windows 3.1

:option. d
:special. scanDefine <name>[=text]
:usage.  define text macro <name>[=text]
:jusage. テキストマクロを定義します <name>[=text]

:option. ad
:usage.  generate auto dependency information for use by wmake

:option. ap
:target. unused
:usage.  prepend string

:option. bt
:special. scanTarget =<target> [windows|nt|os2]
:number.
:usage.  set the build <target> [windows|nt|os2]

:option. c
:file.
:usage.  set code page conversion file

:option. e
:usage.  for a DLL, global memory above EMS line

:option. fe
:file.
:usage.  set the output executable file to name

:option. fo
:file.
:usage.  set the output resource file to name

:option. fr
:file.
:usage.  specify an additional input resource file

:option. g
:target. unused
:special. scanSearchReplace
:usage.  find/replace string

:option. i
:path.
:usage.  look in path for include files

:option. k
:enumerate. segm_sort
:usage.  don't sort segments (same as -s0)

:option. l
:usage.  program uses LIM 3.2 EMS directly

:option. m
:usage.  each instance of program has its own EMS bank

:option. n
:target. unused
:usage.  each instance of program has its own EMS bank

:option. o
:usage.  preprocess only

:option. p
:usage.  private DLL

:option. r
:usage.  only build the resource file

:usagechain. . s
:usage. Segment and resource sorting method

:option. s0
:enumerate. segm_sort
:usage.  no sorting, leave segments in the linker order

:option. s1
:enumerate. segm_sort
:usage.  move preload segments to front and mark for fast load

:option. s2
:enumerate. segm_sort
:usage.  (*) move preload, data, non-discard. segments to front

:option. t
:usage.  protected mode only

:usagechain. . v
:usage. Verbose output

:option. v
:internal.
:usage.  print tokens as they are scanned

:option. v1
:internal.
:usage.  print grammar rules as they are reduced

:option. v2
:internal.
:usage.  print both tokens and grammar rules

:option. v3
:internal.
:usage.  print tokens as they are scanned

:option. v4
:internal.
:usage.  print grammar rules as they are reduced

:option. x
:usage.  ignore the INCLUDE environment variable

:option. xb
:usage.  don't create the __<target>__ macro

:option. xc
:usage.  ignore the current working directory

:usagechain. . zk
:usage. Multi-byte characters support

:option. zk0 zk
:enumerate. charset
:usage.  (*) Japanese (Kanji, CP 932)

:option. zk1
:enumerate. charset
:usage.  Chinese (Traditional, CP 950)

:option. zk2
:enumerate. charset
:usage.  Korean (Wansung, CP 949)

:option. zk3
:enumerate. charset
:usage.  Chinese (Simplified, CP 936)

:option. zku8
:enumerate. charset
:usage.  Unicode(NT)/UTF-8(Windows,OS2), UTF-8 source

:option. zku0
:enumerate. charset
:usage.  Japanese (Kanji, CP 932), UTF-8 source

:option. zm
:usage.  output Microsoft/IBM format .res files

:option. zn
:usage.  don't preprocess the file
