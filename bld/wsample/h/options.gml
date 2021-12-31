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
:cmt.* Description:  wsample command line options.
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


:title. Usage: wsample [options] program [program arguments]
:jtitle. 使用方法: wsample [options] program [program arguments]
:target. any
:title.  ..
:jtitle. ..
:target. any
:title.  Options:
:jtitle. オプション:
:target. any
:title.  ..
:jtitle. ..
:target. bsd linux osx qnx haiku
:title.  .     ( /option is also accepted )
:jtitle. .     ( /optionも使用できます )
:target. any
:ntarget. bsd linux osx qnx haiku
:title.  .     '#' may be used instead of '=' in options (eg. -b#10)
:jtitle. .     オプションの中では '=' の代りに '#' を使ってください (例 -b#10)
:target. any

:option. ? h
:target. any
:usage.  display this screen
:jusage. この画面を表示します

:option.  b
:target.  any
:ntarget. win
:id.      . <size>
:usage.   specify the buffer size for samples (default: 63KB)
:jusage.  サンプリング用バッファサイズ (ﾃﾞﾌｫﾙﾄ: 63KB)

:option. c
:target. any
:usage.  callgraph information is to be included in the sample file
:jusage. コールグラフ情報をサンプルファイルに含めます

:option. d
:target. dos
:usage.  disable assignment of DOS interrupts to application
:jusage. DOS割込みの監視の禁止

:option. e
:target. nov
:usage.  Estimate the timer terminal count for -o= (see above)
:jusage. Estimate the timer terminal count for -o= (see above)

:option. f
:target. any
:file.
:usage.  specify the output filename for the sample file
:jusage. サンプルファイルのファイル名

:option. i
:target. dos
:id.     . <intr>
:usage.  specify an interrupt number for assigning samples
:jusage. サンプリングを行う割込み番号

:option. o
:target. nov
:id.     . <tmct>
:usage.  Specify a count to be used to reset the timer at unload
:jusage. Specify a count to be used to reset the timer at unload

:option. r
:target. any
:id.     . <rate>
:usage.  specify the sampling rate
:jusage. サンプリング間隔

:option. s
:target. os2
:usage.  start the program in a new session
:jusage. 新しいセッションでのプログラムの開始

:footer.  ..
:jfooter. ..
:target.  any

:footer.  .   <size>  is a number in the range 1 to 63 (default: 63 KB)
:jfooter. .   <size>  は 1 から 63 までの範囲です (ﾃﾞﾌｫﾙﾄ: 63 KB)
:target.  any
:ntarget. win

:footer.  .   <rate>  is a number (range 1 to 55) specifying the approximate
:jfooter. .   <rate>  はサンプリング間のおよその時間間隔(ﾐﾘ秒単位)を
:target.  dos nov pls rsi
:footer.  .           time interval (in milliseconds) between samples (default: 55)
:jfooter. .           指定する数値(範囲:1から55まで)です (ﾃﾞﾌｫﾙﾄ: 55)
:target.  dos nov pls rsi
:footer.  .           NB: use this switch with caution. It will cause the time
:jfooter. .           注意: このスイッチの使用には注意してください.プログラム実行中の
:target.  nov pls
:footer.  .               to be incorrect during program execution. Also, rates
:jfooter. .               時間が正しくなくなります.また、値が小さすぎると、遅いシステム
:target.  nov pls
:footer.  .               which are too small, may crash slower systems.
:jfooter. .               の場合、クラッシュする可能性があります.
:target.  nov pls

:footer.  .   <rate>  is a number (range 1 to 1000) specifying the approximate
:jfooter. .   <rate>  はサンプリング間のおよその時間間隔 (単位:ﾐﾘ秒) を
:target.  os2 nt linux win
:footer.  .           time interval (in milliseconds) between samples (default: 55)
:jfooter. .           指定する数値 (範囲:1 から 1000まで) です (ﾃﾞﾌｫﾙﾄ: 55)
:target.  os2 nt win
:footer.  .           time interval (in milliseconds) between samples (default: 10)
:jfooter. .           指定する数値 (範囲:1 から 1000まで) です (ﾃﾞﾌｫﾙﾄ: 10)
:target.  linux

:footer.  .   <intr>  is a hex (base 16) number (range 20 to ff) specifying
:jfooter. .   <intr>  は16進数(範囲:20からffまで)で、以下の特別な方法で処理する
:target.  dos
:footer.  .           an interrupt number to treat in a special manner -
:jfooter. .           割込み番号です -
:target.  dos
:footer.  .           any samples during the execution of the software interrupt
:jfooter. .           ソフトウェア割込み実行中のサンプリングは、割込みを発生した命令に
:target.  dos
:footer.  .           are assigned to the instruction that invoked the interrupt
:jfooter. .           割り当てます
:target.  dos
:footer.  .           (default: the DOS interrupt 21 is monitored)
:jfooter. .           (ﾃﾞﾌｫﾙﾄ: DOS 割込み 21 がﾓﾆﾀｰされています)
:target.  dos
