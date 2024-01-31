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
:cmt.* Description:  WFC and WFL compilers command line options.
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


:title. Usage: wfc [options] <file-spec> [options]
:jtitle. 使用方法: wfc [options] <file-spec> [options]
:target. i86
:ntarget. targ1

:title. Usage: wfc386 [options] <file-spec> [options]
:jtitle. 使用方法: wfc386 [options] <file-spec> [options]
:target. 386
:ntarget. targ1

:title. Usage: wfcaxp [options] <file-spec> [options]
:jtitle. 使用方法: wfcaxp [options] <file-spec> [options]
:target. axp
:ntarget. targ1

:title. Usage: wfcmps [options] <file-spec> [options]
:jtitle. 使用方法: wfcmps [options] <file-spec> [options]
:target. mps
:ntarget. targ1

:title. Usage: wfcppc [options] <file-spec> [options]
:jtitle. 使用方法: wfcppc [options] <file-spec> [options]
:target. ppc
:ntarget. targ1

:title. Usage: wfl [options] <file-spec> [options]
:jtitle. 使用方法: wfl [options] <file-spec> [options]
:target. targ1
:ntarget. 386 axp mps ppc

:title. Usage: wfl386 [options] <file-spec> [options]
:jtitle. 使用方法: wfl386 [options] <file-spec> [options]
:target. targ1
:ntarget. i86 axp mps ppc

:title. Usage: wflaxp [options] <file-spec> [options]
:jtitle. 使用方法: wflaxp [options] <file-spec> [options]
:target. targ1
:ntarget. i86 386 mps ppc

:title. Usage: wflmps [options] <file-spec> [options]
:jtitle. 使用方法: wflmps [options] <file-spec> [options]
:target. targ1
:ntarget. i86 386 axp ppc

:title. Usage: wflppc [options] <file-spec> [options]
:jtitle. 使用方法: wflppc [options] <file-spec> [options]
:target. targ1
:ntarget. i86 386 axp mps

:title. Options:
:jtitle. オプション:
:title.  .         ( /option is also accepted )
:jtitle. .         ( /option も使用できます )
:ntarget. bsd linux osx qnx haiku

:cmt.
:cmt.   Compile and Link Options
:cmt.
:usagegroup. 1
:usage. *Compile and Link Options*
:jusage. *ｺﾝﾊﾟｲﾙ/ﾘﾝｸｵﾌﾟｼｮﾝ*

:option. C
:target. targ1
:group. 1
:usage. compile only, no linking
:jusage. ｺﾝﾊﾟｲﾙのみ, ﾘﾝｸは行わない

:option. Y
:target. targ1
:group. 1
:usage. ignore WFL variable
:jusage. WFL環境変数を無視

:option. P
:target. targ1
:group. 1
:usage. run protected mode compiler
:jusage. ﾌﾟﾛﾃｸﾄﾓｰﾄﾞｺﾝﾊﾟｲﾗｰの実行

:option. FM
:target. targ1
:group. 1
:file.
:optional.
:usage. generate map file
:jusage. ﾏｯﾌﾟﾌｧｲﾙの生成

:option. FE
:target. targ1
:group. 1
:file.
:usage. name executable file
:jusage. 実行ファイル名

:option. FD
:target. targ1
:group. 1
:file.
:optional.
:usage. keep directive file
:jusage. 擬似命令ﾌｧｲﾙの保持

:option. FI
:target. targ1
:group. 1
:file.
:usage. include directive file
:jusage. 擬似命令ﾌｧｲﾙのｲﾝｸﾙｰﾄﾞ

:option. LP
:target. targ2
:group. 1
:ntarget. 386 axp ppc
:usage. link for OS/2 protect-mode
:jusage. OS/2 ﾌﾟﾛﾃｸﾄﾓｰﾄﾞ用ﾘﾝｸ

:option. LR
:target. targ2
:group. 1
:ntarget. 386 axp ppc
:usage. link for DOS real-mode
:jusage. DOS ﾘｱﾙﾓｰﾄﾞ用ﾘﾝｸ

:option. L
:target. targ1
:group. 1
:id.
:usage. link for specified system
:jusage. 指定ｼｽﾃﾑ用ﾘﾝｸ

:option. K
:target. targ1
:group. 1
.number.
:usage. set stack size
:jusage. ｽﾀｯｸｻｲｽﾞの設定

:option. "directives"
:target. targ1
:group. 1
:usage. additional linker directives
:jusage. ﾘﾝｶｰ擬似命令の追加

:cmt.
:cmt.   File Management Options
:cmt.
:usagegroup. 2
:usage. *File Management Options*
:jusage. *ﾌｧｲﾙﾏﾈｰｼﾞﾒﾝﾄｵﾌﾟｼｮﾝ*

:option. LISt
:group. 2
:usage. generate a listing file
:jusage. ﾘｽﾄﾌｧｲﾙの生成

:option. PRint
:group. 2
:usage. listing file to printer
:jusage. ﾘｽﾄﾌｧｲﾙをﾌﾟﾘﾝﾀｰに出力

:option. TYpe
:group. 2
:usage. listing file to terminal
:jusage. ﾘｽﾄﾌｧｲﾙを画面に出力

:option. DIsk
:group. 2
:usage. listing file to disk
:jusage. ﾘｽﾄﾌｧｲﾙをﾃﾞｨｽｸに出力

:option. INCList
:group. 2
:usage. list INCLUDE files
:jusage. INCLUDE ﾌｧｲﾙのﾘｽﾄ

:option. ERrorfile FR
:group. 2
:usage. generate an error file
:jusage. ｴﾗｰﾌｧｲﾙの生成

:option. INCPath I
:group. 2
:usage. path for INCLUDE files
:jusage. INCLUDEﾌｧｲﾙ用のﾊﾟｽ

:option. FO
:group. 2
:usage. name object file
:jusage. ｵﾌﾞｼﾞｪｸﾄﾌｧｲﾙ名の指定

:cmt.
:cmt.   Diagnostic Options
:cmt.
:usagegroup. 3
:usage. *Diagnostic Options*
:jusage. *診断ｵﾌﾟｼｮﾝ*

:option. EXtensions
:group. 3
:usage. issue extension messages
:jusage. 拡張ﾒｯｾｰｼﾞの出力

:option. Reference
:group. 3
:usage. issue unreferenced warning
:jusage. 未参照警告ｴﾗｰの出力

:option. WArnings
:group. 3
:usage. issue warning messages
:jusage. 警告ｴﾗｰﾒｯｾｰｼﾞの出力

:option. EXPlicit
:group. 3
:usage. explicit typing required
:jusage. 明示的な型が必要

:option. DEBug
:group. 3
:usage. -trace -bounds

:option. TRace
:group. 3
:usage. generate run-time traceback
:jusage. ﾗﾝﾀｲﾑﾄﾚｰｽﾊﾞｯｸの生成

:option. BOunds
:group. 3
:usage. generate bounds checking
:jusage. 境界ﾁｪｯｸの生成

:option. STack
:group. 3
:usage. stack checking
:jusage. ｽﾀｯｸﾁｪｯｸ

:cmt.
:cmt.   Debugging Options
:cmt.
:usagegroup. 4
:usage. *Debugging Options*
:jusage. *ﾃﾞﾊﾞｯｸﾞｵﾌﾟｼｮﾝ*

:option. D1
:group. 4
:usage. line # debugging information
:jusage. line # ﾃﾞﾊﾞｯｸﾞ情報

:option. D2
:group. 4
:usage. full debugging information
:jusage. 全てのﾃﾞﾊﾞｯｸﾞ情報

:option. HC
:group. 4
:usage. Codeview debugging info.
:jusage. Codeviewﾃﾞﾊﾞｯｸﾞ情報

:option. HD
:group. 4
:usage. DWARF debugging information
:jusage. DWARFﾃﾞﾊﾞｯｸﾞ情報

:option. HW
:group. 4
:usage. WATCOM debugging information
:jusage. WATCOMﾃﾞﾊﾞｯｸﾞ情報

:option. DB
:group. 4
:internal.
:usage. generate browse information
:jusage. ﾌﾞﾗｳｽﾞ情報の生成

:cmt.
:cmt.   Floating-Point Options
:cmt.
:usagegroup. 5
:usage. *Floating-Point Options*
:jusage. *浮動小数点ｵﾌﾟｼｮﾝ*

:option. FPC
:target. i86 386
:group. 5
:usage. floating-point calls
:jusage. 浮動小数点呼び出し

:option. FPI
:target. i86 386
:group. 5
:usage. emulate 80x87 instructions
:jusage. 80x87 命令のｴﾐｭﾚｰﾄ

:option. FPI87
:target. i86 386
:group. 5
:usage. in-line 80x87 instructions
:jusage. 80x87 ｲﾝﾗｲﾝ命令

:option. FP287
:target. i86 386
:group. 5
:usage. 80287 instructions
:jusage. 80287 命令

:option. FP387
:target. i86 386
:group. 5
:usage. 80387 instructions
:jusage. 80387 命令

:option. FP5
:target. i86 386
:group. 5
:usage. -fp3 optimized for Pentium
:jusage. Pentium用最適化をする-fp3ｵﾌﾟｼｮﾝ

:option. FP6
:target. i86 386
:group. 5
:usage. -fp3 optimized for Pentium Pro
:jusage. Pentium Pro用に最適化された-fp3

:option. FPR
:target. i86 386
:group. 5
:usage. 80x87 reverse compatibility
:jusage. 80x87 ﾘﾊﾞｰｽ互換

:option. FPD
:target. i86 386
:group. 5
:usage. enable Pentium FDIV check
:jusage. Pentium FDIV命令のﾁｪｯｸを有効にします

:cmt.
:cmt.   Optimization Options
:cmt.
:usagegroup. 6
:usage. *Optimizations*
:jusage. *最適化*

:option. OB
:target. 386
:group. 6
:usage. base pointer optimizations
:jusage. ﾍﾞｰｽﾎﾟｲﾝﾀの最適化

:option. OBP
:group. 6
:usage. branch prediction
:jusage. 分岐予測

:option. OC
:group. 6
:usage. no call-ret optimizations
:jusage. call-retの最適化なし

:option. OD
:group. 6
:usage. disable optimizations
:jusage. 最適化の禁止

:option. OF
:target. 386
:group. 6
:usage. no stack frame optimizations
:jusage. ｽﾀｯｸﾌﾚｰﾑの最適化なし

:option. OH
:group. 6
:usage. optimize at expense of compile-time
:jusage. ｺﾝﾊﾟｲﾙ時間が長引いても最適化を強化します

:option. OI
:group. 6
:usage. statement functions in-line
:jusage. ｲﾝﾗｲﾝ文関数

:option. OK
:group. 6
:usage. move register saves into flow graph
:jusage. ﾚｼﾞｽﾀ保存をﾌﾛｰｸﾞﾗﾌに移動します

:option. OL
:group. 6
:usage. loop optimizations
:jusage. ﾙｰﾌﾟの最適化

:option. OL+
:group. 6
:usage. loop optimizations/unrolling
:jusage. ﾙｰﾌﾟ最適化/ｱﾝﾛｰﾘﾝｸﾞ

:option. OLF
:group. 6
:internal.
:usage. loop optimizations/assume loop invariant float-pt. variables are init.
:jusage. ﾙｰﾌﾟ最適化/ﾙｰﾌﾟ内で浮動小数点数が不変であると仮定します。変数は初期化されます

:option. OM
:group. 6
:usage. math optimizations
:jusage. 数学的最適化

:option. ON
:group. 6
:usage. numerical optimizations
:jusage. 数値の最適化

:option. OP
:group. 6
:usage. precision optimizations
:jusage. 精度の最適化

:option. OR
:group. 6
:usage. instruction scheduling
:jusage. 命令ｽｹｼﾞｭｰﾘﾝｸﾞ

:option. OS
:group. 6
:usage. space optimizations
:jusage. ｻｲｽﾞの最適化

:option. OT
:group. 6
:usage. time optimizations
:jusage. 時間の最適化

:option. ODO
:group. 6
:usage. DO-variables do not overflow
:jusage. DO変数がｵｰﾊﾞｰﾌﾛｰしません

:option. OX
:target. 386
:group. 6
:usage. -o[b,bp,i,k,l,m,r,t,do]

:option. OX
:target. i86 axp ppc
:group. 6
:usage. -o[bp,i,k,l,m,r,t,do]

:cmt.
:cmt.   Memory Models
:cmt.
:usagegroup. 7
:usage. *Memory Models*
:jusage. *ﾒﾓﾘﾓﾃﾞﾙ*

:option. MF
:target. 386
:group. 7
:usage. flat memory model
:jusage. ﾌﾗｯﾄﾒﾓﾘﾓﾃﾞﾙ

:option. MS
:target. 386
:group. 7
:usage. small memory model
:jusage. ｽﾓｰﾙﾒﾓﾘﾓﾃﾞﾙ

:option. MC
:target. 386
:group. 7
:usage. compact memory model
:jusage. ｺﾝﾊﾟｸﾄﾒﾓﾘﾓﾃﾞﾙ

:option. MM
:target. i86 386
:group. 7
:usage. medium memory model
:jusage. ﾐﾃﾞｨｱﾑﾒﾓﾘﾓﾃﾞﾙ

:option. ML
:target. i86 386
:group. 7
:usage. large memory model
:jusage. ﾗｰｼﾞﾒﾓﾘﾓﾃﾞﾙ

:option. MH
:target. i86
:group. 7
:usage. huge memory model
:jusage. ﾋｭｰｼﾞﾒﾓﾘﾓﾃﾞﾙ

:cmt.
:cmt.   CPU Targets
:cmt.
:usagegroup. 8
:usage. *CPU Targets*
:jusage. *CPU ﾀｰｹﾞｯﾄ*

:option. 0
:target. i86
:group. 8
:usage. assume 8086 processor
:jusage. 8086 ﾌﾟﾛｾｯｻ命令を使用

:option. 1
:target. i86
:group. 8
:usage. assume 80186 processor
:jusage. 80186 ﾌﾟﾛｾｯｻ命令を使用

:option. 2
:target. i86
:group. 8
:usage. assume 80286 processor
:jusage. 80286 ﾌﾟﾛｾｯｻ命令を使用

:option. 3
:target. i86 386
:group. 8
:usage. assume 80386 processor
:jusage. 80386 ﾌﾟﾛｾｯｻ命令を使用

:option. 4
:target. i86 386
:group. 8
:usage. optimized for 80486
:jusage. 80486 用に最適化

:option. 5
:target. i86 386
:group. 8
:usage. optimized for Pentium
:jusage. Pentium 用に最適化された

:option. 6
:target. i86 386
:group. 8
:usage. optimized for Pentium Pro
:jusage. Pentium Pro 用に最適化

:cmt.
:cmt.   Application Type
:cmt.
:usagegroup. 9
:usage. *Application Type*
:jusage. *ｱﾌﾟﾘｹｰｼｮﾝﾀｲﾌﾟ*

:option. BW
:group. 9
:usage. default windowed application
:jusage. ﾃﾞﾌｫﾙﾄｳｨﾝﾄﾞｳｱﾌﾟﾘｹｰｼｮﾝ

:option. BM
:target. 386 axp ppc
:group. 9
:usage. multithread application
:jusage. ﾏﾙﾁｽﾚｯﾄﾞｱﾌﾟﾘｹｰｼｮﾝ

:option. BD
:target. 386 axp ppc
:group. 9
:usage. dynamic link library
:jusage. ﾀﾞｲﾅﾐｯｸﾘﾝｸﾗｲﾌﾞﾗﾘ

:option. WINdows
:target. i86 386
:group. 9
:usage. generate code for Windows
:jusage. ｳｨﾝﾄﾞｳｽﾞ用ｺｰﾄﾞ生成


:cmt.
:cmt.   Character set
:cmt.
:usagegroup. 10
:usage. *Character Set Options*
:jusage. *文字ｾｯﾄｵﾌﾟｼｮﾝ*

:option. CHInese
:group. 10
:usage. Chinese character set
:jusage. 中国語文字ｾｯﾄ

:option. Japanese
:group. 10
:usage. Japanese character set
:jusage. 日本語文字ｾｯﾄ

:option. KOrean
:group. 10
:usage. Korean character set
:jusage. 韓国語文字ｾｯﾄ


:cmt.
:cmt.   Miscellaneous Options
:cmt.
:usagegroup. 11
:usage. *Miscellaneous Options*
:jusage. *その他のｵﾌﾟｼｮﾝ*

:option. SHort
:group. 11
:usage. INTEGER/LOGICAL size 2/1
:jusage. INTEGER/LOGICAL の大きさは 2/1

:option. XFloat
:group. 11
:usage. extend float-pt. precision
:jusage. 拡張浮動小数点数精度

:option. XLine
:group. 11
:usage. extend line length
:jusage. 行の長さを拡張

:option. DEFine
:group. 11
:usage. define macro
:jusage. ﾏｸﾛ定義

:option. FORmat
:group. 11
:usage. relax FORMAT type checking
:jusage. FORMAT ﾀｲﾌﾟﾁｪｯｸの緩和

:option. WILd
:group. 11
:usage. relax wild branch checking
:jusage. ﾜｲﾙﾄﾞﾌﾞﾗﾝﾁﾁｪｯｸの緩和

:option. TErminal
:group. 11
:usage. display diagnostic messages
:jusage. 診断ﾒｯｾｰｼﾞの表示

:option. Quiet ZQ
:group. 11
:usage. operate quietly
:jusage. 起動ﾒｯｾｰｼﾞを表示しない

:option. RESources
:group. 11
:usage. messages in resource file
:jusage. ﾘｿｰｽﾌｧｲﾙ内のﾒｯｾｰｼﾞ

:option. CC
:group. 11
:usage. devices are carriage control
:jusage. 装置はｷｬﾘｯｼﾞ制御されます

:option. LFwithFF
:group. 11
:usage. LF with FF
:jusage. FF付きLF

:option. DEPendency
:group. 11
:usage. generate file dependencies
:jusage. ﾌｧｲﾙ依存性情報の生成

:option. SR
:target. i86 386
:group. 11
:usage. save/restore segment regs
:jusage. ｾｸﾞﾒﾝﾄﾚｼﾞｽﾀの待避/復帰

:option. FSfloats
:target. i86 386
:group. 11
:usage. FS not fixed
:jusage. FSを固定しない

:option. GSfloats
:target. i86 386
:group. 11
:usage. GS not fixed
:jusage. GSを固定しない

:option. SSfloats
:target. i86 386
:group. 11
:usage. SS not default data segment
:jusage. SSはﾃﾞﾌｫﾙﾄのﾃﾞｰﾀｾｸﾞﾒﾝﾄでない

:option. SC
:target. 386
:group. 11
:usage. stack calling convention
:jusage. ｽﾀｯｸ呼び出し規約

:option. EZ
:target. 386
:group. 11
:usage. Easy OMF-386 object files
:jusage. Easy OMF-386 ｵﾌﾞｼﾞｪｸﾄﾌｧｲﾙ

:option. SG
:target. 386
:group. 11
:usage. automatic stack growing
:jusage. ｽﾀｯｸの自動伸張

:option. SYntax
:group. 11
:usage. syntax check only
:jusage. 文法ﾁｪｯｸのみ

:option. LIBinfo
:group. 11
:usage. generate default libraries
:jusage. ﾃﾞﾌｫﾙﾄﾗｲﾌﾞﾗﾘの生成

:option. DT
:group. 11
:usage. set data threshold
:jusage. ﾃﾞｰﾀｽﾚｯｼｮﾙﾄﾞの設定

:option. AUtomatic
:group. 11
:usage. local variables on the stack
:jusage. ｽﾀｯｸ上のﾛｰｶﾙ変数

:option. DEScriptor
:group. 11
:usage. pass character descriptors
:jusage. 文字指示子渡し

:option. SAve
:group. 11
:usage. SAVE local variables
:jusage. ﾛｰｶﾙ変数を待避(SAVE文と同じ)

:option. COde
:target. i86 386
:group. 11
:usage. constants in code segment
:jusage. ｺｰﾄﾞｾｸﾞﾒﾝﾄ内の定数

:option. ALign
:group. 11
:usage. align COMMON segments
:jusage. COMMONｾｸﾞﾒﾝﾄの整合

:option. MAngle
:group. 11
:usage. mangle COMMON segment names
:jusage. COMMONｾｸﾞﾒﾝﾄ名をﾏﾝｸﾞﾙ化します

:option. IPromote
:group. 11
:usage. promote intrinsic arguments
:jusage. 組込み引数を格上げします

:option. SEpcomma
:group. 11
:usage. allow comma separator
:jusage. ｺﾝﾏ区切りを有効にします

:option. LGA
:target. axp ppc
:group. 11
:internal.
:usage.

:option. LGO
:target. axp ppc
:group. 11
:internal.
:usage.

:option. LGC
:group. 11
:internal.
:usage.
