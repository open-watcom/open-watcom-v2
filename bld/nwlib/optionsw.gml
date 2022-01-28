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
:cmt.* Description:  librarian wlib command line options.
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


:title.  Usage: wlib { <option> } <library_name> [ <commands> ]
:jtitle. 使用方法: wlib { <option> } <library_name> [ <commands> ]
:titleu.  Usage: %C { <option> } <library_name> [ <commands> ]
:jtitleu. 使用方法: %C { <option> } <library_name> [ <commands> ]
:target. any

:title.
:jtitle.
:target. any

:title.  Where:
:jtitle. 各フィールドは以下の通りです:
:target. any

:title.  . <commands>  ::= <cmd> <commands>
:jtitle.
:target. any

:title.  .               | @<env_var> <commands>
:jtitle. .               | @<環境変数> <commands>
:target. any

:title.  .               | @<cmd_file> <commands>
:jtitle.
:target. any

:title.  . <cmd_file>  ::= file which contains { <cmd> }
:jtitle. . <cmd_file>  ::= { <cmd> }を含むファイル
:target. any

:title.
:jtitle.
:target. any

:title.  . <cmd>       ::= +<object_file_name>                  (add file)
:jtitle. . <cmd>       ::= +<オブジェクトファイル名>                   (ファイルの追加)
:target. any

:title.  .               | +<library_file_name>.lib             (add library)
:jtitle. .               | +<ライブラリファイル名>.lib                 (ライブラリの追加)
:target. any

:title.  .               | ++<symbol_name>.<DLL_name>[.<export_name>][.<ordinal>]
:jtitle. .               | ++<シンボル名>.<DLL名>[.<エクスポート名>][.<序数>]
:target. any

:title.  .               | -<module_name>                       (delete)
:jtitle. .               | -<モジュール名>                          (削除)
:target. any

:title.  .               | -+<object_file_name>                 (replace)
:jtitle. .               | -+<オブジェクトファイル名>                  (入れ替え)
:target. any

:title.  .               | +-<object_file_name>                 (replace)
:jtitle. .               | +-<オブジェクトファイル名>                  (入れ替え)
:target. any

:title.  .               | :<module_name>[=<object_file_name>]  (extract)
:jtitle. .               | :<モジュール名>[=<オブジェクトファイル名>]   (取り出し)
:target. bsd linux osx qnx haiku

:title.  .               | *<module_name>[=<object_file_name>]  (extract)
:jtitle. .               | *<モジュール名>[=<オブジェクトファイル名>]   (取り出し)
:target. any
:ntarget. bsd linux osx qnx haiku

:title.  .               | -:<module_name>[=<object_file_name>] (extract and delete)
:jtitle. .               | -:<モジュール名>[=<オブジェクトファイル名>]  (取り出しと削除)
:target. bsd linux osx qnx haiku

:title.  .               | -*<module_name>[=<object_file_name>] (extract and delete)
:jtitle. .               | -*<モジュール名>[=<オブジェクトファイル名>]  (取り出しと削除)
:target. any
:ntarget. bsd linux osx qnx haiku

:title.  .               | :-<module_name>[=<object_file_name>] (extract and delete)
:jtitle. .               | :-<モジュール名>[=<オブジェクトファイル名>]  (取り出しと削除)
:target. bsd linux osx qnx haiku

:title.  .               | *-<module_name>[=<object_file_name>] (extract and delete)
:jtitle. .               | *-<モジュール名>[=<オブジェクトファイル名>]  (取り出しと削除)
:target. any
:ntarget. bsd linux osx qnx haiku

:title.
:jtitle.
:target. any

:title.  The default filename extensions are lib, bak, lst, o and lbc.
:jtitle. デフォルトのファイル拡張子は、lib, bak, lst, o, lbcです。
:target. bsd linux osx qnx haiku

:title.  The default filename extensions are lib, bak, lst, obj and lbc.
:jtitle. デフォルトのファイル拡張子は、lib, bak, lst, obj, lbcです。
:target. any
:ntarget. bsd linux osx qnx haiku

:title.  Enclose a file name with quotes, i.e. 'filename', if it contains '-' or '@'.
:jtitle. ﾌｧｲﾙ名に'-'や'@'が含まれるときは、'filename'のように引用符でﾌｧｲﾙ名を囲みます
:target. any

:title.
:jtitle.
:target. any

:title.  Options:
:jtitle. オプション:
:target. any

:title.
:jtitle.
:target. bsd linux osx qnx haiku

:title.  .         ( /option is also accepted )
:jtitle. .         ( /ｵﾌﾟｼｮﾝ でも指定できます )
:target. any
:ntarget. bsd linux osx qnx haiku

:chain. f Output format
:jusage. 出力フォーマット
:target. any

:chain. i Import format
:jusage. インポート形式
:target. any

:option. b
:target. any
:usage.  don't create .bak file
:jusage. .bakファイルを作成しません

:option. c
:target. any
:usage.  case sensitive
:jusage. 大文字小文字を区別します

:option. d
:target. any
:path.
:usage.  object output directory
:jusage.

:option. l
:target. any
:file.
:usage.  list file name
:jusage.

:option. m
:target. any
:usage.  display C++ mangled names
:jusage. C++のマングルド名を表示します

:option. n
:target. any
:usage.  always create a new library
:jusage. 常に新しいライブラリを作成します

:option. o
:target. any
:file.
:usage.  output library file name
:jusage.

:option. p
:target. any
:number.
:usage.  set OMF library page size
:jusage.

:option. q
:target. any
:usage.  don't print header
:jusage. 起動メッセージを表示しません

:option. s
:target. any
:usage.  strip line number debug info
:jusage.

:option. t
:target. any
:usage.  trim THEADR pathnames
:jusage. THEADR パス名を削除します

:option. v
:target. any
:usage.  print header
:jusage.

:option. x
:target. any
:usage.  explode all objects in library
:jusage. ライブラリからすべてのオブジェクトを取り出します

:option. zld
:target. any
:usage.  strip file dependency info
:jusage.

:option. ? h
:target. any
:usage.  display this screen
:jusage. この画面を表示します

:option. fa
:target. any
:usage.  output AR format library
:jusage. AR形式ﾗｲﾌﾞﾗﾘを出力します

:option. fab
:target. any
:usage.  output AR format (BSD) library
:jusage. AR形式ﾗｲﾌﾞﾗﾘを出力します

:option. fac
:target. any
:usage.  output AR format (COFF) library
:jusage. AR形式ﾗｲﾌﾞﾗﾘを出力します

:option. fag
:target. any
:usage.  output AR format (GNU) library
:jusage. AR形式ﾗｲﾌﾞﾗﾘを出力します

:option. fm
:target. any
:usage.  output MLIB format library
:jusage. MLIB形式ライブラリを出力します

:option. fo
:target. any
:usage.  output OMF format library
:jusage. OMF形式ライブラリを出力します


:option. iro
:target. any
:usage.  resident table, ordinal
:jusage. ﾚｼﾞﾃﾞﾝﾄ, 序数

:option. irn
:target. any
:usage.  resident table, name
:jusage. ﾚｼﾞﾃﾞﾝﾄ, 名前

:option. ino
:target. any
:usage.  non-resident table, ordinal
:jusage. 非ﾚｼﾞﾃﾞﾝﾄﾃｰﾌﾞﾙ, 序数

:option. inn
:target. any
:usage.  non-resident table, name
:jusage. 非ﾚｼﾞﾃﾞﾝﾄﾃｰﾌﾞﾙ, 名前

:option. i6
:target. any
:usage.  generate X64 import records
:jusage. X64Cンポート･レコードを生成します

:option. ia
:target. any
:usage.  generate AXP import records
:jusage. AXPｲﾝﾎﾟｰﾄ･ﾚｺｰﾄﾞを生成します

:option. ii
:target. any
:usage.  generate X86 import records
:jusage. X86インポート･レコードを生成します

:option. ip
:target. any
:usage.  generate PPC import records
:jusage. PPCインポート･レコードを生成します

:option. ie
:target. any
:usage.  generate ELF import records
:jusage. ELFインポート･レコードを生成します

:option. ic
:target. any
:usage.  generate COFF import records
:jusage. COFFｲﾝﾎﾟｰﾄ･ﾚｺｰﾄﾞを生成します

:option. icl
:target. any
:usage.  generate 'long' COFF imp. records
:jusage. COFFｲﾝﾎﾟｰﾄ･ﾚｺｰﾄﾞを生成します

:option. io
:target. any
:usage.  generate OMF import records
:jusage. OMFインポート･レコードを生成します
