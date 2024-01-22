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
:cmt.* Description:  wtouch command line options.
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
:cmt.   :usagechain. <option> <usage text>          group of options that start with <option>
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


:title. Usage:   wtouch [<options>] <file_names>
:jtitle. 使用方法: wtouch [<ｵﾌﾟｼｮﾝ>] <ﾌｧｲﾙ名>
:target. any

:title. ..
:jtitle. ..
:target. any

:title. Options: ('/' may be used instead of '-' to specify options)
:jtitle. ｵﾌﾟｼｮﾝ: ('/'はオプション指定の際の'-'の替わりとして使用します)
:target. any

:argequal.  ..

:option. c
:usage. do not create files that do not exist
:jusage. 存在しないファイルは作成されません
:target. any

:option. i
:usage. increment time stamp before touching <file_names>
:jusage. <ﾌｧｲﾙ名>をタッチする前にタイムスタンプをインクリメントします
:target. any

:option. r
:usage. touch file even if it is marked read only
:jusage. 読み込み専用ファイルであっても、ﾀｲﾑｽﾀﾝﾌﾟを書き換えます。
:target. any

:option. s
:usage. recurse through subdirectories
:target. any

:option. d  
:usage. specify date part of the time-stamp explicitly
:jusage. タイム・スタンプの日付部分を明示指定します
:id. . <date>
:target. any

:option. f
:usage. use date-time from <file>
:jusage. <ﾌｧｲﾙ>からの日付時間を使用します
:file. <file>
:target. any

:option. t
:usage. specify time part of the time-stamp explicitly
:jusage. タイム・スタンプの時間部分を明示指定します
:id. . <time>
:target. any

:option. u
:usage. use USA date/time format regardless of country
:jusage. USAの日付時間フォーマットを使用します
:target. any

:option. q
:usage. suppress informational messages
:jusage. 情報メッセージを抑制します
:target. any

:option. ?
:usage. display this help screen
:jusage. このヘルプ画面を表示します
:target. any
