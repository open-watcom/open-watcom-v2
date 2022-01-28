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
:cmt.* Description:  x86 assembler wasm command line options.
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


:title.  Usage: wasm [options] asm_file [options] [@env_var]
:jtitle. 使用方法: wasm [options] file [options] [@env_var]
:titleu.  Usage: %C [options] asm_file [options] [@env_var]
:jtitleu. 使用方法: %C [options] file [options] [@env_var]
:target. any

:title.  Options:
:jtitle. オプション:
:target. any
:title.  .         ( /option is also accepted )
:jtitle. .         ( /ｵﾌﾟｼｮﾝ でも指定できます )
:target. any
:ntarget. bsd linux osx qnx haiku

:option. ? h
:target. any
:usage. print this message
:jusage. このメッセージを表示します

:option. 0
:target. any
:usage. 8086 instructions
:jusage. 8086 命令

:option. 1
:target. any
:usage. 80186 instructions
:jusage. 80186 命令

:chain. 2 80286 instructions
:jusage. 80286 命令

:option. 2p
:target. any
:usage. protected mode instructions
:jusage. protected mode instructions

:chain. 3 80386 instructions
:jusage. 80386 命令

:option. 3r
:target. any
:usage. register calling conventions
:jusage. レジスタ呼び出し規約

:option. 3s
:target. any
:usage. stack calling conventions
:jusage. スタック呼び出し規約

:option. 3p
:target. any
:usage. protected mode instructions
:jusage. protected mode instructions

:chain. 4 80486 instructions
:jusage. 80486 命令

:option. 4r
:target. any
:usage. register calling conventions
:jusage. レジスタ呼び出し規約

:option. 4s
:target. any
:usage. stack calling conventions
:jusage. スタック呼び出し規約

:option. 4p
:target. any
:usage. protected mode instructions
:jusage. protected mode instructions

:chain. 5 Pentium instructions
:jusage. Pentium 命令

:option. 5r
:target. any
:usage. register calling conventions
:jusage. レジスタ呼び出し規約

:option. 5p
:target. any
:usage. protected mode instructions
:jusage. protected mode instructions

:option. 5s
:target. any
:usage. stack calling conventions
:jusage. スタック呼び出し規約

:chain. 6 Pentium Pro instructions
:jusage. 6 Pentium Pro 命令

:option. 6r
:target. any
:usage. register calling conventions
:jusage. レジスタ呼び出し規約

:option. 6s
:target. any
:usage. stack calling conventions
:jusage. スタック呼び出し規約

:option. 6p
:target. any
:usage. protected mode instructions
:jusage. protected mode instructions

:option. bt
:target. any
:id. . <os>
:optional.
:usage. set the build target to <os>
:jusage. set the build target to <os>

:option. c
:target. any
:usage. disable output OMF COMMENT record about data in code
:jusage. disable output OMF COMMENT record about data in code

:option. d
:target. any
:special. scanDefine <name>[=text]
:usage. define text macro <name>[=text]
:jusage. テキストマクロを定義します <name>[=text]

:option. d1
:target. any
:usage. line number debugging support
:jusage. 行番号デバッグ情報を出力します

:option. e
:target. any
:usage. stop reading ASM file at END directive
:jusage. ENDディレクティブでASMファイルの読み込みを止めます

:option. e
:target. any
:number.
:usage. set error limit number
:jusage. set error limit number

:option. fi
:target. any
:file.
:usage. force <file> to be included
:jusage. force <file> to be included

:option. fo
:target. any
:file.
:optional.
:usage. set object file name
:jusage. オブジェクトファイルを設定します

:option. fp0
:target. any
:usage. floating-point for 8087
:jusage. floating-point for 8087

:option. fp2
:target. any
:usage. floating-point for 287
:jusage. floating-point for 287

:option. fp3
:target. any
:usage. floating-point for 387
:jusage. floating-point for 387

:option. fp5
:target. any
:usage. floating-point for Pentium
:jusage. floating-point for Pentium

:option. fp6
:target. any
:usage. floating-point for Pentium Pro
:jusage. floating-point for Pentium Pro

:option. fpc
:target. any
:usage. calls to floating-point library
:jusage. calls to floating-point library

:option. fpi
:target. any
:usage. inline 80x87 instructions with emulation
:jusage. inline 80x87 instructions with emulation

:option. fpi87
:target. any
:usage. inline 80x87 instructions
:jusage. inline 80x87 instructions

:option. fr fe
:target. any
:file.
:optional.
:usage. set error file name
:jusage. エラーファイル名を指定します

:option. i
:target. any
:path.
:usage. add directory to list of include directories
:jusage. インクルード・ディレクトリのリストを追加します

:option. j s
:target. any
:usage. force signed types to be used for signed values
:jusage. 符号付き型を符号付き値のために使用するようにします

:chain. m Memory model
:jusage. メモリ・モデル

:option. mc
:target. any
:usage. Compact
:jusage. Compact

:option. mf
:target. any
:usage. Flat
:jusage. Flat

:option. mh
:target. any
:usage. Huge
:jusage. Huge

:option. ml
:target. any
:usage. Large
:jusage. Large

:option. mm
:target. any
:usage. Medium
:jusage. Medium

:option. ms
:target. any
:usage. Small
:jusage. Small

:option. mt
:target. any
:usage. Tiny
:jusage. Tiny

:option. nc
:target. any
:id. . <name>
:internal.
:usage. set code class name
:jusage. set code class name

:option. nd
:target. any
:id. . <name>
:usage. set data segment name
:jusage. set data segment name

:option. nm
:target. any
:id. . <name>
:usage. set module name
:jusage. set module name

:option. nt
:target. any
:id. . <name>
:usage. set name of text segment
:jusage. set name of text segment

:option. o
:target. any
:usage. allow C form of octal constants
:jusage. allow C form of octal constants

:option. od
:target. any
:internal.
:usage. disable all optimization
:jusage. disable all optimization

:option. zcm
:target. any
:special. scanMode [=<mode>]
:usage. set compatibility mode - watcom (default), masm, tasm or ideal
:jusage. set compatibility mode - watcom (default), masm, tasm or ideal

:option. zld
:target. any
:usage. suppress file dependency info in object file
:jusage. suppress file dependency info in object file

:option. zq q
:target. any
:usage. operate quietly
:jusage. メッセージ等の出力をしません

:option. zz
:target. any
:usage. remove '@size' from STDCALL function names
:jusage. remove '@size' from STDCALL function names

:option. zzo
:target. any
:usage. don't mangle STDCALL symbols (backward compatible)
:jusage. don't mangle STDCALL symbols (backward compatible)

:option. w
:target. any
:number.
:usage. set warning level number
:jusage. set warning level number

:option. we
:target. any
:usage. treat all warnings as errors
:jusage. treat all warnings as errors

:option. wx
:target. any
:usage. set warning level to the highest level
:jusage. set warning level to the highest level
