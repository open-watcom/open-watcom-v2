:cmt.*****************************************************************************
:cmt.*
:cmt.*                            Open Watcom Project
:cmt.*
:cmt.* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
:cmt.*     UTF-8 encoding, ¥
:cmt.*
:cmt.*****************************************************************************
:cmt.
:cmt.
:cmt. GML Macros used:
:cmt.
:cmt.   :chain. <option> <usage text>               options that start with <option>
:cmt.                                                   can be chained together i.e.,
:cmt.                                                   -oa -ox -ot => -oaxt
:cmt.   :target. <targ1> <targ2> ...                valid for these targets
:cmt.   :ntarget. <targ1> <targ2> ...               not valid for these targets
:cmt.   :usageogrp. <option> <usage text>           group of options that start with <option>
:cmt.                                                   are chained together in usage
:cmt.   :usagegrp. <num> <usage text>               group of options that have group <num>
:cmt.                                                   are chained together in usage
:cmt.   :title. <text>                              English title usage text
:cmt.   :jtitle. <text>                             Japanese title usage text
:cmt.   :titleu. <text>                             English title usage text for QNX resource file
:cmt.   :jtitleu. <text>                            Japanese title usage text for QNX resource file
:cmt.
:cmt.   :option. <option> <synonym> ...             define an option
:cmt.   :immediate. <fn> [<usage argid>]            <fn> is called when option parsed
:cmt.   :code. <source-code>                        <source-code> is executed when option parsed
:cmt.   :enumerate. <name> [<option>]               option is one value in <name> enumeration
:cmt.   :number. [<fn>] [<default>] [<usage argid>] =<num> allowed; call <fn> to check
:cmt.   :id. [<fn>] [<usage argid>]                 =<id> req'd; call <fn> to check
:cmt.   :char. [<fn>] [<usage argid>]               =<char> req'd; call <fn> to check
:cmt.   :file. [<usage argid>]                      =<file> req'd
:cmt.   :path. [<usage argid>]                      =<path> req'd
:cmt.   :special. <fn> [<usage argid>]              call <fn> to parse option
:cmt.   :usage. <text>                              English usage text
:cmt.   :jusage. <text>                             Japanese usage text
:cmt.
:cmt.   :optional.                                  value is optional
:cmt.   :internal.                                  option is undocumented
:cmt.   :prefix.                                    prefix of a :special. option
:cmt.   :nochain.                                   option isn't chained with other options
:cmt.   :timestamp.                                 kludge to record "when" an option
:cmt.                                                   is set so that dependencies
:cmt.                                                   between options can be simulated
:cmt.   :negate.                                    negate option value
:cmt.   :group. <num>                               group <num> to which option is included
:cmt.
:cmt. Global macros
:cmt.
:cmt.   :noequal.                                   args can't have option '='
:cmt.   :argequal. <char>                           args use <char> instead of '='
:cmt.
:cmt. where <targ>:
:cmt.   default - any, dbg
:cmt.   architecture - i86, 386, x64, axp, ppc, mps, sparc
:cmt.   host OS - bsd, dos, linux, nt, os2, osx, qnx, haiku, rdos, win
:cmt.   extra - targ1, targ2
:cmt.
:cmt. Translations are required for the :jtitle. and :jusage. tags
:cmt.   if there is no text associated with the tag.


:title.  Usage: wrc {options} input-filename {options} [output-filename] {options}
:jtitle. 使用方法: wrc {options} input-filename {options} [output-filename] {options}
:target. any

:title.  Options:
:jtitle. オプション:
:target. any
:title.  .         ( /option is also accepted )
:jtitle. .         ( /ｵﾌﾟｼｮﾝ でも指定できます )
:target. any
:ntarget. bsd linux osx qnx haiku

:chain. s Segment and resource sorting method
:jusage.
:chain. v Verbose output
:jusage.
:chain. zk Multi-byte characters support
:jusage.

:option. ? h
:target. any
:usage.  print this message
:jusage. このメッセージを表示します

:option. q
:target. any
:usage.  operate quietly
:jusage.

:option. 30
:target. any
:usage.  stamp file as requiring Windows 3.0
:jusage.

:option. 31
:target. any
:usage.  (*) stamp file as requiring Windows 3.1
:jusage.

:option. d
:target. any
:special. scanDefine <name>[=text]
:usage.  define text macro <name>[=text]
:jusage. テキストマクロを定義します <name>[=text]

:option. ad
:target. any
:usage.  generate auto dependency information for use by wmake
:jusage.

:option. bt
:target. any
:id. . <os>
:enumerate. x windows nt os2
:usage.  set the build target to <os> [windows|nt|os2]
:jusage.

:option. c
:target. any
:file.
:usage.  set code page conversion file
:jusage.

:option. e
:target. any
:usage.  for a DLL, global memory above EMS line
:jusage.

:option. fo
:target. any
:file.
:usage.  set the output resource file to name
:jusage.

:option. fe
:target. any
:file.
:usage.  set the output executable file to name
:jusage.

:option. i
:target. any
:path.
:usage.  look in path for include files
:jusage.

:option. k
:target. any
:usage.  don't sort segments (same as -s0)
:jusage.

:option. l
:target. any
:usage.  program uses LIM 3.2 EMS directly
:jusage.

:option. m
:target. any
:usage.  each instance of program has its own EMS bank
:jusage.

:option. o
:target. any
:usage.  preprocess only
:jusage.

:option. p
:target. any
:usage.  private DLL
:jusage.

:option. r
:target. any
:usage.  only build the resource file
:jusage.

:option. s0
:target. any
:usage.  no sorting, leave segments in the linker order
:jusage.

:option. s1
:target. any
:usage.  move preload segments to front and mark for fast load
:jusage.

:option. s2
:target. any
:usage.  (*) move preload, data, non-discard. segments to front
:jusage.

:option. t
:target. any
:usage.  protected mode only
:jusage.

:option. v1
:target. any
:internal.
:usage.  print tokens as they are scanned
:jusage.

:option. v2
:target. any
:internal.
:usage.  print grammar rules as they are reduced
:jusage.

:option. v3
:target. any
:internal.
:usage.  print both tokens and grammar rules
:jusage.

:option. v4
:target. any
:internal.
:usage.  print tokens as they are scanned
:jusage.

:option. x
:target. any
:usage.  ignore the INCLUDE environment variable
:jusage.

:option. zk0
:target. any
:usage.  (*) Japanese (Kanji, CP 932)
:jusage.

:option. zk1
:target. any
:usage.  Chinese (Traditional, CP 950)
:jusage.

:option. zk2
:target. any
:usage.  Korean (Wansung, CP 949)
:jusage.

:option. zk3
:target. any
:usage.  Chinese (Simplified, CP 936)
:jusage.

:option. zku8
:target. any
:usage.  Unicode UTF-8
:jusage.

:option. zku0
:target. any
:usage.  Japanese (Kanji, CP 932), UTF-8 source
:jusage.

:option. zm
:target. any
:usage.  output Microsoft/IBM format .res files
:jusage.

:option. zn
:target. any
:usage.  don't preprocess the file
:jusage.
