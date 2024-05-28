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
:cmt.* Description:  x86 assembler wasm command line options.
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


:title.  Usage: wasm [options] asm_file [options] [@env_var]
:jtitle. 使用方法: wasm [options] file [options] [@env_var]
:titleu.  Usage: %C [options] asm_file [options] [@env_var]
:jtitleu. 使用方法: %C [options] file [options] [@env_var]

:title.  Options:
:jtitle. オプション:
:title.  .         ( /option is also accepted )
:jtitle. .         ( /ｵﾌﾟｼｮﾝ でも指定できます )
:ntarget. bsd linux osx qnx haiku


:chain. 2 3 4 5 6

:option. ? h
:usagenochain.
:usage. print this message
:jusage. このメッセージを表示します

:option. 0
:enumerate. cpu_info
:usage. 8086 instructions
:jusage. 8086 命令

:option. 1
:enumerate. cpu_info
:usage. 80186 instructions
:jusage. 80186 命令

:usagechain. . 2
:usage. 80286 instructions
:jusage. 80286 命令

:option. 2
:enumerate. cpu_info
:usage. real mode instructions
:jusage. real mode instructions

:option. 2p
:usage. protected mode instructions
:jusage. protected mode instructions

:usagechain. . 3
:usage. 80386 instructions
:jusage. 80386 命令

:option. 3
:enumerate. cpu_info
:usage. real mode instructions
:jusage. real mode instructions

:option. 3r
:usage. register calling conventions
:jusage. レジスタ呼び出し規約

:option. 3s
:usage. stack calling conventions
:jusage. スタック呼び出し規約

:option. 3p
:usage. protected mode instructions
:jusage. protected mode instructions

:usagechain. . 4
:usage. 80486 instructions
:jusage. 80486 命令

:option. 4
:enumerate. cpu_info
:usage. real mode instructions
:jusage. real mode instructions

:option. 4r
:usage. register calling conventions
:jusage. レジスタ呼び出し規約

:option. 4s
:usage. stack calling conventions
:jusage. スタック呼び出し規約

:option. 4p
:usage. protected mode instructions
:jusage. protected mode instructions

:usagechain. . 5
:usage. Pentium instructions
:jusage. Pentium 命令

:option. 5
:enumerate. cpu_info
:usage. real mode instructions
:jusage. real mode instructions

:option. 5r
:usage. register calling conventions
:jusage. レジスタ呼び出し規約

:option. 5s
:usage. stack calling conventions
:jusage. スタック呼び出し規約

:option. 5p
:usage. protected mode instructions
:jusage. protected mode instructions

:usagechain. . 6
:usage. Pentium Pro instructions
:jusage. 6 Pentium Pro 命令

:option. 6
:enumerate. cpu_info
:usage. real mode instructions
:jusage. real mode instructions

:option. 6r
:usage. register calling conventions
:jusage. レジスタ呼び出し規約

:option. 6s
:usage. stack calling conventions
:jusage. スタック呼び出し規約

:option. 6p
:usage. protected mode instructions
:jusage. protected mode instructions

:option. bt
:id. . <os>
:optional.
:usage. set the build target to <os>
:jusage. set the build target to <os>

:option. c
:usage. disable output OMF COMMENT record about data in code
:jusage. disable output OMF COMMENT record about data in code

:option. cx
:usage. symbols are not case-sensitive
:jusage. symbols are not case-sensitive

:option. d
:special. scanDefine <name>[=text]
:usage. define text macro <name>[=text]
:jusage. テキストマクロを定義します <name>[=text]

:option. d+
:internal.
:enumerate ignore
:usage. define extended text macro <name>[=text]
:jusage. define extended text macro <name>[=text]

:option. d0
:enumerate. debug_info
:usage. 
:jusage. 

:option. d1
:enumerate. debug_info
:usage. line number debugging support
:jusage. 行番号デバッグ情報を出力します

:option. d2
:enumerate. debug_info
:internal.
:usage. 
:jusage. 

:option. d3
:enumerate. debug_info
:internal.
:usage. 
:jusage. 

:option. ee
:usage. stop reading ASM file at END directive
:jusage. ENDディレクティブでASMファイルの読み込みを止めます

:option. e
:number.
:usage. set error limit number
:jusage. set error limit number

:option. fe
:internal.
:file.
:optional.
:usage. set error file name
:jusage. エラーファイル名を指定します

:option. fi
:file.
:usage. force <file> to be included
:jusage. force <file> to be included

:option. fl
:file.
:usage. listing file
:jusage. listing file

:option. fo
:file.
:optional.
:usage. set object file name
:jusage. オブジェクトファイルを設定します

:option. fp0
:enumerate. fpu_info
:usage. floating-point for 8087
:jusage. floating-point for 8087

:option. fp2
:enumerate. fpu_info
:usage. floating-point for 287
:jusage. floating-point for 287

:option. fp3
:enumerate. fpu_info
:usage. floating-point for 387
:jusage. floating-point for 387

:option. fp5
:enumerate. fpu_info
:usage. floating-point for Pentium
:jusage. floating-point for Pentium

:option. fp6
:enumerate. fpu_info
:usage. floating-point for Pentium Pro
:jusage. floating-point for Pentium Pro

:option. fpc
:enumerate. fpu_type
:usage. calls to floating-point library
:jusage. calls to floating-point library

:option. fpi
:enumerate. fpu_type
:usage. inline 80x87 instructions with emulation
:jusage. inline 80x87 instructions with emulation

:option. fpi87
:enumerate. fpu_type
:usage. inline 80x87 instructions
:jusage. inline 80x87 instructions

:option. fr
:file.
:optional.
:usage. set error file name
:jusage. エラーファイル名を指定します

:option. hc
:internal.
:enumerate ignore
:usage. debug info format CodeView
:jusage. debug info format CodeView

:option. hd
:internal.
:enumerate ignore
:usage. debug info format DWARF
:jusage. debug info format DWARF

:option. hw
:internal.
:enumerate ignore
:usage. debug info format WATCOM
:jusage. debug info format WATCOM

:option. i
:path.
:usage. add directory to list of include directories
:jusage. インクルード・ディレクトリのリストを追加します

:option. j s
:usage. force signed types to be used for signed values
:jusage. 符号付き型を符号付き値のために使用するようにします

:usagechain. . m
:usage. Memory model
:jusage. メモリ・モデル

:option. mc
:enumerate. mem_model
:usage. Compact
:jusage. Compact

:option. mf
:enumerate. mem_model
:usage. Flat
:jusage. Flat

:option. mh
:enumerate. mem_model
:usage. Huge
:jusage. Huge

:option. ml
:enumerate. mem_model
:usage. Large
:jusage. Large

:option. mm
:enumerate. mem_model
:usage. Medium
:jusage. Medium

:option. ms
:enumerate. mem_model
:usage. Small
:jusage. Small

:option. mt
:enumerate. mem_model
:usage. Tiny
:jusage. Tiny

:option. nc
:id. . <name>
:internal.
:usage. set code class name
:jusage. set code class name

:option. nd
:id. . <name>
:usage. set data segment name
:jusage. set data segment name

:option. nm
:file. . <name>
:usage. set module name
:jusage. set module name

:option. nt
:id. . <name>
:usage. set name of text segment
:jusage. set name of text segment

:option. o
:usage. allow C form of octal constants
:jusage. allow C form of octal constants

:option. od
:internal.
:usage. disable all optimization
:jusage. disable all optimization

:option. of
:enumerate. trace_stack
:usage. generate traceable stack frames as needed
:jusage. generate traceable stack frames as needed

:option. of+
:enumerate. trace_stack
:usage. always generate traceable stack frames
:jusage. always generate traceable stack frames

:option. zcm
:special. scanMode [=<mode>]
:number.
:usage. set compatibility mode - watcom (default), masm, tasm or ideal
:jusage. set compatibility mode - watcom (default), masm, tasm or ideal

:option. zld
:usage. suppress file dependency info in object file
:jusage. suppress file dependency info in object file

:option. zq q
:usage. operate quietly
:jusage. メッセージ等の出力をしません

:option. zz
:usage. remove '@size' from STDCALL function names
:jusage. remove '@size' from STDCALL function names

:option. zzo
:usage. don't mangle STDCALL symbols (backward compatible)
:jusage. don't mangle STDCALL symbols (backward compatible)

:option. w
:number.
:usage. set warning level number
:jusage. set warning level number

:option. we
:usage. treat all warnings as errors
:jusage. treat all warnings as errors

:option. wx
:usage. set warning level to the highest level
:jusage. set warning level to the highest level
