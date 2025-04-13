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
:cmt.* Description:  librarian wlib command line options.
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


:title.  Usage: wlib { <option> } <library_name> [ <commands> ]
:jtitle. 使用方法: wlib { <option> } <library_name> [ <commands> ]
:titleu.  Usage: %C { <option> } <library_name> [ <commands> ]
:jtitleu. 使用方法: %C { <option> } <library_name> [ <commands> ]

:title.
:jtitle.

:title.  Where:
:jtitle. 各フィールドは以下の通りです:

:title.  . <commands>  ::= <cmd> <commands>
:jtitle.

:title.  .               | @<env_var> <commands>
:jtitle. .               | @<環境変数> <commands>

:title.  .               | @<cmd_file> <commands>
:jtitle.

:title.  . <cmd_file>  ::= file which contains { <cmd> }
:jtitle. . <cmd_file>  ::= { <cmd> }を含むファイル

:title.
:jtitle.

:title.  . <cmd>       ::= +<object_file_name>                  (add file)
:jtitle. . <cmd>       ::= +<オブジェクトファイル名>                   (ファイルの追加)

:title.  .               | +<library_file_name>.lib             (add library)
:jtitle. .               | +<ライブラリファイル名>.lib                 (ライブラリの追加)

:title.  .               | ++<symbol_name>.<DLL_name>[.<export_name>][.<ordinal>]
:jtitle. .               | ++<シンボル名>.<DLL名>[.<エクスポート名>][.<序数>]

:title.  .               | -<module_name>                       (delete)
:jtitle. .               | -<モジュール名>                          (削除)

:title.  .               | -+<object_file_name>                 (replace)
:jtitle. .               | -+<オブジェクトファイル名>                  (入れ替え)

:title.  .               | +-<object_file_name>                 (replace)
:jtitle. .               | +-<オブジェクトファイル名>                  (入れ替え)

:title.  .               | :<module_name>[=<object_file_name>]  (extract)
:jtitle. .               | :<モジュール名>[=<オブジェクトファイル名>]   (取り出し)
:target. bsd linux osx qnx haiku

:title.  .               | *<module_name>[=<object_file_name>]  (extract)
:jtitle. .               | *<モジュール名>[=<オブジェクトファイル名>]   (取り出し)
:ntarget. bsd linux osx qnx haiku

:title.  .               | -:<module_name>[=<object_file_name>] (extract and delete)
:jtitle. .               | -:<モジュール名>[=<オブジェクトファイル名>]  (取り出しと削除)
:target. bsd linux osx qnx haiku

:title.  .               | -*<module_name>[=<object_file_name>] (extract and delete)
:jtitle. .               | -*<モジュール名>[=<オブジェクトファイル名>]  (取り出しと削除)
:ntarget. bsd linux osx qnx haiku

:title.  .               | :-<module_name>[=<object_file_name>] (extract and delete)
:jtitle. .               | :-<モジュール名>[=<オブジェクトファイル名>]  (取り出しと削除)
:target. bsd linux osx qnx haiku

:title.  .               | *-<module_name>[=<object_file_name>] (extract and delete)
:jtitle. .               | *-<モジュール名>[=<オブジェクトファイル名>]  (取り出しと削除)
:ntarget. bsd linux osx qnx haiku

:title.
:jtitle.

:title.  The default filename extensions are lib, bak, lst, o and lbc.
:jtitle. デフォルトのファイル拡張子は、lib, bak, lst, o, lbcです。
:target. bsd linux osx qnx haiku

:title.  The default filename extensions are lib, bak, lst, obj and lbc.
:jtitle. デフォルトのファイル拡張子は、lib, bak, lst, obj, lbcです。
:ntarget. bsd linux osx qnx haiku

:title.  Enclose a file name with quotes, i.e. 'filename', if it contains '-' or '@'.
:jtitle. ﾌｧｲﾙ名に'-'や'@'が含まれるときは、'filename'のように引用符でﾌｧｲﾙ名を囲みます

:title.
:jtitle.

:title.  Options:
:jtitle. オプション:

:title.
:jtitle.
:target. bsd linux osx qnx haiku

:title.  .         ( /option is also accepted )
:jtitle. .         ( /ｵﾌﾟｼｮﾝ でも指定できます )
:ntarget. bsd linux osx qnx haiku

:option. ? h
:usage.  display this screen
:jusage. この画面を表示します

:option. b
:usage.  don't create .bak file
:jusage. .bakファイルを作成しません

:option. c
:usage.  case sensitive
:jusage. 大文字小文字を区別します

:option. d
:path.
:usage.  object output directory
:jusage. オブジェクト出力ディレクトリ

:usagechain. . f
:usage. Output Library format
:jusage. 出力ライブラリ形式

:option. fa
:enumerate. libtype
:usage.  output AR format library
:jusage. AR形式ﾗｲﾌﾞﾗﾘを出力します

:option. fab
:enumerate. libformat
:usage.  output AR format (BSD) library
:jusage. AR形式ﾗｲﾌﾞﾗﾘを出力します

:option. fac
:enumerate. libformat
:usage.  output AR format (COFF) library
:jusage. AR形式ﾗｲﾌﾞﾗﾘを出力します

:option. fag
:enumerate. libformat
:usage.  output AR format (GNU) library
:jusage. AR形式ﾗｲﾌﾞﾗﾘを出力します

:option. fm
:enumerate. libtype
:usage.  output MLIB format library
:jusage. MLIB形式ライブラリを出力します

:option. fo
:enumerate. libtype
:usage.  output OMF format library
:jusage. OMF形式ライブラリを出力します

:usagegroup. 1
:usage. Import records architecture
:jusage. インポートレコードのアーキテクチャ

:usagechain. 1 i

:usagegroup. 2
:usage. Import records format
:jusage. インポートレコード形式

:usagechain. 2 i

:option. i6
:group. 1
:enumerate. processor
:usage.  generate X64 import records
:jusage. X64インポート･レコードを生成します

:option. ia
:group. 1
:enumerate. processor
:usage.  generate AXP import records
:jusage. AXPｲﾝﾎﾟｰﾄ･ﾚｺｰﾄﾞを生成します

:option. ic
:group. 2
:enumerate. filetype
:usage.  generate COFF imp. records (short)

:option. icl
:group. 2
:enumerate. filetype
:usage.  generate COFF imp. records (long)

:option. ie
:group. 2
:enumerate. filetype
:usage.  generate ELF import records
:jusage. ELFインポート･レコードを生成します

:option. ii
:group. 1
:enumerate. processor
:usage.  generate X86 import records
:jusage. X86インポート･レコードを生成します

:option. im
:group. 1
:enumerate. processor
:usage.  generate MIPS import records
:jusage. MIPSインポート･レコードを生成します

:usagechain. . in
:usage. Non-resident table imports

:option. inn
:enumerate. non_resident
:usage.  non-resident table symbols by name
:jusage. 非常駐テーブルシンボル (名前別)

:option. ino
:enumerate. non_resident
:usage.  non-resident table symbols by ordinal
:jusage. 序数による非常駐テーブルシンボル

:option. io
:group. 2
:enumerate. filetype
:usage.  generate OMF import records
:jusage. OMFインポート･レコードを生成します

:option. ip
:group. 1
:enumerate. processor
:usage.  generate PPC import records
:jusage. PPCインポート･レコードを生成します

:usagechain. . ir
:usage. Resident table imports

:option. irn
:enumerate. resident
:usage.  resident table symbols by name
:jusage. 名前ごとの常駐テーブルシンボル

:option. iro
:enumerate. resident
:usage.  resident table symbols by ordinal
:jusage. 序数による常駐テーブルシンボル

:option. l
:file.
:optional.
:usage.  list file name
:jusage. リストファイル名

:option. m
:usage.  display C++ mangled names
:jusage. C++のマングルド名を表示します

:option. n
:usage.  always create a new library
:jusage. 常に新しいライブラリを作成します

:option. o
:file.
:usage.  output library file name
:jusage. 出力ライブラリファイル名

:option. p
:number.
:enumerate. page_size
:usage.  set OMF library page size
:jusage. OMF ライブラリのページ サイズを設定する

:option. pa
:enumerate. page_size
:usage.  set OMF library optimal page size
:jusage. OMF ライブラリの最適なページ サイズを設定する

:option. q
:usage.  don't print header
:jusage. 起動メッセージを表示しません
:timestamp.

:option. s
:usage.  strip line number debug info

:option. t
:usage.  trim THEADR pathnames
:jusage. THEADR パス名を削除します

:option. tl
:usage.  internal terse listing

:option. v
:usage.  print header
:timestamp.

:option. x
:file.
:optional.
:usage.  explode all objects in library
:jusage. ライブラリからすべてのオブジェクトを取り出します

:option. z
:file.
:optional.
:usage.  strip exported symbols info

:option. zld
:usage.  strip file dependency info

:option. zll
:usage.  strip library dependency info
